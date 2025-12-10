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
        """获取用户ID - 修复版，从事件对象中正确提取QQ号"""
        qq_id = ""
        
        try:
            # 首先尝试从事件属性中获取
            logger.info(f"【调试】事件对象类型: {type(event)}")
            
            # 根据日志，QQ号可能在get_sender_name()返回的字符串中
            try:
                sender_name = event.get_sender_name()
                logger.info(f"【调试】get_sender_name(): {sender_name}")
                
                # 检查是否是"用户名/QQ号"格式
                if sender_name and '/' in sender_name:
                    parts = sender_name.split('/')
                    if len(parts) >= 2 and parts[1].isdigit():
                        qq_id = parts[1]
                        logger.info(f"【调试】从get_sender_name()解析出QQ: {qq_id}")
                        return qq_id
            except Exception as e:
                logger.info(f"【调试】get_sender_name()失败: {e}")
            
            # 尝试从常见属性获取
            attrs_to_check = ['sender_id', 'user_id', 'qq_id', 'user_qq', 'user', 'from_user']
            
            for attr_name in attrs_to_check:
                if hasattr(event, attr_name):
                    value = getattr(event, attr_name)
                    logger.info(f"【调试】事件.{attr_name}: {value} (类型: {type(value)})")
                    if value:
                        qq_id = str(value)
                        logger.info(f"【调试】从事件.{attr_name}获取QQ: {qq_id}")
                        return qq_id
            
            # 尝试从sender对象获取
            if hasattr(event, 'sender'):
                sender = event.sender
                logger.info(f"【调试】sender对象类型: {type(sender)}")
                
                # 检查sender对象的属性
                sender_attrs = ['id', 'user_id', 'qq_id', 'qq', 'user_qq']
                for attr_name in sender_attrs:
                    if hasattr(sender, attr_name):
                        value = getattr(sender, attr_name)
                        logger.info(f"【调试】sender.{attr_name}: {value}")
                        if value:
                            qq_id = str(value)
                            logger.info(f"【调试】从sender.{attr_name}获取QQ: {qq_id}")
                            return qq_id
            
            # 尝试从原始消息中解析
            if hasattr(event, 'raw_message') and event.raw_message:
                raw_msg = str(event.raw_message)
                logger.info(f"【调试】原始消息: {raw_msg}")
                
                # 尝试解析At消息中的QQ号
                import re
                at_pattern = r'\[CQ:at,qq=(\d+)\]'
                matches = re.findall(at_pattern, raw_msg)
                if matches:
                    qq_id = matches[0]
                    logger.info(f"【调试】从原始消息解析AtQQ: {qq_id}")
                    return qq_id
            
            # 如果以上方法都失败，使用备选方案
            logger.info(f"【调试】无法获取QQ号，使用备选方案")
            
            # 尝试从session_id获取
            try:
                session_id = event.get_session_id()
                logger.info(f"【调试】session_id: {session_id}")
                
                # 尝试从session_id中解析QQ号
                if session_id and '/' in session_id:
                    parts = session_id.split('/')
                    for part in parts:
                        if part.isdigit() and len(part) >= 5:  # QQ号通常至少5位
                            qq_id = part
                            logger.info(f"【调试】从session_id解析出QQ: {qq_id}")
                            return qq_id
            except Exception as e:
                logger.info(f"【调试】获取session_id失败: {e}")
                    
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
        except:
            info_lines.append("发送者名称: 无法获取")
        
        info_lines.append(f"获取到的QQ号: {qq_id}")
        
        # 显示事件对象的关键属性
        info_lines.append("\n📋 事件对象属性：")
        key_attrs = ['sender_id', 'user_id', 'from_id', 'sender', 'raw_message', 'group_id', 'message_id']
        
        for attr in key_attrs:
            if hasattr(event, attr):
                try:
                    value = getattr(event, attr)
                    value_type = type(value).__name__
                    value_repr = repr(value)[:100] + "..." if len(repr(value)) > 100 else repr(value)
                    info_lines.append(f"  {attr}: {value_repr} (类型: {value_type})")
                except:
                    info_lines.append(f"  {attr}: 无法访问")
        
        # 尝试获取session_id
        try:
            session_id = event.get_session_id()
            info_lines.append(f"  session_id: {session_id}")
        except:
            info_lines.append("  session_id: 无法获取")
        
        yield event.plain_result("\n".join(info_lines))
    
    @filter.command("测试获取所有属性")
    async def test_all_attrs_cmd(self, event: AstrMessageEvent):
        """测试获取所有属性"""
        logger.info(f"【测试获取所有属性】被触发")
        
        info_lines = ["🔍 事件对象所有属性："]
        
        try:
            # 获取所有属性
            all_attrs = [attr for attr in dir(event) if not attr.startswith('_')]
            info_lines.append(f"属性数量: {len(all_attrs)}")
            
            # 只显示前20个属性
            for attr in all_attrs[:20]:
                try:
                    value = getattr(event, attr)
                    value_type = type(value).__name__
                    value_repr = repr(value)[:50] + "..." if len(repr(value)) > 50 else repr(value)
                    info_lines.append(f"  {attr}: {value_repr} (类型: {value_type})")
                except Exception as e:
                    info_lines.append(f"  {attr}: 访问失败 - {e}")
        
        except Exception as e:
            info_lines.append(f"错误: {e}")
        
        yield event.plain_result("\n".join(info_lines))
    
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
        
        # 如果QQ号获取失败，提示用户并提供解决方案
        if qq_id == "unknown":
            response_msg = (
                "❌ 无法获取您的QQ信息\n\n"
                "请尝试以下方法：\n"
                "1. 在群里发送 /测试QQ 查看您的QQ信息\n"
                "2. 如果还是无法获取，请联系管理员使用以下命令：\n"
                f"   /强制绑定 [您的QQ号] {game_account}\n\n"
                "例如：/强制绑定 123456789 test123"
            )
            yield event.plain_result(response_msg)
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
            # 如果无法获取QQ号，提供帮助信息
            if qq_id == "unknown":
                yield event.plain_result("❌ 无法识别您的身份\n请发送 /测试QQ 查看您的QQ信息，然后联系管理员")
            else:
                yield event.plain_result("❌ 您尚未绑定游戏账号\n请使用：/绑定账号 游戏账号")
    
    @filter.command("解绑账号")
    async def unbind_account_cmd(self, event: AstrMessageEvent):
        """解绑游戏账号"""
        logger.info(f"【解绑账号】被触发")
        
        qq_id = self._get_user_id(event)
        
        if qq_id in self.bindings:
            game_account = self.bindings[qq_id]["game_account"]
            account_name = self.bindings[qq_id].get("account_name", game_account)
            del self.bindings[qq_id]
            self._save_json(self.bind_file, self.bindings)
            
            logger.info(f"解绑成功：QQ:{qq_id} -> 账号:{account_name}")
            yield event.plain_result(f"✅ 解绑成功！\n已移除账号 {account_name} 的绑定")
        else:
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
        
        # 检查绑定
        if qq_id not in self.bindings:
            yield event.plain_result("❌ 您尚未绑定游戏账号，请先使用 /绑定账号 游戏账号")
            return
        
        game_account = self.bindings[qq_id]["game_account"]
        account_name = self.bindings[qq_id].get("account_name", game_account)
        uid = self.bindings[qq_id].get("uid", "")
        
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
            if qq_id not in self.bindings:
                yield event.plain_result("❌ 您尚未绑定游戏账号，请先绑定或指定游戏账号")
                return
            game_account = self.bindings[qq_id]["game_account"]
        
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
        for bind_qq, data in self.bindings.items():
            count += 1
            lines.append(f"━━━━━━━━━━━━━━━━━━━━")
            lines.append(f"#{count} QQ：{bind_qq}")
            lines.append(f"🎮 账号：{data.get('account_name', '未知')}")
            lines.append(f"🆔 账号ID：{data.get('uid', '未知')}")
            lines.append(f"⏰ 绑定时间：{data.get('bind_time', '未知')}")
        
        lines.append(f"\n📊 总计：{count} 条绑定记录")
        
        yield event.plain_result("\n".join(lines))
    
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
            bind_info = "❌ 未绑定游戏账号"
        
        # 构建回复信息
        info_lines = [
            "📱 您的账户信息：",
            f"🆔 QQ号：{qq_id if qq_id != 'unknown' else '无法获取'}",
            f"🎮 {bind_info}"
        ]
        
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
