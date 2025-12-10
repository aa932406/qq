import json
import os
from datetime import datetime
from astrbot.api.event import filter, AstrMessageEvent
from astrbot.api.star import Context, Star, register
from astrbot.api import logger

@register("game_binder", "aa932406", "游戏账号绑定插件", "1.0.0")
class GameAccountBinder(Star):
    def __init__(self, context: Context):
        super().__init__(context)
        # 插件数据存储路径
        self.data_dir = os.path.join(os.path.dirname(__file__), "data")
        os.makedirs(self.data_dir, exist_ok=True)
        self.bind_file = os.path.join(self.data_dir, "bindings.json")
        self.bindings = self._load_bindings()
        logger.info("游戏账号绑定插件初始化完成")

    def _load_bindings(self):
        """加载绑定数据"""
        try:
            if os.path.exists(self.bind_file):
                with open(self.bind_file, 'r', encoding='utf-8') as f:
                    return json.load(f)
            else:
                return {}
        except Exception as e:
            logger.error(f"加载绑定数据失败: {e}")
            return {}

    def _save_bindings(self):
        """保存绑定数据"""
        try:
            with open(self.bind_file, 'w', encoding='utf-8') as f:
                json.dump(self.bindings, f, ensure_ascii=False, indent=2)
        except Exception as e:
            logger.error(f"保存绑定数据失败: {e}")

    # ========== 核心指令 ==========
    @filter.command("绑定")
    async def bind_account(self, event: AstrMessageEvent):
        """绑定游戏账号：/绑定 游戏账号"""
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result("❌ 格式错误。请使用：/绑定 游戏账号")
            return
        
        qq_id = str(event.sender_id)
        user_name = event.get_sender_name()
        game_account = parts[1]
        
        # 检查是否已绑定
        if qq_id in self.bindings:
            old_account = self.bindings[qq_id]["game_account"]
            yield event.plain_result(f"⚠️ {user_name}，您已绑定账号：{old_account}\n如需更改，请先使用 /解绑")
            return
        
        # 创建绑定记录
        self.bindings[qq_id] = {
            "game_account": game_account,
            "user_name": user_name,
            "bind_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "qq_id": qq_id
        }
        self._save_bindings()
        
        logger.info(f"用户 {qq_id}({user_name}) 绑定了游戏账号: {game_account}")
        yield event.plain_result(f"✅ 绑定成功！\n👤 用户：{user_name}\n🎮 游戏账号：{game_account}\n⏰ 时间：{self.bindings[qq_id]['bind_time']}")

    @filter.command("我的绑定")
    async def my_binding(self, event: AstrMessageEvent):
        """查询绑定信息"""
        qq_id = str(event.sender_id)
        
        if qq_id in self.bindings:
            data = self.bindings[qq_id]
            yield event.plain_result(
                f"📋 您的绑定信息：\n"
                f"👤 用户：{data['user_name']}\n"
                f"📱 QQ：{data['qq_id']}\n"
                f"🎮 游戏账号：{data['game_account']}\n"
                f"⏰ 绑定时间：{data['bind_time']}"
            )
        else:
            yield event.plain_result("❓ 您尚未绑定游戏账号\n请使用：/绑定 游戏账号")

    @filter.command("解绑")
    async def unbind_account(self, event: AstrMessageEvent):
        """解绑游戏账号"""
        qq_id = str(event.sender_id)
        user_name = event.get_sender_name()
        
        if qq_id in self.bindings:
            game_account = self.bindings[qq_id]["game_account"]
            del self.bindings[qq_id]
            self._save_bindings()
            
            logger.info(f"用户 {qq_id}({user_name}) 解绑了账号: {game_account}")
            yield event.plain_result(f"✅ {user_name}，解绑成功！\n已移除账号：{game_account}")
        else:
            yield event.plain_result(f"❓ {user_name}，您未绑定任何账号")

    # ========== 充值相关功能 ==========
    @filter.command("充值验证")
    async def recharge_check(self, event: AstrMessageEvent):
        """验证绑定状态用于充值"""
        qq_id = str(event.sender_id)
        
        if qq_id not in self.bindings:
            yield event.plain_result("❌ 充值失败：未绑定游戏账号\n请先使用 /绑定 游戏账号")
            return
        
        game_account = self.bindings[qq_id]["game_account"]
        
        # 这里可以添加调用游戏服务器API的代码
        # 示例：result = await self.call_recharge_api(game_account)
        
        yield event.plain_result(
            f"✅ 验证通过！\n"
            f"🎮 游戏账号：{game_account}\n"
            f"👤 用户：{self.bindings[qq_id]['user_name']}\n"
            f"📝 状态：可以执行充值操作\n\n"
            f"💡 提示：实际充值需要接入游戏服务器API"
        )

    # ========== 管理员功能 ==========
    @filter.command("查询绑定")
    @filter.require("admin")
    async def admin_query(self, event: AstrMessageEvent):
        """管理员查询绑定信息：/查询绑定 QQ号"""
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result("❌ 格式：/查询绑定 QQ号")
            return
        
        target_qq = parts[1]
        
        if target_qq in self.bindings:
            data = self.bindings[target_qq]
            yield event.plain_result(
                f"👑 管理员查询结果：\n"
                f"📱 QQ：{target_qq}\n"
                f"👤 用户：{data['user_name']}\n"
                f"🎮 游戏账号：{data['game_account']}\n"
                f"⏰ 绑定时间：{data['bind_time']}"
            )
        else:
            yield event.plain_result(f"ℹ️ QQ {target_qq} 未绑定账号")

    @filter.command("绑定统计")
    @filter.require("admin")
    async def bind_stats(self, event: AstrMessageEvent):
        """查看绑定统计"""
        count = len(self.bindings)
        yield event.plain_result(f"📊 当前绑定总数：{count} 条")

    async def initialize(self):
        """插件启用"""
        logger.info("游戏账号绑定插件已启用")
        
    async def terminate(self):
        """插件禁用"""
        logger.info("游戏账号绑定插件已禁用")
