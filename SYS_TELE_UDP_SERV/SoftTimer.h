#pragma once

#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <map>

class SoftTimer {
public:
	typedef void (*TimCB)(SoftTimer* SoftTimer);

	static constexpr double floatZeroThresholdTime = 1e-6;

	SoftTimer() {}

	SoftTimer(float timeInterval, int userNo, TimCB timerCallback = NULL, bool start = false, std::string timerName = "") :
		timeInterval(timeInterval),
		started(start),
		timerCb(timerCallback),
		userTimerNo(userNo)
	{
		this->bornTime			= std::chrono::steady_clock::now();
		this->lastCallTimeDiff  = std::chrono::duration<float>(0.0);
		this->startTime			= std::chrono::steady_clock::now();
		this->lastCallTime	    = std::chrono::steady_clock::now();
		this->lastCallTimeDiff	= std::chrono::duration<double>(0.0);

		this->timerNo = SoftTimer::totalObjectNum++;
		
		timerName.empty() ? (this->timerName = "Timer" + std::to_string(timerNo)) : this->timerName = timerName;

		this->userData = NULL;
	}

	SoftTimer(const SoftTimer& obj)
	{
		*this = obj;
		SoftTimer::totalObjectNum++;
	}

	~SoftTimer()
	{
		--(this->totalObjectNum);
	}

	void setInterval(float timeInterval);

	void startTimer();
	void stopTimer();
	void resetTimer();

	void timerProcess();

	void registerTimElapsedCallback(TimCB timerCallback);

	bool												isStarted();
	bool												isTriggered();
	int													getTimerNo();
	int													getUserTimerNo();
	double												getElapsedTime();
	double												getRemainingTime();
	std::string											getTimerName();
	std::chrono::time_point<std::chrono::steady_clock>  getBornTime();

	void* userData;

protected:
	std::chrono::time_point<std::chrono::steady_clock>  bornTime;
	std::chrono::time_point<std::chrono::steady_clock>  startTime;
	std::chrono::time_point<std::chrono::steady_clock>  lastCallTime;
	std::chrono::duration<double>						lastCallTimeDiff;
	std::chrono::duration<double>						remainingTime;
	
	bool started;
	bool triggered;
	float timeInterval;

	/* callback of timer */
	TimCB		  timerCb;

	/* names of Timer - string + int */
	std::string   timerName;
	int			  timerNo;
	int			  userTimerNo;
	static size_t totalObjectNum;
};



/* using new in push_back at vector */

/*
std::vector<std::unique_ptr<Pictureframe>> vec;
vec.push_back(std::unique_ptr<Pictureframe>(new Pictureframe(...)));
// do stuff
vec.clear();
*/


class multiTimer {
public:
	typedef void (*TimCB)(SoftTimer* SoftTimer);

	static constexpr int    defaultTimerNo	      = (-100);
	static constexpr float  defaultTimerInterval  =   2.0;

	multiTimer()
	{
		this->addTimer(defaultTimerInterval, defaultTimerNo, NULL, false);
	}

	void TIMERS_EVENT();

	int   addTimer(float timeInterval, int userTimerNo, TimCB timerCallback = NULL, bool start = false);
	// void  deleteTimer(int userTimerNo, int TimerNo = -1);

	SoftTimer& getTimer(int timerNo);

protected:
	std::map<int, SoftTimer> Timers;
};