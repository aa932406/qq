import json
import os
from datetime import datetime
from astrbot.api.event import filter, AstrMessageEvent, MessageEventResult
from astrbot.api.star import Context, Star, register
from astrbot.api import logger

# 注册插件 - 修改这里！
@register("game_account_binder", "aa932406", "游戏账号绑定与管理插件", "1.0.0")
class GameAccountBinder(Star):
    def __init__(self, context: Context):
        super().__init__(context)
        # 初始化数据存储
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
        except:
            pass
        return {}

    def _save_bindings(self):
        """保存绑定数据"""
        try:
            with open(self.bind_file, 'w', encoding='utf-8') as f:
                json.dump(self.bindings, f, ensure_ascii=False, indent=2)
        except Exception as e:
            logger.error(f"保存失败: {e}")

    async def initialize(self):
        """插件启用时调用"""
        logger.info("游戏账号绑定插件已启用")

    # ========== 用户指令 ==========
    @filter.command("绑定")
    async def bind_account(self, event: AstrMessageEvent):
        """绑定游戏账号：/绑定 游戏账号"""
        user_name = event.get_sender_name()
        message_str = event.message_str
        
        parts = message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result("❌ 格式错误。请使用：/绑定 游戏账号")
            return
        
        game_account = parts[1]
        qq_id = str(event.sender_id)
        
        # 检查是否已绑定
        if qq_id in self.bindings:
            old_account = self.bindings[qq_id]["game_account"]
            yield event.plain_result(f"⚠️ {user_name}，您已绑定账号：{old_account}")
            return
        
        # 创建绑定记录
        self.bindings[qq_id] = {
            "game_account": game_account,
            "user_name": user_name,
            "bind_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "qq_id": qq_id
        }
        self._save_bindings()
        
        logger.info(f"用户 {qq_id} 绑定了游戏账号: {game_account}")
        yield event.plain_result(f"✅ 绑定成功！\n账号：{game_account}")

    @filter.command("我的绑定")
    async def my_binding(self, event: AstrMessageEvent):
        """查询绑定信息：/我的绑定"""
        user_name = event.get_sender_name()
        qq_id = str(event.sender_id)
        
        if qq_id in self.bindings:
            data = self.bindings[qq_id]
            yield event.plain_result(
                f"📋 {user_name}的绑定信息：\n"
                f"🎮 游戏账号：{data['game_account']}\n"
                f"⏰ 绑定时间：{data['bind_time']}"
            )
        else:
            yield event.plain_result(f"❓ {user_name}，您尚未绑定游戏账号")

    @filter.command("解绑")
    async def unbind_account(self, event: AstrMessageEvent):
        """解绑游戏账号：/解绑"""
        user_name = event.get_sender_name()
        qq_id = str(event.sender_id)
        
        if qq_id in self.bindings:
            game_account = self.bindings[qq_id]["game_account"]
            del self.bindings[qq_id]
            self._save_bindings()
            
            logger.info(f"用户 {qq_id} 解绑了账号: {game_account}")
            yield event.plain_result(f"✅ {user_name}，解绑成功！")
        else:
            yield event.plain_result(f"❓ {user_name}，您未绑定账号")

    # ========== 充值验证 ==========
    @filter.command("充值验证")
    async def recharge_check(self, event: AstrMessageEvent):
        """充值验证：/充值验证"""
        user_name = event.get_sender_name()
        qq_id = str(event.sender_id)
        
        if qq_id not in self.bindings:
            yield event.plain_result(f"❌ {user_name}，您未绑定游戏账号")
            return
        
        game_account = self.bindings[qq_id]["game_account"]
        yield event.plain_result(
            f"✅ {user_name}，验证通过！\n"
            f"🎮 游戏账号：{game_account}\n"
            f"📝 状态：可以执行充值操作"
        )

    async def terminate(self):
        """插件禁用时调用"""
        logger.info("游戏账号绑定插件已禁用")
