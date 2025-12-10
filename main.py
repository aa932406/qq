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
            "timeout": 30,
            "qq_bot_secret": "ws7ecejjsznhtxurchknmdemax2fnp5d"  # 设置你的密钥
        }
    
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
        """获取用户ID - 使用get_sender_id()方法"""
        qq_id = ""
        
        try:
            # 直接使用 get_sender_id() 方法
            sender_id = event.get_sender_id()
            if sender_id:
                qq_id = str(sender_id)
                return qq_id
            
            # 备选方案：从get_sender_name()解析
            sender_name = event.get_sender_name()
            if sender_name and '/' in sender_name:
                parts = sender_name.split('/')
                if len(parts) >= 2 and parts[1].isdigit():
                    qq_id = parts[1]
                    return qq_id
                    
        except Exception as e:
            logger.error(f"获取用户ID异常: {e}")
        
        return qq_id if qq_id else "unknown"
    
    def _is_account_already_bound(self, game_account: str, exclude_qq: str = None) -> tuple:
        """检查游戏账号是否已被绑定
        
        Args:
            game_account: 要检查的游戏账号
            exclude_qq: 要排除的QQ号（用于修改绑定时）
            
        Returns:
            tuple: (是否已绑定, 绑定此账号的QQ号, 绑定信息)
        """
        for qq_id, bind_info in self.bindings.items():
            if bind_info.get("game_account") == game_account:
                if exclude_qq and qq_id == exclude_qq:
                    continue  # 跳过要排除的QQ号
                return True, qq_id, bind_info
        return False, None, None
    
    async def initialize(self):
        logger.info("游戏账号绑定与充值插件已启用")
    
    # ========== 帮助功能 ==========
    @filter.command("帮助")
    async def help_cmd(self, event: AstrMessageEvent):
        """显示帮助信息"""
        help_text = """🎮 游戏账号绑定与充值插件 🎮

📋 基本命令：
/帮助 - 显示此帮助信息
/我的信息 - 查看我的QQ信息
/绑定账号 <游戏账号> - 绑定游戏账号
/我的绑定 - 查看我的绑定信息
/解绑账号 - 解绑当前游戏账号
/查询账号 [游戏账号] - 查询账号信息（不填查自己的）

💰 充值功能：
/账号充值 <金额> [备注] - 为绑定账号充值

🔧 其他功能：
/测试连接 - 测试API连接状态

👨‍💼 管理员命令：
/查看绑定 - 查看所有绑定记录
/充值记录 - 查看充值记录
/强制绑定 <QQ号> <游戏账号> - 强制绑定账号

💡 使用示例：
/绑定账号 test123
/我的绑定
/账号充值 1000 元宝充值
/查询账号 test123
"""
        yield event.plain_result(help_text)
    
    @filter.command("菜单")
    async def menu_cmd(self, event: AstrMessageEvent):
        """显示简洁菜单"""
        menu_text = """📱 游戏账号插件菜单

🔐 绑定相关：
/绑定账号 <账号> - 绑定账号
/我的绑定 - 查看绑定
/解绑账号 - 解绑账号

💰 充值相关：
/账号充值 <金额> - 充值元宝
/查询账号 [账号] - 查询信息

❓ 帮助：
/帮助 - 详细帮助
"""
        yield event.plain_result(menu_text)
    
    # ========== 基础功能 ==========
    @filter.command("绑定账号")
    async def bind_account_cmd(self, event: AstrMessageEvent):
        """绑定PHP游戏账号：/绑定账号 游戏账号"""
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result("❌ 格式：/绑定账号 游戏账号\n例如：/绑定账号 test123")
            return
        
        game_account = parts[1]
        qq_id = self._get_user_id(event)
        
        # 如果QQ号获取失败
        if qq_id == "unknown":
            yield event.plain_result("❌ 无法获取您的QQ信息，请联系管理员")
            return
        
        # 检查此QQ是否已绑定其他账号
        if qq_id in self.bindings:
            old_account = self.bindings[qq_id]["game_account"]
            yield event.plain_result(
                f"⚠️ 您已绑定游戏账号：{old_account}\n"
                f"如需更改，请先使用 /解绑账号 解绑当前账号\n"
                f"或使用 /修改绑定 {game_account} 直接修改绑定"
            )
            return
        
        # 检查此游戏账号是否已被其他QQ绑定
        is_bound, bound_qq, bind_info = self._is_account_already_bound(game_account)
        if is_bound:
            bound_time = bind_info.get("bind_time", "未知时间")
            yield event.plain_result(
                f"❌ 游戏账号 {game_account} 已被其他用户绑定\n"
                f"绑定QQ：{bound_qq}\n"
                f"绑定时间：{bound_time}\n"
                f"如需强制绑定，请联系管理员使用 /强制绑定 命令"
            )
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
        yield event.plain_result(
            f"✅ 绑定成功！\n"
            f"🎮 游戏账号：{account_name}\n"
            f"🆔 账号ID：{uid}\n"
            f"⏰ 绑定时间：{self.bindings[qq_id]['bind_time']}"
        )
    
    @filter.command("修改绑定")
    async def modify_bind_cmd(self, event: AstrMessageEvent):
        """修改绑定：/修改绑定 新游戏账号"""
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result("❌ 格式：/修改绑定 新游戏账号\n例如：/修改绑定 new123")
            return
        
        new_account = parts[1]
        qq_id = self._get_user_id(event)
        
        if qq_id == "unknown":
            yield event.plain_result("❌ 无法获取您的QQ信息")
            return
        
        # 检查是否已绑定
        if qq_id not in self.bindings:
            yield event.plain_result("❌ 您尚未绑定任何游戏账号，请使用 /绑定账号 命令")
            return
        
        # 获取旧账号信息
        old_account = self.bindings[qq_id]["game_account"]
        
        # 检查新账号是否已被绑定（排除自己）
        is_bound, bound_qq, bind_info = self._is_account_already_bound(new_account, exclude_qq=qq_id)
        if is_bound:
            bound_time = bind_info.get("bind_time", "未知时间")
            yield event.plain_result(
                f"❌ 游戏账号 {new_account} 已被其他用户绑定\n"
                f"绑定QQ：{bound_qq}\n"
                f"绑定时间：{bound_time}\n"
                f"无法修改到此账号"
            )
            return
        
        # 验证新账号是否存在
        try:
            account_info = await self._get_account_info(new_account)
            if not account_info:
                yield event.plain_result(f"❌ 游戏账号 {new_account} 不存在，请确认后重试")
                return
        except Exception as e:
            logger.error(f"验证游戏账号失败: {e}")
            yield event.plain_result("❌ 验证游戏账号失败，请检查网络或联系管理员")
            return
        
        # 更新绑定信息
        old_bind_time = self.bindings[qq_id]["bind_time"]
        
        self.bindings[qq_id] = {
            "game_account": new_account,
            "uid": account_info.get("uid", "未知"),
            "account_name": account_info.get("passport", new_account),
            "bind_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "qq_id": qq_id,
            "old_account": old_account,
            "old_bind_time": old_bind_time
        }
        self._save_json(self.bind_file, self.bindings)
        
        account_name = account_info.get("passport", new_account)
        uid = account_info.get("uid", "未知")
        yield event.plain_result(
            f"✅ 修改绑定成功！\n"
            f"🔁 从：{old_account}\n"
            f"🔁 到：{account_name}\n"
            f"🆔 账号ID：{uid}\n"
            f"⏰ 原绑定时间：{old_bind_time}\n"
            f"⏰ 新绑定时间：{self.bindings[qq_id]['bind_time']}"
        )
    
    @filter.command("我的绑定")
    async def mybind_cmd(self, event: AstrMessageEvent):
        """查询绑定信息"""
        qq_id = self._get_user_id(event)
        
        if qq_id in self.bindings:
            data = self.bindings[qq_id]
            response = f"📋 您的绑定信息：\n🎮 游戏账号：{data.get('account_name', '未知')}\n🆔 账号ID：{data.get('uid', '未知')}\n⏰ 绑定时间：{data['bind_time']}"
            
            # 显示修改记录
            if 'old_account' in data:
                response += f"\n📝 由账号 {data['old_account']} 修改而来\n⏰ 原绑定时间：{data['old_bind_time']}"
            
            yield event.plain_result(response)
        else:
            yield event.plain_result("❌ 您尚未绑定游戏账号\n请使用：/绑定账号 游戏账号")
    
    @filter.command("解绑账号")
    async def unbind_account_cmd(self, event: AstrMessageEvent):
        """解绑游戏账号"""
        qq_id = self._get_user_id(event)
        
        if qq_id in self.bindings:
            game_account = self.bindings[qq_id]["game_account"]
            account_name = self.bindings[qq_id].get("account_name", game_account)
            bind_time = self.bindings[qq_id].get("bind_time", "未知时间")
            
            # 记录解绑日志
            unbind_log = {
                "qq_id": qq_id,
                "game_account": game_account,
                "account_name": account_name,
                "unbind_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                "bind_time": bind_time
            }
            
            # 保存到解绑日志文件
            unbind_file = os.path.join(self.data_dir, "unbind_logs.json")
            unbind_logs = self._load_json(unbind_file)
            unbind_logs[f"U{datetime.now().strftime('%Y%m%d%H%M%S')}_{qq_id}"] = unbind_log
            self._save_json(unbind_file, unbind_logs)
            
            # 删除绑定
            del self.bindings[qq_id]
            self._save_json(self.bind_file, self.bindings)
            
            yield event.plain_result(f"✅ 解绑成功！\n已移除账号 {account_name} 的绑定")
        else:
            yield event.plain_result("❌ 您未绑定任何游戏账号")
    
    # ========== 充值功能 ==========
    @filter.command("账号充值")
    async def account_recharge_cmd(self, event: AstrMessageEvent):
        """为绑定账号充值：/账号充值 金额 [备注]"""
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
        
        # 执行充值
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
                yield event.plain_result(f"❌ 充值失败：{error_msg}")
                
        except Exception as e:
            logger.error(f"充值异常：{e}")
            yield event.plain_result(f"❌ 充值过程出现异常，请稍后重试或联系管理员")
    
    @filter.command("查询账号")
    async def query_account_cmd(self, event: AstrMessageEvent):
        """查询游戏账号信息：/查询账号 [游戏账号]"""
        parts = event.message_str.strip().split()
        
        if len(parts) >= 2:
            # 查询指定账号
            game_account = parts[1]
            show_extra_info = True  # 显示额外信息
        else:
            # 查询自己绑定的账号
            qq_id = self._get_user_id(event)
            if qq_id not in self.bindings:
                yield event.plain_result("❌ 您尚未绑定游戏账号，请先绑定或指定游戏账号")
                return
            game_account = self.bindings[qq_id]["game_account"]
            show_extra_info = False  # 不显示额外信息
        
        try:
            account_info = await self._get_account_info(game_account)
            if not account_info:
                yield event.plain_result(f"❌ 游戏账号 {game_account} 不存在")
                return
            
            # 构建基本信息
            info_lines = [
                f"🎮 账号信息：{account_info.get('passport', '未知')}",
                f"🆔 账号ID：{account_info.get('uid', '未知')}",
                f"💰 当前余额：{account_info.get('gold_pay', 0)}",
                f"📈 累计充值：{account_info.get('gold_pay_total', 0)}"
            ]
            
            # 添加额外信息
            if show_extra_info and account_info.get('name'):
                info_lines.append(f"👤 角色名称：{account_info['name']}")
            if show_extra_info and account_info.get('cid'):
                info_lines.append(f"🆔 角色ID：{account_info['cid']}")
            
            # 检查此账号是否被绑定
            is_bound, bound_qq, bind_info = self._is_account_already_bound(game_account)
            if is_bound:
                info_lines.append(f"🔗 绑定状态：已绑定")
                info_lines.append(f"📱 绑定QQ：{bound_qq}")
                info_lines.append(f"⏰ 绑定时间：{bind_info.get('bind_time', '未知')}")
            else:
                info_lines.append(f"🔗 绑定状态：未绑定")
            
            yield event.plain_result("\n".join(info_lines))
            
        except Exception as e:
            logger.error(f"查询账号失败：{e}")
            yield event.plain_result(f"❌ 查询账号失败，请稍后重试")
    
    @filter.command("测试连接")
    async def test_connection_cmd(self, event: AstrMessageEvent):
        """测试API连接"""
        yield event.plain_result("🔄 正在测试API连接...")
        
        try:
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
            
            # 显示修改记录
            if 'old_account' in data:
                lines.append(f"📝 由账号 {data['old_account']} 修改而来")
        
        lines.append(f"\n📊 总计：{count} 条绑定记录")
        
        yield event.plain_result("\n".join(lines))
    
    @filter.command("强制绑定")
    async def force_bind_cmd(self, event: AstrMessageEvent):
        """强制绑定：/强制绑定 QQ号 游戏账号"""
        parts = event.message_str.strip().split()
        if len(parts) < 3:
            yield event.plain_result("❌ 格式：/强制绑定 QQ号 游戏账号\n例如：/强制绑定 123456 test123")
            return
        
        qq_id = parts[1]
        game_account = parts[2]
        
        if not qq_id.isdigit():
            yield event.plain_result("❌ QQ号必须是数字")
            return
        
        # 检查此游戏账号是否已被绑定
        is_bound, bound_qq, bind_info = self._is_account_already_bound(game_account)
        if is_bound:
            bound_time = bind_info.get("bind_time", "未知时间")
            yield event.plain_result(
                f"⚠️ 游戏账号 {game_account} 已被绑定\n"
                f"原绑定QQ：{bound_qq}\n"
                f"绑定时间：{bound_time}\n"
                f"是否要强制替换？(回复: 确认替换 或 取消)"
            )
            
            # 这里可以添加确认逻辑，但为了简化，我们直接强制替换
            # 删除原绑定
            if bound_qq in self.bindings:
                del self.bindings[bound_qq]
        
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
        
        # 检查此QQ是否已绑定其他账号
        old_account = None
        if qq_id in self.bindings:
            old_account = self.bindings[qq_id]["game_account"]
        
        # 保存绑定
        self.bindings[qq_id] = {
            "game_account": game_account,
            "uid": account_info.get("uid", "未知"),
            "account_name": account_info.get("passport", game_account),
            "bind_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "qq_id": qq_id,
            "is_forced": True
        }
        
        if old_account:
            self.bindings[qq_id]["replaced_account"] = old_account
        
        self._save_json(self.bind_file, self.bindings)
        
        account_name = account_info.get("passport", game_account)
        uid = account_info.get("uid", "未知")
        
        response = f"✅ 强制绑定成功！\n🆔 QQ号：{qq_id}\n🎮 游戏账号：{account_name}\n📊 账号ID：{uid}"
        
        if old_account:
            response += f"\n⚠️ 替换了原有绑定账号：{old_account}"
        
        yield event.plain_result(response)
    
    @filter.command("我的信息")
    async def my_info_cmd(self, event: AstrMessageEvent):
        """显示我的QQ信息"""
        qq_id = self._get_user_id(event)
        
        # 检查绑定状态
        if qq_id in self.bindings:
            data = self.bindings[qq_id]
            bind_info = f"✅ 已绑定游戏账号\n账号：{data.get('account_name', '未知')}\n账号ID：{data.get('uid', '未知')}"
            if 'is_forced' in data:
                bind_info += "\n🔧 此绑定为强制绑定"
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
                # 通过passport查询账号
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
                                "gold_pay_total": player.get('total_recharge', 0),
                                "cid": player.get('cid'),
                                "name": player.get('name')
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
                form_data = aiohttp.FormData()
                form_data.add_field("action", "recharge")
                form_data.add_field("passport", passport)  # 使用passport
                form_data.add_field("amount", str(amount))
                form_data.add_field("remark", remark)
                form_data.add_field("source", "qq_bot")  # 来源标识
                form_data.add_field("secret", self.api_config["qq_bot_secret"])  # 使用配置的密钥
                
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
