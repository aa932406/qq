#ifndef __FUNCTION_OPEN_H__
#define __FUNCTION_OPEN_H__
#include "ExtSystemBase.h"

enum FunctionType
{
	FT_DUAN_ZHAO				= 9,		//����
	FT_CYCLE_TASK				= 26,		//ѭ������
	FT_CHOU_JIANG				= 50,		//�齱
	FT_FA_BAO					= 51,		//����
	FT_BOSS						= 53,		//boss
	FT_JUE_WEI					= 54,		//��λ
	FT_BAI_TAN					= 55,		//��̯
	FT_LING_SOU					= 56,		//����
	FT_QI_FU					= 57,		//����
	FT_HUN_LI					= 58,		//����
	FT_HUO_DONG_DA_TING			= 59,		//�����
	FT_JIANG_LI_DA_TING			= 60,		//��������
	FT_SHOU_CHONG_ICON			= 62,		//�׳�ͼ�����
	FT_NEW_SERVER_TE_HUI		= 63,		//�·��ػ�
	FT_EVERYDAY_SHOUCHONG		= 64,		//ÿ���׳�
	FT_TOU_ZI					= 65,		//Ͷ��
	FT_KAI_FU_HUO_DONG			= 66,		//�����
	FT_GUAN_WEI					= 67,		//��λ
	FT_HALL_OF_FAME				= 68,		//������
	FT_HUO_YUE_DU				= 197,		//��Ծ��
};


class CFunctionOpen:
	public CExtSystemBase
{
public:
	CFunctionOpen();
	~CFunctionOpen();
	virtual	void		OnCleanUp();
	void				InitFunctionOpen( int32_t TaskId, int32_t Level );
	void				CheckFunctionOpne( int32_t TaskId, int32_t Level );
	bool				IsOpened( int32_t FunctionId );
	void				FunctionInit( int32_t FunctionId );
private:
	std::list<int32_t> m_OpenedList;			//�ѿ�����
};

#endif