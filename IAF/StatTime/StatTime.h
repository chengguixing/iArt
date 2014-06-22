#ifndef _STATTIME_H_0A57F90C_659F_4551_8455_3349085AB3D5
#define _STATTIME_H_0A57F90C_659F_4551_8455_3349085AB3D5

/*
	author   : izhuxin
	function : statistical time
*/

#include <windows.h>
#include <assert.h>
#include <time.h>

class StatTime
{
public:
	StatTime();
	~StatTime();
	void Start();
	void Stop();
	void ReportServiceTime();

public:
	static StatTime& GetInstance()
	{
		static StatTime instance;
		return instance;
	}

private:
	clock_t  begin;
	clock_t  end;
	int elapsedSeconds;
};

#define StatTimeInstance StatTime::GetInstance()


#endif