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
		// ������ͣʱ��
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

		// ��¼��ͣʱ��
		mStopTime = currTime;
		mStopped = true;

	}
}

// ÿ֡����
void GameTimer::Tick()
{
	if (mStopped)
	{
		mDeltaTime = 0.0f;
		return;
	}

	// ��ѯ��ǰ�ǵڼ�֡
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;

	// ���ݾ�����֡�� �� ÿ֡��ʱ�� ���ʱ����
	mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;

	mPrevTime = mCurrTime;

	// �ڴ������ڽ���ʡ��ģʽ�����л���������ʱ��mDeltaTime����Ϊ��ֵ��DXSDK��
	if (mDeltaTime < 0.0)
	{
		mDeltaTime = 0.0;
	}
}