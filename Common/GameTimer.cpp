#include <windows.h>
#include "GameTimer.h"

GameTimer::GameTimer(): 
	mSecondsPerCount(0.0), 
	mDeltaTime(-1.0), 
	mBaseTime(0), 
	mPausedTime(0), 
	mPrevTime(0), 
	mCurrTime(0), 
	mStopped(false)
{
	__int64 countsPreSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPreSec);
	mSecondsPerCount = 1.0 / (double)countsPreSec;
}

float GameTimer::TotalTime() const
{
	if (mStopped)
	{
		return (float)(((mStopTime - mPausedTime) - mBaseTime)*mSecondsPerCount);
	}
	else
	{
		return (float)(((mCurrTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
	}
}

float GameTimer::DeltaTime() const
{
	return (float)mDeltaTime;
}

void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mStopped = false;
}

void GameTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if (mStopped)
	{
		// 增加暂停时间
		mPausedTime += (startTime - mStopTime);

		mPrevTime = startTime;
		mStopTime = 0;
		mStopped = false;
	}

}

void GameTimer::Stop()
{
	if (!mStopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		// 记录暂停时间
		mStopTime = currTime;
		mStopped = true;

	}
}

// 每帧调用
void GameTimer::Tick()
{
	if (mStopped)
	{
		mDeltaTime = 0.0f;
		return;
	}

	// 查询当前是第几帧
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;

	// 根据经过的帧数 和 每帧的时间 算出时间间隔
	mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;

	mPrevTime = mCurrTime;

	// 在处理器在进入省电模式、或切换处理器的时候，mDeltaTime可能为负值（DXSDK）
	if (mDeltaTime < 0.0)
	{
		mDeltaTime = 0.0;
	}
}