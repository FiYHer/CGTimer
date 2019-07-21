#include "CGTimer.h"


cgt::CGTimer::CGTimer()
{
	//true����߳����˳�
	m_bExit = true;
}

cgt::CGTimer::~CGTimer()
{
	DeleteAllCGTimer();
}

void cgt::CGTimer::StartThread()
{
	if (m_cCGTimerList.size())
	{
		if (m_bExit)
		{
			m_bExit = false;
			std::thread cThread(CheckCGTimerTick, this);
			cThread.detach();
		}
	}
}

void cgt::CGTimer::StopThread()
{
	m_bExit = true;
}

void cgt::CGTimer::CheckCGTimerTick(CGTimer* pThis)
{
	unsigned long long ullCurrentTick;
	while (!pThis->m_bExit)
	{
		ullCurrentTick = GetTickCount64();
		for (std::list<CGTimerInfo>::iterator it = pThis->m_cCGTimerList.begin();
			it != pThis->m_cCGTimerList.end(); it++)
		{
			//���ô������˻��߱�ͣ��
			if (it->nCount == 0 || it->bLess)
			{
				pThis->m_cCGTimerList.erase(it);
				if (!pThis->m_cCGTimerList.size())
					pThis->m_bExit = true;
				break;
			}
			//������ʱ��������
			if (it->ullLastTime + it->nInterval < ullCurrentTick)
			{
				it->ullLastTime = ullCurrentTick;
				if (it->nCount > 0)
					it->nCount--;
				if(it->lpfn)
					it->lpfn(it->pBuffer);
				pThis->OnCGTimerEvent(&(*it));
			}
		}
		//����ܴ���ȼ���CPUռ��
		Sleep(1);
	}
}

bool cgt::CGTimer::AddCGTimer(int nID, int nInterval, 
	int nCount, void(*lpfn)(void*), void* pBuffer)
{
	//���ID���Ƿ��г�ͻ
	for (std::list<CGTimerInfo>::iterator it = m_cCGTimerList.begin();
		it != m_cCGTimerList.end(); it++)
	{
		if (it->nID == nID)
			return false;
	}

	CGTimerInfo stTime;
	stTime.bLess = false;
	stTime.lpfn = lpfn;
	stTime.nCount = nCount;
	stTime.nID = nID;
	stTime.nInterval = nInterval;
	stTime.pBuffer = pBuffer;
	stTime.ullLastTime = 0;

	m_cCGTimerList.emplace_back(stTime);
	StartThread();
	return true;
}

bool cgt::CGTimer::DeleteCGTimer(int nID)
{
	for (std::list<CGTimerInfo>::iterator it = m_cCGTimerList.begin();
		it != m_cCGTimerList.end(); it++)
	{
		if (it->nID == nID)
		{
			//���ô���Ϊ0�����߳���ɾ���������̲߳�ͬ������
			it->nCount = 0;
			return true;
		}
	}
	return false;
}

bool cgt::CGTimer::DeleteAllCGTimer()
{
	StopThread();
	m_cCGTimerList.clear();
	m_cCGTimerListLess.clear();
	return true;
}

bool cgt::CGTimer::EnableCGTimer(int nID, bool bEnable)
{
	//���ü�ʱ��������ʱ����Ϣ��ͣ�ü�ʱ����������ʹ�ü�ʱ�������к�ɾ��
	if (bEnable)
	{
		for (std::list<CGTimerInfo>::iterator it = m_cCGTimerListLess.begin();
			it != m_cCGTimerListLess.end(); it++)
		{
			if (it->nID == nID)
			{
				it->bLess = false;
				m_cCGTimerList.emplace_back(*it);
				m_cCGTimerListLess.erase(it);
				StartThread();
				return true;
			}
		}
		return false;
	}
	//ͣ�ü�ʱ��������ʱ����Ϣ��ʹ�ü�ʱ����������ʹ�ü�ʱ�������У����߳���ɾ��
	for (std::list<CGTimerInfo>::iterator it = m_cCGTimerList.begin();
		it != m_cCGTimerList.end(); it++)
	{
		if (it->nID = nID)
		{
			m_cCGTimerListLess.emplace_back(*it);
			it->bLess = true;
			return true;
		}
	}
	return false;
}
