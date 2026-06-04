#ifndef __CHAR_TITLE_H__
#define __CHAR_TITLE_H__

#include "ExtSystemBase.h"

class Player;

struct MemTitle
{
	int32_t nType;		// 称号类型
	int32_t nValue;		// 称号值/参数
	int32_t nExpireTime;	// 过期时间
};

typedef std::vector<MemTitle> TitleVector;

class CExtCharTitle : public CExtSystemBase
{
public:
	CExtCharTitle();
	virtual ~CExtCharTitle();

public:
	virtual void		OnCleanUp();
	virtual void		OnLoadFromDB( const PlayerDBData& dbData );
	virtual void		OnSaveToDB( PlayerDBData& dbData );
	virtual void		OnUpdate( int64_t curTick );
	virtual void		OnDaySwitch( int32_t nDiffDays );

public:
	// 移除指定类型的称号
	void				RemoveTitle( int32_t nType, int32_t nParam = 0 );

	// 检查并添加称号（如果玩家已有更高级则跳过）
	bool				CheckAddTitle( int32_t nType, int32_t nValue );

	// 直接添加称号
	void				AddTitle( int32_t nType, int32_t nValue, int32_t nExpireTime = 0 );

	// 发送称号列表协议包到客户端
	void				SendTitleInfo();

	// 获取指定类型称号的值
	int32_t				GetTitleValue( int32_t nType ) const;

	// 是否有指定类型称号
	bool				HasTitle( int32_t nType ) const;

private:
	// 清理已过期的称号
	void				CleanExpiredTitles();

	// 发送系统提示消息
	void				SendTitleNotice( int32_t nNoticeType, int32_t nTitleType, int32_t nTitleValue );

private:
	TitleVector			m_vTitles;
	int32_t				m_nLastExpireCheck;	// 上次过期检查时间
};

#endif	//__CHAR_TITLE_H__
