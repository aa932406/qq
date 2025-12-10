import json
import os
from astrbot.api.event import filter, AstrMessageEvent
from astrbot.api.star import Context, Star, register
from astrbot.api import logger

# 插件注册信息
@register("game_account_binder", "YourName", "游戏账号绑定与管理插件", "1.0.0")
class GameAccountBinder(Star):
    def __init__(self, context: Context):
        super().__init__(context)
        # 插件的数据存储路径
        self.data_dir = os.path.join(os.path.dirname(__file__), "data")
        os.makedirs(self.data_dir, exist_ok=True)
        self.bind_file = os.path.join(self.data_dir, "bindings.json")
        self._load_bindings()
        logger.info("游戏账号绑定插件已初始化")

    def _load_bindings(self):
        """加载绑定数据"""
        if os.path.exists(self.bind_file):
            with open(self.bind_file, 'r', encoding='utf-8') as f:
                self.bindings = json.load(f)
        else:
            self.bindings = {}
        logger.info(f"已加载 {len(self.bindings)} 条绑定记录")

    def _save_bindings(self):
        """保存绑定数据"""
        with open(self.bind_file, 'w', encoding='utf-8') as f:
            json.dump(self.bindings, f, ensure_ascii=False, indent=2)

    # ========== 核心指令定义 ==========

    @filter.command("bind")
    async def bind_account(self, event: AstrMessageEvent):
        """绑定游戏账号：格式 /bind 游戏账号"""
        args = event.message_str.strip().split()
        if len(args) < 2:
            yield event.plain_result("❌ 格式错误。请使用：`/bind 你的游戏账号`")
            return
        
        game_account = args[1]
        qq_id = str(event.sender_id)
        
        # 检查是否已绑定
        if qq_id in self.bindings:
            old_account = self.bindings[qq_id]
            yield event.plain_result(f"⚠️ 您已绑定过账号 `{old_account}`。如需更新，请先使用 `/unbind` 解绑。")
            return
        
        # 执行绑定
        self.bindings[qq_id] = game_account
        self._save_bindings()
        
        logger.info(f"用户 {qq_id} 绑定了游戏账号: {game_account}")
        yield event.plain_result(f"✅ 绑定成功！\nQQ：`{qq_id}`\n已绑定游戏账号：`{game_account}`")

    @filter.command("mybind")
    async def query_binding(self, event: AstrMessageEvent):
        """查询自己的绑定信息"""
        qq_id = str(event.sender_id)
        game_account = self.bindings.get(qq_id)
        
        if game_account:
            yield event.plain_result(f"📋 您的绑定信息：\nQQ：`{qq_id}`\n游戏账号：`{game_account}`")
        else:
            yield event.plain_result("❓ 您尚未绑定任何游戏账号。请使用 `/bind 游戏账号` 进行绑定。")

    @filter.command("unbind")
    async def unbind_account(self, event: AstrMessageEvent):
        """解绑游戏账号"""
        qq_id = str(event.sender_id)
        
        if qq_id in self.bindings:
            game_account = self.bindings.pop(qq_id)
            self._save_bindings()
            logger.info(f"用户 {qq_id} 解绑了游戏账号: {game_account}")
            yield event.plain_result(f"✅ 解绑成功！已移除账号 `{game_account}` 的绑定。")
        else:
            yield event.plain_result("❓ 您尚未绑定任何游戏账号，无需解绑。")

    @filter.command("admin_check")
    @filter.require("admin")  # 需要管理员权限
    async def admin_check_binding(self, event: AstrMessageEvent):
        """管理员：检查指定QQ的绑定信息"""
        args = event.message_str.strip().split()
        if len(args) < 2:
            yield event.plain_result("❌ 格式错误。请使用：`/admin_check QQ号`")
            return
        
        target_qq = args[1]
        game_account = self.bindings.get(target_qq)
        
        if game_account:
            yield event.plain_result(f"👑 管理员查询结果：\nQQ：`{target_qq}`\n绑定账号：`{game_account}`")
        else:
            yield event.plain_result(f"ℹ️ QQ `{target_qq}` 未绑定任何游戏账号。")

    # ========== 插件生命周期 ==========
    async def initialize(self):
        """插件启用时调用"""
        logger.info("游戏账号绑定插件已启用")

    async def terminate(self):
        """插件禁用时调用"""
        logger.info("游戏账号绑定插件已禁用")
