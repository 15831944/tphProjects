#ifndef ELAPSED_TIME_H
#define ELAPSED_TIME_H

class ElapsedTime
{
private:
    long    m_seconds;
    short   m_ms;
public:
    ElapsedTime(){ m_seconds = 0L; m_ms = 0; }
    ElapsedTime(long seconds, short ms=0){ m_seconds = seconds; m_ms = ms; }
    ElapsedTime(float precise){ m_seconds = (long)(precise); m_ms = (short)(precise-(float)m_seconds)*1000L; }
    ElapsedTime(const ElapsedTime& other){ m_seconds = other.m_seconds; m_ms = other.m_ms; }

    inline ElapsedTime& operator=(long seconds){ m_seconds = seconds; m_ms = 0L; return *this; }
    inline ElapsedTime& operator=(float seconds)
	{ 
		m_seconds = (long)(seconds); 
		m_ms = (short)(seconds-(float)m_seconds)*1000L;
		return *this;
	}
    inline ElapsedTime& operator=(const ElapsedTime& other){ m_seconds = other.m_seconds; m_ms = other.m_ms; return *this; }

    inline ElapsedTime& operator++(){ m_seconds++; return *this; }
    inline ElapsedTime operator+(float precise) const
    { 
        long seconds = m_seconds + (long)(precise);
        short ms = m_ms + (short)(precise-(float)m_seconds)*1000L;
		if(ms >= 1000)
		{
			seconds++;
			ms -= 1000;
		}
        return ElapsedTime(seconds, ms);
    }
    inline ElapsedTime operator-(float precise) const
    { 
        long seconds = m_seconds - (long)(precise);
        short ms = m_ms - (short)(precise-(float)m_seconds)*1000L;
        if(ms < 0)
        {
            seconds--;
            ms += 1000;
        }
        return ElapsedTime(seconds, ms);
    }
    inline ElapsedTime operator+(long seconds) const{ return ElapsedTime(m_seconds + seconds, m_ms); }
    inline ElapsedTime operator-(long seconds) const{ return ElapsedTime(m_seconds - seconds, m_ms); }
    inline float operator/(long seconds) const{ return (seconds)? (m_seconds / seconds): 0l; }

    inline void operator+=(long seconds){ m_seconds += seconds; }
    inline void operator+=(float precise)
    {
        long seconds = (long)precise;
        m_seconds += seconds;
        short ms = (short)((precise-(float)seconds) * 1000.0f);
        m_ms += ms;
        if(m_ms >= 1000)
        {
            m_seconds++;
            m_ms -= 1000;
        }
    }
    inline void operator-=(long seconds){ m_seconds -= seconds; }
    inline void operator-=(float precise)
    {
        long seconds = (long)precise;
        m_seconds -= seconds;
        short ms = (short)((precise-(float)seconds) * 1000.0f);
        m_ms -= ms;
        if(m_ms < 0)
        {
            m_seconds--;
            m_ms += 1000;
        }
    }
    inline void operator*=(long count)
    { 
        m_seconds *= count; 
        m_ms *= (short)count; 
        if(m_ms >= 1000)
        {
            m_seconds+=(m_ms/1000);
            m_ms = m_ms%1000;
        }
    }
    inline void operator*=(float count)
    { 
        float secResult = ((float)m_seconds)*count;
        m_seconds = (long)secResult;
        float secDiff = secResult - (float)m_seconds;
        float msResult = ((float)m_ms)*count;
        m_ms = (short)(msResult + secDiff*1000.0f);
        if(m_ms >= 1000)
        {
            m_seconds+=(m_ms/1000);
            m_ms = m_ms%1000;
        }
    }
    inline void operator/=(long i)
	{ m_seconds = (i)? (m_seconds / i): 0l; }
    
    inline ElapsedTime operator+(const ElapsedTime& other) const
    { 
        short ms = m_ms + other.m_ms;
        long seconds = other.m_seconds;
        if(ms >= 1000)
        {
            seconds++;
            ms -= 1000;
        }
        return ElapsedTime(m_seconds + seconds, ms);
    }
    inline ElapsedTime operator-(const ElapsedTime& other) const
    { 
        short ms = m_ms - other.m_ms;
        long seconds = other.m_seconds;
        if(ms < 0)
        {
            seconds--;
            ms += 1000;
        }
        return ElapsedTime(m_seconds + seconds, ms);
    }
    inline void operator+=(const ElapsedTime& other) 
    {
        m_seconds += other.m_seconds;
        m_ms += other.m_ms;
        if(m_ms >= 1000)
        {
            m_seconds++;
            m_ms -= 1000;
        }
    }
    inline void operator-=(const ElapsedTime& other)
    {
        m_seconds -= other.m_seconds;
        m_ms -= other.m_ms;
        if(m_ms < 0)
        {
            m_seconds--;
            m_ms += 1000;
        }
    }
  
    inline bool operator==(const ElapsedTime& other) const{ return m_seconds==other.m_seconds && m_ms==other.m_ms; }
    inline bool operator!=(const ElapsedTime& other) const{ return m_seconds!=other.m_seconds || m_ms!=other.m_ms; }
    inline bool operator<(const ElapsedTime& other) const
    {
        if(m_seconds == other.m_seconds)
            return m_ms < other.m_ms;
        else
            return m_seconds < other.m_seconds;
    }    
    inline bool operator>(const ElapsedTime& other) const
    {
        if(m_seconds == other.m_seconds)
            return m_ms > other.m_ms;
        else
            return m_seconds > other.m_seconds;
    } 
    inline bool operator>=(const ElapsedTime& other) const
    {
        if(m_seconds > other.m_seconds)
            return true;
        else if(m_seconds == other.m_seconds)
            return m_ms >= other.m_ms;
        else 
            return false;
    }
    inline bool operator<=(const ElapsedTime& other) const
    {
        if(m_seconds < other.m_seconds)
            return true;
        else if(m_seconds == other.m_seconds)
            return m_ms <= other.m_ms;
        else 
            return false;
    }

    inline bool operator==(long seconds) const{ return m_seconds == seconds; }
    inline bool operator<(long seconds) const{ return m_seconds < seconds; }
    inline bool operator>(long seconds) const
    {
        if(m_seconds > seconds)
            return true;
        if(m_seconds == seconds)
            return m_ms > 0;
    }

    inline bool operator>=(long seconds) const{ return (m_seconds >= seconds); }
    inline bool operator<=(long seconds) const
    { 
        if(m_seconds < seconds)
            return true;
        else if(m_seconds == seconds)
            return m_ms == 0;
        else
            return false;
    }
    
    inline short GetMilliSecond() const{ return m_ms; }
    inline void SetMilliSecond(short ms){ m_ms = ms; }
    inline long GetAsSecond() const{ return m_seconds; }
    inline void SetAsSecond(long seconds){ m_seconds = seconds; }
    inline long GetAsMinute() const{ return m_seconds/60L; }
    inline void SetAsMinute(long minutes){ m_seconds = minutes*60L; }
    inline long GetAsHour() const{ return m_seconds/3600L; }
    inline void SetAsHour(long hours){ m_seconds = hours*3600L; }
    inline long GetAsDay() const{ return m_seconds/86400L; }
    inline void SetAsDay(long days){ m_seconds = days*86400L; }
    inline void SetByHourMinuteSecond(long hour, long minute, long second){ m_seconds = (hour*3600L + minute*60L + second); }
    bool SetByString (const char *str);
    void PrintTime(char *str, bool printSeconds = true) const;
};

#endif
