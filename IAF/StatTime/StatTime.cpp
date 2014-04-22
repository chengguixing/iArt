#include "StatTime.h"

StatTime::StatTime()
{
	NULL;
}
StatTime::~StatTime()
{
	NULL;
}
void StatTime::Start()
{
	begin = clock();
}
void StatTime::Stop()
{
	end = clock();

	ReportServiceTime();
}
void StatTime::ReportServiceTime()
{
	elapsedMinutes = static_cast<int>(static_cast<double>(end - begin) / CLOCKS_PER_SEC) / SECONDSINMINUTE;
}