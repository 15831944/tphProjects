#if !defined(ANIMATIONDATA_H_INCLUDED_)
#define ANIMATIONDATA_H_INCLUDED_

#include "Common\StartDate.h"

class AnimationData
{
public:
	AnimationData(){ bMoveDay = FALSE; };
	
	CStartDate m_startDate;
	long nAnimStartTime;			// in 1/100 seconds
	long nAnimEndTime;				// in 1/100 seconds
	long nFirstEntryTime;			// in 1/100 seconds
	long nLastExitTime;				// in 1/100 seconds
	long nLastAnimTime;				// in 1/100 seconds
	int nAnimSpeed;					// in 1/10 units -> 10 = 1x speed
	LARGE_INTEGER nLastSystemTime;	// in perf ticks
	LARGE_INTEGER nPerfTickFreq;	// in ticks/second
	BOOL bMoveDay;

	CString GetTimeTag() const
	{
		int nHour = nLastAnimTime / 360000;
		int nMin = (nLastAnimTime - nHour*360000)/6000;
		int nSec = (nLastAnimTime - nHour*360000 - nMin*6000)/100;

		bool bAbsDate;
		COleDateTime date;
		int nDtIdx;
		COleDateTime time;
		m_startDate.GetDateTime( ElapsedTime(nLastAnimTime / TimePrecision), bAbsDate, date, nDtIdx, time );

		CString s;
		if( bAbsDate )
		{
			s = date.Format(_T("%Y-%m-%d"));
		}	
		else
		{
			if(bMoveDay)
				nDtIdx--;
			s.Format(_T("Day %d"), nDtIdx + 1);
		}
		s += _T("  ") + time.Format(_T("%H:%M:%S"));

		return s;
	}

public:
	class AnimationModels 
	{
	protected:
		bool m_bTerminal;
		bool m_bAirside;
		bool m_bLandSide;
		bool m_bOnBoard;
	public:
		AnimationModels()
		{
			m_bTerminal = false;
			m_bAirside  = false;
			m_bLandSide = false;
			m_bOnBoard = false;
		}
	public:
		void Reset()
		{
			m_bTerminal = false;
			m_bAirside  = false;
			m_bLandSide = false;
			m_bOnBoard = false;
		}
		void SelTerminalModel()
		{
			m_bTerminal = true;
		}
		bool IsTerminalSel()
		{
			return m_bTerminal;
		}

		void SelAirsideModel()
		{
			m_bAirside = true;
		}
		bool IsAirsideSel()
		{
			return m_bAirside;
		}
		void SelLandsideModel()
		{
			m_bLandSide = true;
		}
		bool IsLandsideSel()
		{
			return m_bLandSide;
		}
		void SelOnBoardModel()
		{
			m_bOnBoard = true;
		}
		bool IsOnBoardSel()
		{
			return m_bOnBoard;
		}
	};
	AnimationModels m_AnimationModels;

};

//typedef struct _animdata {
//
//} AnimationData;


#endif // !defined(ANIMATIONDATA_H_INCLUDED_)