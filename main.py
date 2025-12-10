import json
import os
from datetime import datetime
from astrbot.api.event import filter, AstrMessageEvent, MessageEventResult
from astrbot.api.star import Context, Star, register
from astrbot.api import logger

# ========== 关键修改：去掉 @filter.at_bot() ==========
@register("game_bind", "aa932406", "游戏账号绑定插件", "1.0.0")
class GameBindPlugin(Star):
    def __init__(self, context: Context):
        super().__init__(context)
        # 初始化数据
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

    # ========== 方法1：基础指令（可能需要@机器人） ==========
    @filter.command("绑定")
    async def bind_cmd(self, event: AstrMessageEvent):
        """绑定游戏账号"""
        logger.info(f"【绑定指令】被触发，消息: {event.message_str}")
        
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result("❌ 格式：/绑定 游戏账号")
            return
        
        qq_id = str(event.sender_id)
        game_account = parts[1]
        
        if qq_id in self.bindings:
            old = self.bindings[qq_id]["game_account"]
            yield event.plain_result(f"⚠️ 您已绑定：{old}")
            return
        
        # 保存绑定
        self.bindings[qq_id] = {
            "game_account": game_account,
            "bind_time": datetime.now().strftime("%Y-%m-%d %H:%M"),
            "qq_id": qq_id
        }
        self._save_bindings()
        
        logger.info(f"【绑定成功】QQ:{qq_id} 账号:{game_account}")
        yield event.plain_result(f"✅ 绑定成功！\n游戏账号：{game_account}")

    @filter.command("我的绑定")
    async def mybind_cmd(self, event: AstrMessageEvent):
        """查询绑定"""
        logger.info(f"【我的绑定】被触发")
        
        qq_id = str(event.sender_id)
        if qq_id in self.bindings:
            data = self.bindings[qq_id]
            yield event.plain_result(f"📋 您的绑定：\n账号：{data['game_account']}\n时间：{data['bind_time']}")
        else:
            yield event.plain_result("❌ 您未绑定账号")

    @filter.command("解绑")
    async def unbind_cmd(self, event: AstrMessageEvent):
        """解绑"""
        logger.info(f"【解绑】被触发")
        
        qq_id = str(event.sender_id)
        if qq_id in self.bindings:
            del self.bindings[qq_id]
            self._save_bindings()
            yield event.plain_result("✅ 解绑成功")
        else:
            yield event.plain_result("❌ 您未绑定账号")

    @filter.command("充值验证")
    async def recharge_cmd(self, event: AstrMessageEvent):
        """充值验证"""
        logger.info(f"【充值验证】被触发")
        
        qq_id = str(event.sender_id)
        if qq_id not in self.bindings:
            yield event.plain_result("❌ 未绑定账号")
            return
        
        account = self.bindings[qq_id]["game_account"]
        yield event.plain_result(f"✅ 验证通过！\n账号：{account}\n可执行充值操作")

    # ========== 方法2：关键词触发（不需要@） ==========
    @filter.keyword("绑定账号")
    async def bind_keyword(self, event: AstrMessageEvent):
        """关键词触发绑定：说'绑定账号 游戏ID'"""
        msg = event.message_str
        if "绑定账号" in msg:
            parts = msg.split("绑定账号")
            if len(parts) > 1 and parts[1].strip():
                game_account = parts[1].strip()
                qq_id = str(event.sender_id)
                
                if qq_id in self.bindings:
                    yield event.plain_result(f"您已绑定：{self.bindings[qq_id]['game_account']}")
                else:
                    self.bindings[qq_id] = {
                        "game_account": game_account,
                        "bind_time": datetime.now().strftime("%Y-%m-%d %H:%M"),
                        "qq_id": qq_id
                    }
                    self._save_bindings()
                    yield event.plain_result(f"✅ 关键词绑定成功！\n账号：{game_account}")

    # ========== 方法3：智能检测@消息 ==========
    @filter.command("智能绑定")
    async def smart_bind(self, event: AstrMessageEvent):
        """智能绑定，自动检测是否@机器人"""
        from astrbot.api.message_components import At
        
        # 检查消息中是否@了机器人
        messages = event.get_messages()
        is_at_bot = False
        
        for msg in messages:
            if isinstance(msg, At):
                # 这里需要获取机器人ID（具体获取方式可能不同）
                # 简单版本：只要有@就认为@了机器人
                is_at_bot = True
                break
        
        if is_at_bot:
            yield event.plain_result("检测到@机器人，执行绑定...")
            # 这里可以调用绑定逻辑
        else:
            yield event.plain_result("请先@机器人再使用此命令")

    # ========== 测试指令 ==========
    @filter.command("测试")
    async def test_cmd(self, event: AstrMessageEvent):
        """测试插件是否工作"""
        logger.info("【测试指令】被触发！！！")
        yield event.plain_result("✅ 插件工作正常！")

    async def initialize(self):
        logger.info("【游戏绑定插件】已启用！！！")
        
    async def terminate(self):
        logger.info("游戏绑定插件已禁用")
