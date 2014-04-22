#include <windows.h>
#include <assert.h>
#include <iostream>
using namespace std;

BOOL iQueryPerformanceCounter()
{
	cout << "enter iQueryPerformanceCounter" << endl;
	return TRUE;
}


class TimeStrategy 
{

	unsigned long _secs;
	unsigned long _millis;

public:

	TimeStrategy() 
	{

		// Keep track of the relative time the program started
		static LARGE_INTEGER i;
		static BOOL valid(::QueryPerformanceCounter(&i));


		assert(valid == TRUE);

		LARGE_INTEGER j;
		::QueryPerformanceCounter(&j);

		j.LowPart -= i.LowPart;
		j.LowPart /= frequency();


		// Mask off the high bits        
		_millis = (unsigned long)j.LowPart / 1000;
		_secs   = (unsigned long)j.LowPart - _millis;


	}

	unsigned long seconds() const 
	{
		return _secs;
	}

	unsigned long milliseconds() const 
	{  
		return _millis;    
	}

	unsigned long seconds(unsigned long s) 
	{

		unsigned long z = seconds();

		_secs = s;
		return z;

	}

	unsigned long milliseconds(unsigned long ms) 
	{

		unsigned long z = milliseconds();

		_millis = ms;
		return z;

	}

private:

	// Get the frequency
	static DWORD frequency() 
	{

		static LARGE_INTEGER i;
		static BOOL valid(::QueryPerformanceFrequency(&i));

		assert(valid == TRUE);
		return i.LowPart;

	}

};

class Time 
{

	unsigned long _seconds;
	unsigned long _milliseconds;

	//! Create a new Time object
	Time(unsigned long secs, unsigned long millis)
		: _seconds(secs), _milliseconds(millis) { }

	/**
	* Set the number of milliseconds in this Time object.
	*
	* @param millis - new milliseconds value
	* @return unsigned long old milliseconds value
	*/
	unsigned long milliseconds(unsigned long millis) 
	{

		unsigned long n = _milliseconds;
		_milliseconds = millis;

		return n;

	}

	/**
	* Set the number of seconds in this Time object.
	*
	* @param secs - new seconds value
	* @return unsigned long old seconds value
	*/
	unsigned long seconds(unsigned long secs) 
	{

		unsigned long n = _seconds;
		_seconds = secs;

		return n;

	}

public:


	/**
	* Create a Time object with the current time relative to the
	* beginning of the program.
	*/
	Time()
	{
		// System startup time
		static TimeStrategy firstHelper;
		TimeStrategy helper;

		Time then(firstHelper.seconds(), firstHelper.milliseconds());

		Time now(helper.seconds(), helper.milliseconds());

		now -= then;


		_seconds = now.seconds(); 
		_milliseconds = now.milliseconds();
	}

	/**
	* Create a Time object by copying another.
	*
	* @param t - Time object to copy.
	*/
	Time(const Time& t)
		: _seconds(t._seconds), _milliseconds(t._milliseconds) { }


	/**
	* Get the number of milliseconds in this Time object.
	*
	* @return unsigned long milliseconds value
	*/
	unsigned long milliseconds() const 
	{
		return _milliseconds;
	}

	/**
	* Get the number of seconds in this Time object.
	*
	* @return unsigned long seconds value
	*/
	unsigned long seconds() const {
		return _seconds;
	}

	/**
	* Add some number of milliseconds to this Time object.
	*
	* @param millis - number of milliseconds to add to this Time object
	* @return const Time& this object
	*/
	const Time& operator+=(unsigned long millis) 
	{
		_milliseconds += millis;
		_seconds += (_milliseconds / 1000);
		_milliseconds %= 1000;

		return *this;

	}

	/**
	* Subtract some number of milliseconds to this Time object.
	*
	* @param millis - number of milliseconds to subtract from this Time object
	* @return const Time& this object
	*/
	const Time& operator-=(unsigned long millis) 
	{

		if(_milliseconds > millis)
			_milliseconds -= millis;

		else {

			while(_seconds > 0 && _milliseconds < millis) {

				_milliseconds += 1000;
				_seconds -= 1;

			}

			_milliseconds = (_milliseconds < millis) ? 0 : (_milliseconds - millis);

		}

		return *this;

	}


	/**
	* Add the value of another Time object to this one.
	*
	* @param t - Time object whose value should be added to this object
	* @return const Time& this object
	*/
	const Time& operator+=(const Time& t) 
	{

		_milliseconds += t.milliseconds();
		_seconds += (_milliseconds / 1000) + t.seconds();
		_milliseconds %= 1000;

		return *this;

	}

	/**
	* Subtract the value of another Time object from this one.
	* This function has a floor of 0.
	*
	* @param t - Time object whose value should be subtracted from this object
	* @return const Time& this object
	*/
	const Time& operator-=(const Time& t)
	{

		unsigned long millis = t.milliseconds();
		unsigned long secs = t.seconds();

		if(_seconds >= secs) {

			if(_milliseconds > millis) {
				_milliseconds -= millis;
				_seconds -= secs;

			} else {

				while(_seconds > 0 && _milliseconds < millis) {

					_milliseconds += 1000;
					_seconds -= 1;

				}

				_milliseconds = (_milliseconds < millis) ? 0 : (_milliseconds - millis);
				_seconds = (_seconds < secs) ? 0 : (_seconds - secs);

			}

		} else {

			_milliseconds = 0;
			_seconds = 0;

		}

		return *this;

	}
};