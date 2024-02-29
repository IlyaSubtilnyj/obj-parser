#pragma once

#include <chrono>
#include <ctime>
#include <iostream>
#include <sys/time.h>

class Interval
{
private:
    std::int64_t start;
public:
    // Ctor
    inline Interval()
    {
        std::timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        start = (std::int64_t(ts.tv_sec) * 1000) + (ts.tv_nsec / 1000000);
    }

    // Dtor
    virtual ~Interval()
    {
    }

    inline auto value() const
    {
        std::timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        std::int64_t end = (std::int64_t(ts.tv_sec) * 1000) + (ts.tv_nsec / 1000000);
        return end - start;
    }
};

class Fps
{
protected:
    unsigned int m_fps;
    unsigned int m_fpscount;
    Interval m_fpsinterval;

public:
    // Constructor
    Fps() : m_fps(0), m_fpscount(0)
    {
    }

    // Update
    void update()
    {
        // increase the counter by one
        m_fpscount++;

        // one second elapsed? (= 1000 milliseconds)
        if (m_fpsinterval.value() > 1000)
        {
            // save the current counter value to m_fps
            m_fps         = m_fpscount;
           
            // reset the counter and the interval
            m_fpscount    = 0;
            m_fpsinterval = Interval();
        }
    }

    // Get fps
    unsigned int get() const
    {
        return m_fps;
    }
};