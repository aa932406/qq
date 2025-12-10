import json
import os
from datetime import datetime
from astrbot.api.event import filter, AstrMessageEvent
from astrbot.api.star import Context, Star, register
from astrbot.api import logger

@register("game_account_binder", "你的名字", "游戏账号绑定与管理插件", "1.0.0")
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

    # ========== 用户指令 ==========
    @filter.command("绑定")
    async def bind_account(self, event: AstrMessageEvent):
        """绑定游戏账号。用法：/绑定 游戏账号"""
        args = event.message_str.strip().split()
        if len(args) < 2:
            yield event.plain_result("❌ 格式错误。请使用：/绑定 游戏账号")
            return

        game_account = args[1]
        qq_id = str(event.sender_id)
        user_name = event.get_sender_name()

        # 检查是否已绑定
        if qq_id in self.bindings:
            old_account = self.bindings[qq_id]['game_account']
            yield event.plain_result(f"⚠️ {user_name}，您已绑定过账号 [{old_account}]。如需更改，请先使用 /解绑")
            return

        # 创建绑定记录
        self.bindings[qq_id] = {
            "game_account": game_account,
            "bind_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "user_name": user_name
        }
        self._save_bindings()

        logger.info(f"用户 {qq_id}({user_name}) 绑定了游戏账号: {game_account}")
        yield event.plain_result(f"✅ 绑定成功！\n👤 用户：{user_name}\n🎮 游戏账号：{game_account}\n⏰ 时间：{self.bindings[qq_id]['bind_time']}")

    @filter.command("我的绑定")
    async def my_binding(self, event: AstrMessageEvent):
        """查询你绑定的游戏账号"""
        qq_id = str(event.sender_id)
        
        if qq_id in self.bindings:
            bind_data = self.bindings[qq_id]
            yield event.plain_result(
                f"📋 你的绑定信息：\n"
                f"👤 用户：{bind_data['user_name']}\n"
                f"🎮 游戏账号：{bind_data['game_account']}\n"
                f"⏰ 绑定时间：{bind_data['bind_time']}"
            )
        else:
            yield event.plain_result("❓ 你还没有绑定任何游戏账号。\n使用 /绑定 游戏账号 进行绑定")

    @filter.command("解绑")
    async def unbind_account(self, event: AstrMessageEvent):
        """解绑游戏账号"""
        qq_id = str(event.sender_id)
        user_name = event.get_sender_name()
        
        if qq_id in self.bindings:
            game_account = self.bindings[qq_id]['game_account']
            del self.bindings[qq_id]
            self._save_bindings()
            
            logger.info(f"用户 {qq_id}({user_name}) 解绑了游戏账号: {game_account}")
            yield event.plain_result(f"✅ {user_name}，解绑成功！\n已移除账号 [{game_account}] 的绑定")
        else:
            yield event.plain_result(f"❓ {user_name}，您尚未绑定任何游戏账号")

    # ========== 管理员指令 ==========
    @filter.command("查询绑定")
    @filter.require("admin")
    async def admin_check(self, event: AstrMessageEvent):
        """管理员：查询指定QQ的绑定信息"""
        args = event.message_str.strip().split()
        if len(args) < 2:
            yield event.plain_result("❌ 格式错误。请使用：/查询绑定 QQ号")
            return

        target_qq = args[1]
        
        if target_qq in self.bindings:
            bind_data = self.bindings[target_qq]
            yield event.plain_result(
                f"👑 管理员查询结果：\n"
                f"📱 QQ：{target_qq}\n"
                f"👤 用户名：{bind_data['user_name']}\n"
                f"🎮 游戏账号：{bind_data['game_account']}\n"
                f"⏰ 绑定时间：{bind_data['bind_time']}"
            )
        else:
            yield event.plain_result(f"ℹ️ QQ {target_qq} 未绑定任何游戏账号")

    @filter.command("绑定总数")
    @filter.require("admin")
    async def binding_count(self, event: AstrMessageEvent):
        """管理员：查看总绑定数量"""
        count = len(self.bindings)
        yield event.plain_result(f"📊 当前总绑定数量：{count} 条")

    # ========== 充值验证框架 ==========
    @filter.command("充值测试")
    async def recharge_test(self, event: AstrMessageEvent):
        """充值测试（验证绑定）"""
        qq_id = str(event.sender_id)
        user_name = event.get_sender_name()
        
        # 验证是否已绑定
        if qq_id not in self.bindings:
            yield event.plain_result("❌ 充值失败：您尚未绑定游戏账号！\n请先使用 /绑定 游戏账号")
            return

        game_account = self.bindings[qq_id]['game_account']
        
        # 这里可以调用您的游戏服务器充值API
        # 示例：await self.call_game_recharge_api(game_account, amount)
        
        yield event.plain_result(
            f"✅ 充值验证通过！\n"
            f"用户：{user_name}\n"
            f"游戏账号：{game_account}\n"
            f"⚠️ 注意：这是测试功能，实际充值需要接入游戏服务器API"
        )

    async def initialize(self):
        """插件启用时调用"""
        logger.info("游戏账号绑定插件已启用")
        
    async def terminate(self):
        """插件禁用时调用"""
        logger.info("游戏账号绑定插件已禁用")
