import json
import os
import aiohttp
import asyncio
from datetime import datetime
from typing import Optional
from astrbot.api.event import filter, AstrMessageEvent
from astrbot.api.star import Context, Star, register
from astrbot.api import logger

@register("game_bind", "aa932406", "游戏账号绑定与充值插件", "2.2.0")
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
        
        logger.info(f"【游戏账号绑定与充值插件】初始化完成！")
    
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
        """获取用户ID - 从事件对象中正确提取QQ号"""
        qq_id = ""
        
        try:
            logger.info(f"【调试】开始获取QQ号")
            
            # 方法1：优先尝试从get_sender_name()中解析
            try:
                sender_name = event.get_sender_name()
                logger.info(f"【调试】get_sender_name(): {sender_name}")
                
                # 从日志中看到格式是 "UI/965959320"
                if sender_name and '/' in sender_name:
                    parts = sender_name.split('/')
                    if len(parts) >= 2:
                        # 尝试获取QQ号部分
                        possible_qq = parts[1]
                        if possible_qq.isdigit() and len(possible_qq) >= 5:
                            qq_id = possible_qq
                            logger.info(f"【调试】从get_sender_name()解析出QQ: {qq_id}")
                            return qq_id
            except Exception as e:
                logger.info(f"【调试】get_sender_name()失败: {e}")
            
            # 方法2：尝试调用get_sender_id()方法
            try:
                sender_id = event.get_sender_id()
                logger.info(f"【调试】get_sender_id(): {sender_id}")
                if sender_id:
                    qq_id = str(sender_id)
                    logger.info(f"【调试】从get_sender_id()获取QQ: {qq_id}")
                    return qq_id
            except Exception as e:
                logger.info(f"【调试】get_sender_id()失败: {e}")
            
            # 方法3：尝试从sender对象获取
            if hasattr(event, 'sender'):
                try:
                    sender = event.sender
                    logger.info(f"【调试】sender对象: {sender}")
                    
                    # 尝试获取user_id
                    if hasattr(sender, 'user_id'):
                        user_id = sender.user_id
                        logger.info(f"【调试】sender.user_id: {user_id}")
                        if user_id:
                            qq_id = str(user_id)
                            logger.info(f"【调试】从sender.user_id获取QQ: {qq_id}")
                            return qq_id
                    
                    # 尝试获取qq_id
                    if hasattr(sender, 'qq_id'):
                        qq_id_attr = sender.qq_id
                        logger.info(f"【调试】sender.qq_id: {qq_id_attr}")
                        if qq_id_attr:
                            qq_id = str(qq_id_attr)
                            logger.info(f"【调试】从sender.qq_id获取QQ: {qq_id}")
                            return qq_id
                except Exception as e:
                    logger.info(f"【调试】访问sender对象失败: {e}")
            
            # 方法4：尝试从事件属性获取
            attrs_to_check = ['user_id', 'qq_id', 'sender_id', 'user']
            for attr_name in attrs_to_check:
                if hasattr(event, attr_name):
                    try:
                        value = getattr(event, attr_name)
                        logger.info(f"【调试】事件.{attr_name}: {value}")
                        if value:
                            qq_id = str(value)
                            logger.info(f"【调试】从事件.{attr_name}获取QQ: {qq_id}")
                            return qq_id
                    except Exception as e:
                        logger.info(f"【调试】访问事件.{attr_name}失败: {e}")
            
            # 方法5：尝试从session_id中解析（虽然1041562424不是QQ号）
            try:
                session_id = event.get_session_id()
                logger.info(f"【调试】session_id: {session_id}")
                
                # 从session_id中尝试提取可能的QQ号
                if session_id and '/' in str(session_id):
                    parts = str(session_id).split('/')
                    for part in parts:
                        if part.isdigit() and len(part) >= 5:
                            # 检查是否是合理的QQ号
                            qq_num = int(part)
                            if 10000 <= qq_num <= 9999999999:  # QQ号范围
                                qq_id = part
                                logger.info(f"【调试】从session_id解析出可能的QQ: {qq_id}")
                                return qq_id
            except Exception as e:
                logger.info(f"【调试】获取session_id失败: {e}")
            
            # 方法6：特殊处理 - 如果是"UI"用户，使用固定的965959320
            try:
                sender_name = event.get_sender_name()
                if sender_name == "UI":
                    qq_id = "965959320"
                    logger.info(f"【调试】识别为UI用户，使用固定QQ: {qq_id}")
                    return qq_id
            except:
                pass
                
        except Exception as e:
            logger.error(f"获取用户ID异常: {e}")
            import traceback
            logger.error(f"异常详情: {traceback.format_exc()}")
        
        logger.info(f"【调试】最终获取的QQ_ID: {qq_id if qq_id else 'unknown'}")
        return qq_id if qq_id else "unknown"
    
    async def initialize(self):
        logger.info("【游戏账号绑定与充值插件】已启用")
    
    # ========== 调试功能 ==========
    @filter.command("测试QQ")
    async def test_qq_cmd(self, event: AstrMessageEvent):
        """测试获取QQ号"""
        logger.info(f"【测试QQ】被触发")
        
        qq_id = self._get_user_id(event)
        
        # 获取详细信息
        info_lines = ["🔍 QQ号获取测试："]
        
        # 基本信息
        try:
            sender_name = event.get_sender_name()
            info_lines.append(f"发送者名称: {sender_name}")
            
            # 尝试解析发送者名称
            if sender_name and '/' in sender_name:
                parts = sender_name.split('/')
                info_lines.append(f"发送者名称解析: 用户名={parts[0]}, QQ号={parts[1] if len(parts) > 1 else '无'}")
        except:
            info_lines.append("发送者名称: 无法获取")
        
        info_lines.append(f"获取到的QQ号: {qq_id}")
        
        # 显示事件对象的关键方法返回值
        info_lines.append("\n📋 方法返回值：")
        
        methods_to_check = ['get_sender_id', 'get_session_id', 'get_self_id']
        for method_name in methods_to_check:
            try:
                if hasattr(event, method_name):
                    method = getattr(event, method_name)
                    value = method()
                    info_lines.append(f"  {method_name}(): {value}")
                else:
                    info_lines.append(f"  {method_name}: 方法不存在")
            except Exception as e:
                info_lines.append(f"  {method_name}(): 调用失败 - {e}")
        
        # 显示sender对象信息
        info_lines.append("\n👤 Sender对象信息：")
        if hasattr(event, 'sender'):
            try:
                sender = event.sender
                info_lines.append(f"  sender类型: {type(sender)}")
                
                # 获取sender的属性
                sender_attrs = [attr for attr in dir(sender) if not attr.startswith('_')]
                info_lines.append(f"  sender属性数量: {len(sender_attrs)}")
                
                # 显示关键属性
                key_attrs = ['user_id', 'qq_id', 'id', 'nickname', 'card']
                for attr in key_attrs:
                    if hasattr(sender, attr):
                        try:
                            value = getattr(sender, attr)
                            info_lines.append(f"  sender.{attr}: {value}")
                        except:
                            info_lines.append(f"  sender.{attr}: 无法访问")
            except Exception as e:
                info_lines.append(f"  访问sender对象失败: {e}")
        else:
            info_lines.append("  事件没有sender属性")
        
        yield event.plain_result("\n".join(info_lines))
    
    @filter.command("测试原始消息")
    async def test_raw_message_cmd(self, event: AstrMessageEvent):
        """测试原始消息内容"""
        logger.info(f"【测试原始消息】被触发")
        
        info_lines = ["🔍 原始消息测试："]
        
        # 检查原始消息相关属性
        raw_attrs = ['raw_message', 'message', 'original_message', 'event']
        
        for attr_name in raw_attrs:
            if hasattr(event, attr_name):
                try:
                    value = getattr(event, attr_name)
                    value_type = type(value).__name__
                    
                    if isinstance(value, dict):
                        # 如果是字典，显示关键字段
                        info_lines.append(f"\n📄 {attr_name} (类型: {value_type}):")
                        for key, val in value.items():
                            info_lines.append(f"  {key}: {repr(val)[:50]}")
                    else:
                        # 其他类型，直接显示
                        value_repr = repr(value)[:100] + "..." if len(repr(value)) > 100 else repr(value)
                        info_lines.append(f"\n📄 {attr_name}: {value_repr} (类型: {value_type})")
                except Exception as e:
                    info_lines.append(f"\n❌ {attr_name}: 访问失败 - {e}")
        
        # 尝试解析消息中的At信息
        info_lines.append("\n🔍 At消息解析：")
        try:
            import re
            # 从事件的消息字符串中解析
            message_str = event.get_message_str()
            at_pattern = r'\[CQ:at,qq=(\d+)\]'
            matches = re.findall(at_pattern, message_str)
            
            if matches:
                info_lines.append(f"  发现At消息，QQ号: {matches}")
            else:
                info_lines.append("  未发现At消息")
        except Exception as e:
            info_lines.append(f"  解析At消息失败: {e}")
        
        yield event.plain_result("\n".join(info_lines))
    
    @filter.command("强制设置QQ")
    async def force_set_qq_cmd(self, event: AstrMessageEvent):
        """强制设置当前用户的QQ号（测试用）"""
        logger.info(f"【强制设置QQ】被触发")
        
        parts = event.message_str.strip().split()
        if len(parts) >= 2:
            # 指定QQ号
            qq_id = parts[1]
            if qq_id.isdigit():
                # 创建一个临时绑定用于测试
                temp_key = f"temp_{qq_id}"
                if temp_key not in self.bindings:
                    self.bindings[temp_key] = {
                        "game_account": "test_account",
                        "uid": "test_uid",
                        "account_name": "测试账号",
                        "bind_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                        "qq_id": qq_id,
                        "is_temp": True
                    }
                    self._save_json(self.bind_file, self.bindings)
                
                yield event.plain_result(f"✅ 已设置测试QQ号: {qq_id}\n现在可以使用 /绑定账号 命令")
            else:
                yield event.plain_result("❌ QQ号必须是数字")
        else:
            # 使用默认测试QQ
            test_qq = "100000001"
            yield event.plain_result(f"📝 使用测试QQ号: {test_qq}\n格式: /强制设置QQ 123456789")
    
    # ========== 基础功能 ==========
    @filter.command("绑定账号")
    async def bind_account_cmd(self, event: AstrMessageEvent):
        """绑定PHP游戏账号：/绑定账号 游戏账号"""
        logger.info(f"【绑定账号指令】被触发")
        
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result("❌ 格式：/绑定账号 游戏账号\n例如：/绑定账号 test123")
            return
        
        game_account = parts[1]  # PHP系统中的账号（passport）
        qq_id = self._get_user_id(event)
        
        logger.info(f"绑定请求 - QQ:{qq_id}, 游戏账号:{game_account}")
        
        # 如果QQ号获取失败，提供详细的错误信息
        if qq_id == "unknown":
            # 尝试获取更多信息帮助诊断
            error_info = ["❌ 无法获取您的QQ信息"]
            
            try:
                sender_name = event.get_sender_name()
                error_info.append(f"发送者名称: {sender_name}")
                
                if sender_name and '/' in sender_name:
                    parts = sender_name.split('/')
                    if len(parts) >= 2 and parts[1].isdigit():
                        error_info.append(f"⚠️ 检测到可能的QQ号: {parts[1]}")
                        error_info.append(f"请确保使用命令格式: /强制绑定 {parts[1]} {game_account}")
            except:
                pass
            
            error_info.append("\n请尝试以下方法：")
            error_info.append("1. 发送 /测试QQ 查看详细信息")
            error_info.append("2. 联系管理员使用 /强制绑定 QQ号 游戏账号")
            error_info.append("3. 发送 /测试原始消息 查看更多信息")
            
            yield event.plain_result("\n".join(error_info))
            return
        
        # 检查是否已绑定
        if qq_id in self.bindings:
            old_account = self.bindings[qq_id]["game_account"]
            yield event.plain_result(f"⚠️ 您已绑定游戏账号：{old_account}\n如需更改，请先使用 /解绑账号")
            return
        
        # 先验证游戏账号是否存在
        try:
            account_info = await self._get_account_info(game_account)
            if not account_info:
                yield event.plain_result(f"❌ 游戏账号 {game_account} 不存在，请确认后重试")
                return
        except Exception as e:
            logger.error(f"验证游戏账号失败: {e}")
            yield event.plain_result("❌ 验证游戏账号失败，请检查网络或联系管理员")
            return
        
        # 保存绑定
        self.bindings[qq_id] = {
            "game_account": game_account,
            "uid": account_info.get("uid", "未知"),
            "account_name": account_info.get("passport", game_account),
            "bind_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "qq_id": qq_id
        }
        self._save_json(self.bind_file, self.bindings)
        
        account_name = account_info.get("passport", game_account)
        uid = account_info.get("uid", "未知")
        logger.info(f"绑定成功：QQ:{qq_id} -> 游戏账号:{account_name}(UID:{uid})")
        yield event.plain_result(f"✅ 绑定成功！\n🎮 游戏账号：{account_name}\n🆔 账号ID(UID)：{uid}\n⏰ 时间：{self.bindings[qq_id]['bind_time']}")
    
    @filter.command("我的绑定")
    async def mybind_cmd(self, event: AstrMessageEvent):
        """查询绑定信息"""
        logger.info(f"【我的绑定】被触发")
        
        qq_id = self._get_user_id(event)
        
        if qq_id in self.bindings:
            data = self.bindings[qq_id]
            yield event.plain_result(
                f"📋 您的绑定信息：\n"
                f"🎮 游戏账号：{data.get('account_name', '未知')}\n"
                f"🆔 账号ID：{data.get('uid', '未知')}\n"
                f"⏰ 绑定时间：{data['bind_time']}"
            )
        else:
            # 检查临时绑定
            temp_keys = [k for k in self.bindings.keys() if k.startswith('temp_')]
            temp_found = False
            for temp_key in temp_keys:
                if self.bindings[temp_key].get('qq_id') == qq_id:
                    data = self.bindings[temp_key]
                    yield event.plain_result(
                        f"📋 您的临时绑定信息：\n"
                        f"🎮 游戏账号：{data.get('account_name', '未知')}\n"
                        f"🆔 账号ID：{data.get('uid', '未知')}\n"
                        f"⏰ 绑定时间：{data['bind_time']}\n"
                        f"⚠️ 这是临时绑定，建议重新绑定"
                    )
                    temp_found = True
                    break
            
            if not temp_found:
                yield event.plain_result("❌ 您尚未绑定游戏账号\n请使用：/绑定账号 游戏账号")
    
    @filter.command("解绑账号")
    async def unbind_account_cmd(self, event: AstrMessageEvent):
        """解绑游戏账号"""
        logger.info(f"【解绑账号】被触发")
        
        qq_id = self._get_user_id(event)
        
        # 检查正式绑定
        if qq_id in self.bindings:
            game_account = self.bindings[qq_id]["game_account"]
            account_name = self.bindings[qq_id].get("account_name", game_account)
            del self.bindings[qq_id]
            self._save_json(self.bind_file, self.bindings)
            
            logger.info(f"解绑成功：QQ:{qq_id} -> 账号:{account_name}")
            yield event.plain_result(f"✅ 解绑成功！\n已移除账号 {account_name} 的绑定")
        else:
            # 检查临时绑定
            temp_keys = [k for k in self.bindings.keys() if k.startswith('temp_')]
            temp_found = False
            for temp_key in temp_keys:
                if self.bindings[temp_key].get('qq_id') == qq_id:
                    account_name = self.bindings[temp_key].get("account_name", "未知")
                    del self.bindings[temp_key]
                    self._save_json(self.bind_file, self.bindings)
                    
                    logger.info(f"解绑临时绑定成功：QQ:{qq_id}")
                    yield event.plain_result(f"✅ 解绑成功！\n已移除临时账号 {account_name} 的绑定")
                    temp_found = True
                    break
            
            if not temp_found:
                yield event.plain_result("❌ 您未绑定任何游戏账号")
    
    # ========== 充值功能 ==========
    @filter.command("账号充值")
    async def account_recharge_cmd(self, event: AstrMessageEvent):
        """为绑定账号充值：/账号充值 金额 [备注]"""
        logger.info(f"【账号充值指令】被触发")
        
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result("❌ 格式：/账号充值 金额 [备注]\n例如：/账号充值 1000 元宝充值")
            return
        
        try:
            amount = float(parts[1])
            if amount <= 0:
                raise ValueError("金额必须大于0")
            remark = " ".join(parts[2:]) if len(parts) > 2 else "QQ机器人充值"
        except ValueError:
            yield event.plain_result("❌ 充值金额必须是数字且大于0")
            return
        
        qq_id = self._get_user_id(event)
        
        # 检查绑定（包括临时绑定）
        found_binding = None
        if qq_id in self.bindings:
            found_binding = self.bindings[qq_id]
        else:
            # 检查临时绑定
            temp_keys = [k for k in self.bindings.keys() if k.startswith('temp_')]
            for temp_key in temp_keys:
                if self.bindings[temp_key].get('qq_id') == qq_id:
                    found_binding = self.bindings[temp_key]
                    break
        
        if not found_binding:
            yield event.plain_result("❌ 您尚未绑定游戏账号，请先使用 /绑定账号 游戏账号")
            return
        
        game_account = found_binding["game_account"]
        account_name = found_binding.get("account_name", game_account)
        uid = found_binding.get("uid", "")
        
        # 执行充值（使用账号充值）
        try:
            result = await self._execute_account_recharge(game_account, amount, remark)
            
            if result.get("success"):
                # 记录充值日志
                recharge_id = f"R{datetime.now().strftime('%Y%m%d%H%M%S')}_{qq_id}"
                self.recharge_logs[recharge_id] = {
                    "qq_id": qq_id,
                    "game_account": game_account,
                    "account_name": account_name,
                    "uid": uid,
                    "amount": amount,
                    "remark": remark,
                    "recharge_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                    "api_response": result
                }
                self._save_json(self.recharge_file, self.recharge_logs)
                
                logger.info(f"充值成功：QQ:{qq_id} -> 账号:{account_name} 金额:{amount}")
                
                response_data = result.get("data", {})
                yield event.plain_result(
                    f"✅ 充值成功！\n"
                    f"🎮 游戏账号：{account_name}\n"
                    f"🆔 账号ID：{uid}\n"
                    f"💰 充值金额：{amount} 元宝\n"
                    f"📝 备注：{remark}\n"
                    f"🧾 新余额：{response_data.get('new_gold_pay', '未知')}\n"
                    f"💰 累计充值：{response_data.get('new_gold_pay_total', '未知')}\n"
                    f"⏰ 时间：{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"
                )
            else:
                error_msg = result.get("error", "未知错误")
                logger.error(f"充值失败：QQ:{qq_id} 错误：{error_msg}")
                yield event.plain_result(f"❌ 充值失败：{error_msg}")
                
        except Exception as e:
            logger.error(f"充值异常：QQ:{qq_id} 异常：{str(e)}")
            yield event.plain_result(f"❌ 充值过程出现异常，请稍后重试或联系管理员\n错误：{str(e)}")
    
    @filter.command("查询账号")
    async def query_account_cmd(self, event: AstrMessageEvent):
        """查询游戏账号信息：/查询账号 [游戏账号]"""
        logger.info(f"【查询账号】被触发")
        
        parts = event.message_str.strip().split()
        
        if len(parts) >= 2:
            # 查询指定账号
            game_account = parts[1]
        else:
            # 查询自己绑定的账号
            qq_id = self._get_user_id(event)
            
            # 检查绑定（包括临时绑定）
            found_binding = None
            if qq_id in self.bindings:
                found_binding = self.bindings[qq_id]
            else:
                # 检查临时绑定
                temp_keys = [k for k in self.bindings.keys() if k.startswith('temp_')]
                for temp_key in temp_keys:
                    if self.bindings[temp_key].get('qq_id') == qq_id:
                        found_binding = self.bindings[temp_key]
                        break
            
            if not found_binding:
                yield event.plain_result("❌ 您尚未绑定游戏账号，请先绑定或指定游戏账号")
                return
            
            game_account = found_binding["game_account"]
        
        try:
            account_info = await self._get_account_info(game_account)
            if not account_info:
                yield event.plain_result(f"❌ 游戏账号 {game_account} 不存在")
                return
            
            # 格式化账号信息
            info_lines = [
                f"🎮 账号信息：{account_info.get('passport', '未知')}",
                f"🆔 账号ID(UID)：{account_info.get('uid', '未知')}",
                f"💰 当前余额：{account_info.get('gold_pay', 0)}",
                f"📈 累计充值：{account_info.get('gold_pay_total', 0)}"
            ]
            
            yield event.plain_result("\n".join(info_lines))
            
        except Exception as e:
            logger.error(f"查询账号失败：{e}")
            yield event.plain_result(f"❌ 查询账号失败：{str(e)}")
    
    @filter.command("测试连接")
    async def test_connection_cmd(self, event: AstrMessageEvent):
        """测试API连接"""
        logger.info(f"【测试连接】被触发")
        
        yield event.plain_result("🔄 正在测试API连接...")
        
        try:
            # 测试搜索功能
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
                            yield event.plain_result(f"✅ API连接正常！\n共找到 {result['data']['total']} 个账号")
                        else:
                            yield event.plain_result(f"⚠️ API响应异常：{result.get('error', '未知错误')}")
                    else:
                        yield event.plain_result(f"❌ API连接失败，状态码：{response.status}")
                        
        except Exception as e:
            yield event.plain_result(f"❌ API连接失败：{str(e)}\n请检查API地址和网络配置")
    
    # ========== 管理员功能 ==========
    @filter.command("充值记录")
    async def recharge_history_cmd(self, event: AstrMessageEvent):
        """查看充值记录（管理员）"""
        logger.info(f"【充值记录】被触发")
        
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
            lines.append(f"🎮 账号：{log.get('account_name', '未知')}")
            lines.append(f"💰 金额：{log.get('amount', 0)} 元宝")
            lines.append(f"⏰ 时间：{log.get('recharge_time', '未知')}")
        
        yield event.plain_result("\n".join(lines))
    
    @filter.command("查看绑定")
    async def view_bindings_cmd(self, event: AstrMessageEvent):
        """查看所有绑定记录（管理员）"""
        logger.info(f"【查看绑定】被触发")
        
        if not self.bindings:
            yield event.plain_result("暂无绑定记录")
            return
        
        lines = ["📋 所有绑定记录："]
        count = 0
        temp_count = 0
        
        for bind_key, data in self.bindings.items():
            count += 1
            lines.append(f"━━━━━━━━━━━━━━━━━━━━")
            
            if bind_key.startswith('temp_'):
                lines.append(f"#{count} [临时] QQ：{data.get('qq_id', '未知')}")
                temp_count += 1
            else:
                lines.append(f"#{count} QQ：{bind_key}")
            
            lines.append(f"🎮 账号：{data.get('account_name', '未知')}")
            lines.append(f"🆔 账号ID：{data.get('uid', '未知')}")
            lines.append(f"⏰ 绑定时间：{data.get('bind_time', '未知')}")
        
        lines.append(f"\n📊 总计：{count} 条绑定记录（{temp_count} 条临时绑定）")
        
        yield event.plain_result("\n".join(lines))
    
    @filter.command("强制绑定")
    async def force_bind_cmd(self, event: AstrMessageEvent):
        """强制绑定：/强制绑定 QQ号 游戏账号"""
        logger.info(f"【强制绑定】被触发")
        
        parts = event.message_str.strip().split()
        if len(parts) < 3:
            yield event.plain_result("❌ 格式：/强制绑定 QQ号 游戏账号\n例如：/强制绑定 123456 test123")
            return
        
        qq_id = parts[1]
        game_account = parts[2]
        
        if not qq_id.isdigit():
            yield event.plain_result("❌ QQ号必须是数字")
            return
        
        # 验证游戏账号是否存在
        try:
            account_info = await self._get_account_info(game_account)
            if not account_info:
                yield event.plain_result(f"❌ 游戏账号 {game_account} 不存在，请确认后重试")
                return
        except Exception as e:
            logger.error(f"验证游戏账号失败: {e}")
            yield event.plain_result("❌ 验证游戏账号失败，请检查网络或联系管理员")
            return
        
        # 保存绑定
        self.bindings[qq_id] = {
            "game_account": game_account,
            "uid": account_info.get("uid", "未知"),
            "account_name": account_info.get("passport", game_account),
            "bind_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "qq_id": qq_id
        }
        self._save_json(self.bind_file, self.bindings)
        
        account_name = account_info.get("passport", game_account)
        uid = account_info.get("uid", "未知")
        logger.info(f"强制绑定成功：QQ:{qq_id} -> 游戏账号:{account_name}(UID:{uid})")
        yield event.plain_result(f"✅ 强制绑定成功！\n🆔 QQ号：{qq_id}\n🎮 游戏账号：{account_name}\n📊 账号ID(UID)：{uid}\n⏰ 时间：{self.bindings[qq_id]['bind_time']}")
    
    @filter.command("我的信息")
    async def my_info_cmd(self, event: AstrMessageEvent):
        """显示我的QQ信息"""
        logger.info(f"【我的信息】被触发")
        
        qq_id = self._get_user_id(event)
        
        # 检查绑定状态
        bind_info = ""
        if qq_id in self.bindings:
            data = self.bindings[qq_id]
            bind_info = f"✅ 已绑定游戏账号\n账号：{data.get('account_name', '未知')}\n账号ID：{data.get('uid', '未知')}"
        else:
            # 检查临时绑定
            temp_keys = [k for k in self.bindings.keys() if k.startswith('temp_')]
            temp_found = False
            for temp_key in temp_keys:
                if self.bindings[temp_key].get('qq_id') == qq_id:
                    data = self.bindings[temp_key]
                    bind_info = f"⚠️ 临时绑定游戏账号\n账号：{data.get('account_name', '未知')}\n账号ID：{data.get('uid', '未知')}"
                    temp_found = True
                    break
            
            if not temp_found:
                bind_info = "❌ 未绑定游戏账号"
        
        # 构建回复信息
        info_lines = [
            "📱 您的账户信息：",
            f"🆔 QQ号：{qq_id if qq_id != 'unknown' else '无法获取'}"
        ]
        
        # 添加发送者名称信息
        try:
            sender_name = event.get_sender_name()
            info_lines.append(f"👤 发送者名称：{sender_name}")
            
            if sender_name and '/' in sender_name:
                parts = sender_name.split('/')
                if len(parts) >= 2:
                    info_lines.append(f"🔍 解析到的QQ号：{parts[1]}")
        except:
            pass
        
        info_lines.append(f"🎮 {bind_info}")
        
        yield event.plain_result("\n".join(info_lines))
    
    # ========== API调用方法 ==========
    async def _get_account_info(self, passport: str) -> Optional[dict]:
        """调用API查询账号信息"""
        try:
            async with aiohttp.ClientSession() as session:
                # 使用搜索功能查询账号
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
                                "uid": player.get('uid', '未知'),
                                "gold_pay": player.get('cash_gold', 0),
                                "gold_pay_total": player.get('total_recharge', 0)
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
                # 使用POST方法发送充值请求
                form_data = aiohttp.FormData()
                form_data.add_field("action", "recharge")
                form_data.add_field("passport", passport)  # 使用passport
                form_data.add_field("amount", str(amount))
                form_data.add_field("remark", remark)
                form_data.add_field("source", "qq_bot")  # 添加来源标识
                
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
