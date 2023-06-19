#include "SoftTimer.h"

size_t SoftTimer::totalObjectNum = 0;

void SoftTimer::setInterval(float timeInterval)
{
	this->timeInterval = timeInterval;
}

bool SoftTimer::isStarted()
{
	return this->started;
}

bool SoftTimer::isTriggered()
{
	return this->triggered;
}

std::chrono::time_point<std::chrono::steady_clock>  SoftTimer::getBornTime()
{
	return this->bornTime;
}

double SoftTimer::getElapsedTime()
{
	return this->lastCallTimeDiff.count();
}

double SoftTimer::getRemainingTime()
{
	return this->remainingTime.count();
}

void SoftTimer::startTimer()
{
	startTime = std::chrono::steady_clock::now();
	startTime -= std::chrono::duration_cast<std::chrono::seconds>(lastCallTimeDiff);
	started = true;
}

void SoftTimer::stopTimer()
{
	started = false;
}

void SoftTimer::resetTimer()
{
	startTime = std::chrono::steady_clock::now();
	triggered = false;
}

void SoftTimer::timerProcess()
{
	if (!started) return;

	lastCallTime = std::chrono::steady_clock::now();
	lastCallTimeDiff = lastCallTime - startTime;

	remainingTime = std::chrono::duration<float>(timeInterval) - lastCallTimeDiff;
	if (remainingTime.count() <= floatZeroThresholdTime) remainingTime = std::chrono::duration<double>::zero();

	if (lastCallTimeDiff.count() >= timeInterval)
	{	
		triggered = true;
		if (this->timerCb != NULL) this->timerCb(this);
		startTime = std::chrono::steady_clock::now();
	}
}

void SoftTimer::registerTimElapsedCallback(TimCB timerCallback)
{
	this->timerCb = timerCallback;
}

int SoftTimer::getTimerNo()
{
	return this->timerNo;
}

int SoftTimer::getUserTimerNo()
{
	return this->userTimerNo;
}


std::string SoftTimer::getTimerName()
{
	return this->timerName;
}


int multiTimer::addTimer(float timeInterval, int userTimerNo, TimCB timerCallback, bool start)
{	
	// TimersNoLookUp.insert( { userTimerNo, Timers.size() } );
	Timers.insert({userTimerNo, SoftTimer(timeInterval, userTimerNo, timerCallback, start)} );

	return Timers.rbegin()->second.getTimerNo();
}

SoftTimer& multiTimer::getTimer(int timerUserNo)
{
	// int timerVectorElement =  TimersNoLookUp[timerUserNo];
    // insert pair {timerUserNo, 0} if searching element (key) does not exist -- so better use this below

	auto search = Timers.find(timerUserNo);
	if (search != Timers.end())
	{
		return search->second;
	}
	else
	{
		// if timer does not exist, return default timer which always exist -- see constructor
		return Timers.find(defaultTimerNo)->second;
	}
}

void multiTimer::TIMERS_EVENT()
{
	for (auto& _timers : this->Timers)
	{
		_timers.second.timerProcess();
	}
}

