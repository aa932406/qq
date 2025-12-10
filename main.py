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
        self.points_file = os.path.join(self.data_dir, "user_points.json")
        self.sign_file = os.path.join(self.data_dir, "sign_records.json")
        
        # 加载数据
        self.bindings = self._load_json(self.bind_file)
        self.user_points = self._load_json(self.points_file)
        self.sign_records = self._load_json(self.sign_file)
        
        # API配置
        self.api_config = {
            "base_url": "http://115.190.64.181:881/api/players.php",
            "timeout": 30,
            "qq_bot_secret": "ws7ecejjsznhtxurchknmdemax2fnp5d"
        }
        
        logger.info("✨ 游戏账号插件初始化完成！")
    
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
        """获取用户ID"""
        try:
            sender_id = event.get_sender_id()
            if sender_id:
                return str(sender_id)
        except Exception as e:
            logger.error(f"获取用户ID异常: {e}")
        return "unknown"
    
    async def initialize(self):
        logger.info("🚀 游戏账号插件已启动！")
    
    # ========== 关键词触发处理器 ==========
    @filter.on_message()
    async def keyword_handler(self, event: AstrMessageEvent):
        """处理关键词触发"""
        msg = event.message_str.strip()
        
        # 如果消息包含@机器人，去掉@部分
        if "@" in msg:
            # 提取@后面的内容
            parts = msg.split("@")
            if len(parts) > 1:
                msg = parts[1].strip()
        
        # 关键词映射
        keyword_actions = {
            "帮助": self._show_help,
            "绑定账号": self._bind_account,
            "我的积分": self._show_points,
            "签到": self._daily_sign,
            "积分充值": self._recharge_points,
            "查询账号": self._query_account,
            "修改绑定": self._modify_bind,
            "解绑账号": self._unbind_account,
            "测试连接": self._test_connection
        }
        
        # 检查消息是否包含关键词
        for keyword, action in keyword_actions.items():
            if keyword in msg:
                # 如果是"绑定账号"等需要参数的命令，提取参数
                if keyword in ["绑定账号", "积分充值", "查询账号", "修改绑定"]:
                    await action(event, msg)
                else:
                    await action(event)
                break
    
    # ========== 处理函数 ==========
    async def _show_help(self, event: AstrMessageEvent):
        """显示帮助"""
        help_text = """游戏插件关键词：
• 帮助 - 显示帮助信息
• 绑定账号 游戏账号 - 绑定游戏账号
• 我的积分 - 查看积分余额
• 签到 - 每日签到得积分
• 积分充值 数量 - 用积分充值游戏
• 查询账号 [账号] - 查询账号信息
• 修改绑定 新账号 - 修改绑定账号
• 解绑账号 - 解绑当前账号
• 测试连接 - 测试API连接

说明：直接发送关键词即可，不需要加/或@"""
        
        yield event.plain_result(help_text)
    
    async def _bind_account(self, event: AstrMessageEvent, msg: str):
        """绑定账号"""
        parts = msg.split()
        if len(parts) < 2:
            yield event.plain_result("格式：绑定账号 游戏账号\n例如：绑定账号 xhl2511686")
            return
        
        qq_id = self._get_user_id(event)
        if qq_id == "unknown":
            yield event.plain_result("无法获取用户ID")
            return
        
        game_account = parts[1]
        
        # 检查是否已绑定
        if qq_id in self.bindings:
            old_account = self.bindings[qq_id]["game_account"]
            yield event.plain_result(f"你已绑定账号：{old_account}")
            return
        
        # 验证账号是否存在
        try:
            account_info = await self._get_account_info(game_account)
            if not account_info:
                yield event.plain_result(f"账号 {game_account} 不存在")
                return
        except Exception as e:
            logger.error(f"验证账号失败: {e}")
            yield event.plain_result("网络异常，请稍后重试")
            return
        
        # 保存绑定
        self.bindings[qq_id] = {
            "game_account": game_account,
            "account_name": account_info.get("passport", game_account),
            "bind_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "qq_id": qq_id
        }
        self._save_json(self.bind_file, self.bindings)
        
        content = f"""✅ 绑定成功！

游戏账号：{account_info.get('passport', game_account)}
当前余额：{account_info.get('gold_pay', 0):,} 元宝
累计充值：{account_info.get('gold_pay_total', 0):,} 元宝
绑定时间：{self.bindings[qq_id]['bind_time']}"""
        
        yield event.plain_result(content)
    
    async def _show_points(self, event: AstrMessageEvent):
        """查看积分"""
        qq_id = self._get_user_id(event)
        if qq_id == "unknown":
            yield event.plain_result("无法获取用户ID")
            return
        
        # 获取用户积分
        if qq_id not in self.user_points:
            self.user_points[qq_id] = {
                "points": 0,
                "total_earned": 0,
                "last_sign": None,
                "continuous_days": 0
            }
        
        user_points = self.user_points[qq_id]
        points = user_points.get("points", 0)
        
        # 检查绑定状态
        bind_status = ""
        if qq_id in self.bindings:
            account_name = self.bindings[qq_id].get("account_name", "未知")
            bind_status = f"绑定账号：{account_name}"
        else:
            bind_status = "未绑定账号"
        
        content = f"""💰 我的积分

当前积分：{points}
累计获得：{user_points.get('total_earned', 0)}
连续签到：{user_points.get('continuous_days', 0)} 天
{bind_status}

💎 积分规则：
• 1积分 = 10000元宝
• 使用"积分充值"命令兑换元宝"""
        
        yield event.plain_result(content)
    
    async def _daily_sign(self, event: AstrMessageEvent):
        """每日签到"""
        qq_id = self._get_user_id(event)
        if qq_id == "unknown":
            yield event.plain_result("无法获取用户ID")
            return
        
        today = date.today().isoformat()
        
        # 获取用户积分
        if qq_id not in self.user_points:
            self.user_points[qq_id] = {
                "points": 0,
                "total_earned": 0,
                "last_sign": None,
                "continuous_days": 0
            }
        
        user_points = self.user_points[qq_id]
        
        # 检查是否已签到
        if user_points.get("last_sign") == today:
            yield event.plain_result(f"今日已签到\n当前积分：{user_points['points']}")
            return
        
        # 计算连续天数
        if user_points.get("last_sign") == (date.today() - timedelta(days=1)).isoformat():
            user_points["continuous_days"] = user_points.get("continuous_days", 0) + 1
        else:
            user_points["continuous_days"] = 1
        
        # 签到奖励（根据连续天数）
        continuous_days = user_points["continuous_days"]
        if continuous_days <= 7:
            reward = continuous_days  # 1-7天：1-7积分
        elif continuous_days <= 14:
            reward = 7 + (continuous_days - 7) * 2  # 8-14天：每天+2
        else:
            reward = 21 + (continuous_days - 14) * 3  # 15天以上：每天+3
        
        # 更新积分
        user_points["points"] = user_points.get("points", 0) + reward
        user_points["total_earned"] = user_points.get("total_earned", 0) + reward
        user_points["last_sign"] = today
        
        self.user_points[qq_id] = user_points
        self._save_json(self.points_file, self.user_points)
        
        # 保存签到记录
        self.sign_records[qq_id] = {
            "last_sign": today,
            "reward": reward,
            "continuous_days": continuous_days
        }
        self._save_json(self.sign_file, self.sign_records)
        
        content = f"""✨ 签到成功！

获得积分：{reward}
连续签到：{continuous_days} 天
当前积分：{user_points['points']}
累计获得：{user_points['total_earned']}

💎 可兑换：{reward * 10000:,} 元宝
⏰ 时间：{datetime.now().strftime('%Y-%m-%d %H:%M')}"""
        
        yield event.plain_result(content)
    
    async def _recharge_points(self, event: AstrMessageEvent, msg: str):
        """积分充值"""
        parts = msg.split()
        if len(parts) < 2:
            yield event.plain_result("格式：积分充值 数量\n例如：积分充值 10")
            return
        
        try:
            points = int(parts[1])
            if points <= 0:
                raise ValueError
        except:
            yield event.plain_result("请输入正确的积分数量")
            return
        
        qq_id = self._get_user_id(event)
        if qq_id == "unknown":
            yield event.plain_result("无法获取用户ID")
            return
        
        # 检查是否绑定
        if qq_id not in self.bindings:
            yield event.plain_result("请先绑定游戏账号")
            return
        
        # 检查积分是否足够
        user_points = self.user_points.get(qq_id, {"points": 0})
        if user_points.get("points", 0) < points:
            yield event.plain_result(f"积分不足\n需要：{points}\n当前：{user_points.get('points', 0)}")
            return
        
        # 计算充值金额（1积分=10000元宝）
        recharge_amount = points * 10000
        game_account = self.bindings[qq_id]["game_account"]
        
        # 执行充值
        try:
            result = await self._execute_account_recharge(game_account, recharge_amount, "积分兑换")
            
            if result.get("success"):
                # 扣减积分
                user_points["points"] = user_points.get("points", 0) - points
                self.user_points[qq_id] = user_points
                self._save_json(self.points_file, self.user_points)
                
                yield event.plain_result(f"✅ 充值成功！\n消耗积分：{points}\n充值金额：{recharge_amount:,} 元宝\n剩余积分：{user_points['points']}")
            else:
                error_msg = result.get("error", "未知错误")
                yield event.plain_result(f"充值失败：{error_msg}")
        except Exception as e:
            logger.error(f"充值异常：{e}")
            yield event.plain_result("充值异常，请稍后重试")
    
    async def _query_account(self, event: AstrMessageEvent, msg: str):
        """查询账号"""
        parts = msg.split()
        
        if len(parts) >= 2:
            # 查询指定账号
            game_account = parts[1]
        else:
            # 查询自己绑定的账号
            qq_id = self._get_user_id(event)
            if qq_id not in self.bindings:
                yield event.plain_result("请先绑定账号或指定要查询的账号\n格式：查询账号 游戏账号")
                return
            game_account = self.bindings[qq_id]["game_account"]
        
        try:
            account_info = await self._get_account_info(game_account)
            if not account_info:
                yield event.plain_result(f"账号 {game_account} 不存在")
                return
        except Exception as e:
            logger.error(f"查询账号失败：{e}")
            yield event.plain_result("网络异常，请稍后重试")
            return
        
        content = f"""📋 账号信息

游戏账号：{account_info.get('passport', '未知')}
当前余额：{account_info.get('gold_pay', 0):,} 元宝
累计充值：{account_info.get('gold_pay_total', 0):,} 元宝"""
        
        if account_info.get('name'):
            content += f"\n角色名称：{account_info['name']}"
        
        yield event.plain_result(content)
    
    async def _modify_bind(self, event: AstrMessageEvent, msg: str):
        """修改绑定"""
        parts = msg.split()
        if len(parts) < 2:
            yield event.plain_result("格式：修改绑定 新游戏账号\n例如：修改绑定 new_account")
            return
        
        new_account = parts[1]
        qq_id = self._get_user_id(event)
        
        if qq_id not in self.bindings:
            yield event.plain_result("你还没有绑定账号")
            return
        
        # 验证新账号是否存在
        try:
            account_info = await self._get_account_info(new_account)
            if not account_info:
                yield event.plain_result(f"账号 {new_account} 不存在")
                return
        except Exception as e:
            logger.error(f"验证账号失败: {e}")
            yield event.plain_result("网络异常，请稍后重试")
            return
        
        # 更新绑定
        old_account = self.bindings[qq_id]["game_account"]
        self.bindings[qq_id] = {
            "game_account": new_account,
            "account_name": account_info.get("passport", new_account),
            "bind_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "qq_id": qq_id,
            "old_account": old_account
        }
        self._save_json(self.bind_file, self.bindings)
        
        content = f"""✅ 修改成功！

原账号：{old_account}
新账号：{account_info.get('passport', new_account)}
当前余额：{account_info.get('gold_pay', 0):,} 元宝
修改时间：{self.bindings[qq_id]['bind_time']}"""
        
        yield event.plain_result(content)
    
    async def _unbind_account(self, event: AstrMessageEvent):
        """解绑账号"""
        qq_id = self._get_user_id(event)
        
        if qq_id in self.bindings:
            account_name = self.bindings[qq_id].get("account_name", "未知")
            del self.bindings[qq_id]
            self._save_json(self.bind_file, self.bindings)
            
            yield event.plain_result(f"✅ 解绑成功！\n已解绑账号：{account_name}")
        else:
            yield event.plain_result("你还没有绑定账号")
    
    async def _test_connection(self, event: AstrMessageEvent):
        """测试连接"""
        try:
            async with aiohttp.ClientSession() as session:
                params = {"action": "search", "page": 1, "pageSize": 1}
                async with session.get(self.api_config["base_url"], params=params, timeout=30) as response:
                    if response.status == 200:
                        result = await response.json()
                        if result.get("success"):
                            yield event.plain_result(f"✅ API连接正常！\n账号数量：{result['data']['total']:,} 个")
                        else:
                            error_msg = result.get('error', '未知错误')
                            yield event.plain_result(f"API异常：{error_msg}")
                    else:
                        yield event.plain_result(f"连接失败，状态码：{response.status}")
        except Exception as e:
            yield event.plain_result(f"连接失败：{str(e)}")
    
    # ========== API调用方法 ==========
    async def _get_account_info(self, passport: str) -> Optional[dict]:
        """调用API查询账号信息"""
        try:
            async with aiohttp.ClientSession() as session:
                params = {
                    "action": "search",
                    "passport": passport,
                    "page": 1,
                    "pageSize": 1
                }
                
                async with session.get(
                    self.api_config["base_url"],
                    params=params,
                    timeout=30
                ) as response:
                    if response.status == 200:
                        result = await response.json()
                        if result.get("success") and result['data']['total'] > 0:
                            player = result['data']['players'][0]
                            return {
                                "passport": player.get('passport'),
                                "gold_pay": player.get('cash_gold', 0),
                                "gold_pay_total": player.get('total_recharge', 0),
                                "name": player.get('name')
                            }
        except Exception as e:
            logger.error(f"查询账号异常：{e}")
        return None
    
    async def _execute_account_recharge(self, passport: str, amount: float, remark: str) -> dict:
        """调用API为账号执行充值"""
        try:
            async with aiohttp.ClientSession() as session:
                form_data = aiohttp.FormData()
                form_data.add_field("action", "recharge")
                form_data.add_field("passport", passport)
                form_data.add_field("amount", str(amount))
                form_data.add_field("remark", remark)
                form_data.add_field("source", "qq_bot")
                form_data.add_field("secret", self.api_config["qq_bot_secret"])
                
                async with session.post(
                    self.api_config["base_url"],
                    data=form_data,
                    timeout=30
                ) as response:
                    if response.status == 200:
                        return await response.json()
        except Exception as e:
            logger.error(f"充值请求异常：{e}")
        return {"success": False, "error": "请求失败"}
    
    async def terminate(self):
        logger.info("游戏账号插件已禁用")
