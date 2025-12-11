import json
import os
import aiohttp
import asyncio
import random
from datetime import datetime, date, timedelta
from typing import Optional, Dict, List
from astrbot.api.event import filter, AstrMessageEvent
from astrbot.api.star import Context, Star, register
from astrbot.api import logger

@register("game_bind", "aa932406", "游戏账号绑定与充值插件", "3.0.0")
class GameBindPlugin(Star):
    def __init__(self, context: Context):
        super().__init__(context)
        # 初始化数据存储
        self.data_dir = os.path.join(os.path.dirname(__file__), "data")
        os.makedirs(self.data_dir, exist_ok=True)
        
        # 数据文件
        self.bind_file = os.path.join(self.data_dir, "bindings.json")
        self.recharge_file = os.path.join(self.data_dir, "recharge_logs.json")
        self.points_file = os.path.join(self.data_dir, "user_points.json")
        self.sign_file = os.path.join(self.data_dir, "sign_records.json")
        
        # 加载数据
        self.bindings = self._load_json(self.bind_file)
        self.recharge_logs = self._load_json(self.recharge_file)
        self.user_points = self._load_json(self.points_file)
        self.sign_records = self._load_json(self.sign_file)
        
        # API配置
        self.api_config = {
            "base_url": "http://115.190.64.181:881/api/players.php",
            "timeout": 30,
            "qq_bot_secret": "ws7ecejjsznhtxurchknmdemax2fnp5d"
        }
        
        # 系统配置
        self.system_config = {
            # 积分系统
            "points": {
                "recharge_ratio": 10000,  # 1积分=10000元宝
                # 签到奖励（积分）
                "sign_rewards": {
                    1: 1,      # 第1天：1积分
                    2: 2,      # 第2天：2积分
                    3: 3,      # 第3天：3积分
                    4: 4,      # 第4天：4积分
                    5: 5,      # 第5天：5积分
                    6: 6,      # 第6天：6积分
                    7: 10,     # 第7天：10积分（周末奖励）
                    14: 15,    # 第14天：15积分
                    30: 30     # 第30天：30积分
                }
            }
        }
        
        logger.info("✨ 游戏账号插件初始化完成！")
    
    def _load_json(self, file_path: str) -> dict:
        """加载JSON文件"""
        try:
            if os.path.exists(file_path):
                with open(file_path, 'r', encoding='utf-8') as f:
                    return json.load(f)
        except Exception as e:
            logger.error(f"📂 加载文件失败 {file_path}: {e}")
        return {}
    
    def _save_json(self, file_path: str, data: dict):
        """保存JSON文件"""
        try:
            with open(file_path, 'w', encoding='utf-8') as f:
                json.dump(data, f, ensure_ascii=False, indent=2)
        except Exception as e:
            logger.error(f"💾 保存文件失败 {file_path}: {e}")
    
    def _get_user_id(self, event: AstrMessageEvent) -> str:
        """获取用户ID"""
        qq_id = ""
        try:
            sender_id = event.get_sender_id()
            if sender_id:
                qq_id = str(sender_id)
                return qq_id
            
            sender_name = event.get_sender_name()
            if sender_name and '/' in sender_name:
                parts = sender_name.split('/')
                if len(parts) >= 2 and parts[1].isdigit():
                    qq_id = parts[1]
                    return qq_id
        except Exception as e:
            logger.error(f"❓ 获取用户ID异常: {e}")
        return qq_id if qq_id else "unknown"
    
    def _is_account_already_bound(self, game_account: str, exclude_qq: str = None) -> tuple:
        """检查游戏账号是否已被绑定"""
        for qq_id, bind_info in self.bindings.items():
            if bind_info.get("game_account") == game_account:
                if exclude_qq and qq_id == exclude_qq:
                    continue
                return True, qq_id, bind_info
        return False, None, None
    
    def _get_user_points(self, qq_id: str) -> Dict:
        """获取用户积分信息"""
        if qq_id not in self.user_points:
            self.user_points[qq_id] = {
                "points": 0,          # 当前积分（元宝余额）
                "total_earned": 0,    # 累计获得积分
                "total_spent": 0,     # 累计消耗积分
                "first_sign_date": None,
                "last_sign_date": None,
                "continuous_days": 0
            }
        return self.user_points[qq_id]
    
    def _update_user_points(self, qq_id: str, points_data: Dict):
        """更新用户积分信息"""
        self.user_points[qq_id] = points_data
        self._save_json(self.points_file, self.user_points)
    
    async def initialize(self):
        logger.info("🚀 游戏账号插件已启动！")
    
    # ========== 帮助功能 ==========
    @filter.command("帮助")
    async def help_cmd(self, event: AstrMessageEvent):
        """显示帮助信息"""
        help_text = """游戏账号插件 - 命令列表

📌 常用命令：
• /绑定账号 <游戏账号>     # 绑定游戏账号
• /我的积分               # 查看积分余额
• /签到                  # 每日签到获得积分
• /积分充值 <积分数量>    # 用积分充值游戏
• /查询账号 [账号]        # 查看账号信息

🔧 其他命令：
• /修改绑定 <新账号>      # 修改绑定账号
• /解绑账号              # 解绑当前账号
• /测试连接              # 测试API连接

💎 规则：
• 1积分 = 10000元宝
• 签到获得积分
• 积分用于充值游戏账号
• 没有积分无法充值"""
        yield event.plain_result(help_text)
    
    # ========== 绑定功能 ==========
    @filter.command("绑定账号")
    async def bind_account_cmd(self, event: AstrMessageEvent):
        """绑定PHP游戏账号"""
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result("❌ 格式错误\n正确格式：/绑定账号 游戏账号\n例如：/绑定账号 xhl2511686")
            return
        
        game_account = parts[1]
        qq_id = self._get_user_id(event)
        
        if qq_id == "unknown":
            yield event.plain_result("❌ 身份验证失败，无法获取QQ信息")
            return
        
        # 检查此QQ是否已绑定
        if qq_id in self.bindings:
            old_account = self.bindings[qq_id]["game_account"]
            bind_time = self.bindings[qq_id]["bind_time"]
            yield event.plain_result(f"⚠️ 已绑定账号\n当前绑定：{old_account}\n绑定时间：{bind_time}\n\n如需更换账号：\n1. 先使用 /解绑账号\n2. 再重新绑定新账号")
            return
        
        # 检查账号是否已被绑定
        is_bound, bound_qq, bind_info = self._is_account_already_bound(game_account)
        if is_bound:
            yield event.plain_result(f"❌ 账号已被绑定\n游戏账号：{game_account}\n已被QQ：{bound_qq} 绑定\n绑定时间：{bind_info.get('bind_time', '未知')}")
            return
        
        # 验证账号是否存在
        try:
            account_info = await self._get_account_info(game_account)
            if not account_info:
                yield event.plain_result(f"❌ 账号不存在\n游戏账号：{game_account}\n在系统中未找到此账号")
                return
        except Exception as e:
            logger.error(f"验证游戏账号失败: {e}")
            yield event.plain_result("❌ 验证失败，网络连接异常，请稍后重试")
            return
        
        # 保存绑定
        self.bindings[qq_id] = {
            "game_account": game_account,
            "account_name": account_info.get("passport", game_account),
            "bind_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "qq_id": qq_id
        }
        self._save_json(self.bind_file, self.bindings)
        
        account_name = account_info.get("passport", game_account)
        content = f"""✅ 绑定成功！

游戏账号：{account_name}
当前余额：{account_info.get('gold_pay', 0):,} 元宝
累计充值：{account_info.get('gold_pay_total', 0):,} 元宝
绑定时间：{self.bindings[qq_id]['bind_time']}"""
        
        yield event.plain_result(content)
    
    # ========== 我的积分功能 ==========
    @filter.command("我的积分")
    async def my_points_cmd(self, event: AstrMessageEvent):
        """查看我的积分"""
        qq_id = self._get_user_id(event)
        
        if qq_id == "unknown":
            yield event.plain_result("❌ 身份验证失败，无法获取QQ信息")
            return
        
        user_points = self._get_user_points(qq_id)
        recharge_ratio = self.system_config["points"]["recharge_ratio"]
        
        content = f"""💰 我的积分

当前积分：{user_points['points']} 积分
累计获得：{user_points['total_earned']} 积分
累计消耗：{user_points['total_spent']} 积分
连续签到：{user_points['continuous_days']} 天

💎 积分用途：
• 1积分 = {recharge_ratio:,}元宝
• 可兑换：{user_points['points'] * recharge_ratio:,}元宝
• 使用 /积分充值 命令兑换"""
        
        if user_points["last_sign_date"]:
            content += f"\n\n📅 上次签到：{user_points['last_sign_date']}"
        
        content += "\n\n💡 每日签到可获得积分！"
        
        yield event.plain_result(content)
    
    # ========== 签到功能 ==========
    @filter.command("签到")
    async def sign_cmd(self, event: AstrMessageEvent):
        """每日签到获得积分"""
        qq_id = self._get_user_id(event)
        
        if qq_id == "unknown":
            yield event.plain_result("❌ 身份验证失败，无法获取QQ信息")
            return
        
        today = date.today().isoformat()
        
        # 检查是否已签到
        if qq_id in self.sign_records and self.sign_records[qq_id].get("last_sign") == today:
            user_points = self._get_user_points(qq_id)
            yield event.plain_result(f"⏳ 今日已签到\n签到时间：今天\n下次签到：明天\n当前积分：{user_points['points']} 积分")
            return
        
        user_points = self._get_user_points(qq_id)
        
        # 计算连续天数
        yesterday = (date.today() - timedelta(days=1)).isoformat()
        if user_points["last_sign_date"] == yesterday:
            user_points["continuous_days"] += 1
        elif user_points["last_sign_date"] != today:
            user_points["continuous_days"] = 1
        
        # 计算签到奖励（积分）
        continuous_days = user_points["continuous_days"]
        
        # 基础奖励
        base_reward = 1  # 默认1积分
        
        # 特殊天数奖励
        for day, reward in self.system_config["points"]["sign_rewards"].items():
            if continuous_days == day:
                base_reward = reward
                break
        else:
            # 如果不在特殊天数列表中，使用连续天数作为奖励（最高10积分）
            base_reward = min(continuous_days, 10)
        
        total_reward = base_reward
        
        # 更新积分
        user_points["points"] += total_reward
        user_points["total_earned"] += total_reward
        user_points["last_sign_date"] = today
        
        if not user_points["first_sign_date"]:
            user_points["first_sign_date"] = today
        
        self._update_user_points(qq_id, user_points)
        
        # 保存签到记录
        self.sign_records[qq_id] = {
            "last_sign": today,
            "reward": total_reward,
            "continuous_days": continuous_days
        }
        self._save_json(self.sign_file, self.sign_records)
        
        # 构建响应
        recharge_ratio = self.system_config["points"]["recharge_ratio"]
        content = f"""✨ 签到成功！

获得积分：{total_reward} 积分
连续签到：{continuous_days} 天
当前积分：{user_points['points']} 积分
累计获得：{user_points['total_earned']} 积分

💎 积分价值：
• 可兑换：{total_reward * recharge_ratio:,} 元宝
• 总可兑换：{user_points['points'] * recharge_ratio:,} 元宝

⏰ 签到时间：{datetime.now().strftime('%Y-%m-%d %H:%M')}"""
        
        yield event.plain_result(content)
    
    # ========== 积分充值功能 ==========
    @filter.command("积分充值")
    async def points_recharge_cmd(self, event: AstrMessageEvent):
        """使用积分充值游戏账号"""
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result("❌ 格式错误\n正确格式：/积分充值 <积分数量> [备注]\n例如：/积分充值 10 兑换元宝")
            return
        
        try:
            points_to_use = int(parts[1])
            if points_to_use <= 0:
                raise ValueError("必须是正数")
            remark = " ".join(parts[2:]) if len(parts) > 2 else "积分兑换"
        except ValueError:
            yield event.plain_result("❌ 参数错误，积分数量必须是正整数")
            return
        
        qq_id = self._get_user_id(event)
        
        if qq_id == "unknown":
            yield event.plain_result("❌ 身份验证失败，无法获取QQ信息")
            return
        
        # 检查绑定
        if qq_id not in self.bindings:
            yield event.plain_result("❌ 未绑定账号\n请先绑定游戏账号\n使用命令：/绑定账号 <游戏账号>")
            return
        
        user_points = self._get_user_points(qq_id)
        
        if user_points["points"] < points_to_use:
            yield event.plain_result(f"❌ 积分不足\n需要积分：{points_to_use}\n当前积分：{user_points['points']}\n\n💡 获取积分：每日签到，多签多得")
            return
        
        # 计算充值金额（1积分=10000元宝）
        recharge_ratio = self.system_config["points"]["recharge_ratio"]
        recharge_amount = points_to_use * recharge_ratio
        
        game_account = self.bindings[qq_id]["game_account"]
        account_name = self.bindings[qq_id].get("account_name", game_account)
        
        # 执行充值
        try:
            result = await self._execute_account_recharge(game_account, recharge_amount, remark)
            
            if result.get("success"):
                # 扣减积分
                user_points["points"] -= points_to_use
                user_points["total_spent"] += points_to_use
                self._update_user_points(qq_id, user_points)
                
                # 记录充值日志
                recharge_id = f"P{datetime.now().strftime('%Y%m%d%H%M%S')}_{qq_id}"
                self.recharge_logs[recharge_id] = {
                    "qq_id": qq_id,
                    "game_account": game_account,
                    "account_name": account_name,
                    "points_used": points_to_use,
                    "recharge_amount": recharge_amount,
                    "remark": remark,
                    "recharge_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                    "api_response": result
                }
                self._save_json(self.recharge_file, self.recharge_logs)
                
                response_data = result.get("data", {})
                
                content = f"""✅ 充值成功！

游戏账号：{account_name}
消耗积分：{points_to_use} 积分
充值金额：{recharge_amount:,} 元宝
充值备注：{remark}

📊 账户信息：
新余额：{response_data.get('new_gold_pay', '未知'):,}
累计充值：{response_data.get('new_gold_pay_total', '未知'):,}
剩余积分：{user_points['points']} 积分

⏰ 充值时间：{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"""
                
                yield event.plain_result(content)
            else:
                error_msg = result.get("error", "未知错误")
                yield event.plain_result(f"❌ 充值失败\n错误信息：{error_msg}")
                
        except Exception as e:
            logger.error(f"充值异常：{e}")
            yield event.plain_result("❌ 充值异常，请稍后重试或联系管理员")
    
    # ========== 查询账号功能 ==========
    @filter.command("查询账号")
    async def query_account_cmd(self, event: AstrMessageEvent):
        """查询游戏账号信息"""
        parts = event.message_str.strip().split()
        
        if len(parts) >= 2:
            # 查询指定账号
            game_account = parts[1]
            show_extra_info = True
        else:
            # 查询自己绑定的账号
            qq_id = self._get_user_id(event)
            if qq_id not in self.bindings:
                yield event.plain_result("❌ 未绑定账号\n请先绑定游戏账号或指定要查询的账号\n\n💡 使用方法：\n1. /查询账号 xhl2511686\n2. 或先绑定账号再查询")
                return
            game_account = self.bindings[qq_id]["game_account"]
            show_extra_info = False
        
        try:
            account_info = await self._get_account_info(game_account)
            if not account_info:
                yield event.plain_result(f"❌ 账号不存在\n游戏账号 {game_account} 不存在")
                return
        except Exception as e:
            logger.error(f"查询账号失败：{e}")
            yield event.plain_result("❌ 查询失败，网络连接异常，请稍后重试")
            return
        
        # 构建基本信息
        content = f"""🎮 账号信息

游戏账号：{account_info.get('passport', '未知')}
当前余额：{account_info.get('gold_pay', 0):,} 元宝
累计充值：{account_info.get('gold_pay_total', 0):,} 元宝"""
        
        # 添加额外信息
        if show_extra_info and account_info.get('name'):
            content += f"\n角色名称：{account_info['name']}"
        if show_extra_info and account_info.get('cid'):
            content += f"\n角色ID：{account_info['cid']}"
        
        # 检查此账号是否被绑定
        is_bound, bound_qq, bind_info = self._is_account_already_bound(game_account)
        content += f"\n\n绑定状态：{'已绑定' if is_bound else '未绑定'}"
        
        if is_bound:
            content += f"\n绑定QQ：{bound_qq}\n绑定时间：{bind_info.get('bind_time', '未知')}"
        else:
            content += f"\n💡 使用 /绑定账号 可绑定此账号"
        
        yield event.plain_result(content)
    
    # ========== 修改绑定功能 ==========
    @filter.command("修改绑定")
    async def modify_bind_cmd(self, event: AstrMessageEvent):
        """修改绑定账号"""
        parts = event.message_str.strip().split()
        if len(parts) < 2:
            yield event.plain_result("❌ 格式错误\n正确格式：/修改绑定 新游戏账号\n例如：/修改绑定 new_account")
            return
        
        new_account = parts[1]
        qq_id = self._get_user_id(event)
        
        if qq_id == "unknown":
            yield event.plain_result("❌ 身份验证失败，无法获取QQ信息")
            return
        
        # 检查是否已绑定
        if qq_id not in self.bindings:
            yield event.plain_result("❌ 未绑定账号\n您尚未绑定任何游戏账号\n请先使用 /绑定账号 命令")
            return
        
        # 获取旧账号信息
        old_account = self.bindings[qq_id]["game_account"]
        old_bind_time = self.bindings[qq_id]["bind_time"]
        
        # 检查新账号是否已被绑定（排除自己）
        is_bound, bound_qq, bind_info = self._is_account_already_bound(new_account, exclude_qq=qq_id)
        if is_bound:
            yield event.plain_result(f"❌ 账号已被绑定\n游戏账号：{new_account}\n已被QQ：{bound_qq} 绑定\n绑定时间：{bind_info.get('bind_time', '未知')}")
            return
        
        # 验证新账号是否存在
        try:
            account_info = await self._get_account_info(new_account)
            if not account_info:
                yield event.plain_result(f"❌ 账号不存在\n游戏账号 {new_account} 不存在")
                return
        except Exception as e:
            logger.error(f"验证游戏账号失败: {e}")
            yield event.plain_result("❌ 验证失败，网络连接异常，请稍后重试")
            return
        
        # 更新绑定信息
        self.bindings[qq_id] = {
            "game_account": new_account,
            "account_name": account_info.get("passport", new_account),
            "bind_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "qq_id": qq_id,
            "old_account": old_account,
            "old_bind_time": old_bind_time
        }
        self._save_json(self.bind_file, self.bindings)
        
        account_name = account_info.get("passport", new_account)
        content = f"""✅ 修改成功！

原账号：{old_account}
新账号：{account_name}
当前余额：{account_info.get('gold_pay', 0):,} 元宝
原绑定：{old_bind_time}
新绑定：{self.bindings[qq_id]['bind_time']}"""
        
        yield event.plain_result(content)
    
    # ========== 解绑功能 ==========
    @filter.command("解绑账号")
    async def unbind_account_cmd(self, event: AstrMessageEvent):
        """解绑游戏账号"""
        qq_id = self._get_user_id(event)
        
        if qq_id in self.bindings:
            game_account = self.bindings[qq_id]["game_account"]
            account_name = self.bindings[qq_id].get("account_name", game_account)
            bind_time = self.bindings[qq_id].get("bind_time", "未知时间")
            
            # 删除绑定
            del self.bindings[qq_id]
            self._save_json(self.bind_file, self.bindings)
            
            content = f"""✅ 解绑成功！

已解绑账号：{account_name}
原绑定时间：{bind_time}
解绑时间：{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}

💡 如需重新绑定，请使用 /绑定账号 命令"""
            
            yield event.plain_result(content)
        else:
            yield event.plain_result("⚠️ 未绑定账号\n您未绑定任何游戏账号")
    
    # ========== 测试连接功能 ==========
    @filter.command("测试连接")
    async def test_connection_cmd(self, event: AstrMessageEvent):
        """测试API连接"""
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
                            content = f"""✅ API连接正常！

连接状态：正常
账号数量：{result['data']['total']:,} 个
响应时间：正常
服务状态：在线"""
                            yield event.plain_result(content)
                        else:
                            error_msg = result.get('error', '未知错误')
                            yield event.plain_result(f"⚠️ API异常\nAPI响应异常：{error_msg}")
                    else:
                        yield event.plain_result(f"❌ 连接失败\nAPI连接失败，状态码：{response.status}")
                        
        except Exception as e:
            yield event.plain_result(f"❌ 连接失败\nAPI连接失败：{str(e)}\n请检查API地址和网络配置")
    
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
