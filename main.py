import json
import os
import aiohttp
import asyncio
import random
from datetime import datetime, date, timedelta
from typing import Optional, Dict, List
from astrbot.api.event import filter, AstrMessageEvent
from astrbot.api.star import Context, Star, register
from astrbot.api import logger

@register("game_bind", "aa932406", "游戏账号绑定与充值插件", "3.0.0")
class GameBindPlugin(Star):
    def __init__(self, context: Context):
        super().__init__(context)
        # 初始化数据存储
        self.data_dir = os.path.join(os.path.dirname(__file__), "data")
        os.makedirs(self.data_dir, exist_ok=True)
        
        # 数据文件
        self.bind_file = os.path.join(self.data_dir, "bindings.json")
        self.points_file = os.path.join(self.data_dir, "user_points.json")
        self.sign_file = os.path.join(self.data_dir, "sign_records.json")
        self.recharge_logs_file = os.path.join(self.data_dir, "recharge_logs.json")
        self.admin_logs_file = os.path.join(self.data_dir, "admin_logs.json")
        
        # 加载数据
        self.bindings = self._load_json(self.bind_file)
        self.user_points = self._load_json(self.points_file)
        self.sign_records = self._load_json(self.sign_file)
        self.recharge_logs = self._load_json(self.recharge_logs_file)
        self.admin_logs = self._load_json(self.admin_logs_file)
        
        # API配置
        self.api_config = {
            "base_url": "http://115.190.64.181:881/api/players.php",
            "timeout": 30,
            "qq_bot_secret": "ws7ecejjsznhtxurchknmdemax2fnp5d"
        }
        
        # 系统配置
        self.system_config = {
            "points": {
                "recharge_ratio": 10000,  # 1积分=10000元宝
                "sign_rewards": {
                    1: 1, 2: 2, 3: 3, 4: 4, 5: 5, 6: 6,
                    7: 10, 14: 15, 30: 30
                }
            }
        }
        
        logger.info("✨ 游戏账号插件初始化完成！")
    
    def _load_json(self, file_path: str) -> dict:
        """加载JSON文件"""
        try:
            if os.path.exists(file_path):
                with open(file_path, 'r', encoding='utf-8') as f:
                    return json.load(f)
        except Exception as e:
            logger.error(f"加载文件失败 {file_path}: {e}")
        return {}
    
    def _save_json(self, file_path: str, data: dict):
        """保存JSON文件"""
        try:
            with open(file_path, 'w', encoding='utf-8') as f:
                json.dump(data, f, ensure_ascii=False, indent=2)
        except Exception as e:
            logger.error(f"保存文件失败 {file_path}: {e}")
    
    def _get_user_id(self, event: AstrMessageEvent) -> str:
        """获取用户ID"""
        try:
            sender_id = event.get_sender_id()
            if sender_id:
                return str(sender_id)
        except Exception as e:
            logger.error(f"获取用户ID异常: {e}")
        return "unknown"
    
    def _is_admin(self, event: AstrMessageEvent) -> bool:
        """检查是否为管理员"""
        try:
            # 这里可以根据你的配置检查管理员
            # 简单示例：检查发送者ID是否在管理员列表中
            qq_id = self._get_user_id(event)
            # 你可以在这里添加管理员检查逻辑
            # 例如：return qq_id in ["管理员QQ1", "管理员QQ2"]
            return False  # 默认返回False，需要你根据实际情况修改
        except:
            return False
    
    async def initialize(self):
        logger.info("🚀 游戏账号插件已启动！")
    
    # ========== 使用filter.message()监听所有消息 ==========
    @filter.message()
    async def handle_all_messages(self, event: AstrMessageEvent):
        """处理所有消息，关键词触发"""
        msg = event.message_str.strip()
        
        # 如果消息是空的，直接返回
        if not msg:
            return
        
        # 去掉可能的@机器人部分
        if "@" in msg:
            parts = msg.split("@")
            if len(parts) > 1:
                msg = parts[-1].strip()
        
        # 关键词映射
        keyword_handlers = {
            "帮助": self._help_handler,
            "绑定账号": self._bind_handler,
            "我的积分": self._points_handler,
            "签到": self._sign_handler,
            "积分充值": self._recharge_handler,
            "查询账号": self._query_handler,
            "修改绑定": self._modify_handler,
            "解绑账号": self._unbind_handler,
            "测试连接": self._test_handler,
            "给别人充值": self._recharge_others_handler,
            "管理员充值": self._admin_recharge_handler
        }
        
        # 检查消息是否以关键词开头
        for keyword, handler in keyword_handlers.items():
            if msg.startswith(keyword):
                await handler(event, msg)
                break
    
    # ========== 处理函数 ==========
    async def _help_handler(self, event: AstrMessageEvent, msg: str):
        """处理帮助"""
        help_text = """🎮 游戏插件关键词：
• 帮助 - 显示本帮助
• 绑定账号 游戏账号 - 绑定游戏账号
• 我的积分 - 查看积分余额
• 签到 - 每日签到得积分
• 积分充值 数量 - 用积分充值游戏
• 给别人充值 QQ号 积分数量 - 给他人充值游戏（需对方已绑定）
• 查询账号 [账号] - 查询账号信息
• 修改绑定 新账号 - 修改绑定账号
• 解绑账号 - 解绑当前账号
• 测试连接 - 测试API连接

👑 管理员功能：
• 管理员充值 QQ号 积分数量 [备注] - 给他人添加积分

💎 规则：
• 1积分 = 10000元宝
• 签到获得积分
• 积分用于充值游戏账号
• 没有积分无法充值"""
        yield event.plain_result(help_text)
    
    async def _bind_handler(self, event: AstrMessageEvent, msg: str):
        """处理绑定账号"""
        parts = msg.split()
        if len(parts) < 2:
            yield event.plain_result("格式：绑定账号 游戏账号\n例如：绑定账号 xhl2511686")
            return
        
        qq_id = self._get_user_id(event)
        if qq_id == "unknown":
            yield event.plain_result("无法获取用户ID")
            return
        
        game_account = parts[1]
        
        # 检查是否已绑定
        if qq_id in self.bindings:
            old_account = self.bindings[qq_id]["game_account"]
            yield event.plain_result(f"你已绑定账号：{old_account}\n如需修改请使用：修改绑定 新账号")
            return
        
        # 检查账号是否已被其他人绑定
        for uid, bind_info in self.bindings.items():
            if bind_info.get("game_account") == game_account:
                yield event.plain_result(f"账号 {game_account} 已被其他人绑定")
                return
        
        # 验证账号是否存在
        try:
            account_info = await self._get_account_info(game_account)
            if not account_info:
                yield event.plain_result(f"账号 {game_account} 不存在")
                return
        except Exception as e:
            logger.error(f"验证账号失败: {e}")
            yield event.plain_result("网络异常，请稍后重试")
            return
        
        # 保存绑定
        self.bindings[qq_id] = {
            "game_account": game_account,
            "account_name": account_info.get("passport", game_account),
            "bind_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "qq_id": qq_id
        }
        self._save_json(self.bind_file, self.bindings)
        
        # 初始化用户积分
        if qq_id not in self.user_points:
            self.user_points[qq_id] = {
                "points": 0,
                "total_earned": 0,
                "total_spent": 0,
                "last_sign": None,
                "continuous_days": 0
            }
            self._save_json(self.points_file, self.user_points)
        
        content = f"""✅ 绑定成功！

游戏账号：{account_info.get('passport', game_account)}
当前余额：{account_info.get('gold_pay', 0):,} 元宝
累计充值：{account_info.get('gold_pay_total', 0):,} 元宝
绑定时间：{self.bindings[qq_id]['bind_time']}"""
        
        yield event.plain_result(content)
    
    async def _points_handler(self, event: AstrMessageEvent, msg: str):
        """处理我的积分"""
        qq_id = self._get_user_id(event)
        if qq_id == "unknown":
            yield event.plain_result("无法获取用户ID")
            return
        
        # 获取用户积分
        if qq_id not in self.user_points:
            self.user_points[qq_id] = {
                "points": 0,
                "total_earned": 0,
                "total_spent": 0,
                "last_sign": None,
                "continuous_days": 0
            }
        
        user_points = self.user_points[qq_id]
        points = user_points.get("points", 0)
        
        # 检查绑定状态
        bind_status = ""
        if qq_id in self.bindings:
            account_name = self.bindings[qq_id].get("account_name", "未知")
            bind_status = f"绑定账号：{account_name}"
        else:
            bind_status = "未绑定账号"
        
        # 计算可充值金额
        recharge_amount = points * self.system_config["points"]["recharge_ratio"]
        
        content = f"""💰 我的积分

当前积分：{points}
累计获得：{user_points.get('total_earned', 0)}
累计消耗：{user_points.get('total_spent', 0)}
连续签到：{user_points.get('continuous_days', 0)} 天
{bind_status}

💎 积分用途：
• 1积分 = {self.system_config["points"]["recharge_ratio"]:,}元宝
• 可兑换：{recharge_amount:,}元宝
• 使用"积分充值"命令兑换"""
        
        if user_points.get("last_sign"):
            content += f"\n\n📅 上次签到：{user_points['last_sign']}"
        
        yield event.plain_result(content)
    
    async def _sign_handler(self, event: AstrMessageEvent, msg: str):
        """处理签到"""
        qq_id = self._get_user_id(event)
        if qq_id == "unknown":
            yield event.plain_result("无法获取用户ID")
            return
        
        today = date.today().isoformat()
        
        # 获取用户积分
        if qq_id not in self.user_points:
            self.user_points[qq_id] = {
                "points": 0,
                "total_earned": 0,
                "total_spent": 0,
                "last_sign": None,
                "continuous_days": 0
            }
        
        user_points = self.user_points[qq_id]
        
        # 检查是否已签到
        if user_points.get("last_sign") == today:
            yield event.plain_result(f"⏳ 今日已签到\n当前积分：{user_points['points']}\n下次签到：明天")
            return
        
        # 计算连续天数
        last_sign_date = user_points.get("last_sign")
        if last_sign_date:
            try:
                last_date = date.fromisoformat(last_sign_date)
                days_diff = (date.today() - last_date).days
                if days_diff == 1:
                    user_points["continuous_days"] = user_points.get("continuous_days", 0) + 1
                else:
                    user_points["continuous_days"] = 1
            except:
                user_points["continuous_days"] = 1
        else:
            user_points["continuous_days"] = 1
        
        # 计算签到奖励（根据连续天数）
        continuous_days = user_points["continuous_days"]
        
        # 基础奖励
        reward = 1  # 默认1积分
        
        # 特殊天数奖励
        for day, day_reward in self.system_config["points"]["sign_rewards"].items():
            if continuous_days == day:
                reward = day_reward
                break
        else:
            # 如果不在特殊天数列表中，使用连续天数作为奖励（最高10积分）
            reward = min(continuous_days, 10)
        
        # 更新积分
        user_points["points"] = user_points.get("points", 0) + reward
        user_points["total_earned"] = user_points.get("total_earned", 0) + reward
        user_points["last_sign"] = today
        
        self.user_points[qq_id] = user_points
        self._save_json(self.points_file, self.user_points)
        
        # 保存签到记录
        self.sign_records[qq_id] = {
            "last_sign": today,
            "reward": reward,
            "continuous_days": continuous_days,
            "total_rewards": self.sign_records.get(qq_id, {}).get("total_rewards", 0) + reward
        }
        self._save_json(self.sign_file, self.sign_records)
        
        # 计算可充值金额
        recharge_amount = reward * self.system_config["points"]["recharge_ratio"]
        total_recharge_amount = user_points["points"] * self.system_config["points"]["recharge_ratio"]
        
        content = f"""✨ 签到成功！

获得积分：{reward}
连续签到：{continuous_days} 天
当前积分：{user_points['points']}
累计获得：{user_points['total_earned']}

💎 积分价值：
• 本次可兑：{recharge_amount:,} 元宝
• 总可兑换：{total_recharge_amount:,} 元宝

⏰ 签到时间：{datetime.now().strftime('%Y-%m-%d %H:%M')}"""
        
        yield event.plain_result(content)
    
    async def _recharge_handler(self, event: AstrMessageEvent, msg: str):
        """处理积分充值"""
        parts = msg.split()
        if len(parts) < 2:
            yield event.plain_result("格式：积分充值 数量 [备注]\n例如：积分充值 10 兑换元宝")
            return
        
        try:
            points = int(parts[1])
            if points <= 0:
                raise ValueError
            remark = " ".join(parts[2:]) if len(parts) > 2 else "积分兑换"
        except:
            yield event.plain_result("请输入正确的积分数量")
            return
        
        qq_id = self._get_user_id(event)
        if qq_id == "unknown":
            yield event.plain_result("无法获取用户ID")
            return
        
        # 检查是否绑定
        if qq_id not in self.bindings:
            yield event.plain_result("请先绑定游戏账号\n使用：绑定账号 游戏账号")
            return
        
        # 检查积分是否足够
        user_points = self.user_points.get(qq_id, {"points": 0})
        if user_points.get("points", 0) < points:
            yield event.plain_result(f"❌ 积分不足\n需要：{points}\n当前：{user_points.get('points', 0)}\n\n💡 获取积分：每日签到")
            return
        
        # 计算充值金额（1积分=10000元宝）
        recharge_amount = points * self.system_config["points"]["recharge_ratio"]
        game_account = self.bindings[qq_id]["game_account"]
        account_name = self.bindings[qq_id].get("account_name", game_account)
        
        # 执行充值
        try:
            result = await self._execute_account_recharge(game_account, recharge_amount, remark)
            
            if result.get("success"):
                # 扣减积分
                user_points["points"] = user_points.get("points", 0) - points
                user_points["total_spent"] = user_points.get("total_spent", 0) + points
                self.user_points[qq_id] = user_points
                self._save_json(self.points_file, self.user_points)
                
                # 记录充值日志
                log_id = f"R{datetime.now().strftime('%Y%m%d%H%M%S')}"
                self.recharge_logs[log_id] = {
                    "qq_id": qq_id,
                    "game_account": game_account,
                    "account_name": account_name,
                    "points_used": points,
                    "recharge_amount": recharge_amount,
                    "remark": remark,
                    "recharge_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                    "api_response": result
                }
                self._save_json(self.recharge_logs_file, self.recharge_logs)
                
                response_data = result.get("data", {})
                
                content = f"""✅ 充值成功！

游戏账号：{account_name}
消耗积分：{points}
充值金额：{recharge_amount:,} 元宝
充值备注：{remark}

📊 账户信息：
新余额：{response_data.get('new_gold_pay', '未知'):,}
累计充值：{response_data.get('new_gold_pay_total', '未知'):,}
剩余积分：{user_points['points']}

⏰ 充值时间：{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"""
                
                yield event.plain_result(content)
            else:
                error_msg = result.get("error", "未知错误")
                yield event.plain_result(f"❌ 充值失败\n错误信息：{error_msg}")
        except Exception as e:
            logger.error(f"充值异常：{e}")
            yield event.plain_result("❌ 充值异常，请稍后重试或联系管理员")
    
    async def _recharge_others_handler(self, event: AstrMessageEvent, msg: str):
        """处理给别人充值"""
        parts = msg.split()
        if len(parts) < 3:
            yield event.plain_result("格式：给别人充值 QQ号 积分数量\n例如：给别人充值 123456 10")
            return
        
        try:
            target_qq = parts[1]
            points = int(parts[2])
            if points <= 0:
                raise ValueError
            remark = " ".join(parts[3:]) if len(parts) > 3 else "他人代充"
        except:
            yield event.plain_result("格式错误\n正确格式：给别人充值 QQ号 积分数量")
            return
        
        # 验证目标QQ
        if not target_qq.isdigit():
            yield event.plain_result("目标QQ号必须是数字")
            return
        
        qq_id = self._get_user_id(event)
        if qq_id == "unknown":
            yield event.plain_result("无法获取用户ID")
            return
        
        # 检查自己是否有足够积分
        user_points = self.user_points.get(qq_id, {"points": 0})
        if user_points.get("points", 0) < points:
            yield event.plain_result(f"❌ 你的积分不足\n需要：{points}\n当前：{user_points.get('points', 0)}")
            return
        
        # 检查目标用户是否绑定
        if target_qq not in self.bindings:
            yield event.plain_result(f"❌ 目标用户 {target_qq} 未绑定游戏账号")
            return
        
        # 获取目标用户信息
        target_game_account = self.bindings[target_qq]["game_account"]
        target_account_name = self.bindings[target_qq].get("account_name", target_game_account)
        
        # 计算充值金额
        recharge_amount = points * self.system_config["points"]["recharge_ratio"]
        
        # 执行充值
        try:
            result = await self._execute_account_recharge(target_game_account, recharge_amount, remark)
            
            if result.get("success"):
                # 扣减自己的积分
                user_points["points"] = user_points.get("points", 0) - points
                user_points["total_spent"] = user_points.get("total_spent", 0) + points
                self.user_points[qq_id] = user_points
                
                # 记录充值日志
                log_id = f"OR{datetime.now().strftime('%Y%m%d%H%M%S')}"
                self.recharge_logs[log_id] = {
                    "from_qq": qq_id,
                    "to_qq": target_qq,
                    "game_account": target_game_account,
                    "account_name": target_account_name,
                    "points_used": points,
                    "recharge_amount": recharge_amount,
                    "remark": remark,
                    "recharge_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                    "api_response": result
                }
                self._save_json(self.recharge_logs_file, self.recharge_logs)
                
                # 保存数据
                self._save_json(self.points_file, self.user_points)
                
                response_data = result.get("data", {})
                
                content = f"""✅ 代充成功！

充值目标：QQ {target_qq}
游戏账号：{target_account_name}
消耗积分：{points}（从你的账户扣除）
充值金额：{recharge_amount:,} 元宝
充值备注：{remark}

📊 目标账户：
新余额：{response_data.get('new_gold_pay', '未知'):,}
累计充值：{response_data.get('new_gold_pay_total', '未知'):,}
你的剩余积分：{user_points['points']}

⏰ 充值时间：{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"""
                
                yield event.plain_result(content)
            else:
                error_msg = result.get("error", "未知错误")
                yield event.plain_result(f"❌ 充值失败\n错误信息：{error_msg}")
        except Exception as e:
            logger.error(f"代充异常：{e}")
            yield event.plain_result("❌ 充值异常，请稍后重试")
    
    async def _admin_recharge_handler(self, event: AstrMessageEvent, msg: str):
        """管理员给别人充值积分"""
        # 检查是否为管理员
        if not self._is_admin(event):
            yield event.plain_result("❌ 此功能仅限管理员使用")
            return
        
        parts = msg.split()
        if len(parts) < 3:
            yield event.plain_result("格式：管理员充值 QQ号 积分数量 [备注]\n例如：管理员充值 123456 100 活动奖励")
            return
        
        try:
            target_qq = parts[1]
            points = int(parts[2])
            if points <= 0:
                raise ValueError
            remark = " ".join(parts[3:]) if len(parts) > 3 else "管理员添加"
        except:
            yield event.plain_result("格式错误\n正确格式：管理员充值 QQ号 积分数量 [备注]")
            return
        
        # 验证目标QQ
        if not target_qq.isdigit():
            yield event.plain_result("目标QQ号必须是数字")
            return
        
        # 确保目标用户有积分记录
        if target_qq not in self.user_points:
            self.user_points[target_qq] = {
                "points": 0,
                "total_earned": 0,
                "total_spent": 0,
                "last_sign": None,
                "continuous_days": 0
            }
        
        # 添加积分
        target_points = self.user_points[target_qq]
        target_points["points"] = target_points.get("points", 0) + points
        target_points["total_earned"] = target_points.get("total_earned", 0) + points
        
        self.user_points[target_qq] = target_points
        self._save_json(self.points_file, self.user_points)
        
        # 记录管理员操作日志
        admin_qq = self._get_user_id(event)
        log_id = f"A{datetime.now().strftime('%Y%m%d%H%M%S')}"
        self.admin_logs[log_id] = {
            "admin_qq": admin_qq,
            "target_qq": target_qq,
            "points_added": points,
            "remark": remark,
            "operate_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        }
        self._save_json(self.admin_logs_file, self.admin_logs)
        
        # 检查目标用户是否绑定
        bind_info = ""
        if target_qq in self.bindings:
            account_name = self.bindings[target_qq].get("account_name", "未知")
            bind_info = f"\n绑定账号：{account_name}"
        
        content = f"""👑 管理员操作成功！

目标QQ：{target_qq}
添加积分：+{points}
操作备注：{remark}
{bind_info}

📊 目标用户信息：
原积分：{target_points.get("points", 0) - points}
现积分：{target_points["points"]}
累计获得：{target_points["total_earned"]}

⏰ 操作时间：{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"""
        
        yield event.plain_result(content)
    
    async def _query_handler(self, event: AstrMessageEvent, msg: str):
        """处理查询账号"""
        parts = msg.split()
        
        if len(parts) >= 2:
            # 查询指定账号
            game_account = parts[1]
            show_extra_info = True
        else:
            # 查询自己绑定的账号
            qq_id = self._get_user_id(event)
            if qq_id not in self.bindings:
                yield event.plain_result("请先绑定账号或指定要查询的账号\n格式：查询账号 游戏账号")
                return
            game_account = self.bindings[qq_id]["game_account"]
            show_extra_info = False
        
        try:
            account_info = await self._get_account_info(game_account)
            if not account_info:
                yield event.plain_result(f"账号 {game_account} 不存在")
                return
        except Exception as e:
            logger.error(f"查询账号失败：{e}")
            yield event.plain_result("网络异常，请稍后重试")
            return
        
        content = f"""📋 账号信息

游戏账号：{account_info.get('passport', '未知')}
当前余额：{account_info.get('gold_pay', 0):,} 元宝
累计充值：{account_info.get('gold_pay_total', 0):,} 元宝"""
        
        if show_extra_info and account_info.get('name'):
            content += f"\n角色名称：{account_info['name']}"
        
        # 检查此账号是否被绑定
        is_bound = False
        for uid, bind_info in self.bindings.items():
            if bind_info.get("game_account") == game_account:
                is_bound = True
                bound_qq = uid
                bind_time = bind_info.get("bind_time", "未知")
                break
        
        content += f"\n\n绑定状态：{'已绑定' if is_bound else '未绑定'}"
        
        if is_bound:
            content += f"\n绑定QQ：{bound_qq}\n绑定时间：{bind_time}"
        else:
            content += f"\n💡 使用\"绑定账号\"可绑定此账号"
        
        yield event.plain_result(content)
    
    async def _modify_handler(self, event: AstrMessageEvent, msg: str):
        """处理修改绑定"""
        parts = msg.split()
        if len(parts) < 2:
            yield event.plain_result("格式：修改绑定 新游戏账号\n例如：修改绑定 new_account")
            return
        
        new_account = parts[1]
        qq_id = self._get_user_id(event)
        
        if qq_id not in self.bindings:
            yield event.plain_result("你还没有绑定账号")
            return
        
        # 检查新账号是否已被其他人绑定
        for uid, bind_info in self.bindings.items():
            if uid != qq_id and bind_info.get("game_account") == new_account:
                yield event.plain_result(f"账号 {new_account} 已被其他人绑定")
                return
        
        # 验证新账号是否存在
        try:
            account_info = await self._get_account_info(new_account)
            if not account_info:
                yield event.plain_result(f"账号 {new_account} 不存在")
                return
        except Exception as e:
            logger.error(f"验证账号失败: {e}")
            yield event.plain_result("网络异常，请稍后重试")
            return
        
        # 更新绑定
        old_account = self.bindings[qq_id]["game_account"]
        old_bind_time = self.bindings[qq_id].get("bind_time", "未知")
        
        self.bindings[qq_id] = {
            "game_account": new_account,
            "account_name": account_info.get("passport", new_account),
            "bind_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "qq_id": qq_id,
            "old_account": old_account,
            "old_bind_time": old_bind_time
        }
        self._save_json(self.bind_file, self.bindings)
        
        content = f"""✅ 修改成功！

原账号：{old_account}
新账号：{account_info.get('passport', new_account)}
当前余额：{account_info.get('gold_pay', 0):,} 元宝
原绑定：{old_bind_time}
新绑定：{self.bindings[qq_id]['bind_time']}"""
        
        yield event.plain_result(content)
    
    async def _unbind_handler(self, event: AstrMessageEvent, msg: str):
        """处理解绑账号"""
        qq_id = self._get_user_id(event)
        
        if qq_id in self.bindings:
            account_name = self.bindings[qq_id].get("account_name", "未知")
            bind_time = self.bindings[qq_id].get("bind_time", "未知时间")
            
            # 删除绑定
            del self.bindings[qq_id]
            self._save_json(self.bind_file, self.bindings)
            
            content = f"""✅ 解绑成功！

已解绑账号：{account_name}
原绑定时间：{bind_time}
解绑时间：{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}

💡 如需重新绑定，请使用\"绑定账号\"命令"""
            
            yield event.plain_result(content)
        else:
            yield event.plain_result("⚠️ 未绑定账号\n你还没有绑定任何游戏账号")
    
    async def _test_handler(self, event: AstrMessageEvent, msg: str):
        """处理测试连接"""
        try:
            async with aiohttp.ClientSession() as session:
                params = {"action": "search", "page": 1, "pageSize": 1}
                async with session.get(self.api_config["base_url"], params=params, timeout=30) as response:
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
                    timeout=30
                ) as response:
                    if response.status == 200:
                        result = await response.json()
                        if result.get("success") and result['data']['total'] > 0:
                            player = result['data']['players'][0]
                            return {
                                "passport": player.get('passport'),
                                "gold_pay": player.get('cash_gold', 0),
                                "gold_pay_total": player.get('total_recharge', 0),
                                "name": player.get('name')
                            }
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
                    timeout=30
                ) as response:
                    if response.status == 200:
                        return await response.json()
        except Exception as e:
            logger.error(f"充值请求异常：{e}")
        return {"success": False, "error": "请求失败"}
    
    async def terminate(self):
        logger.info("游戏账号插件已禁用")
