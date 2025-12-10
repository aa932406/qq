import json
import os
from datetime import datetime
from astrbot.api.event import filter, AstrMessageEvent, MessageEventResult
from astrbot.api.star import Context, Star, register
from astrbot.api import logger

@register("game_bind", "aa932406", "游戏账号绑定插件", "1.0.0")
class GameBindPlugin(Star):
    def __init__(self, context: Context):
        super().__init__(context)
        self.data_dir = os.path.join(os.path.dirname(__file__), "data")
        os.makedirs(self.data_dir, exist_ok=True)
        self.bind_file = os.path.join(self.data_dir, "bindings.json")
        self.bindings = self._load_bindings()
        logger.info("【游戏绑定插件】初始化完成！")
    
    def _load_bindings(self):
        try:
            if os.path.exists(self.bind_file):
                with open(self.bind_file, 'r', encoding='utf-8') as f:
                    return json.load(f)
        except:
            pass
        return {}
    
    def _save_bindings(self):
        try:
            with open(self.bind_file, 'w', encoding='utf-8') as f:
                json.dump(self.bindings, f, ensure_ascii=False, indent=2)
        except Exception as e:
            logger.error(f"保存失败: {e}")

    async def initialize(self):
        """插件启用"""
        logger.info("【游戏绑定插件】已启用")
        
    # ========== 修复：旧版API的正确参数格式 ==========
    # 旧版AstrBot的命令处理器接收3个参数：bot, event, args
    
    @filter.command("绑定")
    async def bind_cmd(self, bot, event, args):
        """绑定游戏账号"""
        logger.info(f"【绑定指令】被触发，args: {args}")
        
        if not args or len(args) < 1:
            yield MessageEventResult.reply("❌ 格式：/绑定 游戏账号")
            return
        
        game_account = args[0]
        
        # 获取用户ID - 旧版可能是event对象或字典
        try:
            # 尝试不同方式获取用户ID
            if hasattr(event, 'user_id'):
                qq_id = str(event.user_id)
            elif hasattr(event, 'from_id'):
                qq_id = str(event.from_id)
            elif hasattr(event, 'sender_id'):
                qq_id = str(event.sender_id)
            elif isinstance(event, dict) and 'user_id' in event:
                qq_id = str(event['user_id'])
            else:
                # 最后尝试从bot获取
                qq_id = str(getattr(bot, 'user_id', 'unknown'))
        except:
            qq_id = "unknown"
        
        if qq_id in self.bindings:
            old = self.bindings[qq_id]["game_account"]
            yield MessageEventResult.reply(f"⚠️ 您已绑定：{old}")
            return
        
        # 保存绑定
        self.bindings[qq_id] = {
            "game_account": game_account,
            "bind_time": datetime.now().strftime("%Y-%m-%d %H:%M"),
            "qq_id": qq_id
        }
        self._save_bindings()
        
        logger.info(f"绑定成功：QQ:{qq_id} 账号:{game_account}")
        yield MessageEventResult.reply(f"✅ 绑定成功！游戏账号：{game_account}")

    @filter.command("我的绑定")
    async def mybind_cmd(self, bot, event, args):
        """查询绑定"""
        logger.info(f"【我的绑定】被触发")
        
        # 获取用户ID
        try:
            if hasattr(event, 'user_id'):
                qq_id = str(event.user_id)
            elif hasattr(event, 'from_id'):
                qq_id = str(event.from_id)
            else:
                qq_id = "unknown"
        except:
            qq_id = "unknown"
        
        if qq_id in self.bindings:
            data = self.bindings[qq_id]
            yield MessageEventResult.reply(f"📋 您的绑定：\n账号：{data['game_account']}\n时间：{data['bind_time']}")
        else:
            yield MessageEventResult.reply("❌ 您未绑定账号")

    @filter.command("解绑")
    async def unbind_cmd(self, bot, event, args):
        """解绑"""
        logger.info(f"【解绑】被触发")
        
        # 获取用户ID
        try:
            if hasattr(event, 'user_id'):
                qq_id = str(event.user_id)
            elif hasattr(event, 'from_id'):
                qq_id = str(event.from_id)
            else:
                qq_id = "unknown"
        except:
            qq_id = "unknown"
        
        if qq_id in self.bindings:
            del self.bindings[qq_id]
            self._save_bindings()
            yield MessageEventResult.reply("✅ 解绑成功")
        else:
            yield MessageEventResult.reply("❌ 您未绑定账号")

    @filter.command("充值验证")
    async def recharge_cmd(self, bot, event, args):
        """充值验证"""
        logger.info(f"【充值验证】被触发")
        
        # 获取用户ID
        try:
            if hasattr(event, 'user_id'):
                qq_id = str(event.user_id)
            elif hasattr(event, 'from_id'):
                qq_id = str(event.from_id)
            else:
                qq_id = "unknown"
        except:
            qq_id = "unknown"
        
        if qq_id not in self.bindings:
            yield MessageEventResult.reply("❌ 未绑定账号")
            return
        
        account = self.bindings[qq_id]["game_account"]
        yield MessageEventResult.reply(f"✅ 验证通过！账号：{account}")

    @filter.command("测试")
    async def test_cmd(self, bot, event, args):
        """测试插件"""
        logger.info("【测试指令】被触发")
        
        # 尝试获取用户信息
        user_info = ""
        try:
            if hasattr(event, 'user_id'):
                user_info = f"\n用户ID：{event.user_id}"
            if hasattr(event, 'sender') and hasattr(event.sender, 'nickname'):
                user_info += f"\n昵称：{event.sender.nickname}"
        except:
            pass
        
        yield MessageEventResult.reply(f"✅ 插件工作正常！{user_info}")

    async def terminate(self):
        """插件禁用"""
        logger.info("游戏绑定插件已禁用")
