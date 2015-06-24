// IntervalStat.h: interface for the CIntervalStat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTERVALSTAT_H__F4325636_29F4_43FD_AE5C_817D92CBCD02__INCLUDED_)
#define AFX_INTERVALSTAT_H__F4325636_29F4_43FD_AE5C_817D92CBCD02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIntervalStat  
{
public:
	CIntervalStat();
	virtual ~CIntervalStat();

protected:

	int count;
	int minimum;
	int maximum;
	long total;

public:

	void clear (void) { total = count = maximum = 0; minimum =  32767; }
	void update (int value);

	int getCount (void) { return count; }
	int getMin (void) 
	{ 
		if( minimum ==  32767)
			return 0;
		return minimum; 
	}
	float getAvg (void) 
	{ 
		if(count==0)
			return 0;
		return (float) total / (float) count; 
	}
	int getMax (void) { return maximum; }
	long getTotal (void) { return total; }
};

#endif // !defined(AFX_INTERVALSTAT_H__F4325636_29F4_43FD_AE5C_817D92CBCD02__INCLUDED_)
