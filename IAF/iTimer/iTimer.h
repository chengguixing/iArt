#ifndef _ITIMER_H_7A96CDFB_C9C1_4131_8284_F590CCA7129E
#define _ITIMER_H_7A96CDFB_C9C1_4131_8284_F590CCA7129E


#include <windows.h>

namespace IAF
{
	namespace ITime
	{
		class iTimer
		{
		public:
			iTimer();
			~iTimer();
			void restart();
			double elapse();

		private:
			static LARGE_INTEGER freq;
		private:
			static LARGE_INTEGER GetFrequency();
		private:
			LARGE_INTEGER start;
			LARGE_INTEGER end;
			
		};
	}
}
#endif