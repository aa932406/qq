import json
import os
import aiohttp
import asyncio
import random
from datetime import datetime, date, timedelta
from typing import Optional, Dict, List, Any
from astrbot.api.event import filter, AstrMessageEvent
from astrbot.api.star import Context, Star, register
from astrbot.api import logger, AstrBotConfig


@register("game_bind", "aa932406", "游戏账号绑定与充值插件", "3.0.0")
class GameBindPlugin(Star):
    def __init__(self, context: Context, config: AstrBotConfig):
        super().__init__(context)
        self.config = config
        
        # 初始化数据存储
        self.data_dir = os.path.join(os.path.dirname(__file__), "data")
        os.makedirs(self.data_dir, exist_ok=True)
        
        # 数据文件
        self.bind_file = os.path.join(self.data_dir, "bindings.json")
        self.recharge_file = os.path.join(self.data_dir, "recharge_logs.json")
        self.points_file = os.path.join(self.data_dir, "user_points.json")
        self.sign_file = os.path.join(self.data_dir, "sign_records.json")
        self.admins_file = os.path.join(self.data_dir, "admins.json")
        
        # 加载数据
        self.bindings = self._load_json(self.bind_file)
        self.recharge_logs = self._load_json(self.recharge_file)
        self.user_points = self._load_json(self.points_file)
        self.sign_records = self._load_json(self.sign_file)
        self.admins_data = self._load_json(self.admins_file)
        
        # 从配置获取管理员列表（优先从配置文件获取，否则使用本地文件，最后使用默认）
        config_admins = self.config.get("admins", [])
        if config_admins:
            self.admins = config_admins
        elif self.admins_data:
            self.admins = self.admins_data.get("admin_qq_ids", [])
        else:
            # 默认管理员
            self.admins = [965959320]
        
        # API配置 - 从config中获取或使用默认值
        self.api_config = {
            "base_url": self.config.get("api_url", "http://115.190.64.181:881/api/players.php"),
            "timeout": self.config.get("timeout", 30),
            "qq_bot_secret": self.config.get("api_secret", "ws7ecejjsznhtxurchknmdemax2fnp5d")
        }
        
        # 系统配置 - 从config中获取或使用默认值
        self.system_config = {
            "recharge_ratio": self.config.get("recharge_ratio", 100000),
            "sign_rewards": self.config.get("sign_rewards", {
                1: 1, 2: 2, 3: 3, 4: 4, 5: 5, 6: 6, 7: 10, 14: 15, 30: 30
            }),
            "max_points_per_day": self.config.get("max_points_per_day", 1000),
            "min_recharge_points": self.config.get("min_recharge_points", 1)
        }
        
        # 功能开关 - 从config中获取或使用默认值
        self.features = self.config.get("features", {
            "allow_modify_bind": True,
            "allow_gift_points": True,
            "allow_recharge_others": True,
            "allow_points_transfer": True
        })
        
        logger.info("✨ 游戏账号插件初始化完成！")
        logger.info(f"配置信息：API地址={self.api_config['base_url']}, 管理员数量={len(self.admins)}")
    
    def _load_json(self, file_path: str) -> dict:
        """加载JSON文件"""
        try:
            if os.path.exists(file_path):
                with open(file_path, 'r', encoding='utf-8') as f:
                    return json.load(f)
        except Exception as e:
            logger.error(f"📂 加载文件失败 {file_path}: {e}")
        return {}
    
    def _save_json(self, file_path: str, data: dict):
        """保存JSON文件"""
        try:
            with open(file_path, 'w', encoding='utf-8') as f:
                json.dump(data, f, ensure_ascii=False, indent=2)
        except Exception as e:
            logger.error(f"💾 保存文件失败 {file_path}: {e}")
    
    def _get_user_id(self, event: AstrMessageEvent) -> str:
        """获取用户ID"""
        qq_id = ""
        try:
            sender_id = event.get_sender_id()
            if sender_id:
                qq_id = str(sender_id)
                return qq_id
            
            sender_name = event.get_sender_name()
            if sender_name and '/' in sender_name:
                parts = sender_name.split('/')
                if len(parts) >= 2 and parts[1].isdigit():
                    qq_id = parts[1]
                    return qq_id
        except Exception as e:
            logger.error(f"❓ 获取用户ID异常: {e}")
        return qq_id if qq_id else "unknown"
    
    def _is_admin(self, qq_id: str) -> bool:
        """检查是否为管理员"""
        return str(qq_id) in [str(admin) for admin in self.admins]
    
    def _add_admin(self, qq_id: str) -> bool:
        """添加管理员"""
        qq_id_str = str(qq_id)
        if qq_id_str not in [str(admin) for admin in self.admins]:
            self.admins.append(qq_id_str)
            
            # 保存到文件
            self.admins_data = {
                "admin_qq_ids": self.admins,
                "last_updated": datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            }
            self._save_json(self.admins_file, self.admins_data)
            return True
        return False
    
    def _remove_admin(self, qq_id: str) -> bool:
        """移除管理员"""
        qq_id_str = str(qq_id)
        original_len = len(self.admins)
        self.admins = [admin for admin in self.admins if str(admin) != qq_id_str]
        
        if len(self.admins) < original_len:
            # 保存到文件
            self.admins_data = {
                "admin_qq_ids": self.admins,
                "last_updated": datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            }
            self._save_json(self.admins_file, self.admins_data)
            return True
        return False
    
    def _is_account_already_bound(self, game_account: str, exclude_qq: str = None) -> tuple:
        """检查游戏账号是否已被绑定"""
        for qq_id, bind_info in self.bindings.items():
            if bind_info.get("game_account") == game_account:
                if exclude_qq and qq_id == exclude_qq:
                    continue
                return True, qq_id, bind_info
        return False, None, None
    
    def _get_user_points(self, qq_id: str) -> Dict:
        """获取用户积分信息"""
        if qq_id not in self.user_points:
            self.user_points[qq_id] = {
                "points": 0,
                "total_earned": 0,
                "total_spent": 0,
                "first_sign_date": None,
                "last_sign_date": None,
                "continuous_days": 0
            }
        return self.user_points[qq_id]
    
    def _update_user_points(self, qq_id: str, points_data: Dict):
        """更新用户积分信息"""
        self.user_points[qq_id] = points_data
        self._save_json(self.points_file, self.user_points)
    
    def _transfer_points(self, from_qq: str, to_qq: str, points: int, reason: str = "") -> tuple:
        """转移积分"""
        if not self.features.get("allow_points_transfer", True):
            return False, "积分转移功能已禁用"
        
        if from_qq not in self.user_points:
            return False, "源用户不存在"
        if to_qq not in self.user_points:
            return False, "目标用户不存在"
        
        if self.user_points[from_qq]["points"] < points:
            return False, "积分不足"
        
        # 执行转移
        self.user_points[from_qq]["points"] -= points
        self.user_points[from_qq]["total_spent"] += points
        
        self.user_points[to_qq]["points"] += points
        self.user_points[to_qq]["total_earned"] += points
        
        self._save_json(self.points_file, self.user_points)
        
        # 记录转移日志
        transfer_id = f"T{datetime.now().strftime('%Y%m%d%H%M%S')}_{from_qq}"
        self.recharge_logs[transfer_id] = {
            "type": "points_transfer",
            "from_qq": from_qq,
            "to_qq": to_qq,
            "points": points,
            "reason": reason,
            "transfer_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        }
        self._save_json(self.recharge_file, self.recharge_logs)
        
        return True, "转移成功"
    
    async def initialize(self):
        logger.info("🚀 游戏账号插件已启动！")
    
    # ========== 帮助功能 ==========
    @filter.command("帮助")
    async def help_cmd(self, event: AstrMessageEvent):
        """显示帮助信息"""
        qq_id = self._get_user_id(event)
        is_admin = self._is_admin(qq_id)
        
        help_text = """游戏账号插件 - 命令列表

📌 常用命令：
• /绑定账号 <游戏账号>     # 绑定游戏账号
• /我的积分               # 查看积分余额
• /签到                  # 每日签到获得积分
• /积分充值 <积分数量>    # 用积分充值游戏
• /查询账号 [账号]        # 查看账号信息

💰 积分相关："""
        
        if self.features.get("allow_gift_points", True):
            help_text += """
• /赠送积分 <QQ> <积分> [备注]  # 赠送积分给他人"""
        
        if self.features.get("allow_recharge_others", True):
            help_text += """
• /给别人充值 <QQ> <积分> [备注] # 为他人充值游戏"""
        
        help_text += """
• /查询积分 <QQ>          # 查询他人积分

🔧 其他命令："""
        
        if self.features.get("allow_modify_bind", True):
            help_text += """
• /修改绑定 <新账号>      # 修改绑定账号"""
        
        help_text += """
• /解绑账号              # 解绑当前账号
• /测试连接              # 测试API连接"""
        
        if is_admin:
            help_text += """

👑 管理员命令：
• /添加积分 <QQ> <积分> [备注]  # 给用户添加积分
• /添加管理员 <QQ>         # 添加管理员
• /移除管理员 <QQ>         # 移除管理员
• /管理员列表             # 查看管理员列表
• /用户列表 [页码]        # 查看所有用户
• /充值记录 [数量]        # 查看充值记录
• /插件配置              # 查看当前配置"""

        help_text += f"""

💎 规则：
• 1积分 = {self.system_config['recharge_ratio']:,}元宝
• 签到获得积分
• 积分用于充值游戏账号
• 没有积分无法充值"""
        
        yield event.plain_result(help_text)
    
    # ========== 绑定功能 ==========
    @filter.command("绑定账号")
    async def bind_account_cmd(self, event: AstrMessageEvent):
        """绑定PHP游戏账号"""
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result("❌ 格式错误\n正确格式：/绑定账号 游戏账号")
            return
        
        game_account = parts[1]
        qq_id = self._get_user_id(event)
        
        if qq_id == "unknown":
            yield event.plain_result("❌ 身份验证失败，无法获取QQ信息")
            return
        
        # 检查此QQ是否已绑定
        if qq_id in self.bindings:
            old_account = self.bindings[qq_id]["game_account"]
            bind_time = self.bindings[qq_id]["bind_time"]
            yield event.plain_result(f"⚠️ 已绑定账号\n当前绑定：{old_account}\n绑定时间：{bind_time}\n\n如需更换账号：\n1. 先使用 /解绑账号\n2. 再重新绑定新账号")
            return
        
        # 检查账号是否已被绑定
        is_bound, bound_qq, bind_info = self._is_account_already_bound(game_account)
        if is_bound:
            yield event.plain_result(f"❌ 账号已被绑定\n游戏账号：{game_account}\n已被QQ：{bound_qq} 绑定\n绑定时间：{bind_info.get('bind_time', '未知')}")
            return
        
        # 验证账号是否存在
        try:
            account_info = await self._get_account_info(game_account)
            if not account_info:
                yield event.plain_result(f"❌ 账号不存在\n游戏账号：{game_account}\n在系统中未找到此账号")
                return
        except Exception as e:
            logger.error(f"验证游戏账号失败: {e}")
            yield event.plain_result("❌ 验证失败，网络连接异常，请稍后重试")
            return
        
        # 保存绑定
        self.bindings[qq_id] = {
            "game_account": game_account,
            "account_name": account_info.get("passport", game_account),
            "bind_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "qq_id": qq_id
        }
        self._save_json(self.bind_file, self.bindings)
        
        account_name = account_info.get("passport", game_account)
        content = f"""✅ 绑定成功！

游戏账号：{account_name}
当前余额：{account_info.get('gold_pay', 0):,} 元宝
累计充值：{account_info.get('gold_pay_total', 0):,} 元宝
绑定时间：{self.bindings[qq_id]['bind_time']}"""
        
        yield event.plain_result(content)
    
    # ========== 我的积分功能 ==========
    @filter.command("我的积分")
    async def my_points_cmd(self, event: AstrMessageEvent):
        """查看我的积分"""
        qq_id = self._get_user_id(event)
        
        if qq_id == "unknown":
            yield event.plain_result("❌ 身份验证失败，无法获取QQ信息")
            return
        
        user_points = self._get_user_points(qq_id)
        recharge_ratio = self.system_config["recharge_ratio"]
        
        content = f"""💰 我的积分

当前积分：{user_points['points']} 积分
累计获得：{user_points['total_earned']} 积分
累计消耗：{user_points['total_spent']} 积分
连续签到：{user_points['continuous_days']} 天

💎 积分用途：
• 1积分 = {recharge_ratio:,}元宝
• 可兑换：{user_points['points'] * recharge_ratio:,}元宝
• 使用 /积分充值 命令兑换"""
        
        if user_points["last_sign_date"]:
            content += f"\n\n📅 上次签到：{user_points['last_sign_date']}"
        
        content += "\n\n💡 每日签到可获得积分！"
        
        yield event.plain_result(content)
    
    # ========== 签到功能 ==========
    @filter.command("签到")
    async def sign_cmd(self, event: AstrMessageEvent):
        """每日签到获得积分"""
        qq_id = self._get_user_id(event)
        
        if qq_id == "unknown":
            yield event.plain_result("❌ 身份验证失败，无法获取QQ信息")
            return
        
        today = date.today().isoformat()
        
        # 检查是否已签到
        if qq_id in self.sign_records and self.sign_records[qq_id].get("last_sign") == today:
            user_points = self._get_user_points(qq_id)
            yield event.plain_result(f"⏳ 今日已签到\n签到时间：今天\n下次签到：明天\n当前积分：{user_points['points']} 积分")
            return
        
        user_points = self._get_user_points(qq_id)
        
        # 计算连续天数
        yesterday = (date.today() - timedelta(days=1)).isoformat()
        if user_points["last_sign_date"] == yesterday:
            user_points["continuous_days"] += 1
        elif user_points["last_sign_date"] != today:
            user_points["continuous_days"] = 1
        
        # 计算签到奖励（积分）
        continuous_days = user_points["continuous_days"]
        
        # 基础奖励
        base_reward = 1
        
        # 特殊天数奖励
        for day, reward in self.system_config["sign_rewards"].items():
            if continuous_days == day:
                base_reward = reward
                break
        else:
            # 如果不在特殊天数列表中，使用连续天数作为奖励（最高10积分）
            base_reward = min(continuous_days, 10)
        
        total_reward = base_reward
        
        # 更新积分
        user_points["points"] += total_reward
        user_points["total_earned"] += total_reward
        user_points["last_sign_date"] = today
        
        if not user_points["first_sign_date"]:
            user_points["first_sign_date"] = today
        
        self._update_user_points(qq_id, user_points)
        
        # 保存签到记录
        self.sign_records[qq_id] = {
            "last_sign": today,
            "reward": total_reward,
            "continuous_days": continuous_days
        }
        self._save_json(self.sign_file, self.sign_records)
        
        # 构建响应
        recharge_ratio = self.system_config["recharge_ratio"]
        content = f"""✨ 签到成功！

获得积分：{total_reward} 积分
连续签到：{continuous_days} 天
当前积分：{user_points['points']} 积分
累计获得：{user_points['total_earned']} 积分

💎 积分价值：
• 可兑换：{total_reward * recharge_ratio:,} 元宝
• 总可兑换：{user_points['points'] * recharge_ratio:,} 元宝

⏰ 签到时间：{datetime.now().strftime('%Y-%m-%d %H:%M')}"""
        
        yield event.plain_result(content)
    
    # ========== 积分充值功能 ==========
    @filter.command("积分充值")
    async def points_recharge_cmd(self, event: AstrMessageEvent):
        """使用积分充值游戏账号"""
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result("❌ 格式错误\n正确格式：/积分充值 <积分数量> [备注]\n例如：/积分充值 10 兑换元宝")
            return
        
        try:
            points_to_use = int(parts[1])
            if points_to_use <= 0:
                raise ValueError("必须是正数")
            
            # 检查最小充值积分限制
            min_points = self.system_config.get("min_recharge_points", 1)
            if points_to_use < min_points:
                yield event.plain_result(f"❌ 充值积分过少\n最少需要 {min_points} 积分才能充值")
                return
                
            remark = " ".join(parts[2:]) if len(parts) > 2 else "积分兑换"
        except ValueError:
            yield event.plain_result("❌ 参数错误，积分数量必须是正整数")
            return
        
        qq_id = self._get_user_id(event)
        
        if qq_id == "unknown":
            yield event.plain_result("❌ 身份验证失败，无法获取QQ信息")
            return
        
        # 检查绑定
        if qq_id not in self.bindings:
            yield event.plain_result("❌ 未绑定账号\n请先绑定游戏账号\n使用命令：/绑定账号 <游戏账号>")
            return
        
        user_points = self._get_user_points(qq_id)
        
        if user_points["points"] < points_to_use:
            yield event.plain_result(f"❌ 积分不足\n需要积分：{points_to_use}\n当前积分：{user_points['points']}\n\n💡 获取积分：每日签到，多签多得")
            return
        
        # 计算充值金额
        recharge_ratio = self.system_config["recharge_ratio"]
        recharge_amount = points_to_use * recharge_ratio
        
        game_account = self.bindings[qq_id]["game_account"]
        account_name = self.bindings[qq_id].get("account_name", game_account)
        
        # 执行充值
        try:
            result = await self._execute_account_recharge(game_account, recharge_amount, remark)
            
            if result.get("success"):
                # 扣减积分
                user_points["points"] -= points_to_use
                user_points["total_spent"] += points_to_use
                self._update_user_points(qq_id, user_points)
                
                # 记录充值日志
                recharge_id = f"P{datetime.now().strftime('%Y%m%d%H%M%S')}_{qq_id}"
                self.recharge_logs[recharge_id] = {
                    "qq_id": qq_id,
                    "game_account": game_account,
                    "account_name": account_name,
                    "points_used": points_to_use,
                    "recharge_amount": recharge_amount,
                    "remark": remark,
                    "recharge_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                    "api_response": result
                }
                self._save_json(self.recharge_file, self.recharge_logs)
                
                response_data = result.get("data", {})
                
                content = f"""✅ 充值成功！

游戏账号：{account_name}
消耗积分：{points_to_use} 积分
充值金额：{recharge_amount:,} 元宝
充值备注：{remark}

📊 账户信息：
新余额：{response_data.get('new_gold_pay', '未知'):,}
累计充值：{response_data.get('new_gold_pay_total', '未知'):,}
剩余积分：{user_points['points']} 积分

⏰ 充值时间：{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"""
                
                yield event.plain_result(content)
            else:
                error_msg = result.get("error", "未知错误")
                yield event.plain_result(f"❌ 充值失败\n错误信息：{error_msg}")
                
        except Exception as e:
            logger.error(f"充值异常：{e}")
            yield event.plain_result("❌ 充值异常，请稍后重试或联系管理员")
    
    # ========== 给别人账号充值功能 ==========
    @filter.command("给别人充值")
    async def recharge_for_others_cmd(self, event: AstrMessageEvent):
        """给他人游戏账号充值（消耗自己的积分）"""
        if not self.features.get("allow_recharge_others", True):
            yield event.plain_result("❌ 功能已禁用\n此功能已被管理员禁用")
            return
        
        parts = event.message_str.strip().split()
        if len(parts) < 3:
            yield event.plain_result("❌ 格式错误\n正确格式：/给别人充值 <QQ> <积分数量> [备注]\n例如：/给别人充值 123456 10 赠送")
            return
        
        try:
            target_qq = parts[1]
            points_to_use = int(parts[2])
            if points_to_use <= 0:
                raise ValueError("必须是正数")
            remark = " ".join(parts[3:]) if len(parts) > 3 else "好友赠送"
        except ValueError:
            yield event.plain_result("❌ 参数错误，积分数量必须是正整数")
            return
        
        from_qq = self._get_user_id(event)
        
        if from_qq == "unknown":
            yield event.plain_result("❌ 身份验证失败，无法获取QQ信息")
            return
        
        # 检查目标QQ是否绑定
        if target_qq not in self.bindings:
            yield event.plain_result(f"❌ 目标用户未绑定账号\nQQ {target_qq} 未绑定游戏账号")
            return
        
        # 检查自己是否有足够积分
        from_points = self._get_user_points(from_qq)
        if from_points["points"] < points_to_use:
            yield event.plain_result(f"❌ 积分不足\n需要积分：{points_to_use}\n当前积分：{from_points['points']}")
            return
        
        # 获取目标用户的游戏账号
        game_account = self.bindings[target_qq]["game_account"]
        account_name = self.bindings[target_qq].get("account_name", game_account)
        
        # 计算充值金额
        recharge_ratio = self.system_config["recharge_ratio"]
        recharge_amount = points_to_use * recharge_ratio
        
        # 执行充值
        try:
            result = await self._execute_account_recharge(game_account, recharge_amount, remark)
            
            if result.get("success"):
                # 扣减自己的积分
                from_points["points"] -= points_to_use
                from_points["total_spent"] += points_to_use
                self._update_user_points(from_qq, from_points)
                
                # 记录充值日志
                recharge_id = f"G{datetime.now().strftime('%Y%m%d%H%M%S')}_{from_qq}"
                self.recharge_logs[recharge_id] = {
                    "type": "gift_recharge",
                    "from_qq": from_qq,
                    "to_qq": target_qq,
                    "game_account": game_account,
                    "account_name": account_name,
                    "points_used": points_to_use,
                    "recharge_amount": recharge_amount,
                    "remark": remark,
                    "recharge_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                    "api_response": result
                }
                self._save_json(self.recharge_file, self.recharge_logs)
                
                response_data = result.get("data", {})
                
                content = f"""🎁 赠送充值成功！

赠送对象：QQ {target_qq}
游戏账号：{account_name}
消耗积分：{points_to_use} 积分（您的积分）
充值金额：{recharge_amount:,} 元宝
充值备注：{remark}

📊 对方账户：
新余额：{response_data.get('new_gold_pay', '未知'):,}
累计充值：{response_data.get('new_gold_pay_total', '未知'):,}
您剩余积分：{from_points['points']} 积分

⏰ 赠送时间：{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"""
                
                yield event.plain_result(content)
            else:
                error_msg = result.get("error", "未知错误")
                yield event.plain_result(f"❌ 充值失败\n错误信息：{error_msg}")
                
        except Exception as e:
            logger.error(f"给他人充值异常：{e}")
            yield event.plain_result("❌ 充值异常，请稍后重试或联系管理员")
    
    # ========== 查询账号功能 ==========
    @filter.command("查询账号")
    async def query_account_cmd(self, event: AstrMessageEvent):
        """查询游戏账号信息"""
        parts = event.message_str.strip().split()
        
        if len(parts) >= 2:
            # 查询指定账号
            game_account = parts[1]
            show_extra_info = True
        else:
            # 查询自己绑定的账号
            qq_id = self._get_user_id(event)
            if qq_id not in self.bindings:
                yield event.plain_result("❌ 未绑定账号\n请先绑定游戏账号或指定要查询的账号\n\n💡 使用方法：\n1. /查询账号 xhl2511686\n2. 或先绑定账号再查询")
                return
            game_account = self.bindings[qq_id]["game_account"]
            show_extra_info = False
        
        try:
            account_info = await self._get_account_info(game_account)
            if not account_info:
                yield event.plain_result(f"❌ 账号不存在\n游戏账号 {game_account} 不存在")
                return
        except Exception as e:
            logger.error(f"查询账号失败：{e}")
            yield event.plain_result("❌ 查询失败，网络连接异常，请稍后重试")
            return
        
        # 构建基本信息
        content = f"""🎮 账号信息

游戏账号：{account_info.get('passport', '未知')}
当前余额：{account_info.get('gold_pay', 0):,} 元宝
累计充值：{account_info.get('gold_pay_total', 0):,} 元宝"""
        
        # 添加额外信息
        if show_extra_info and account_info.get('name'):
            content += f"\n角色名称：{account_info['name']}"
        if show_extra_info and account_info.get('cid'):
            content += f"\n角色ID：{account_info['cid']}"
        
        # 检查此账号是否被绑定
        is_bound, bound_qq, bind_info = self._is_account_already_bound(game_account)
        content += f"\n\n绑定状态：{'已绑定' if is_bound else '未绑定'}"
        
        if is_bound:
            content += f"\n绑定QQ：{bound_qq}\n绑定时间：{bind_info.get('bind_time', '未知')}"
        else:
            content += f"\n💡 使用 /绑定账号 可绑定此账号"
        
        yield event.plain_result(content)
    
    # ========== 赠送积分功能 ==========
    @filter.command("赠送积分")
    async def gift_points_cmd(self, event: AstrMessageEvent):
        """赠送积分给其他用户"""
        if not self.features.get("allow_gift_points", True):
            yield event.plain_result("❌ 功能已禁用\n此功能已被管理员禁用")
            return
        
        parts = event.message_str.strip().split()
        if len(parts) < 3:
            yield event.plain_result("❌ 格式错误\n正确格式：/赠送积分 <QQ> <积分数量> [备注]\n例如：/赠送积分 123456 50 节日礼物")
            return
        
        try:
            target_qq = parts[1]
            points_to_gift = int(parts[2])
            if points_to_gift <= 0:
                raise ValueError("必须是正数")
            remark = " ".join(parts[3:]) if len(parts) > 3 else "好友赠送"
        except ValueError:
            yield event.plain_result("❌ 参数错误，积分数量必须是正整数")
            return
        
        from_qq = self._get_user_id(event)
        
        if from_qq == "unknown":
            yield event.plain_result("❌ 身份验证失败，无法获取QQ信息")
            return
        
        if from_qq == target_qq:
            yield event.plain_result("❌ 不能给自己赠送积分")
            return
        
        # 检查是否有足够积分
        from_points = self._get_user_points(from_qq)
        if from_points["points"] < points_to_gift:
            yield event.plain_result(f"❌ 积分不足\n需要积分：{points_to_gift}\n当前积分：{from_points['points']}")
            return
        
        # 转移积分
        success, message = self._transfer_points(from_qq, target_qq, points_to_gift, remark)
        
        if success:
            # 获取转移后的积分
            from_points = self._get_user_points(from_qq)
            to_points = self._get_user_points(target_qq)
            
            content = f"""🎁 积分赠送成功！

赠送对象：QQ {target_qq}
赠送积分：{points_to_gift} 积分
赠送备注：{remark}

📊 积分变化：
您的积分：{from_points['points']}（已扣减 {points_to_gift}）
对方积分：{to_points['points']}（已增加 {points_to_gift}）

⏰ 赠送时间：{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"""
            
            yield event.plain_result(content)
        else:
            yield event.plain_result(f"❌ 赠送失败\n{message}")
    
    # ========== 查询他人积分功能 ==========
    @filter.command("查询积分")
    async def query_points_cmd(self, event: AstrMessageEvent):
        """查询其他用户的积分"""
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result("❌ 格式错误\n正确格式：/查询积分 <QQ>\n例如：/查询积分 123456")
            return
        
        target_qq = parts[1]
        current_qq = self._get_user_id(event)
        is_admin = self._is_admin(current_qq)
        
        # 管理员可以查询任何人，普通用户只能查询自己
        if not is_admin and current_qq != target_qq:
            yield event.plain_result("❌ 权限不足\n只有管理员可以查询其他用户的积分")
            return
        
        if target_qq not in self.user_points:
            yield event.plain_result(f"❌ 用户不存在\nQQ {target_qq} 暂无积分记录")
            return
        
        user_points = self._get_user_points(target_qq)
        recharge_ratio = self.system_config["recharge_ratio"]
        
        content = f"""💰 用户积分查询

用户QQ：{target_qq}
当前积分：{user_points['points']} 积分
累计获得：{user_points['total_earned']} 积分
累计消耗：{user_points['total_spent']} 积分
连续签到：{user_points['continuous_days']} 天

💎 积分价值：
• 可兑换：{user_points['points'] * recharge_ratio:,} 元宝"""
        
        if user_points["first_sign_date"]:
            content += f"\n首次签到：{user_points['first_sign_date']}"
        if user_points["last_sign_date"]:
            content += f"\n最后签到：{user_points['last_sign_date']}"
        
        # 显示绑定状态
        if target_qq in self.bindings:
            account = self.bindings[target_qq]["game_account"]
            content += f"\n\n🎮 绑定账号：{account}"
        
        yield event.plain_result(content)
    
    # ========== 管理员添加积分功能 ==========
    @filter.command("添加积分")
    async def add_points_cmd(self, event: AstrMessageEvent):
        """管理员给用户添加积分"""
        parts = event.message_str.strip().split()
        if len(parts) < 3:
            yield event.plain_result("❌ 格式错误\n正确格式：/添加积分 <QQ> <积分数量> [备注]\n例如：/添加积分 123456 100 活动奖励")
            return
        
        try:
            target_qq = parts[1]
            points_to_add = int(parts[2])
            if points_to_add <= 0:
                raise ValueError("必须是正数")
            remark = " ".join(parts[3:]) if len(parts) > 3 else "管理员添加"
        except ValueError:
            yield event.plain_result("❌ 参数错误，积分数量必须是正整数")
            return
        
        admin_qq = self._get_user_id(event)
        
        if not self._is_admin(admin_qq):
            yield event.plain_result("❌ 权限不足\n只有管理员可以使用此命令")
            return
        
        # 检查每日积分上限
        max_per_day = self.system_config.get("max_points_per_day", 1000)
        if points_to_add > max_per_day:
            yield event.plain_result(f"❌ 超出每日上限\n单次最多添加 {max_per_day} 积分")
            return
        
        # 添加积分
        user_points = self._get_user_points(target_qq)
        user_points["points"] += points_to_add
        user_points["total_earned"] += points_to_add
        self._update_user_points(target_qq, user_points)
        
        # 记录管理员操作
        admin_action_id = f"A{datetime.now().strftime('%Y%m%d%H%M%S')}_{target_qq}"
        self.recharge_logs[admin_action_id] = {
            "type": "admin_add_points",
            "target_qq": target_qq,
            "points": points_to_add,
            "reason": remark,
            "admin_qq": admin_qq,
            "action_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        }
        self._save_json(self.recharge_file, self.recharge_logs)
        
        recharge_ratio = self.system_config["recharge_ratio"]
        
        content = f"""👑 管理员操作成功！

目标用户：QQ {target_qq}
添加积分：{points_to_add} 积分
操作备注：{remark}

📊 用户当前状态：
总积分：{user_points['points']} 积分
累计获得：{user_points['total_earned']} 积分
可兑换：{user_points['points'] * recharge_ratio:,} 元宝

操作管理员：{admin_qq}
操作时间：{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"""
        
        yield event.plain_result(content)
    
    # ========== 管理员管理功能 ==========
    @filter.command("添加管理员")
    async def add_admin_cmd(self, event: AstrMessageEvent):
        """添加管理员"""
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result("❌ 格式错误\n正确格式：/添加管理员 <QQ>\n例如：/添加管理员 123456")
            return
        
        target_qq = parts[1]
        admin_qq = self._get_user_id(event)
        
        if not self._is_admin(admin_qq):
            yield event.plain_result("❌ 权限不足\n只有管理员可以使用此命令")
            return
        
        success = self._add_admin(target_qq)
        
        if success:
            content = f"""✅ 管理员添加成功！

新管理员：QQ {target_qq}
操作管理员：{admin_qq}
操作时间：{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}

当前管理员数量：{len(self.admins)} 人"""
            
            yield event.plain_result(content)
        else:
            yield event.plain_result(f"⚠️ 操作提示\nQQ {target_qq} 已经是管理员")
    
    @filter.command("移除管理员")
    async def remove_admin_cmd(self, event: AstrMessageEvent):
        """移除管理员"""
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result("❌ 格式错误\n正确格式：/移除管理员 <QQ>\n例如：/移除管理员 123456")
            return
        
        target_qq = parts[1]
        admin_qq = self._get_user_id(event)
        
        if not self._is_admin(admin_qq):
            yield event.plain_result("❌ 权限不足\n只有管理员可以使用此命令")
            return
        
        success = self._remove_admin(target_qq)
        
        if success:
            content = f"""✅ 管理员移除成功！

移除的管理员：QQ {target_qq}
操作管理员：{admin_qq}
操作时间：{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}

当前管理员数量：{len(self.admins)} 人"""
            
            yield event.plain_result(content)
        else:
            yield event.plain_result(f"⚠️ 操作提示\nQQ {target_qq} 不是管理员")
    
    @filter.command("管理员列表")
    async def admin_list_cmd(self, event: AstrMessageEvent):
        """查看管理员列表"""
        admin_qq = self._get_user_id(event)
        
        if not self._is_admin(admin_qq):
            yield event.plain_result("❌ 权限不足\n只有管理员可以查看此列表")
            return
        
        if not self.admins:
            content = "当前没有管理员"
        else:
            content = f"""👑 管理员列表

管理员数量：{len(self.admins)} 人

管理员QQ列表："""
            for i, qq in enumerate(self.admins, 1):
                content += f"\n{i}. {qq}"
        
        content += f"\n\n当前操作用户：{admin_qq}"
        
        yield event.plain_result(content)
    
    @filter.command("用户列表")
    async def user_list_cmd(self, event: AstrMessageEvent):
        """查看所有用户列表"""
        admin_qq = self._get_user_id(event)
        
        if not self._is_admin(admin_qq):
            yield event.plain_result("❌ 权限不足\n只有管理员可以查看用户列表")
            return
        
        parts = event.message_str.strip().split()
        page = 1
        if len(parts) >= 2:
            try:
                page = int(parts[1])
                if page < 1:
                    page = 1
            except ValueError:
                page = 1
        
        page_size = 10
        start_index = (page - 1) * page_size
        end_index = start_index + page_size
        
        # 获取用户列表
        all_users = list(self.user_points.keys())
        total_users = len(all_users)
        total_pages = (total_users + page_size - 1) // page_size
        
        if page > total_pages and total_pages > 0:
            page = total_pages
            start_index = (page - 1) * page_size
            end_index = start_index + page_size
        
        current_users = all_users[start_index:end_index]
        
        content = f"""👥 用户列表

总用户数：{total_users} 人
当前页数：{page}/{total_pages}
每页显示：{page_size} 人"""

        if not current_users:
            content += "\n\n当前页无用户数据"
        else:
            content += f"\n\n用户列表（{start_index + 1}-{min(end_index, total_users)}）："
            
            for i, qq in enumerate(current_users, start_index + 1):
                user_data = self.user_points[qq]
                is_bound = qq in self.bindings
                account_info = f"已绑定" if is_bound else "未绑定"
                
                content += f"\n\n{i}. QQ: {qq}"
                content += f"\n   积分: {user_data['points']}"
                content += f"\n   累计获得: {user_data['total_earned']}"
                content += f"\n   连续签到: {user_data['continuous_days']}天"
                content += f"\n   账号状态: {account_info}"
                
                if is_bound:
                    game_account = self.bindings[qq]["game_account"]
                    content += f" ({game_account})"
        
        if total_pages > 1:
            content += f"\n\n📄 翻页：/用户列表 <页码>"
        
        yield event.plain_result(content)
    
    @filter.command("充值记录")
    async def recharge_logs_cmd(self, event: AstrMessageEvent):
        """查看充值记录"""
        admin_qq = self._get_user_id(event)
        
        if not self._is_admin(admin_qq):
            yield event.plain_result("❌ 权限不足\n只有管理员可以查看充值记录")
            return
        
        parts = event.message_str.strip().split()
        limit = 10
        if len(parts) >= 2:
            try:
                limit = int(parts[1])
                if limit < 1:
                    limit = 10
                elif limit > 50:
                    limit = 50
            except ValueError:
                limit = 10
        
        # 获取最近的充值记录
        recharge_ids = list(self.recharge_logs.keys())
        recharge_ids.sort(reverse=True)
        recent_logs = recharge_ids[:limit]
        
        content = f"""📋 充值记录

显示最近 {limit} 条记录：
--------------------------"""
        
        if not recent_logs:
            content += "\n暂无充值记录"
        else:
            for i, log_id in enumerate(recent_logs, 1):
                log = self.recharge_logs[log_id]
                log_type = log.get("type", "normal")
                
                if log_type == "normal":
                    content += f"\n\n{i}. 普通充值"
                    content += f"\n   用户: {log.get('qq_id', '未知')}"
                    content += f"\n   账号: {log.get('account_name', '未知')}"
                    content += f"\n   积分: {log.get('points_used', 0)}"
                elif log_type == "gift_recharge":
                    content += f"\n\n{i}. 赠送充值"
                    content += f"\n   赠送者: {log.get('from_qq', '未知')}"
                    content += f"\n   接收者: {log.get('to_qq', '未知')}"
                    content += f"\n   积分: {log.get('points_used', 0)}"
                elif log_type == "points_transfer":
                    content += f"\n\n{i}. 积分转移"
                    content += f"\n   转出: {log.get('from_qq', '未知')}"
                    content += f"\n   转入: {log.get('to_qq', '未知')}"
                    content += f"\n   积分: {log.get('points', 0)}"
                elif log_type == "admin_add_points":
                    content += f"\n\n{i}. 管理员添加"
                    content += f"\n   目标用户: {log.get('target_qq', '未知')}"
                    content += f"\n   积分: {log.get('points', 0)}"
                
                content += f"\n   时间: {log.get('recharge_time', log.get('transfer_time', log.get('action_time', '未知')))}"
                if log.get("remark"):
                    content += f"\n   备注: {log['remark']}"
        
        yield event.plain_result(content)
    
    # ========== 修改绑定功能 ==========
    @filter.command("修改绑定")
    async def modify_bind_cmd(self, event: AstrMessageEvent):
        """修改绑定账号"""
        if not self.features.get("allow_modify_bind", True):
            yield event.plain_result("❌ 功能已禁用\n此功能已被管理员禁用")
            return
        
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result("❌ 格式错误\n正确格式：/修改绑定 新游戏账号\n例如：/修改绑定 new_account")
            return
        
        new_account = parts[1]
        qq_id = self._get_user_id(event)
        
        if qq_id == "unknown":
            yield event.plain_result("❌ 身份验证失败，无法获取QQ信息")
            return
        
        # 检查是否已绑定
        if qq_id not in self.bindings:
            yield event.plain_result("❌ 未绑定账号\n您尚未绑定任何游戏账号\n请先使用 /绑定账号 命令")
            return
        
        # 获取旧账号信息
        old_account = self.bindings[qq_id]["game_account"]
        old_bind_time = self.bindings[qq_id]["bind_time"]
        
        # 检查新账号是否已被绑定（排除自己）
        is_bound, bound_qq, bind_info = self._is_account_already_bound(new_account, exclude_qq=qq_id)
        if is_bound:
            yield event.plain_result(f"❌ 账号已被绑定\n游戏账号：{new_account}\n已被QQ：{bound_qq} 绑定\n绑定时间：{bind_info.get('bind_time', '未知')}")
            return
        
        # 验证新账号是否存在
        try:
            account_info = await self._get_account_info(new_account)
            if not account_info:
                yield event.plain_result(f"❌ 账号不存在\n游戏账号 {new_account} 不存在")
                return
        except Exception as e:
            logger.error(f"验证游戏账号失败: {e}")
            yield event.plain_result("❌ 验证失败，网络连接异常，请稍后重试")
            return
        
        # 更新绑定信息
        self.bindings[qq_id] = {
            "game_account": new_account,
            "account_name": account_info.get("passport", new_account),
            "bind_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "qq_id": qq_id,
            "old_account": old_account,
            "old_bind_time": old_bind_time
        }
        self._save_json(self.bind_file, self.bindings)
        
        account_name = account_info.get("passport", new_account)
        content = f"""✅ 修改成功！

原账号：{old_account}
新账号：{account_name}
当前余额：{account_info.get('gold_pay', 0):,} 元宝
原绑定：{old_bind_time}
新绑定：{self.bindings[qq_id]['bind_time']}"""
        
        yield event.plain_result(content)
    
    # ========== 解绑功能 ==========
    @filter.command("解绑账号")
    async def unbind_account_cmd(self, event: AstrMessageEvent):
        """解绑游戏账号"""
        qq_id = self._get_user_id(event)
        
        if qq_id in self.bindings:
            game_account = self.bindings[qq_id]["game_account"]
            account_name = self.bindings[qq_id].get("account_name", game_account)
            bind_time = self.bindings[qq_id].get("bind_time", "未知时间")
            
            # 删除绑定
            del self.bindings[qq_id]
            self._save_json(self.bind_file, self.bindings)
            
            content = f"""✅ 解绑成功！

已解绑账号：{account_name}
原绑定时间：{bind_time}
解绑时间：{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}

💡 如需重新绑定，请使用 /绑定账号 命令"""
            
            yield event.plain_result(content)
        else:
            yield event.plain_result("⚠️ 未绑定账号\n您未绑定任何游戏账号")
    
    # ========== 插件配置查看功能 ==========
    @filter.command("插件配置")
    async def show_config_cmd(self, event: AstrMessageEvent):
        """查看插件当前配置"""
        admin_qq = self._get_user_id(event)
        
        if not self._is_admin(admin_qq):
            yield event.plain_result("❌ 权限不足\n只有管理员可以查看插件配置")
            return
        
        content = f"""⚙️ 插件当前配置

👑 管理员列表：
{', '.join(map(str, self.admins)) if self.admins else '无'}

🌐 API配置：
• 地址：{self.api_config['base_url']}
• 超时：{self.api_config['timeout']}秒
• 密钥：{'已配置' if self.api_config['qq_bot_secret'] else '未配置'}

💰 积分系统：
• 兑换比例：1积分 = {self.system_config['recharge_ratio']:,}元宝
• 每日上限：{self.system_config.get('max_points_per_day', 1000)}积分
• 最少充值：{self.system_config.get('min_recharge_points', 1)}积分

✅ 功能开关："""
        
        for feature, enabled in self.features.items():
            status = "✅ 开启" if enabled else "❌ 关闭"
            feature_name = {
                "allow_modify_bind": "修改绑定",
                "allow_gift_points": "赠送积分",
                "allow_recharge_others": "给别人充值",
                "allow_points_transfer": "积分转移"
            }.get(feature, feature)
            content += f"\n• {feature_name}: {status}"
        
        content += f"""

📊 签到奖励规则：
连续签到天数 -> 奖励积分"""
        
        for day, reward in sorted(self.system_config["sign_rewards"].items()):
            content += f"\n• 第{day}天: {reward}积分"
        
        content += f"""

📈 数据统计：
• 已绑定用户：{len(self.bindings)} 人
• 积分用户：{len(self.user_points)} 人
• 充值记录：{len(self.recharge_logs)} 条

查看时间：{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"""
        
        yield event.plain_result(content)
    
    # ========== 测试连接功能 ==========
    @filter.command("测试连接")
    async def test_connection_cmd(self, event: AstrMessageEvent):
        """测试API连接"""
        try:
            async with aiohttp.ClientSession() as session:
                params = {
                    "action": "search",
                    "page": 1,
                    "pageSize": 1
                }
                
                async with session.get(
                    self.api_config["base_url"],
                    params=params,
                    timeout=aiohttp.ClientTimeout(total=self.api_config["timeout"])
                ) as response:
                    if response.status == 200:
                        result = await response.json()
                        if result.get("success"):
                            content = f"""✅ API连接正常！

连接状态：正常
账号数量：{result['data']['total']:,} 个
响应时间：正常
服务状态：在线"""
                            yield event.plain_result(content)
                        else:
                            error_msg = result.get('error', '未知错误')
                            yield event.plain_result(f"⚠️ API异常\nAPI响应异常：{error_msg}")
                    else:
                        yield event.plain_result(f"❌ 连接失败\nAPI连接失败，状态码：{response.status}")
                        
        except Exception as e:
            yield event.plain_result(f"❌ 连接失败\nAPI连接失败：{str(e)}\n请检查API地址和网络配置")
    
    # ========== API调用方法 ==========
    async def _get_account_info(self, passport: str) -> Optional[dict]:
        """调用API查询账号信息"""
        try:
            async with aiohttp.ClientSession() as session:
                params = {
                    "action": "search",
                    "passport": passport,
                    "page": 1,
                    "pageSize": 1
                }
                
                async with session.get(
                    self.api_config["base_url"],
                    params=params,
                    timeout=aiohttp.ClientTimeout(total=self.api_config["timeout"])
                ) as response:
                    if response.status == 200:
                        result = await response.json()
                        if result.get("success") and result['data']['total'] > 0:
                            player = result['data']['players'][0]
                            return {
                                "passport": player.get('passport'),
                                "gold_pay": player.get('cash_gold', 0),
                                "gold_pay_total": player.get('total_recharge', 0),
                                "cid": player.get('cid'),
                                "name": player.get('name')
                            }
                    else:
                        logger.error(f"API请求失败，状态码：{response.status}")
        except Exception as e:
            logger.error(f"查询账号异常：{e}")
        
        return None
    
    async def _execute_account_recharge(self, passport: str, amount: float, remark: str) -> dict:
        """调用API为账号执行充值"""
        try:
            async with aiohttp.ClientSession() as session:
                form_data = aiohttp.FormData()
                form_data.add_field("action", "recharge")
                form_data.add_field("passport", passport)
                form_data.add_field("amount", str(amount))
                form_data.add_field("remark", remark)
                form_data.add_field("source", "qq_bot")
                form_data.add_field("secret", self.api_config["qq_bot_secret"])
                
                async with session.post(
                    self.api_config["base_url"],
                    data=form_data,
                    timeout=aiohttp.ClientTimeout(total=self.api_config["timeout"])
                ) as response:
                    if response.status == 200:
                        result = await response.json()
                        return result
                    else:
                        logger.error(f"充值API请求失败，状态码：{response.status}")
                        return {"success": False, "error": f"API请求失败：{response.status}"}
                    
        except asyncio.TimeoutError:
            logger.error("充值请求超时")
            return {"success": False, "error": "请求超时，请稍后重试"}
        except Exception as e:
            logger.error(f"充值请求异常：{e}")
            return {"success": False, "error": f"请求异常：{str(e)}"}
    
    async def terminate(self):
        logger.info("游戏账号绑定与充值插件已禁用")
