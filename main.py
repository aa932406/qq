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
        self.recharge_file = os.path.join(self.data_dir, "recharge_logs.json")
        self.points_file = os.path.join(self.data_dir, "user_points.json")
        self.sign_file = os.path.join(self.data_dir, "sign_records.json")
        
        # 加载数据
        self.bindings = self._load_json(self.bind_file)
        self.recharge_logs = self._load_json(self.recharge_file)
        self.user_points = self._load_json(self.points_file)
        self.sign_records = self._load_json(self.sign_file)
        
        # API配置
        self.api_config = {
            "base_url": "http://115.190.64.181:881/api/players.php",
            "timeout": 30,
            "qq_bot_secret": "ws7ecejjsznhtxurchknmdemax2fnp5d"
        }
        
        # 系统配置
        self.system_config = {
            # 签到系统
            "sign_rewards": {
                1: 100,      # 第1天：100元宝
                3: 300,      # 第3天：300元宝
                7: 700,      # 第7天：700元宝
                14: 1500,    # 第14天：1500元宝
                30: 3000     # 第30天：3000元宝
            },
            # 充值赠送比例（每充值1000元宝，额外赠送多少元宝）
            "recharge_bonus": 0.10,  # 10%的额外赠送
        }
        
        logger.info("✨ 游戏账号插件初始化完成！")
    
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
    
    def _is_account_already_bound(self, game_account: str, exclude_qq: str = None) -> tuple:
        """检查游戏账号是否已被绑定"""
        for qq_id, bind_info in self.bindings.items():
            if bind_info.get("game_account") == game_account:
                if exclude_qq and qq_id == exclude_qq:
                    continue
                return True, qq_id, bind_info
        return False, None, None
    
    def _get_user_info(self, qq_id: str) -> Dict:
        """获取用户信息"""
        if qq_id not in self.user_points:
            self.user_points[qq_id] = {
                "total_sign_days": 0,      # 累计签到天数
                "continuous_days": 0,      # 连续签到天数
                "total_earned": 0,         # 累计获得元宝（签到）
                "total_recharged": 0,      # 累计充值元宝
                "first_sign_date": None,   # 首次签到日期
                "last_sign_date": None,    # 最后签到日期
                "last_recharge_date": None # 最后充值日期
            }
        return self.user_points[qq_id]
    
    def _update_user_info(self, qq_id: str, user_info: Dict):
        """更新用户信息"""
        self.user_points[qq_id] = user_info
        self._save_json(self.points_file, self.user_points)
    
    async def initialize(self):
        logger.info("🚀 游戏账号插件已启动！")
    
    # ========== 简洁的UI装饰器 ==========
    def _create_box(self, title: str, content: str) -> str:
        """创建简洁的文本框"""
        lines = content.strip().split('\n')
        max_len = max(len(line) for line in lines)
        box_width = max(max_len + 4, 40)
        
        # 构建边框
        top = f"╔{'═' * (box_width - 2)}╗\n"
        bottom = f"╚{'═' * (box_width - 2)}╝"
        
        # 标题行
        title_line = f"║ {title.center(box_width - 4)} ║\n"
        separator = f"╠{'═' * (box_width - 2)}╣\n"
        
        # 内容行
        content_lines = []
        for line in lines:
            if line.strip():
                content_lines.append(f"║ {line.ljust(box_width - 4)} ║")
            else:
                content_lines.append(f"║{' ' * (box_width - 2)}║")
        
        return top + title_line + separator + "\n".join(content_lines) + "\n" + bottom
    
    # ========== 帮助功能 ==========
    @filter.prefix("帮助")
    @filter.prefix("/帮助")
    async def help_cmd(self, event: AstrMessageEvent):
        """显示帮助信息"""
        help_text = """
🎮 游戏账号插件 - 简洁版

📋 基础命令：
1. 绑定账号 - /绑定账号 游戏账号
2. 账号充值 - /账号充值 金额 [备注]
3. 查看信息 - /我的信息
4. 每日签到 - /签到
5. 查询账号 - /查询账号 [账号]

🔧 其他功能：
• 修改绑定 - /修改绑定 新账号
• 解绑账号 - /解绑账号
• 测试连接 - /测试连接

📌 说明：
• 签到直接获得元宝奖励
• 充值金额即实际充值金额
• 连续签到奖励更多元宝
"""
        yield event.plain_result(self._create_box("🎮 游戏插件帮助", help_text))
    
    # ========== 绑定功能 ==========
    @filter.prefix("绑定账号")
    @filter.prefix("/绑定账号")
    async def bind_account_cmd(self, event: AstrMessageEvent):
        """绑定游戏账号"""
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result(self._create_box("❌ 格式错误", "正确格式：/绑定账号 游戏账号\n例如：/绑定账号 xhl2511686"))
            return
        
        game_account = parts[1]
        qq_id = self._get_user_id(event)
        
        if qq_id == "unknown":
            yield event.plain_result(self._create_box("❌ 身份验证失败", "无法获取您的QQ信息"))
            return
        
        # 检查此QQ是否已绑定
        if qq_id in self.bindings:
            old_account = self.bindings[qq_id]["game_account"]
            bind_time = self.bindings[qq_id]["bind_time"]
            yield event.plain_result(self._create_box("⚠️ 已绑定账号", 
                f"当前绑定：{old_account}\n绑定时间：{bind_time}\n\n如需更换账号：\n1. 先使用 /解绑账号\n2. 再重新绑定新账号"))
            return
        
        # 检查账号是否已被绑定
        is_bound, bound_qq, bind_info = self._is_account_already_bound(game_account)
        if is_bound:
            yield event.plain_result(self._create_box("❌ 账号已被绑定",
                f"游戏账号：{game_account}\n已被QQ：{bound_qq} 绑定\n绑定时间：{bind_info.get('bind_time', '未知')}"))
            return
        
        # 验证账号是否存在
        try:
            account_info = await self._get_account_info(game_account)
            if not account_info:
                yield event.plain_result(self._create_box("❌ 账号不存在", 
                    f"游戏账号：{game_account}\n在系统中未找到此账号\n请检查账号是否正确"))
                return
        except Exception as e:
            logger.error(f"验证游戏账号失败: {e}")
            yield event.plain_result(self._create_box("❌ 验证失败", "网络连接异常，请稍后重试"))
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
        content = (f"✨ 绑定成功！\n\n"
                  f"🎮 游戏账号：{account_name}\n"
                  f"💰 当前余额：{account_info.get('gold_pay', 0):,} 元宝\n"
                  f"📈 累计充值：{account_info.get('gold_pay_total', 0):,} 元宝\n"
                  f"⏰ 绑定时间：{self.bindings[qq_id]['bind_time']}")
        
        yield event.plain_result(self._create_box("✅ 绑定成功", content))
    
    # ========== 我的信息功能 ==========
    @filter.prefix("我的信息")
    @filter.prefix("/我的信息")
    async def my_info_cmd(self, event: AstrMessageEvent):
        """显示我的账户信息"""
        qq_id = self._get_user_id(event)
        
        if qq_id == "unknown":
            yield event.plain_result(self._create_box("❌ 身份验证失败", "无法获取QQ信息"))
            return
        
        user_info = self._get_user_info(qq_id)
        
        # 构建信息
        lines = [
            f"📱 用户信息",
            f"────────────",
            f"🆔 QQ号码：{qq_id}",
        ]
        
        # 签到信息
        if user_info["first_sign_date"]:
            lines.append(f"📅 累计签到：{user_info['total_sign_days']} 天")
            lines.append(f"🔥 连续签到：{user_info['continuous_days']} 天")
            lines.append(f"💰 签到获得：{user_info['total_earned']:,} 元宝")
        
        # 充值信息
        if user_info["total_recharged"] > 0:
            lines.append(f"💳 累计充值：{user_info['total_recharged']:,} 元宝")
        
        # 绑定状态
        if qq_id in self.bindings:
            account_name = self.bindings[qq_id].get("account_name", "未知")
            bind_time = self.bindings[qq_id]["bind_time"]
            lines.append(f"🎮 绑定账号：{account_name}")
            lines.append(f"⏰ 绑定时间：{bind_time}")
        else:
            lines.append("🎮 绑定账号：未绑定")
        
        lines.append(f"────────────")
        lines.append("💡 使用 /签到 领取每日元宝奖励")
        
        yield event.plain_result(self._create_box("📊 我的信息", "\n".join(lines)))
    
    # ========== 签到功能 ==========
    @filter.prefix("签到")
    @filter.prefix("/签到")
    async def sign_cmd(self, event: AstrMessageEvent):
        """每日签到获取元宝"""
        qq_id = self._get_user_id(event)
        
        if qq_id == "unknown":
            yield event.plain_result(self._create_box("❌ 身份验证失败", "无法获取QQ信息"))
            return
        
        # 检查是否已绑定账号
        if qq_id not in self.bindings:
            yield event.plain_result(self._create_box("❌ 未绑定账号", 
                "请先绑定游戏账号才能签到\n\n"
                "💡 使用命令：\n"
                "/绑定账号 <游戏账号>"))
            return
        
        today = date.today().isoformat()
        
        # 检查是否已签到
        if qq_id in self.sign_records and self.sign_records[qq_id].get("last_sign") == today:
            # 获取游戏账号信息
            game_account = self.bindings[qq_id]["game_account"]
            try:
                account_info = await self._get_account_info(game_account)
                if account_info:
                    content = (f"⏳ 今日已签到\n\n"
                              f"🎮 游戏账号：{account_info.get('passport', game_account)}\n"
                              f"💰 当前余额：{account_info.get('gold_pay', 0):,} 元宝\n"
                              f"⏰ 下次签到：明天")
                else:
                    content = "⏳ 今日已签到，明天再来吧！"
            except:
                content = "⏳ 今日已签到，明天再来吧！"
            
            yield event.plain_result(self._create_box("📅 签到状态", content))
            return
        
        user_info = self._get_user_info(qq_id)
        game_account = self.bindings[qq_id]["game_account"]
        
        # 计算连续天数
        yesterday = (date.today() - timedelta(days=1)).isoformat()
        if user_info["last_sign_date"] == yesterday:
            user_info["continuous_days"] += 1
        elif user_info["last_sign_date"] != today:
            user_info["continuous_days"] = 1
        
        # 计算签到奖励
        continuous_days = user_info["continuous_days"]
        
        # 基础奖励：根据连续天数增加
        if continuous_days <= 7:
            base_reward = continuous_days * 100  # 1-7天：每天100元宝递增
        elif continuous_days <= 14:
            base_reward = 700 + (continuous_days - 7) * 150  # 8-14天：每天150元宝递增
        else:
            base_reward = 1750 + (continuous_days - 14) * 200  # 15天以上：每天200元宝递增
        
        # 特殊天数奖励
        extra_reward = 0
        for day, reward in self.system_config["sign_rewards"].items():
            if continuous_days == day:
                extra_reward = reward - base_reward
                break
        
        total_reward = base_reward + extra_reward
        
        # 更新用户信息
        user_info["total_sign_days"] += 1
        user_info["total_earned"] += total_reward
        user_info["last_sign_date"] = today
        
        if not user_info["first_sign_date"]:
            user_info["first_sign_date"] = today
        
        # 执行充值（将签到奖励充入游戏账号）
        try:
            remark = f"每日签到奖励（连续{continuous_days}天）"
            recharge_result = await self._execute_account_recharge(game_account, total_reward, remark)
            
            if recharge_result.get("success"):
                # 保存用户信息
                self._update_user_info(qq_id, user_info)
                
                # 保存签到记录
                self.sign_records[qq_id] = {
                    "last_sign": today,
                    "last_reward": total_reward,
                    "continuous_days": continuous_days
                }
                self._save_json(self.sign_file, self.sign_records)
                
                # 获取账户最新信息
                account_info = await self._get_account_info(game_account)
                
                # 构建响应
                content_lines = [
                    f"✨ 签到成功！",
                    f"────────────",
                    f"💰 获得奖励：{total_reward:,} 元宝",
                    f"🔥 连续签到：{continuous_days} 天",
                    f"🎮 游戏账号：{account_info.get('passport', game_account)}",
                    f"💎 当前余额：{account_info.get('gold_pay', 0):,} 元宝",
                    f"📊 累计签到：{user_info['total_sign_days']} 天",
                    f"────────────"
                ]
                
                if extra_reward > 0:
                    content_lines.append(f"🎁 特殊奖励：+{extra_reward:,} 元宝")
                
                content_lines.append(f"⏰ 签到时间：{datetime.now().strftime('%Y-%m-%d %H:%M')}")
                
                yield event.plain_result(self._create_box("🎉 签到成功", "\n".join(content_lines)))
            else:
                error_msg = recharge_result.get("error", "未知错误")
                yield event.plain_result(self._create_box("❌ 签到失败", f"充值失败：{error_msg}"))
                
        except Exception as e:
            logger.error(f"签到充值异常：{e}")
            yield event.plain_result(self._create_box("❌ 签到异常", "请稍后重试或联系管理员"))
    
    # ========== 充值功能 ==========
    @filter.prefix("账号充值")
    @filter.prefix("/账号充值")
    async def account_recharge_cmd(self, event: AstrMessageEvent):
        """为绑定账号充值"""
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result(self._create_box("❌ 格式错误", 
                "正确格式：/账号充值 <金额> [备注]\n例如：/账号充值 1000 元宝充值"))
            return
        
        try:
            amount = float(parts[1])
            if amount <= 0:
                raise ValueError("金额必须大于0")
            remark = " ".join(parts[2:]) if len(parts) > 2 else "QQ机器人充值"
        except ValueError:
            yield event.plain_result(self._create_box("❌ 参数错误", "充值金额必须是数字且大于0"))
            return
        
        qq_id = self._get_user_id(event)
        
        # 检查绑定
        if qq_id not in self.bindings:
            yield event.plain_result(self._create_box("❌ 未绑定账号", 
                "请先绑定游戏账号\n\n"
                "💡 使用命令：\n"
                "/绑定账号 <游戏账号>"))
            return
        
        game_account = self.bindings[qq_id]["game_account"]
        account_name = self.bindings[qq_id].get("account_name", game_account)
        
        # 计算实际充值金额（含赠送）
        bonus_rate = self.system_config["recharge_bonus"]
        bonus_amount = int(amount * bonus_rate)
        total_amount = amount + bonus_amount
        
        # 执行充值
        try:
            result = await self._execute_account_recharge(game_account, total_amount, remark)
            
            if result.get("success"):
                # 更新用户信息
                user_info = self._get_user_info(qq_id)
                user_info["total_recharged"] += amount
                user_info["last_recharge_date"] = datetime.now().isoformat()
                self._update_user_info(qq_id, user_info)
                
                # 记录充值日志
                recharge_id = f"R{datetime.now().strftime('%Y%m%d%H%M%S')}_{qq_id}"
                self.recharge_logs[recharge_id] = {
                    "qq_id": qq_id,
                    "game_account": game_account,
                    "account_name": account_name,
                    "base_amount": amount,
                    "bonus_amount": bonus_amount,
                    "total_amount": total_amount,
                    "remark": remark,
                    "recharge_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                    "api_response": result
                }
                self._save_json(self.recharge_file, self.recharge_logs)
                
                response_data = result.get("data", {})
                
                content_lines = [
                    f"✅ 充值成功！",
                    f"────────────",
                    f"🎮 游戏账号：{account_name}",
                    f"💰 充值金额：{amount:,.0f} 元宝",
                    f"🎁 赠送金额：{bonus_amount:,.0f} 元宝",
                    f"💎 实际到账：{total_amount:,.0f} 元宝",
                    f"📝 充值备注：{remark}",
                    f"────────────",
                    f"📊 账户信息：",
                    f"🧾 新余额：{response_data.get('new_gold_pay', '未知'):,}",
                    f"💰 累计充值：{response_data.get('new_gold_pay_total', '未知'):,}",
                    f"────────────",
                    f"⏰ 充值时间：{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"
                ]
                
                yield event.plain_result(self._create_box("✨ 充值成功", "\n".join(content_lines)))
            else:
                error_msg = result.get("error", "未知错误")
                yield event.plain_result(self._create_box("❌ 充值失败", f"错误信息：{error_msg}"))
                
        except Exception as e:
            logger.error(f"充值异常：{e}")
            yield event.plain_result(self._create_box("❌ 充值异常", "请稍后重试或联系管理员"))
    
    # ========== 查询账号功能 ==========
    @filter.prefix("查询账号")
    @filter.prefix("/查询账号")
    async def query_account_cmd(self, event: AstrMessageEvent):
        """查询游戏账号信息"""
        parts = event.message_str.strip().split()
        
        if len(parts) >= 2:
            # 查询指定账号
            game_account = parts[1]
        else:
            # 查询自己绑定的账号
            qq_id = self._get_user_id(event)
            if qq_id not in self.bindings:
                yield event.plain_result(self._create_box("❌ 未绑定账号", 
                    "请先绑定游戏账号或指定要查询的账号\n\n"
                    "💡 使用方法：\n"
                    "1. /查询账号 xhl2511686\n"
                    "2. 或先绑定账号再查询"))
                return
            game_account = self.bindings[qq_id]["game_account"]
        
        try:
            account_info = await self._get_account_info(game_account)
            if not account_info:
                yield event.plain_result(self._create_box("❌ 账号不存在", f"游戏账号 {game_account} 不存在"))
                return
        except Exception as e:
            logger.error(f"查询账号失败：{e}")
            yield event.plain_result(self._create_box("❌ 查询失败", "网络连接异常，请稍后重试"))
            return
        
        # 检查此账号是否被绑定
        is_bound, bound_qq, bind_info = self._is_account_already_bound(game_account)
        
        # 构建信息
        content_lines = [
            f"🎮 账号信息",
            f"────────────",
            f"📝 游戏账号：{account_info.get('passport', '未知')}",
            f"💰 当前余额：{account_info.get('gold_pay', 0):,} 元宝",
            f"📈 累计充值：{account_info.get('gold_pay_total', 0):,} 元宝"
        ]
        
        # 添加额外信息
        if account_info.get('name'):
            content_lines.append(f"👤 角色名称：{account_info['name']}")
        if account_info.get('cid'):
            content_lines.append(f"🆔 角色ID：{account_info['cid']}")
        
        content_lines.append(f"────────────")
        content_lines.append(f"🔗 绑定状态：{'已绑定' if is_bound else '未绑定'}")
        
        if is_bound:
            content_lines.append(f"📱 绑定QQ：{bound_qq}")
            content_lines.append(f"⏰ 绑定时间：{bind_info.get('bind_time', '未知')}")
        else:
            content_lines.append(f"💡 使用 /绑定账号 可绑定此账号")
        
        yield event.plain_result(self._create_box("📋 账号查询", "\n".join(content_lines)))
    
    # ========== 修改绑定功能 ==========
    @filter.prefix("修改绑定")
    @filter.prefix("/修改绑定")
    async def modify_bind_cmd(self, event: AstrMessageEvent):
        """修改绑定账号"""
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result(self._create_box("❌ 格式错误", "正确格式：/修改绑定 新游戏账号\n例如：/修改绑定 new_account"))
            return
        
        new_account = parts[1]
        qq_id = self._get_user_id(event)
        
        if qq_id == "unknown":
            yield event.plain_result(self._create_box("❌ 身份验证失败", "无法获取您的QQ信息"))
            return
        
        # 检查是否已绑定
        if qq_id not in self.bindings:
            yield event.plain_result(self._create_box("❌ 未绑定账号", "您尚未绑定任何游戏账号\n请先使用 /绑定账号 命令"))
            return
        
        # 获取旧账号信息
        old_account = self.bindings[qq_id]["game_account"]
        old_bind_time = self.bindings[qq_id]["bind_time"]
        
        # 检查新账号是否已被绑定（排除自己）
        is_bound, bound_qq, bind_info = self._is_account_already_bound(new_account, exclude_qq=qq_id)
        if is_bound:
            yield event.plain_result(self._create_box("❌ 账号已被绑定",
                f"游戏账号：{new_account}\n已被QQ：{bound_qq} 绑定\n绑定时间：{bind_info.get('bind_time', '未知')}"))
            return
        
        # 验证新账号是否存在
        try:
            account_info = await self._get_account_info(new_account)
            if not account_info:
                yield event.plain_result(self._create_box("❌ 账号不存在", f"游戏账号 {new_account} 不存在"))
                return
        except Exception as e:
            logger.error(f"验证游戏账号失败: {e}")
            yield event.plain_result(self._create_box("❌ 验证失败", "网络连接异常，请稍后重试"))
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
        content = (f"✅ 修改成功！\n\n"
                  f"🔁 原账号：{old_account}\n"
                  f"🎯 新账号：{account_name}\n"
                  f"💰 当前余额：{account_info.get('gold_pay', 0):,} 元宝\n"
                  f"⏰ 原绑定：{old_bind_time}\n"
                  f"⏰ 新绑定：{self.bindings[qq_id]['bind_time']}")
        
        yield event.plain_result(self._create_box("✨ 修改成功", content))
    
    # ========== 解绑功能 ==========
    @filter.prefix("解绑账号")
    @filter.prefix("/解绑账号")
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
            
            content = (f"✅ 解绑成功！\n\n"
                      f"🎮 已解绑账号：{account_name}\n"
                      f"⏰ 原绑定时间：{bind_time}\n"
                      f"🗑️ 解绑时间：{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n\n"
                      f"💡 如需重新绑定，请使用 /绑定账号 命令")
            
            yield event.plain_result(self._create_box("🔓 解绑成功", content))
        else:
            yield event.plain_result(self._create_box("⚠️ 未绑定账号", "您未绑定任何游戏账号"))
    
    # ========== 测试连接功能 ==========
    @filter.prefix("测试连接")
    @filter.prefix("/测试连接")
    async def test_connection_cmd(self, event: AstrMessageEvent):
        """测试API连接"""
        yield event.plain_result(self._create_box("🔄 连接测试", "正在测试API连接..."))
        
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
                            content = (f"✅ API连接正常！\n\n"
                                      f"🌐 连接状态：正常\n"
                                      f"📊 账号数量：{result['data']['total']:,} 个\n"
                                      f"⏱️ 响应时间：正常\n"
                                      f"🔗 服务状态：在线")
                            yield event.plain_result(self._create_box("✅ 连接成功", content))
                        else:
                            error_msg = result.get('error', '未知错误')
                            yield event.plain_result(self._create_box("⚠️ API异常", f"API响应异常：{error_msg}"))
                    else:
                        yield event.plain_result(self._create_box("❌ 连接失败", f"API连接失败，状态码：{response.status}"))
                        
        except Exception as e:
            yield event.plain_result(self._create_box("❌ 连接失败", f"API连接失败：{str(e)}\n请检查API地址和网络配置"))
    
    # ========== API调用方法 ==========
    async def _get_account_info(self, passport: str) -> Optional[dict]:
        """调用API查询账号信息"""
        try:
            async with aiohttp.ClientSession() as session:
                # 通过passport查询账号
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
                            # 获取第一个匹配的账号
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
                form_data.add_field("passport", passport)  # 使用passport
                form_data.add_field("amount", str(amount))
                form_data.add_field("remark", remark)
                form_data.add_field("source", "qq_bot")  # 来源标识
                form_data.add_field("secret", self.api_config["qq_bot_secret"])  # 使用配置的密钥
                
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
