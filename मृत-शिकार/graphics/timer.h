#pragma once
/*
TIMER.H
-----------------------------
// This is adapted from StepTimer class which comes with MS Visual Studio 2013's Direct3D Store App sample
*/

#include <Windows.h>

// A high-resolution timer class, for WINDOWS ONLY
class Timer
{
public:
    Timer() :
        m_elapsedTicks(0),
        m_totalTicks(0),
        m_leftOverTicks(0),
        m_frameCount(0),
        m_framesPerSecond(0),
        m_framesThisSecond(0),
        m_secondCounter(0),
        m_targetElapsedTicks(TicksPerSecond / 60)
    {
        if (!QueryPerformanceFrequency(&m_frequency))
            MessageBox(0, L"Failed to initialize timer", L"ERROR", 0);

        if (!QueryPerformanceCounter(&m_lastTime))
			MessageBox(0, L"Failed to initialize timer", L"ERROR", 0);

        m_maxDelta = m_frequency.QuadPart / 10;
    }

    UINT32 GetFrameCount() const						{ return m_frameCount; }
    UINT32 GetFramesPerSecond() const					{ return m_framesPerSecond; }
	UINT64 GetElapsedTicks() const						{ return m_elapsedTicks; }
	double GetElapsedSeconds() const					{ return TicksToSeconds(m_elapsedTicks); }
	UINT64 GetTotalTicks() const						{ return m_totalTicks; }
	double GetTotalSeconds() const						{ return TicksToSeconds(m_totalTicks); }

    void SetTargetDeltaTicks(UINT64 targetElapsed)	{ m_targetElapsedTicks = targetElapsed; }
    void SetTargetDeltaSeconds(double targetElapsed)	{ m_targetElapsedTicks = SecondsToTicks(targetElapsed); }

	static const UINT64 TicksPerSecond = 10000000;

	static double TicksToSeconds(UINT64 ticks)		{ return static_cast<double>(ticks) / TicksPerSecond; }
	static UINT64 SecondsToTicks(double seconds)		{ return static_cast<UINT64>(seconds * TicksPerSecond); }

    // After an intentional timing discontinuity (for modelAnimation a blocking IO operation)
    // call this to avoid having the fixed timestep logic attempt a set of catch-up
    // Update calls
    void Reset()
    {
        if (!QueryPerformanceCounter(&m_lastTime))
			MessageBox(0, L"Timer Failed", L"ERROR", 0);

        m_leftOverTicks = 0;
        m_framesPerSecond = 0;
        m_framesThisSecond = 0;
        m_secondCounter = 0;
		m_totalTicks = 0;
    }

	void Tick(void(*update)(double, double))
    {
        LARGE_INTEGER currentTime;
        if (!QueryPerformanceCounter(&currentTime))
			MessageBox(0, L"Timer Failed", L"ERROR", 0);

		UINT64 timeDelta = currentTime.QuadPart - m_lastTime.QuadPart;
        m_lastTime = currentTime;
        m_secondCounter += timeDelta;

        if (timeDelta > m_maxDelta)
            timeDelta = m_maxDelta;

        timeDelta *= TicksPerSecond;
        timeDelta /= m_frequency.QuadPart;

        UINT32 lastFrameCount = m_frameCount;

		if (abs(static_cast<int>(timeDelta - m_targetElapsedTicks)) < static_cast<int>(TicksPerSecond / 4000))
            timeDelta = m_targetElapsedTicks;

        m_leftOverTicks += timeDelta;

        while (m_leftOverTicks >= m_targetElapsedTicks)
        {
            m_elapsedTicks = m_targetElapsedTicks;
            m_totalTicks += m_targetElapsedTicks;
            m_leftOverTicks -= m_targetElapsedTicks;
            m_frameCount++;

			update(TicksToSeconds(m_totalTicks), TicksToSeconds(m_elapsedTicks));
        }

        if (m_frameCount != lastFrameCount)
            m_framesThisSecond++;

		if (m_secondCounter >= static_cast<UINT64>(m_frequency.QuadPart))
        {
            m_framesPerSecond = m_framesThisSecond;
            m_framesThisSecond = 0;
            m_secondCounter %= m_frequency.QuadPart;
        }
    }

private:
    LARGE_INTEGER m_frequency;
    LARGE_INTEGER m_lastTime;
	UINT64 m_maxDelta;

	UINT64 m_elapsedTicks;
	UINT64 m_totalTicks;
	UINT64 m_leftOverTicks;

	UINT32 m_frameCount;
	UINT32 m_framesPerSecond;
    UINT32 m_framesThisSecond;
	UINT64 m_secondCounter;

	UINT64 m_targetElapsedTicks;
};
