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
        """插件启用时注册事件监听器"""
        # 根据官方指南注册消息事件监听
        self.register_listener(
            event_type="message",
            listener=self.handle_message
        )
        logger.info("【游戏绑定插件】已启用并注册监听器")

    # ========== 主消息处理函数 ==========
    async def handle_message(self, event: AstrMessageEvent):
        """处理所有消息事件"""
        # 获取消息文本
        message_str = event.message_str.strip()
        
        # 处理 /绑定 指令
        if message_str.startswith("/绑定"):
            await self.process_bind(event)
        
        # 处理 /我的绑定 指令
        elif message_str.startswith("/我的绑定"):
            await self.process_my_bind(event)
        
        # 处理 /解绑 指令
        elif message_str.startswith("/解绑"):
            await self.process_unbind(event)
        
        # 处理 /充值验证 指令
        elif message_str.startswith("/充值验证"):
            await self.process_recharge(event)
        
        # 处理 /测试 指令
        elif message_str.startswith("/测试"):
            await self.process_test(event)

    # ========== 具体指令处理函数 ==========
    async def process_bind(self, event: AstrMessageEvent):
        """处理绑定指令"""
        logger.info(f"处理绑定指令")
        
        # 解析消息：/绑定 游戏账号
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result("❌ 格式：/绑定 游戏账号")
            return
        
        # 获取用户ID（根据官方指南的方式）
        user_id = str(event.user_id)  # 官方文档显示使用 user_id
        game_account = parts[1]
        
        if user_id in self.bindings:
            old_account = self.bindings[user_id]["game_account"]
            yield event.plain_result(f"⚠️ 您已绑定账号：{old_account}")
            return
        
        # 创建绑定记录
        self.bindings[user_id] = {
            "game_account": game_account,
            "bind_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "qq_id": user_id
        }
        self._save_bindings()
        
        logger.info(f"绑定成功：QQ:{user_id} 账号:{game_account}")
        yield event.plain_result(f"✅ 绑定成功！\n游戏账号：{game_account}")

    async def process_my_bind(self, event: AstrMessageEvent):
        """处理查询绑定指令"""
        logger.info(f"处理查询绑定指令")
        
        user_id = str(event.user_id)
        
        if user_id in self.bindings:
            data = self.bindings[user_id]
            result = f"📋 您的绑定信息：\n游戏账号：{data['game_account']}\n绑定时间：{data['bind_time']}"
            yield event.plain_result(result)
        else:
            yield event.plain_result("❌ 您尚未绑定游戏账号")

    async def process_unbind(self, event: AstrMessageEvent):
        """处理解绑指令"""
        logger.info(f"处理解绑指令")
        
        user_id = str(event.user_id)
        
        if user_id in self.bindings:
            del self.bindings[user_id]
            self._save_bindings()
            yield event.plain_result("✅ 解绑成功")
        else:
            yield event.plain_result("❌ 您未绑定任何账号")

    async def process_recharge(self, event: AstrMessageEvent):
        """处理充值验证指令"""
        logger.info(f"处理充值验证指令")
        
        user_id = str(event.user_id)
        
        if user_id not in self.bindings:
            yield event.plain_result("❌ 您尚未绑定游戏账号，无法充值")
            return
        
        game_account = self.bindings[user_id]["game_account"]
        yield event.plain_result(f"✅ 验证通过！\n游戏账号：{game_account}\n可以进行充值操作")

    async def process_test(self, event: AstrMessageEvent):
        """处理测试指令"""
        logger.info(f"处理测试指令")
        
        # 返回用户信息用于调试
        user_id = str(event.user_id)
        
        # 获取用户名称（如果可用）
        try:
            user_name = event.get_sender_name()
        except:
            user_name = "用户"
        
        yield event.plain_result(f"✅ 插件工作正常！\n用户：{user_name}\nQQ：{user_id}")

    async def terminate(self):
        """插件禁用"""
        logger.info("游戏绑定插件已禁用")
