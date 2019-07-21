#pragma once

#include <Windows.h>

#include <list>
#include <thread>

namespace cgt
{
	//��ʱ����Ϣ
	typedef struct _CGTimerInfo
	{
		int nID;						//��ʱ��ID
		int nInterval;					//�������
		unsigned long long ullLastTime;	//��һ�ε��õļ��ʱ��
		int nCount;						//���ö��ٴκ�ɾ����ǰ��ʱ�� -1=��ɾ��
		bool bLess;						//ͣ�ñ��
		void(*lpfn)(void*);				//����ָ��
		void* pBuffer;					//��������
	}CGTimerInfo,*PCGTimerInfo;

	/*
	�������ܴﵽ����С���Ϊ15��������
	1����Ҳ��1000���룬����һ��������ܵ���66�λص�����
	*/

	class CGTimer
	{
	private:
		std::list<CGTimerInfo> m_cCGTimerList;		//ʹ���м�ʱ������
		std::list<CGTimerInfo> m_cCGTimerListLess;	//ͣ���м�ʱ������
		bool m_bExit;								//�߳��˳����

		void StartThread();								//��ʼ�߳�
		void StopThread();								//�����߳�
		static void CheckCGTimerTick(CGTimer* pThis);	//�̺߳���
	public:
		CGTimer();
		virtual ~CGTimer();
	public:
		//�����ʱ���¼����������������ʱ���¼��������
		virtual bool OnCGTimerEvent(PCGTimerInfo pTimeInfo) = 0;

		bool AddCGTimer(int nID, int nInterval, int nCount,
			void(*lpfn)(void*), void* pBuffer);		//���һ����ʱ��
		bool DeleteCGTimer(int nID);				//ɾ��һ����ʱ��
		bool DeleteAllCGTimer();					//ɾ�����м�ʱ��
		bool EnableCGTimer(int nID, bool bEnable);	//���û�������һ����ʱ��
	};
}
