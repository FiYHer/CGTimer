#pragma once

#include <Windows.h>

#include <list>
#include <thread>

namespace cgt
{
	//计时器信息
	typedef struct _CGTimerInfo
	{
		int nID;						//计时器ID
		int nInterval;					//间隔毫秒
		unsigned long long ullLastTime;	//上一次调用的间隔时间
		int nCount;						//调用多少次后删除当前计时器 -1=不删除
		bool bLess;						//停用标记
		void(*lpfn)(void*);				//函数指针
		void* pBuffer;					//函数参数
	}CGTimerInfo,*PCGTimerInfo;

	/*
	经测试能达到的最小间隔为15毫秒左右
	1分钟也就1000毫秒，所以一分钟最多能调用66次回调函数
	*/

	class CGTimer
	{
	private:
		std::list<CGTimerInfo> m_cCGTimerList;		//使用中计时器容器
		std::list<CGTimerInfo> m_cCGTimerListLess;	//停用中计时器容器
		bool m_bExit;								//线程退出标记

		void StartThread();								//开始线程
		void StopThread();								//结束线程
		static void CheckCGTimerTick(CGTimer* pThis);	//线程函数
	public:
		CGTimer();
		virtual ~CGTimer();
	public:
		//纯虚计时器事件函数，发生任意计时器事件都会调用
		virtual bool OnCGTimerEvent(PCGTimerInfo pTimeInfo) = 0;

		bool AddCGTimer(int nID, int nInterval, int nCount,
			void(*lpfn)(void*), void* pBuffer);		//添加一个计时器
		bool DeleteCGTimer(int nID);				//删除一个计时器
		bool DeleteAllCGTimer();					//删除所有计时器
		bool EnableCGTimer(int nID, bool bEnable);	//禁用或者启用一个计时器
	};
}
