#include "iTimer.h"


namespace IAF
{
	namespace ITime
	{
		LARGE_INTEGER iTimer::freq = iTimer::GetFrequency();
		iTimer::iTimer()
		{
			QueryPerformanceCounter(&start);
		}
		iTimer::~iTimer()
		{
			;
		}
		LARGE_INTEGER iTimer::GetFrequency()
		{
			LARGE_INTEGER freq;
			/*此处没有进行错误检查*/
			QueryPerformanceFrequency(&freq);
			return freq;
		}
		void iTimer::restart()
		{
			QueryPerformanceCounter(&start);
		}
		double iTimer::elapse()
		{
			QueryPerformanceCounter(&end);
			return static_cast<double>(end.QuadPart - start.QuadPart) * 1000000 / freq.QuadPart;
		}
	}
}