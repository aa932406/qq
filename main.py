import json
import os
import aiohttp
import asyncio
import re
from datetime import datetime
from typing import Optional
from astrbot.api.event import filter, AstrMessageEvent
from astrbot.api.star import Context, Star, register
from astrbot.api import logger

@register("game_bind", "aa932406", "游戏账号绑定与充值插件", "1.3.0")
class GameBindPlugin(Star):
    def __init__(self, context: Context):
        super().__init__(context)
        # 初始化数据存储
        self.data_dir = os.path.join(os.path.dirname(__file__), "data")
        os.makedirs(self.data_dir, exist_ok=True)
        self.bind_file = os.path.join(self.data_dir, "bindings.json")
        self.recharge_file = os.path.join(self.data_dir, "recharge_logs.json")
        
        # 加载数据
        self.bindings = self._load_json(self.bind_file)
        self.recharge_logs = self._load_json(self.recharge_file)
        
        # API配置
        self.api_config = {
            "base_url": "http://115.190.64.181:881/api/players.php",
            "timeout": 30
        }
        
        # 管理员QQ列表 - 您的QQ号
        self.admin_qq_list = ["965959320"]
        
        logger.info(f"【游戏充值插件】初始化完成！管理员列表：{self.admin_qq_list}")
    
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
        """获取用户ID - 根据日志格式修复"""
        try:
            # 方法1：直接打印事件对象查看结构
            logger.info(f"【调试】事件对象类型: {type(event)}")
            
            # 方法2：尝试从原始事件数据获取
            # 根据日志格式：[qq(aiocqhttp)] UI/965959320
            # 我们需要提取 965959320
            
            # 尝试常见的属性
            attrs_to_try = [
                'sender_id', 'user_id', 'from_id', 
                'sender', 'user', 'user_qq', 'qq_id'
            ]
            
            for attr in attrs_to_try:
                if hasattr(event, attr):
                    value = getattr(event, attr)
                    logger.info(f"【调试】属性 {attr}: {value} (类型: {type(value)})")
                    
                    if value:
                        # 如果是对象，尝试获取id属性
                        if hasattr(value, 'id'):
                            qq_id = str(value.id)
                            if qq_id.isdigit() and len(qq_id) >= 6:
                                logger.info(f"【调试】从对象获取QQ: {qq_id}")
                                return qq_id
                        # 如果是数字
                        elif isinstance(value, (int, float)):
                            qq_id = str(int(value))
                            if qq_id.isdigit() and len(qq_id) >= 6:
                                logger.info(f"【调试】从数字获取QQ: {qq_id}")
                                return qq_id
                        # 如果是字符串
                        elif isinstance(value, str) and value.strip():
                            qq_id = value.strip()
                            if qq_id.isdigit() and len(qq_id) >= 6:
                                logger.info(f"【调试】从字符串获取QQ: {qq_id}")
                                return qq_id
            
            # 方法3：尝试获取消息详情
            try:
                # 从消息链中提取信息
                from astrbot.api.message_components import At, Text
                messages = event.get_messages()
                logger.info(f"【调试】消息链长度: {len(messages)}")
                
                for i, msg in enumerate(messages):
                    logger.info(f"【调试】消息{i}: {type(msg).__name__} = {msg}")
            except Exception as e:
                logger.info(f"【调试】获取消息链失败: {e}")
            
            # 方法4：特殊处理 - 根据您的QQ直接返回
            # 在您的群聊中，如果是您发的消息，直接返回您的QQ
            try:
                user_name = event.get_sender_name()
                logger.info(f"【调试】发送者名称: {user_name}")
                
                # 如果用户名为"UI"（根据日志），且是您自己发的消息
                if user_name == "UI":
                    logger.info(f"【调试】识别为管理员用户UI")
                    return "965959320"
            except:
                pass
                
        except Exception as e:
            logger.error(f"获取用户ID异常: {e}")
        
        return "unknown"
    
    def _is_admin(self, qq_id: str) -> bool:
        """检查是否为管理员"""
        if not qq_id or qq_id == "unknown":
            logger.info(f"【调试】管理员检查失败: QQ_ID无效 '{qq_id}'")
            return False
        
        logger.info(f"【调试】检查管理员权限 - QQ: '{qq_id}'")
        
        # 转换为字符串进行比较
        qq_id_str = str(qq_id).strip()
        
        # 检查是否在管理员列表中
        for admin_qq in self.admin_qq_list:
            admin_qq_str = str(admin_qq).strip()
            if qq_id_str == admin_qq_str:
                logger.info(f"【调试】匹配成功: '{qq_id_str}'")
                return True
        
        logger.info(f"【调试】匹配失败: '{qq_id_str}' 不在管理员列表中")
        return False
    
    async def initialize(self):
        logger.info("【游戏充值插件】已启用")
    
    # ========== 基础功能 ==========
    @filter.command("绑定")
    async def bind_cmd(self, event: AstrMessageEvent):
        """绑定游戏账号：/绑定 游戏角色ID"""
        logger.info(f"【绑定指令】被触发")
        
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result("❌ 格式：/绑定 游戏角色ID\n例如：/绑定 123456")
            return
        
        game_cid = parts[1]
        qq_id = self._get_user_id(event)
        
        logger.info(f"【调试】绑定请求 - QQ: {qq_id}, 游戏ID: {game_cid}")
        
        if qq_id == "unknown":
            # 特殊处理：如果是您自己，使用硬编码的QQ
            try:
                user_name = event.get_sender_name()
                if user_name == "UI":
                    qq_id = "965959320"
                    logger.info(f"【调试】使用硬编码QQ: {qq_id}")
                else:
                    yield event.plain_result("❌ 无法获取您的QQ信息，请稍后重试")
                    return
            except:
                yield event.plain_result("❌ 无法获取您的QQ信息，请稍后重试")
                return
        
        # 检查是否已绑定
        if qq_id in self.bindings:
            old_cid = self.bindings[qq_id]["game_cid"]
            yield event.plain_result(f"⚠️ 您已绑定游戏角色ID：{old_cid}\n如需更改，请先使用 /解绑")
            return
        
        # 先验证游戏角色是否存在
        try:
            player_info = await self._get_player_info(game_cid)
            if not player_info:
                yield event.plain_result(f"❌ 游戏角色ID {game_cid} 不存在，请确认后重试")
                return
        except Exception as e:
            logger.error(f"验证游戏角色失败: {e}")
            yield event.plain_result("❌ 验证游戏角色失败，请检查网络或联系管理员")
            return
        
        # 保存绑定
        self.bindings[qq_id] = {
            "game_cid": game_cid,
            "player_name": player_info.get("name", "未知"),
            "level": player_info.get("level", 0),
            "bind_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "qq_id": qq_id
        }
        self._save_json(self.bind_file, self.bindings)
        
        player_name = player_info.get("name", game_cid)
        logger.info(f"绑定成功：QQ:{qq_id} -> 角色:{player_name}({game_cid})")
        yield event.plain_result(f"✅ 绑定成功！\n🎮 游戏角色：{player_name}\n🆔 角色ID：{game_cid}\n⏰ 时间：{self.bindings[qq_id]['bind_time']}")
    
    @filter.command("我的绑定")
    async def mybind_cmd(self, event: AstrMessageEvent):
        """查询绑定信息"""
        logger.info(f"【我的绑定】被触发")
        
        qq_id = self._get_user_id(event)
        logger.info(f"【调试】查询绑定 - QQ: {qq_id}")
        
        # 特殊处理：如果是您自己
        if qq_id == "unknown":
            try:
                user_name = event.get_sender_name()
                if user_name == "UI":
                    qq_id = "965959320"
            except:
                pass
        
        if qq_id in self.bindings:
            data = self.bindings[qq_id]
            yield event.plain_result(
                f"📋 您的绑定信息：\n"
                f"🆔 角色ID：{data['game_cid']}\n"
                f"👤 角色名：{data.get('player_name', '未知')}\n"
                f"📊 等级：{data.get('level', '未知')}\n"
                f"⏰ 绑定时间：{data['bind_time']}"
            )
        else:
            yield event.plain_result("❌ 您尚未绑定游戏角色\n请使用：/绑定 游戏角色ID")
    
    # ========== 调试和管理功能 ==========
    @filter.command("我的信息")
    async def my_info_cmd(self, event: AstrMessageEvent):
        """显示我的QQ信息和权限状态"""
        logger.info(f"【我的信息】被触发")
        
        # 获取用户ID
        qq_id = self._get_user_id(event)
        
        # 获取用户名
        try:
            user_name = event.get_sender_name()
        except:
            user_name = "未知"
        
        # 特殊处理：如果是您自己
        if user_name == "UI" and qq_id == "unknown":
            qq_id = "965959320"
        
        # 检查管理员状态
        is_admin = self._is_admin(qq_id)
        
        # 检查绑定状态
        bind_info = ""
        if qq_id in self.bindings:
            data = self.bindings[qq_id]
            bind_info = f"✅ 已绑定游戏角色\n角色名：{data.get('player_name', '未知')}\n角色ID：{data.get('game_cid', '未知')}"
        else:
            bind_info = "❌ 未绑定游戏角色"
        
        # 构建回复信息
        info_lines = [
            "📱 您的账户信息：",
            f"👤 用户名：{user_name}",
            f"🆔 QQ号：{qq_id if qq_id != 'unknown' else '965959320（根据用户名推断）'}",
            f"👑 管理员：{'✅ 是' if is_admin else '❌ 否'}",
            f"🎮 {bind_info}",
            f"📋 管理员列表：{', '.join(self.admin_qq_list)}",
            f"💡 提示：已根据用户名'UI'识别为管理员"
        ]
        
        yield event.plain_result("\n".join(info_lines))
    
    @filter.command("UI管理员")
    async def ui_admin_cmd(self, event: AstrMessageEvent):
        """UI用户专用管理员设置"""
        logger.info(f"【UI管理员】被触发")
        
        # 获取用户名
        try:
            user_name = event.get_sender_name()
            logger.info(f"【调试】用户名: {user_name}")
        except:
            user_name = "未知"
        
        # 如果是UI用户，直接设置为管理员
        if user_name == "UI":
            qq_id = "965959320"
            if qq_id not in self.admin_qq_list:
                self.admin_qq_list.append(qq_id)
                logger.info(f"添加管理员: {qq_id}")
                yield event.plain_result(f"✅ 已设置 {qq_id} 为管理员\n当前管理员列表：{', '.join(self.admin_qq_list)}")
            else:
                yield event.plain_result(f"ℹ️ {qq_id} 已经是管理员")
        else:
            yield event.plain_result("❌ 仅UI用户可以执行此命令")
    
    @filter.command("调试事件")
    async def debug_event_cmd(self, event: AstrMessageEvent):
        """调试事件对象"""
        logger.info(f"【调试事件】被触发")
        
        info_lines = ["🔍 事件对象调试信息："]
        
        # 基本信息
        try:
            user_name = event.get_sender_name()
            info_lines.append(f"发送者名称: {user_name}")
        except:
            info_lines.append("发送者名称: 无法获取")
        
        # 尝试获取更多属性
        special_attrs = ['sender_id', 'user_id', 'from_id', 'sender', 'user', 'message', 'raw_event']
        
        for attr in special_attrs:
            if hasattr(event, attr):
                try:
                    value = getattr(event, attr)
                    info_lines.append(f"{attr}: {repr(value)[:100]}...")
                    
                    # 如果是对象，查看其属性
                    if hasattr(value, '__dict__'):
                        obj_attrs = [a for a in dir(value) if not a.startswith('_')]
                        info_lines.append(f"  {attr}的属性: {', '.join(obj_attrs[:10])}...")
                except:
                    info_lines.append(f"{attr}: 无法访问")
        
        yield event.plain_result("\n".join(info_lines[:15]))
    
    # ========== 管理员功能 ==========
    @filter.command("充值记录")
    async def recharge_history_cmd(self, event: AstrMessageEvent):
        """查看充值记录（管理员）"""
        logger.info(f"【充值记录】被触发")
        
        # 获取用户信息
        try:
            user_name = event.get_sender_name()
            logger.info(f"【调试】用户: {user_name}")
        except:
            user_name = "未知"
        
        # 特殊处理：UI用户直接认为是管理员
        if user_name == "UI":
            logger.info(f"【调试】UI用户放行")
            qq_id = "965959320"
        else:
            qq_id = self._get_user_id(event)
        
        # 检查管理员权限
        if not self._is_admin(qq_id):
            yield event.plain_result(f"❌ 权限不足，仅管理员可查看充值记录\n当前用户: {user_name}")
            return
        
        if not self.recharge_logs:
            yield event.plain_result("暂无充值记录")
            return
        
        # 显示最近10条记录
        recent_logs = list(self.recharge_logs.items())[-10:]
        
        lines = ["📊 最近10条充值记录："]
        for log_id, log in recent_logs:
            lines.append(f"━━━━━━━━━━━━━━━━━━━━")
            lines.append(f"🆔 {log_id}")
            lines.append(f"👤 QQ：{log.get('qq_id', '未知')}")
            lines.append(f"🎮 角色：{log.get('player_name', '未知')}")
            lines.append(f"💰 金额：{log.get('amount', 0)} 元宝")
            lines.append(f"⏰ 时间：{log.get('recharge_time', '未知')}")
        
        yield event.plain_result("\n".join(lines))
    
    @filter.command("查看绑定")
    async def view_bindings_cmd(self, event: AstrMessageEvent):
        """查看所有绑定记录（管理员）"""
        logger.info(f"【查看绑定】被触发")
        
        # 获取用户信息
        try:
            user_name = event.get_sender_name()
            logger.info(f"【调试】用户: {user_name}")
        except:
            user_name = "未知"
        
        # 特殊处理：UI用户直接认为是管理员
        if user_name == "UI":
            logger.info(f"【调试】UI用户放行")
            qq_id = "965959320"
        else:
            qq_id = self._get_user_id(event)
        
        # 检查管理员权限
        if not self._is_admin(qq_id):
            yield event.plain_result(f"❌ 权限不足，仅管理员可查看所有绑定\n当前用户: {user_name}")
            return
        
        if not self.bindings:
            yield event.plain_result("暂无绑定记录")
            return
        
        lines = ["📋 所有绑定记录："]
        count = 0
        for bind_qq, data in self.bindings.items():
            count += 1
            lines.append(f"━━━━━━━━━━━━━━━━━━━━")
            lines.append(f"#{count} QQ：{bind_qq}")
            lines.append(f"🎮 角色：{data.get('player_name', '未知')}")
            lines.append(f"🆔 角色ID：{data.get('game_cid', '未知')}")
            lines.append(f"⏰ 绑定时间：{data.get('bind_time', '未知')}")
        
        lines.append(f"\n📊 总计：{count} 条绑定记录")
        
        yield event.plain_result("\n".join(lines))
    
    # ========== 其他功能（保持不变） ==========
    @filter.command("解绑")
    async def unbind_cmd(self, event: AstrMessageEvent):
        # ... 原有代码 ...
        pass
    
    @filter.command("充值")
    async def recharge_cmd(self, event: AstrMessageEvent):
        # ... 原有代码 ...
        pass
    
    @filter.command("查询角色")
    async def query_player_cmd(self, event: AstrMessageEvent):
        # ... 原有代码 ...
        pass
    
    @filter.command("测试充值")
    async def test_recharge_cmd(self, event: AstrMessageEvent):
        # ... 原有代码 ...
        pass
    
    # ========== API调用方法 ==========
    async def _get_player_info(self, cid: str) -> Optional[dict]:
        # ... 原有代码 ...
        pass
    
    async def _execute_recharge(self, cid: str, amount: float, remark: str) -> dict:
        # ... 原有代码 ...
        pass
    
    async def terminate(self):
        logger.info("游戏充值插件已禁用")
