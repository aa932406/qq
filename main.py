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
        self.task_file = os.path.join(self.data_dir, "task_progress.json")
        self.lottery_file = os.path.join(self.data_dir, "lottery_records.json")
        
        # 加载数据
        self.bindings = self._load_json(self.bind_file)
        self.recharge_logs = self._load_json(self.recharge_file)
        self.user_points = self._load_json(self.points_file)
        self.sign_records = self._load_json(self.sign_file)
        self.task_progress = self._load_json(self.task_file)
        self.lottery_records = self._load_json(self.lottery_file)
        
        # API配置
        self.api_config = {
            "base_url": "http://115.190.64.181:881/api/players.php",
            "timeout": 30,
            "qq_bot_secret": "ws7ecejjsznhtxurchknmdemax2fnp5d"
        }
        
        # 系统配置
        self.system_config = {
            # 积分系统
            "points": {
                "sign_reward": 15,  # 每日签到奖励
                "recharge_ratio": 10000,  # 1积分=10000元宝
                "continuous_rewards": {
                    3: 10,   # 连续3天额外奖励
                    7: 25,   # 连续7天额外奖励
                    15: 50,  # 连续15天额外奖励
                    30: 100  # 连续30天额外奖励
                },
                "invite_reward": 100,  # 邀请奖励
                "daily_tasks": {
                    "recharge_any": 30,    # 每日充值任意金额
                    "recharge_1000": 80,   # 充值1000元以上
                    "bind_account": 50,    # 绑定账号
                    "query_account": 10,   # 查询账号
                    "daily_sign": 15       # 每日签到
                }
            },
            # 抽奖系统
            "lottery": {
                "cost_per_try": 50,  # 每次抽奖消耗积分
                "prizes": [
                    {"name": "🎁 特等奖", "points": 500, "probability": 0.5},
                    {"name": "🏆 一等奖", "points": 200, "probability": 2},
                    {"name": "🥈 二等奖", "points": 100, "probability": 5},
                    {"name": "🥉 三等奖", "points": 50, "probability": 10},
                    {"name": "🎯 四等奖", "points": 20, "probability": 20},
                    {"name": "✨ 参与奖", "points": 10, "probability": 30},
                    {"name": "💝 幸运奖", "points": 5, "probability": 32.5}
                ]
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
    
    def _get_user_points(self, qq_id: str) -> Dict:
        """获取用户积分信息"""
        if qq_id not in self.user_points:
            self.user_points[qq_id] = {
                "points": 100,  # 初始赠送100积分
                "total_earned": 100,
                "total_spent": 0,
                "first_sign_date": None,
                "last_sign_date": None,
                "continuous_days": 0,
                "invite_code": self._generate_invite_code(),
                "invited_count": 0,
                "lottery_count": 0,
                "total_lottery_wins": 0
            }
        return self.user_points[qq_id]
    
    def _generate_invite_code(self) -> str:
        """生成邀请码"""
        import string
        return ''.join(random.choices(string.ascii_uppercase + string.digits, k=8))
    
    def _update_user_points(self, qq_id: str, points_data: Dict):
        """更新用户积分信息"""
        self.user_points[qq_id] = points_data
        self._save_json(self.points_file, self.user_points)
    
    def _update_daily_task(self, qq_id: str, task_key: str):
        """更新每日任务进度"""
        today = date.today().isoformat()
        if qq_id not in self.task_progress:
            self.task_progress[qq_id] = {}
        
        if today not in self.task_progress[qq_id]:
            self.task_progress[qq_id][today] = {}
        
        self.task_progress[qq_id][today][task_key] = True
        self._save_json(self.task_file, self.task_progress)
        
        # 奖励积分
        if task_key in self.system_config["points"]["daily_tasks"]:
            reward = self.system_config["points"]["daily_tasks"][task_key]
            user_points = self._get_user_points(qq_id)
            user_points["points"] += reward
            user_points["total_earned"] += reward
            self._update_user_points(qq_id, user_points)
            return reward
        return 0
    
    def _check_and_reward_daily_task(self, qq_id: str, task_key: str) -> int:
        """检查并奖励每日任务"""
        today = date.today().isoformat()
        
        # 检查今天是否已完成此任务
        if (qq_id in self.task_progress and 
            today in self.task_progress[qq_id] and 
            task_key in self.task_progress[qq_id][today]):
            return 0  # 已经完成过了
        
        return self._update_daily_task(qq_id, task_key)
    
    async def initialize(self):
        logger.info("🚀 游戏账号插件已启动！")
    
    # ========== 美观的UI装饰器 ==========
    def _create_box(self, title: str, content: str, width: int = 40) -> str:
        """创建美观的文本框"""
        lines = content.strip().split('\n')
        max_len = max(len(line) for line in lines)
        box_width = max(max_len + 4, width)
        
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
    
    def _create_header(self, title: str, emoji: str = "✨") -> str:
        """创建标题头"""
        return f"{emoji} {title} {emoji}\n{'=' * 40}\n"
    
    # ========== 帮助功能 ==========
    @filter.command("帮助")
    async def help_cmd(self, event: AstrMessageEvent):
        """显示帮助信息"""
        help_text = f"""
{self._create_header("🎮 游戏账号插件", "🌟")}

📋 账户管理：
├─ /我的信息    - 查看账户信息
├─ /绑定账号    - 绑定游戏账号
├─ /我的绑定    - 查看绑定信息
├─ /解绑账号    - 解绑游戏账号
├─ /修改绑定    - 修改绑定账号
└─ /查询账号    - 查询账号信息

💰 充值功能：
├─ /账号充值    - 直接充值元宝
├─ /积分充值    - 使用积分充值
└─ /我的积分    - 查看积分信息

🎁 签到系统：
├─ /签到        - 每日签到得积分
├─ /连续签到    - 查看签到信息
├─ /任务列表    - 查看每日任务
└─ /积分榜      - 查看积分排行

🤝 邀请系统：
├─ /我的邀请    - 获取邀请码
└─ /使用邀请码   - 使用邀请码

🎪 娱乐功能：
├─ /抽奖        - 积分抽奖
└─ /抽奖记录    - 查看抽奖记录

🔧 系统功能：
├─ /测试连接    - 测试API连接
└─ /帮助        - 显示本帮助

👑 管理员功能：
├─ /查看绑定    - 查看所有绑定
├─ /充值记录    - 查看充值记录
├─ /强制绑定    - 强制绑定账号
└─ /添加积分    - 添加用户积分

{'=' * 40}
💡 提示：所有用户初始赠送100积分！
"""
        yield event.plain_result(help_text)
    
    @filter.command("菜单")
    async def menu_cmd(self, event: AstrMessageEvent):
        """显示简洁菜单"""
        menu_text = f"""
{self._create_header("📱 快捷菜单", "🚀")}

🔐 账户管理：
• /我的信息 • /绑定账号 • /我的绑定

💰 充值相关：
• /账号充值 • /积分充值 • /我的积分

🎁 每日福利：
• /签到 • /任务列表 • /抽奖

🤝 社交互动：
• /我的邀请 • /积分榜

❓ 帮助支持：
• /帮助 • /测试连接

{'=' * 40}
✨ 输入 /帮助 查看详细命令
"""
        yield event.plain_result(menu_text)
    
    # ========== 账户管理功能 ==========
    @filter.command("绑定账号")
    async def bind_account_cmd(self, event: AstrMessageEvent):
        """绑定PHP游戏账号"""
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result(self._create_box("❌ 格式错误", "📝 正确格式：/绑定账号 游戏账号\n💡 例如：/绑定账号 xhl2511686"))
            return
        
        game_account = parts[1]
        qq_id = self._get_user_id(event)
        
        if qq_id == "unknown":
            yield event.plain_result(self._create_box("❌ 身份验证失败", "无法获取您的QQ信息，请联系管理员"))
            return
        
        # 检查此QQ是否已绑定
        if qq_id in self.bindings:
            old_account = self.bindings[qq_id]["game_account"]
            bind_time = self.bindings[qq_id]["bind_time"]
            yield event.plain_result(self._create_box("⚠️ 已绑定账号", 
                f"📋 当前绑定：{old_account}\n"
                f"⏰ 绑定时间：{bind_time}\n\n"
                f"💡 如需更换账号：\n"
                f"1. 先使用 /解绑账号\n"
                f"2. 再重新绑定新账号"))
            return
        
        # 检查账号是否已被绑定
        is_bound, bound_qq, bind_info = self._is_account_already_bound(game_account)
        if is_bound:
            yield event.plain_result(self._create_box("❌ 账号已被绑定",
                f"🎮 游戏账号：{game_account}\n"
                f"📱 已被QQ：{bound_qq} 绑定\n"
                f"⏰ 绑定时间：{bind_info.get('bind_time', '未知')}\n\n"
                f"💡 如需强制绑定，请联系管理员"))
            return
        
        # 验证账号是否存在
        try:
            account_info = await self._get_account_info(game_account)
            if not account_info:
                yield event.plain_result(self._create_box("❌ 账号不存在", 
                    f"🎮 游戏账号：{game_account}\n"
                    f"❌ 在系统中未找到此账号\n\n"
                    f"💡 请检查账号是否正确"))
                return
        except Exception as e:
            logger.error(f"验证游戏账号失败: {e}")
            yield event.plain_result(self._create_box("❌ 验证失败", 
                "网络连接异常，请稍后重试\n"
                "或联系管理员检查API连接"))
            return
        
        # 保存绑定
        self.bindings[qq_id] = {
            "game_account": game_account,
            "account_name": account_info.get("passport", game_account),
            "bind_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "qq_id": qq_id
        }
        self._save_json(self.bind_file, self.bindings)
        
        # 每日任务奖励
        task_reward = self._check_and_reward_daily_task(qq_id, "bind_account")
        
        account_name = account_info.get("passport", game_account)
        content = (f"✨ 绑定成功！\n\n"
                  f"🎮 游戏账号：{account_name}\n"
                  f"💰 当前余额：{account_info.get('gold_pay', 0)} 元宝\n"
                  f"📈 累计充值：{account_info.get('gold_pay_total', 0)} 元宝\n"
                  f"⏰ 绑定时间：{self.bindings[qq_id]['bind_time']}\n\n"
                  f"🎯 完成绑定任务")
        
        if task_reward > 0:
            content += f"\n🎁 任务奖励：+{task_reward} 积分"
        
        yield event.plain_result(self._create_box("✅ 绑定成功", content))
    
    @filter.command("我的绑定")
    async def mybind_cmd(self, event: AstrMessageEvent):
        """查询绑定信息"""
        qq_id = self._get_user_id(event)
        
        if qq_id in self.bindings:
            data = self.bindings[qq_id]
            content = (f"🎮 游戏账号：{data.get('account_name', '未知')}\n"
                      f"⏰ 绑定时间：{data['bind_time']}\n\n"
                      f"💡 账号操作：\n"
                      f"• /解绑账号 - 解绑当前账号\n"
                      f"• /修改绑定 - 修改绑定账号\n"
                      f"• /查询账号 - 查看账号详情")
            
            if 'old_account' in data:
                content += f"\n📝 历史：由 {data['old_account']} 修改而来"
            
            yield event.plain_result(self._create_box("📋 绑定信息", content))
        else:
            yield event.plain_result(self._create_box("⚠️ 未绑定账号", 
                "您尚未绑定任何游戏账号\n\n"
                "💡 使用命令：\n"
                "/绑定账号 <游戏账号>\n\n"
                "✨ 例如：/绑定账号 xhl2511686"))
    
    @filter.command("修改绑定")
    async def modify_bind_cmd(self, event: AstrMessageEvent):
        """修改绑定账号"""
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result(self._create_box("❌ 格式错误", "📝 正确格式：/修改绑定 新游戏账号\n💡 例如：/修改绑定 new_account"))
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
                f"🎮 游戏账号：{new_account}\n"
                f"📱 已被QQ：{bound_qq} 绑定\n"
                f"⏰ 绑定时间：{bind_info.get('bind_time', '未知')}\n\n"
                f"💡 无法修改到此账号"))
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
                  f"💰 当前余额：{account_info.get('gold_pay', 0)} 元宝\n"
                  f"⏰ 原绑定：{old_bind_time}\n"
                  f"⏰ 新绑定：{self.bindings[qq_id]['bind_time']}")
        
        yield event.plain_result(self._create_box("✨ 修改成功", content))
    
    @filter.command("解绑账号")
    async def unbind_account_cmd(self, event: AstrMessageEvent):
        """解绑游戏账号"""
        qq_id = self._get_user_id(event)
        
        if qq_id in self.bindings:
            game_account = self.bindings[qq_id]["game_account"]
            account_name = self.bindings[qq_id].get("account_name", game_account)
            bind_time = self.bindings[qq_id].get("bind_time", "未知时间")
            
            # 记录解绑日志
            unbind_log = {
                "qq_id": qq_id,
                "game_account": game_account,
                "account_name": account_name,
                "unbind_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                "bind_time": bind_time
            }
            
            # 保存到解绑日志文件
            unbind_file = os.path.join(self.data_dir, "unbind_logs.json")
            unbind_logs = self._load_json(unbind_file)
            unbind_logs[f"U{datetime.now().strftime('%Y%m%d%H%M%S')}_{qq_id}"] = unbind_log
            self._save_json(unbind_file, unbind_logs)
            
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
    
    # ========== 我的信息功能 ==========
    @filter.command("我的信息")
    async def my_info_cmd(self, event: AstrMessageEvent):
        """显示我的账户信息"""
        qq_id = self._get_user_id(event)
        
        if qq_id == "unknown":
            yield event.plain_result(self._create_box("❌ 身份验证失败", "无法获取QQ信息"))
            return
        
        user_points = self._get_user_points(qq_id)
        
        # 构建信息
        lines = [
            f"📱 用户信息",
            f"{'─' * 30}",
            f"🆔 QQ号码：{qq_id}",
            f"💰 当前积分：{user_points['points']}",
            f"📊 累计获得：{user_points['total_earned']}",
            f"💸 累计消耗：{user_points['total_spent']}",
            f"📅 连续签到：{user_points['continuous_days']} 天",
            f"🎯 抽奖次数：{user_points['lottery_count']}",
            f"🏆 中奖次数：{user_points['total_lottery_wins']}",
            f"👥 邀请人数：{user_points['invited_count']}",
            f"🎫 邀请码：{user_points['invite_code']}"
        ]
        
        # 绑定状态
        if qq_id in self.bindings:
            account_name = self.bindings[qq_id].get("account_name", "未知")
            bind_time = self.bindings[qq_id]["bind_time"]
            lines.append(f"🎮 绑定账号：{account_name}")
            lines.append(f"⏰ 绑定时间：{bind_time}")
        else:
            lines.append("🎮 绑定账号：未绑定")
        
        lines.append(f"{'─' * 30}")
        lines.append("💡 使用 /我的积分 查看详细积分信息")
        
        yield event.plain_result(self._create_box("📊 我的信息", "\n".join(lines)))
    
    # ========== 签到系统 ==========
    @filter.command("签到")
    async def sign_cmd(self, event: AstrMessageEvent):
        """每日签到"""
        qq_id = self._get_user_id(event)
        
        if qq_id == "unknown":
            yield event.plain_result(self._create_box("❌ 身份验证失败", "无法获取QQ信息"))
            return
        
        today = date.today().isoformat()
        
        # 检查是否已签到
        if qq_id in self.sign_records and self.sign_records[qq_id].get("last_sign") == today:
            yield event.plain_result(self._create_box("⏳ 今日已签到", 
                f"📅 签到时间：今天\n"
                f"⏰ 下次签到：明天\n\n"
                f"💡 签到福利：\n"
                f"• 每日签到得积分\n"
                f"• 连续签到有额外奖励\n"
                f"• 完成任务获更多积分"))
            return
        
        user_points = self._get_user_points(qq_id)
        
        # 计算连续天数
        yesterday = (date.today() - timedelta(days=1)).isoformat()
        if user_points["last_sign_date"] == yesterday:
            user_points["continuous_days"] += 1
        elif user_points["last_sign_date"] != today:
            user_points["continuous_days"] = 1
        
        # 基础奖励
        base_reward = self.system_config["points"]["sign_reward"]
        total_reward = base_reward
        
        # 连续奖励
        extra_reward = 0
        for days, reward in self.system_config["points"]["continuous_rewards"].items():
            if user_points["continuous_days"] >= days:
                extra_reward = reward
        
        if extra_reward > 0:
            total_reward += extra_reward
        
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
            "sign_count": self.sign_records.get(qq_id, {}).get("sign_count", 0) + 1
        }
        self._save_json(self.sign_file, self.sign_records)
        
        # 每日任务奖励
        task_reward = self._check_and_reward_daily_task(qq_id, "daily_sign")
        
        # 构建响应
        content_lines = [
            f"✨ 签到成功！",
            f"{'─' * 30}",
            f"💰 获得积分：{total_reward}",
            f"📅 连续签到：{user_points['continuous_days']} 天",
            f"💎 当前积分：{user_points['points']}",
            f"📊 累计获得：{user_points['total_earned']}"
        ]
        
        if extra_reward > 0:
            content_lines.append(f"🎁 连续奖励：+{extra_reward} 积分")
        
        if task_reward > 0:
            content_lines.append(f"✅ 任务奖励：+{task_reward} 积分")
        
        content_lines.append(f"{'─' * 30}")
        
        # 计算下次奖励
        next_reward_days = None
        next_reward_amount = 0
        for days, reward in sorted(self.system_config["points"]["continuous_rewards"].items()):
            if user_points["continuous_days"] < days:
                next_reward_days = days - user_points["continuous_days"]
                next_reward_amount = reward
                break
        
        if next_reward_days:
            content_lines.append(f"🎯 下次连续奖励：")
            content_lines.append(f"再签 {next_reward_days} 天可获得 {next_reward_amount} 额外积分")
        
        yield event.plain_result(self._create_box("🎉 签到成功", "\n".join(content_lines)))
    
    @filter.command("连续签到")
    async def continuous_sign_cmd(self, event: AstrMessageEvent):
        """查看连续签到信息"""
        qq_id = self._get_user_id(event)
        
        if qq_id == "unknown":
            yield event.plain_result(self._create_box("❌ 身份验证失败", "无法获取QQ信息"))
            return
        
        user_points = self._get_user_points(qq_id)
        
        lines = [
            f"📅 签到统计",
            f"{'─' * 30}",
            f"连续签到天数：{user_points['continuous_days']} 天",
            f"上次签到时间：{user_points['last_sign_date'] or '从未签到'}",
            f"首次签到时间：{user_points['first_sign_date'] or '从未签到'}",
            f"{'─' * 30}",
            f"🎁 连续签到奖励表"
        ]
        
        # 显示连续签到奖励
        for days, reward in sorted(self.system_config["points"]["continuous_rewards"].items()):
            if user_points["continuous_days"] >= days:
                lines.append(f"✅ 连续{days}天：{reward} 积分（已达成）")
            else:
                days_needed = days - user_points["continuous_days"]
                lines.append(f"⭕ 连续{days}天：{reward} 积分（还需{days_needed}天）")
        
        lines.append(f"{'─' * 30}")
        lines.append("💡 提示：连续签到天数越高，奖励越丰厚！")
        
        yield event.plain_result(self._create_box("📊 签到信息", "\n".join(lines)))
    
    # ========== 任务系统 ==========
    @filter.command("任务列表")
    async def task_list_cmd(self, event: AstrMessageEvent):
        """查看每日任务"""
        qq_id = self._get_user_id(event)
        today = date.today().isoformat()
        
        tasks = self.system_config["points"]["daily_tasks"]
        today_tasks = self.task_progress.get(qq_id, {}).get(today, {})
        
        lines = [
            f"🎯 每日任务",
            f"{'─' * 30}"
        ]
        
        task_descriptions = {
            "recharge_any": "每日充值任意金额",
            "recharge_1000": "充值1000元以上",
            "bind_account": "绑定游戏账号（仅首次）",
            "query_account": "查询账号信息",
            "daily_sign": "每日签到"
        }
        
        total_rewards = 0
        completed_tasks = 0
        
        for task_key, reward in tasks.items():
            status = "✅" if task_key in today_tasks else "⭕"
            lines.append(f"{status} {task_descriptions.get(task_key, task_key)}")
            lines.append(f"   奖励：{reward} 积分")
            lines.append("")
            
            total_rewards += reward
            if task_key in today_tasks:
                completed_tasks += 1
        
        lines.append(f"{'─' * 30}")
        lines.append(f"📊 今日进度：{completed_tasks}/{len(tasks)}")
        lines.append(f"💰 总奖励：{total_rewards} 积分")
        lines.append(f"💎 可获积分：{total_rewards if completed_tasks < len(tasks) else 0}")
        lines.append(f"{'─' * 30}")
        lines.append("💡 提示：完成任务自动获得积分")
        
        yield event.plain_result(self._create_box("📋 每日任务", "\n".join(lines)))
    
    # ========== 邀请系统 ==========
    @filter.command("我的邀请")
    async def my_invite_cmd(self, event: AstrMessageEvent):
        """查看邀请信息"""
        qq_id = self._get_user_id(event)
        user_points = self._get_user_points(qq_id)
        
        invite_reward = self.system_config["points"]["invite_reward"]
        
        content = [
            f"🤝 邀请系统",
            f"{'─' * 30}",
            f"🎫 你的邀请码：{user_points['invite_code']}",
            f"👥 已邀请人数：{user_points['invited_count']}",
            f"💰 邀请奖励：{invite_reward} 积分/人",
            f"{'─' * 30}",
            f"💡 使用方法：",
            f"让朋友在QQ群中发送：",
            f"/使用邀请码 {user_points['invite_code']}",
            f"{'─' * 30}",
            f"✨ 邀请福利：",
            f"• 邀请人获得 {invite_reward} 积分",
            f"• 被邀请人获得 {invite_reward//2} 积分",
            f"• 邀请越多，奖励越多！"
        ]
        
        yield event.plain_result(self._create_box("🎁 邀请有奖", "\n".join(content)))
    
    @filter.command("使用邀请码")
    async def use_invite_code_cmd(self, event: AstrMessageEvent):
        """使用邀请码"""
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result(self._create_box("❌ 格式错误", "📝 正确格式：/使用邀请码 <邀请码>\n💡 例如：/使用邀请码 ABC12345"))
            return
        
        invite_code = parts[1].upper()
        qq_id = self._get_user_id(event)
        
        if qq_id == "unknown":
            yield event.plain_result(self._create_box("❌ 身份验证失败", "无法获取QQ信息"))
            return
        
        # 查找邀请人
        inviter_qq = None
        for qq, points_data in self.user_points.items():
            if points_data.get("invite_code") == invite_code:
                inviter_qq = qq
                break
        
        if not inviter_qq:
            yield event.plain_result(self._create_box("❌ 邀请码无效", "邀请码不存在或已失效"))
            return
        
        if inviter_qq == qq_id:
            yield event.plain_result(self._create_box("❌ 操作无效", "不能使用自己的邀请码"))
            return
        
        # 检查是否已经使用过邀请码
        user_points = self._get_user_points(qq_id)
        if user_points.get("invited_by"):
            yield event.plain_result(self._create_box("❌ 已使用过", "您已经使用过邀请码了"))
            return
        
        # 奖励双方
        invite_reward = self.system_config["points"]["invite_reward"]
        
        # 奖励邀请人
        inviter_points = self._get_user_points(inviter_qq)
        inviter_points["points"] += invite_reward
        inviter_points["total_earned"] += invite_reward
        inviter_points["invited_count"] += 1
        self._update_user_points(inviter_qq, inviter_points)
        
        # 记录被邀请人
        user_points["invited_by"] = inviter_qq
        user_points["invite_time"] = datetime.now().isoformat()
        
        # 奖励被邀请人
        new_user_reward = invite_reward // 2
        user_points["points"] += new_user_reward
        user_points["total_earned"] += new_user_reward
        self._update_user_points(qq_id, user_points)
        
        content = [
            f"✅ 邀请成功！",
            f"{'─' * 30}",
            f"🎫 邀请码：{invite_code}",
            f"🤝 邀请人：{inviter_qq}",
            f"💰 你获得：{new_user_reward} 积分",
            f"🎁 邀请人获得：{invite_reward} 积分",
            f"{'─' * 30}",
            f"✨ 当前积分：{user_points['points']}",
            f"📈 累计获得：{user_points['total_earned']}",
            f"{'─' * 30}",
            f"💡 提示：现在你也可以邀请朋友赚积分！",
            f"使用 /我的邀请 查看你的邀请码"
        ]
        
        yield event.plain_result(self._create_box("🎉 邀请成功", "\n".join(content)))
    
    # ========== 积分系统 ==========
    @filter.command("我的积分")
    async def my_points_cmd(self, event: AstrMessageEvent):
        """查看积分信息"""
        qq_id = self._get_user_id(event)
        user_points = self._get_user_points(qq_id)
        
        recharge_ratio = self.system_config["points"]["recharge_ratio"]
        
        content = [
            f"💰 积分系统",
            f"{'─' * 30}",
            f"💎 当前积分：{user_points['points']}",
            f"📈 累计获得：{user_points['total_earned']}",
            f"📉 累计消耗：{user_points['total_spent']}",
            f"🎫 邀请人数：{user_points['invited_count']}",
            f"🎯 抽奖次数：{user_points['lottery_count']}",
            f"🏆 中奖次数：{user_points['total_lottery_wins']}",
            f"{'─' * 30}",
            f"💡 积分获取：",
            f"• 每日签到：{self.system_config['points']['sign_reward']} 积分",
            f"• 邀请好友：{self.system_config['points']['invite_reward']} 积分/人",
            f"• 完成任务：5-80 积分/任务",
            f"• 连续签到：额外奖励积分",
            f"{'─' * 30}",
            f"🎯 积分用途：",
            f"• 1 积分 = {recharge_ratio:,} 元宝",
            f"• 使用 /积分充值 命令兑换元宝",
            f"• 使用 /抽奖 参与积分抽奖",
            f"{'─' * 30}",
            f"💎 当前可兑换：{user_points['points'] * recharge_ratio:,} 元宝"
        ]
        
        yield event.plain_result(self._create_box("📊 积分详情", "\n".join(content)))
    
    @filter.command("积分榜")
    async def points_rank_cmd(self, event: AstrMessageEvent):
        """查看积分排行榜"""
        # 按积分排序
        ranked_users = []
        for qq, data in self.user_points.items():
            if data["points"] > 0:
                # 尝试获取用户名
                username = self.bindings.get(qq, {}).get("account_name", f"用户{qq[-4:]}")
                ranked_users.append((username, data["points"], qq))
        
        ranked_users.sort(key=lambda x: x[1], reverse=True)
        ranked_users = ranked_users[:10]  # 取前10名
        
        if not ranked_users:
            yield event.plain_result(self._create_box("📊 积分榜", "暂无积分数据\n💡 快去签到赚积分吧！"))
            return
        
        lines = [
            f"🏆 积分排行榜 TOP10",
            f"{'─' * 30}"
        ]
        
        medals = ["🥇", "🥈", "🥉", "4️⃣", "5️⃣", "6️⃣", "7️⃣", "8️⃣", "9️⃣", "🔟"]
        
        for i, (username, points, qq) in enumerate(ranked_users):
            if i < 3:
                medal = medals[i]
                lines.append(f"{medal} {username}")
                lines.append(f"   积分：{points:,}  💎")
            else:
                lines.append(f"{medals[i]} {username} - {points:,} 积分")
        
        lines.append(f"{'─' * 30}")
        lines.append(f"📊 统计信息：")
        lines.append(f"• 上榜人数：{len(ranked_users)}")
        lines.append(f"• 最高积分：{ranked_users[0][1]:,}")
        lines.append(f"• 平均积分：{sum(u[1] for u in ranked_users)//len(ranked_users):,}")
        lines.append(f"{'─' * 30}")
        lines.append("💡 提升排名：签到、邀请、充值都能获得积分！")
        
        yield event.plain_result(self._create_box("📈 积分排行", "\n".join(lines)))
    
    # ========== 抽奖系统 ==========
    @filter.command("抽奖")
    async def lottery_cmd(self, event: AstrMessageEvent):
        """积分抽奖"""
        qq_id = self._get_user_id(event)
        
        if qq_id == "unknown":
            yield event.plain_result(self._create_box("❌ 身份验证失败", "无法获取QQ信息"))
            return
        
        user_points = self._get_user_points(qq_id)
        lottery_cost = self.system_config["lottery"]["cost_per_try"]
        
        if user_points["points"] < lottery_cost:
            yield event.plain_result(self._create_box("❌ 积分不足", 
                f"💎 需要积分：{lottery_cost}\n"
                f"💰 当前积分：{user_points['points']}\n\n"
                f"💡 获取积分：\n"
                f"• 每日签到\n"
                f"• 邀请好友\n"
                f"• 完成任务"))
            return
        
        # 扣款
        user_points["points"] -= lottery_cost
        user_points["total_spent"] += lottery_cost
        user_points["lottery_count"] += 1
        
        # 抽奖逻辑
        prizes = self.system_config["lottery"]["prizes"]
        total_prob = sum(prize["probability"] for prize in prizes)
        random_num = random.uniform(0, total_prob)
        
        cumulative_prob = 0
        selected_prize = None
        for prize in prizes:
            cumulative_prob += prize["probability"]
            if random_num <= cumulative_prob:
                selected_prize = prize
                break
        
        # 如果没选中（理论上不会发生），给个参与奖
        if not selected_prize:
            selected_prize = {"name": "✨ 参与奖", "points": 5, "probability": 0}
        
        # 发放奖励
        prize_points = selected_prize["points"]
        if prize_points > 0:
            user_points["points"] += prize_points
            user_points["total_earned"] += prize_points
            user_points["total_lottery_wins"] += 1
        
        self._update_user_points(qq_id, user_points)
        
        # 记录抽奖记录
        if qq_id not in self.lottery_records:
            self.lottery_records[qq_id] = []
        
        self.lottery_records[qq_id].append({
            "time": datetime.now().isoformat(),
            "cost": lottery_cost,
            "prize": selected_prize["name"],
            "points_won": prize_points,
            "net_gain": prize_points - lottery_cost
        })
        self._save_json(self.lottery_file, self.lottery_records)
        
        # 构建响应
        content_lines = [
            f"🎰 抽奖结果",
            f"{'─' * 30}",
            f"🎯 消耗积分：{lottery_cost}",
            f"🏆 获得奖项：{selected_prize['name']}",
            f"💰 奖励积分：{prize_points}",
            f"📈 净收益：{prize_points - lottery_cost}",
            f"{'─' * 30}",
            f"📊 抽奖统计：",
            f"💎 当前积分：{user_points['points']}",
            f"🎯 抽奖次数：{user_points['lottery_count']}",
            f"🏆 中奖次数：{user_points['total_lottery_wins']}",
            f"{'─' * 30}"
        ]
        
        if prize_points > 0:
            if prize_points >= 200:
                content_lines.append("🎉 恭喜获得大奖！运气爆棚！")
            elif prize_points >= 50:
                content_lines.append("✨ 恭喜中奖！手气不错！")
            else:
                content_lines.append("👍 恭喜中奖！继续加油！")
        else:
            content_lines.append("💪 再接再厉！下次会更好！")
        
        content_lines.append(f"💡 提示：每次抽奖消耗 {lottery_cost} 积分")
        
        yield event.plain_result(self._create_box("🎪 幸运抽奖", "\n".join(content_lines)))
    
    @filter.command("抽奖记录")
    async def lottery_history_cmd(self, event: AstrMessageEvent):
        """查看抽奖记录"""
        qq_id = self._get_user_id(event)
        
        if qq_id == "unknown":
            yield event.plain_result(self._create_box("❌ 身份验证失败", "无法获取QQ信息"))
            return
        
        user_points = self._get_user_points(qq_id)
        lottery_records = self.lottery_records.get(qq_id, [])
        
        if not lottery_records:
            yield event.plain_result(self._create_box("📝 抽奖记录", 
                f"暂无抽奖记录\n\n"
                f"🎰 抽奖信息：\n"
                f"• 每次消耗：{self.system_config['lottery']['cost_per_try']} 积分\n"
                f"• 中奖概率：见奖品列表\n"
                f"• 抽奖次数：{user_points['lottery_count']}\n\n"
                f"💡 使用 /抽奖 开始抽奖"))
            return
        
        # 只显示最近10条记录
        recent_records = lottery_records[-10:]
        
        lines = [
            f"📝 抽奖记录（最近10次）",
            f"{'─' * 30}"
        ]
        
        total_cost = 0
        total_won = 0
        
        for i, record in enumerate(reversed(recent_records), 1):
            time_str = datetime.fromisoformat(record["time"]).strftime("%m-%d %H:%M")
            net_gain = record["net_gain"]
            gain_symbol = "+" if net_gain >= 0 else ""
            
            lines.append(f"{i}. {time_str}")
            lines.append(f"   奖项：{record['prize']}")
            lines.append(f"   收益：{gain_symbol}{net_gain} 积分")
            lines.append("")
            
            total_cost += record["cost"]
            total_won += record["points_won"]
        
        lines.append(f"{'─' * 30}")
        lines.append(f"📊 统计信息：")
        lines.append(f"• 总抽奖次数：{user_points['lottery_count']}")
        lines.append(f"• 总消耗积分：{total_cost}")
        lines.append(f"• 总获得积分：{total_won}")
        lines.append(f"• 净收益：{total_won - total_cost} 积分")
        lines.append(f"• 中奖率：{user_points['total_lottery_wins']}/{user_points['lottery_count']}")
        lines.append(f"{'─' * 30}")
        lines.append("💡 提示：运气也是实力的一部分！")
        
        yield event.plain_result(self._create_box("🎰 抽奖历史", "\n".join(lines)))
    
    # ========== 积分充值功能 ==========
    @filter.command("积分充值")
    async def points_recharge_cmd(self, event: AstrMessageEvent):
        """使用积分充值：/积分充值 积分数量 [备注]"""
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result(self._create_box("❌ 格式错误", 
                "📝 正确格式：/积分充值 <积分数量> [备注]\n"
                "💡 例如：/积分充值 10 兑换元宝\n\n"
                f"💎 兑换比例：1 积分 = {self.system_config['points']['recharge_ratio']:,} 元宝"))
            return
        
        try:
            points_to_use = int(parts[1])
            if points_to_use <= 0:
                raise ValueError("必须是正数")
            remark = " ".join(parts[2:]) if len(parts) > 2 else "积分充值"
        except ValueError:
            yield event.plain_result(self._create_box("❌ 参数错误", "积分数量必须是正整数"))
            return
        
        qq_id = self._get_user_id(event)
        
        if qq_id == "unknown":
            yield event.plain_result(self._create_box("❌ 身份验证失败", "无法获取QQ信息"))
            return
        
        # 检查绑定
        if qq_id not in self.bindings:
            yield event.plain_result(self._create_box("❌ 未绑定账号", 
                "请先绑定游戏账号\n\n"
                "💡 使用命令：\n"
                "/绑定账号 <游戏账号>\n\n"
                "例如：/绑定账号 xhl2511686"))
            return
        
        user_points = self._get_user_points(qq_id)
        
        if user_points["points"] < points_to_use:
            yield event.plain_result(self._create_box("❌ 积分不足",
                f"💎 需要积分：{points_to_use}\n"
                f"💰 当前积分：{user_points['points']}\n\n"
                f"💡 获取更多积分：\n"
                f"• 每日签到\n"
                f"• 邀请好友\n"
                f"• 完成任务\n"
                f"• 参与抽奖"))
            return
        
        # 计算充值金额
        recharge_ratio = self.system_config["points"]["recharge_ratio"]
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
                
                # 每日任务奖励
                task_reward = 0
                if recharge_amount >= 1000:
                    task_reward = self._check_and_reward_daily_task(qq_id, "recharge_1000")
                else:
                    task_reward = self._check_and_reward_daily_task(qq_id, "recharge_any")
                
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
                
                content_lines = [
                    f"✅ 充值成功！",
                    f"{'─' * 30}",
                    f"🎮 游戏账号：{account_name}",
                    f"💎 消耗积分：{points_to_use}",
                    f"💰 充值金额：{recharge_amount:,} 元宝",
                    f"📝 充值备注：{remark}",
                    f"{'─' * 30}",
                    f"📊 账户信息：",
                    f"🧾 新余额：{response_data.get('new_gold_pay', '未知')}",
                    f"💰 累计充值：{response_data.get('new_gold_pay_total', '未知')}",
                    f"💎 剩余积分：{user_points['points']}",
                    f"{'─' * 30}",
                    f"⏰ 充值时间：{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"
                ]
                
                if task_reward > 0:
                    content_lines.append(f"✅ 完成任务奖励：+{task_reward} 积分")
                
                yield event.plain_result(self._create_box("✨ 积分充值", "\n".join(content_lines)))
            else:
                error_msg = result.get("error", "未知错误")
                yield event.plain_result(self._create_box("❌ 充值失败", f"错误信息：{error_msg}"))
                
        except Exception as e:
            logger.error(f"充值异常：{e}")
            yield event.plain_result(self._create_box("❌ 充值异常", "请稍后重试或联系管理员"))
    
    # ========== 直接充值功能 ==========
    @filter.command("账号充值")
    async def account_recharge_cmd(self, event: AstrMessageEvent):
        """为绑定账号充值：/账号充值 金额 [备注]"""
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result(self._create_box("❌ 格式错误", 
                "📝 正确格式：/账号充值 <金额> [备注]\n"
                "💡 例如：/账号充值 1000 元宝充值"))
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
                "/绑定账号 <游戏账号>\n\n"
                "例如：/绑定账号 xhl2511686"))
            return
        
        game_account = self.bindings[qq_id]["game_account"]
        account_name = self.bindings[qq_id].get("account_name", game_account)
        
        # 执行充值
        try:
            result = await self._execute_account_recharge(game_account, amount, remark)
            
            if result.get("success"):
                # 每日任务奖励
                task_reward = 0
                if amount >= 1000:
                    task_reward = self._check_and_reward_daily_task(qq_id, "recharge_1000")
                else:
                    task_reward = self._check_and_reward_daily_task(qq_id, "recharge_any")
                
                # 记录充值日志
                recharge_id = f"R{datetime.now().strftime('%Y%m%d%H%M%S')}_{qq_id}"
                self.recharge_logs[recharge_id] = {
                    "qq_id": qq_id,
                    "game_account": game_account,
                    "account_name": account_name,
                    "amount": amount,
                    "remark": remark,
                    "recharge_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                    "api_response": result
                }
                self._save_json(self.recharge_file, self.recharge_logs)
                
                response_data = result.get("data", {})
                
                content_lines = [
                    f"✅ 充值成功！",
                    f"{'─' * 30}",
                    f"🎮 游戏账号：{account_name}",
                    f"💰 充值金额：{amount:,} 元宝",
                    f"📝 充值备注：{remark}",
                    f"{'─' * 30}",
                    f"📊 账户信息：",
                    f"🧾 新余额：{response_data.get('new_gold_pay', '未知')}",
                    f"💰 累计充值：{response_data.get('new_gold_pay_total', '未知')}",
                    f"{'─' * 30}",
                    f"⏰ 充值时间：{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"
                ]
                
                if task_reward > 0:
                    content_lines.append(f"✅ 完成任务奖励：+{task_reward} 积分")
                
                yield event.plain_result(self._create_box("✨ 充值成功", "\n".join(content_lines)))
            else:
                error_msg = result.get("error", "未知错误")
                yield event.plain_result(self._create_box("❌ 充值失败", f"错误信息：{error_msg}"))
                
        except Exception as e:
            logger.error(f"充值异常：{e}")
            yield event.plain_result(self._create_box("❌ 充值异常", "请稍后重试或联系管理员"))
    
    # ========== 查询账号功能 ==========
    @filter.command("查询账号")
    async def query_account_cmd(self, event: AstrMessageEvent):
        """查询游戏账号信息：/查询账号 [游戏账号]"""
        parts = event.message_str.strip().split()
        
        if len(parts) >= 2:
            # 查询指定账号
            game_account = parts[1]
            show_extra_info = True
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
            show_extra_info = False
        
        try:
            account_info = await self._get_account_info(game_account)
            if not account_info:
                yield event.plain_result(self._create_box("❌ 账号不存在", f"游戏账号 {game_account} 不存在"))
                return
        except Exception as e:
            logger.error(f"查询账号失败：{e}")
            yield event.plain_result(self._create_box("❌ 查询失败", "网络连接异常，请稍后重试"))
            return
        
        # 每日任务奖励（仅查询自己绑定的账号时）
        task_reward = 0
        if not show_extra_info:
            qq_id = self._get_user_id(event)
            task_reward = self._check_and_reward_daily_task(qq_id, "query_account")
        
        # 构建基本信息
        content_lines = [
            f"🎮 账号信息",
            f"{'─' * 30}",
            f"📝 游戏账号：{account_info.get('passport', '未知')}",
            f"💰 当前余额：{account_info.get('gold_pay', 0):,} 元宝",
            f"📈 累计充值：{account_info.get('gold_pay_total', 0):,} 元宝"
        ]
        
        # 添加额外信息
        if show_extra_info and account_info.get('name'):
            content_lines.append(f"👤 角色名称：{account_info['name']}")
        if show_extra_info and account_info.get('cid'):
            content_lines.append(f"🆔 角色ID：{account_info['cid']}")
        
        # 检查此账号是否被绑定
        is_bound, bound_qq, bind_info = self._is_account_already_bound(game_account)
        content_lines.append(f"{'─' * 30}")
        content_lines.append(f"🔗 绑定状态：{'已绑定' if is_bound else '未绑定'}")
        
        if is_bound:
            content_lines.append(f"📱 绑定QQ：{bound_qq}")
            content_lines.append(f"⏰ 绑定时间：{bind_info.get('bind_time', '未知')}")
        else:
            content_lines.append(f"💡 使用 /绑定账号 可绑定此账号")
        
        if task_reward > 0:
            content_lines.append(f"{'─' * 30}")
            content_lines.append(f"✅ 完成任务奖励：+{task_reward} 积分")
        
        yield event.plain_result(self._create_box("📋 账号查询", "\n".join(content_lines)))
    
    # ========== 管理员功能 ==========
    @filter.command("查看绑定")
    async def view_bindings_cmd(self, event: AstrMessageEvent):
        """查看所有绑定记录（管理员）"""
        if not self.bindings:
            yield event.plain_result(self._create_box("📝 绑定记录", "暂无绑定记录"))
            return
        
        lines = [
            f"📋 所有绑定记录",
            f"{'─' * 30}"
        ]
        
        count = 0
        for bind_qq, data in self.bindings.items():
            count += 1
            lines.append(f"#{count} QQ：{bind_qq}")
            lines.append(f"   账号：{data.get('account_name', '未知')}")
            lines.append(f"   时间：{data.get('bind_time', '未知')}")
            if 'old_account' in data:
                lines.append(f"   历史：由 {data['old_account']} 修改而来")
            lines.append("")
        
        lines.append(f"{'─' * 30}")
        lines.append(f"📊 总计：{count} 条绑定记录")
        
        yield event.plain_result(self._create_box("👑 管理功能", "\n".join(lines)))
    
    @filter.command("充值记录")
    async def recharge_history_cmd(self, event: AstrMessageEvent):
        """查看充值记录（管理员）"""
        if not self.recharge_logs:
            yield event.plain_result(self._create_box("📝 充值记录", "暂无充值记录"))
            return
        
        # 显示最近10条记录
        recent_logs = list(self.recharge_logs.items())[-10:]
        
        lines = [
            f"📊 最近10条充值记录",
            f"{'─' * 30}"
        ]
        
        total_amount = 0
        for log_id, log in recent_logs:
            lines.append(f"🆔 {log_id}")
            lines.append(f"   👤 QQ：{log.get('qq_id', '未知')}")
            lines.append(f"   🎮 账号：{log.get('account_name', '未知')}")
            lines.append(f"   💰 金额：{log.get('amount', log.get('recharge_amount', 0)):,} 元宝")
            lines.append(f"   ⏰ 时间：{log.get('recharge_time', '未知')}")
            lines.append("")
            
            total_amount += log.get('amount', log.get('recharge_amount', 0))
        
        lines.append(f"{'─' * 30}")
        lines.append(f"📈 总充值金额：{total_amount:,} 元宝")
        lines.append(f"📊 总记录数：{len(self.recharge_logs)}")
        
        yield event.plain_result(self._create_box("💰 充值记录", "\n".join(lines)))
    
    @filter.command("强制绑定")
    async def force_bind_cmd(self, event: AstrMessageEvent):
        """强制绑定：/强制绑定 QQ号 游戏账号"""
        parts = event.message_str.strip().split()
        if len(parts) < 3:
            yield event.plain_result(self._create_box("❌ 格式错误", "📝 正确格式：/强制绑定 QQ号 游戏账号\n💡 例如：/强制绑定 123456 test123"))
            return
        
        qq_id = parts[1]
        game_account = parts[2]
        
        if not qq_id.isdigit():
            yield event.plain_result(self._create_box("❌ 参数错误", "QQ号必须是数字"))
            return
        
        # 检查此游戏账号是否已被绑定
        is_bound, bound_qq, bind_info = self._is_account_already_bound(game_account)
        if is_bound:
            yield event.plain_result(self._create_box("⚠️ 账号已被绑定",
                f"🎮 游戏账号：{game_account}\n"
                f"📱 原绑定QQ：{bound_qq}\n"
                f"⏰ 绑定时间：{bind_info.get('bind_time', '未知时间')}\n\n"
                f"💡 是否要强制替换？"))
            
            # 删除原绑定
            if bound_qq in self.bindings:
                del self.bindings[bound_qq]
        
        # 验证游戏账号是否存在
        try:
            account_info = await self._get_account_info(game_account)
            if not account_info:
                yield event.plain_result(self._create_box("❌ 账号不存在", f"游戏账号 {game_account} 不存在"))
                return
        except Exception as e:
            logger.error(f"验证游戏账号失败: {e}")
            yield event.plain_result(self._create_box("❌ 验证失败", "请检查网络或联系管理员"))
            return
        
        # 检查此QQ是否已绑定其他账号
        old_account = None
        if qq_id in self.bindings:
            old_account = self.bindings[qq_id]["game_account"]
        
        # 保存绑定
        self.bindings[qq_id] = {
            "game_account": game_account,
            "account_name": account_info.get("passport", game_account),
            "bind_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "qq_id": qq_id,
            "is_forced": True
        }
        
        if old_account:
            self.bindings[qq_id]["replaced_account"] = old_account
        
        self._save_json(self.bind_file, self.bindings)
        
        account_name = account_info.get("passport", game_account)
        
        content = f"✅ 强制绑定成功！\n\n🆔 QQ号：{qq_id}\n🎮 游戏账号：{account_name}\n💰 当前余额：{account_info.get('gold_pay', 0):,} 元宝"
        
        if old_account:
            content += f"\n\n⚠️ 替换了原有绑定账号：{old_account}"
        
        yield event.plain_result(self._create_box("👑 强制绑定", content))
    
    @filter.command("添加积分")
    async def add_points_cmd(self, event: AstrMessageEvent):
        """管理员添加积分：/添加积分 QQ号 积分数量 [备注]"""
        parts = event.message_str.strip().split()
        if len(parts) < 3:
            yield event.plain_result(self._create_box("❌ 格式错误", "📝 正确格式：/添加积分 QQ号 积分数量 [备注]\n💡 例如：/添加积分 123456 100 活动奖励"))
            return
        
        target_qq = parts[1]
        
        try:
            points_to_add = int(parts[2])
            if points_to_add <= 0:
                raise ValueError("必须是正数")
            remark = " ".join(parts[3:]) if len(parts) > 3 else "管理员添加"
        except ValueError:
            yield event.plain_result(self._create_box("❌ 参数错误", "积分数量必须是正整数"))
            return
        
        if not target_qq.isdigit():
            yield event.plain_result(self._create_box("❌ 参数错误", "QQ号必须是数字"))
            return
        
        # 添加积分
        user_points = self._get_user_points(target_qq)
        user_points["points"] += points_to_add
        user_points["total_earned"] += points_to_add
        self._update_user_points(target_qq, user_points)
        
        content = (f"✅ 积分添加成功！\n\n"
                  f"📱 目标QQ：{target_qq}\n"
                  f"💰 添加积分：{points_to_add}\n"
                  f"📝 操作备注：{remark}\n"
                  f"💎 当前积分：{user_points['points']}\n"
                  f"📈 累计获得：{user_points['total_earned']}")
        
        yield event.plain_result(self._create_box("👑 积分管理", content))
    
    # ========== 系统功能 ==========
    @filter.command("测试连接")
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
