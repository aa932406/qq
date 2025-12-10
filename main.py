import json
import os
import aiohttp
import asyncio
from datetime import datetime
from typing import Optional
from astrbot.api.event import filter, AstrMessageEvent
from astrbot.api.star import Context, Star, register
from astrbot.api import logger

@register("game_bind", "aa932406", "游戏账号绑定与充值插件", "1.2.0")
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
        
        # ⚠️ 请修改这里的配置为您自己的服务器地址
        self.api_config = {
            "base_url": "http://115.190.64.181:881/api/players.php",  # 您的API地址
            "timeout": 30
        }
        
        # 管理员QQ列表 - 您的QQ号
        self.admin_qq_list = ["965959320"]
        
        logger.info(f"【游戏充值插件】初始化完成！管理员列表：{self.admin_qq_list}")
        logger.info(f"API地址配置: {self.api_config['base_url']}")
    
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
        """获取用户ID - 增强版，使用 event.get_sender_name()"""
        try:
            # 方法1：使用 event.get_sender_name() 获取用户名，然后从绑定数据中反查QQ号
            user_name = event.get_sender_name()
            logger.info(f"【调试】获取到用户名: {user_name}")
            
            # 如果用户名是数字格式，可能就是QQ号
            if user_name and user_name.isdigit():
                logger.info(f"【调试】用户名是数字格式，可能是QQ号: {user_name}")
                return user_name
            
            # 方法2：尝试从消息链中获取 @ 信息
            try:
                from astrbot.api.message_components import At
                messages = event.get_messages()
                for msg in messages:
                    if isinstance(msg, At):
                        logger.info(f"【调试】找到@消息: {msg}")
                        # 这里可能需要根据实际情况处理
            except:
                pass
            
            # 方法3：尝试其他可能的属性
            attrs_to_check = ['sender_id', 'user_id', 'from_id', 'sender', 'user']
            for attr in attrs_to_check:
                if hasattr(event, attr):
                    value = getattr(event, attr)
                    logger.info(f"【调试】尝试属性 {attr}: {value}")
                    if value:
                        # 如果是对象，尝试获取id
                        if hasattr(value, 'id'):
                            qq_id = str(value.id)
                            logger.info(f"【调试】从对象获取id: {qq_id}")
                            return qq_id
                        # 如果是数字或字符串
                        elif isinstance(value, (int, str)) and str(value).strip():
                            qq_id = str(value).strip()
                            logger.info(f"【调试】直接获取值: {qq_id}")
                            return qq_id
            
            # 方法4：临时方案 - 如果是您自己，直接返回您的QQ号
            # 在群聊中发送消息测试，如果是您发的消息，直接认为是管理员
            if user_name and ("Shenx" in user_name or "您的昵称" in user_name):
                logger.info(f"【调试】根据用户名识别为管理员: {user_name}")
                return "965959320"
                
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
    
    @filter.command("解绑")
    async def unbind_cmd(self, event: AstrMessageEvent):
        """解绑游戏账号"""
        logger.info(f"【解绑】被触发")
        
        qq_id = self._get_user_id(event)
        logger.info(f"【调试】解绑请求 - QQ: {qq_id}")
        
        if qq_id in self.bindings:
            game_cid = self.bindings[qq_id]["game_cid"]
            player_name = self.bindings[qq_id].get("player_name", game_cid)
            del self.bindings[qq_id]
            self._save_json(self.bind_file, self.bindings)
            
            logger.info(f"解绑成功：QQ:{qq_id} -> 角色:{player_name}")
            yield event.plain_result(f"✅ 解绑成功！\n已移除角色 {player_name} 的绑定")
        else:
            yield event.plain_result("❌ 您未绑定任何游戏角色")
    
    # ========== 充值功能 ==========
    @filter.command("充值")
    async def recharge_cmd(self, event: AstrMessageEvent):
        """游戏充值：/充值 金额 [备注]"""
        logger.info(f"【充值指令】被触发")
        
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result("❌ 格式：/充值 金额 [备注]\n例如：/充值 100 元宝充值")
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
        logger.info(f"【调试】充值请求 - QQ: {qq_id}, 金额: {amount}")
        
        # 检查绑定
        if qq_id not in self.bindings:
            yield event.plain_result("❌ 您尚未绑定游戏角色，请先使用 /绑定 游戏角色ID")
            return
        
        game_cid = self.bindings[qq_id]["game_cid"]
        player_name = self.bindings[qq_id].get("player_name", game_cid)
        
        # 执行充值
        try:
            result = await self._execute_recharge(game_cid, amount, remark)
            
            if result.get("success"):
                # 记录充值日志
                recharge_id = f"R{datetime.now().strftime('%Y%m%d%H%M%S')}_{qq_id}"
                self.recharge_logs[recharge_id] = {
                    "qq_id": qq_id,
                    "game_cid": game_cid,
                    "player_name": player_name,
                    "amount": amount,
                    "remark": remark,
                    "recharge_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                    "api_response": result
                }
                self._save_json(self.recharge_file, self.recharge_logs)
                
                logger.info(f"充值成功：QQ:{qq_id} -> 角色:{player_name} 金额:{amount}")
                
                response_data = result.get("data", {})
                yield event.plain_result(
                    f"✅ 充值成功！\n"
                    f"🎮 游戏角色：{player_name}\n"
                    f"🆔 角色ID：{game_cid}\n"
                    f"💰 充值金额：{amount} 元宝\n"
                    f"📝 备注：{remark}\n"
                    f"🧾 新余额：{response_data.get('new_gold_pay', '未知')}\n"
                    f"💰 累计充值：{response_data.get('new_gold_pay_total', '未知')}\n"
                    f"⏰ 时间：{response_data.get('recharge_time', datetime.now().strftime('%Y-%m-%d %H:%M:%S'))}"
                )
            else:
                error_msg = result.get("error", "未知错误")
                logger.error(f"充值失败：QQ:{qq_id} 错误：{error_msg}")
                yield event.plain_result(f"❌ 充值失败：{error_msg}")
                
        except Exception as e:
            logger.error(f"充值异常：QQ:{qq_id} 异常：{str(e)}")
            yield event.plain_result(f"❌ 充值过程出现异常，请稍后重试或联系管理员\n错误：{str(e)}")
    
    @filter.command("查询角色")
    async def query_player_cmd(self, event: AstrMessageEvent):
        """查询游戏角色信息：/查询角色 [角色ID]"""
        logger.info(f"【查询角色】被触发")
        
        parts = event.message_str.strip().split()
        
        if len(parts) >= 2:
            # 查询指定角色
            game_cid = parts[1]
        else:
            # 查询自己绑定的角色
            qq_id = self._get_user_id(event)
            if qq_id not in self.bindings:
                yield event.plain_result("❌ 您尚未绑定游戏角色，请先绑定或指定角色ID")
                return
            game_cid = self.bindings[qq_id]["game_cid"]
        
        try:
            player_info = await self._get_player_info(game_cid)
            if not player_info:
                yield event.plain_result(f"❌ 角色ID {game_cid} 不存在")
                return
            
            # 格式化角色信息
            info_lines = [
                f"🎮 角色信息：{player_info.get('name', '未知')}",
                f"🆔 角色ID：{game_cid}",
                f"🎯 职业：{player_info.get('job_name', '未知')}",
                f"📊 等级：{player_info.get('level', '未知')}",
                f"⚔️ 战力：{player_info.get('battle', '未知')}",
                f"💰 元宝：{player_info.get('cash_gold', '未知')}",
                f"💎 VIP等级：{player_info.get('vip_level', '未知')}",
                f"📅 创建时间：{player_info.get('create_time_str', '未知')}",
                f"🕒 最后登录：{player_info.get('last_login_time_str', '未知')}",
                f"📈 累计充值：{player_info.get('total_recharge', '未知')}",
                f"🔒 状态：{player_info.get('status', '正常')}"
            ]
            
            yield event.plain_result("\n".join(info_lines))
            
        except Exception as e:
            logger.error(f"查询角色失败：{e}")
            yield event.plain_result(f"❌ 查询角色失败：{str(e)}")
    
    @filter.command("测试充值")
    async def test_recharge_cmd(self, event: AstrMessageEvent):
        """测试充值API连接"""
        logger.info(f"【测试充值】被触发")
        
        yield event.plain_result("🔄 正在测试API连接...")
        
        try:
            # 测试获取一个已知角色（假设有测试角色）
            test_cid = "100001"  # 可以修改为您的测试角色ID
            player_info = await self._get_player_info(test_cid)
            
            if player_info:
                yield event.plain_result(f"✅ API连接正常！\n测试角色：{player_info.get('name', '未知')}")
            else:
                yield event.plain_result("⚠️ API连接正常，但测试角色不存在\n请确认测试角色ID是否正确")
                
        except Exception as e:
            yield event.plain_result(f"❌ API连接失败：{str(e)}\n请检查API地址和网络配置")
    
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
            f"🆔 QQ号：{qq_id if qq_id != 'unknown' else '无法获取'}",
            f"👑 管理员：{'✅ 是' if is_admin else '❌ 否'}",
            f"🎮 {bind_info}",
            f"📋 管理员列表：{', '.join(self.admin_qq_list)}",
            f"💡 提示：如果QQ号无法获取，请尝试使用 /强制管理员 命令"
        ]
        
        yield event.plain_result("\n".join(info_lines))
    
    @filter.command("强制管理员")
    async def force_admin_cmd(self, event: AstrMessageEvent):
        """强制设置为管理员（临时解决方案）"""
        logger.info(f"【强制管理员】被触发")
        
        # 获取用户名
        try:
            user_name = event.get_sender_name()
            logger.info(f"【调试】用户名: {user_name}")
        except:
            user_name = "未知"
        
        # 如果是您自己，直接设置为管理员
        if "Shenx" in user_name or user_name == "您的昵称":
            qq_id = "965959320"
            if qq_id not in self.admin_qq_list:
                self.admin_qq_list.append(qq_id)
                logger.info(f"强制添加管理员: {qq_id}")
                yield event.plain_result(f"✅ 已强制设置 {qq_id} 为管理员\n当前管理员列表：{', '.join(self.admin_qq_list)}")
            else:
                yield event.plain_result(f"ℹ️ {qq_id} 已经是管理员")
        else:
            yield event.plain_result("❌ 权限不足，仅特定用户可以执行此命令")
    
    @filter.command("测试事件")
    async def test_event_cmd(self, event: AstrMessageEvent):
        """测试事件对象信息"""
        logger.info(f"【测试事件】被触发")
        
        info_lines = ["🔍 事件对象信息："]
        
        # 获取所有属性
        for attr in dir(event):
            if not attr.startswith('_'):
                try:
                    value = getattr(event, attr)
                    if not callable(value):
                        info_lines.append(f"{attr}: {repr(value)}")
                except:
                    pass
        
        # 限制输出长度
        yield event.plain_result("\n".join(info_lines[:20]))
    
    # ========== 管理员功能 ==========
    @filter.command("充值记录")
    async def recharge_history_cmd(self, event: AstrMessageEvent):
        """查看充值记录（管理员）"""
        logger.info(f"【充值记录】被触发")
        
        qq_id = self._get_user_id(event)
        
        # 手动检查管理员权限
        if not self._is_admin(qq_id):
            # 临时：如果是您自己，允许查看
            try:
                user_name = event.get_sender_name()
                if "Shenx" in user_name:
                    logger.info(f"【调试】根据用户名放行: {user_name}")
                else:
                    yield event.plain_result(f"❌ 权限不足，仅管理员可查看充值记录\n您的QQ: '{qq_id}'")
                    return
            except:
                yield event.plain_result(f"❌ 权限不足，仅管理员可查看充值记录\n您的QQ: '{qq_id}'")
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
        
        qq_id = self._get_user_id(event)
        
        # 手动检查管理员权限
        if not self._is_admin(qq_id):
            yield event.plain_result(f"❌ 权限不足，仅管理员可查看所有绑定\n您的QQ: '{qq_id}'")
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
    
    # ========== API调用方法 ==========
    async def _get_player_info(self, cid: str) -> Optional[dict]:
        """调用API查询玩家信息"""
        try:
            async with aiohttp.ClientSession() as session:
                params = {
                    "action": "detail",
                    "cid": cid
                }
                
                async with session.get(
                    self.api_config["base_url"],
                    params=params,
                    timeout=aiohttp.ClientTimeout(total=self.api_config["timeout"])
                ) as response:
                    if response.status == 200:
                        result = await response.json()
                        if result.get("success"):
                            return result.get("data")
                        else:
                            logger.warning(f"查询角色失败：{result.get('error')}")
                    else:
                        logger.error(f"API请求失败，状态码：{response.status}")
        except Exception as e:
            logger.error(f"查询角色异常：{e}")
        
        return None
    
    async def _execute_recharge(self, cid: str, amount: float, remark: str) -> dict:
        """调用API执行充值"""
        try:
            async with aiohttp.ClientSession() as session:
                # 使用POST方法发送充值请求
                form_data = aiohttp.FormData()
                form_data.add_field("action", "recharge")
                form_data.add_field("cid", cid)
                form_data.add_field("amount", str(amount))
                form_data.add_field("remark", remark)
                
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
        logger.info("游戏充值插件已禁用")
