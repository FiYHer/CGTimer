#include "CGTimer.h"


cgt::CGTimer::CGTimer()
{
	//true标记线程已退出
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
			//调用次数到了或者被停用
			if (it->nCount == 0 || it->bLess)
			{
				pThis->m_cCGTimerList.erase(it);
				if (!pThis->m_cCGTimerList.size())
					pThis->m_bExit = true;
				break;
			}
			//超过计时器毫秒间隔
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
		//这个能大幅度减少CPU占用
		Sleep(1);
	}
}

bool cgt::CGTimer::AddCGTimer(int nID, int nInterval, 
	int nCount, void(*lpfn)(void*), void* pBuffer)
{
	//检查ID号是否有冲突
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
			//设置次数为0，在线程中删除，避免线程不同步问题
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
	//启用计时器，将计时器信息从停用计时器容器放入使用计时器容器中后删除
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
	//停用计时器，将计时器信息从使用计时器容器放入使用计时器容器中，在线程中删除
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
