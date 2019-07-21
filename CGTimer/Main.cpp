
#include "CGTimer.h"
using namespace cgt;

class MyClass : public CGTimer
{
public:
	MyClass()
	{

	}
	~MyClass()
	{

	}
	int nIndex = 100;
	virtual bool OnCGTimerEvent(PCGTimerInfo pTimeInfo)
	{
		nIndex++;
		printf("ID:%d  LastTime:%lld  Count:%d \n",
			pTimeInfo->nID, pTimeInfo->ullLastTime,pTimeInfo->nCount);
		return 1;
	}

};

void MyPrintf(void* pBuffer)
{
	static int nIndex = 100;
	printf("Current is %d - %s\n", nIndex++,(char*)pBuffer);
}

int main(int argc, char* argv[])
{
	MyClass Test;
	Test.AddCGTimer(1, 1, -1, 0, 0);
	//Test.AddCGTimer(2, 60, -1, 0, 0);
	getchar();
	Test.DeleteCGTimer(1);
	getchar();
	return 0;
}