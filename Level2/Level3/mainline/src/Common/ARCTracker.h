#pragma once

//////////////////////////////////////////////////////////////////////////
// This file is to use to place some track strings to the pro gramme

// 1. _USE_PLACE_TRACK_STRING_
//    when switch this macro on,
//    following sentence will place a track string _T("This is the line.")
//    in the disassembly code
//    PLACE_TRACK_STRING(_T("This is the line."));
//
//    NOTE: Default to on.


// 2. _USE_CLASS_METHOD_TRACK_INCLUDE_STACK_
//    when switch this macro on,
//    Following sentence will place a track string
//    in the disassembly code and process stack
//    PLACE_METHOD_TRACK_STRING();
//
//
//    NOTE: Default to on.


#include <windows.h>

//------------------------------------------------------------
// 
#define _USE_PLACE_TRACK_STRING_
//------------------------------------------------------------


//------------------------------------------------------------
// 
#define _USE_CLASS_METHOD_TRACK_INCLUDE_STACK_
//------------------------------------------------------------


#ifdef _USE_PLACE_TRACK_STRING_


#	include <ostream>
#	include <iosfwd>

	class ARCStringTracker
	{
	public:

#	ifdef _USE_CLASS_METHOD_TRACK_INCLUDE_STACK_
		inline ARCStringTracker(const char* pTrackString)
		{
			m_pTrackString = pTrackString;
			m_preStringTracker = m_lastStringTracker;
			m_lastStringTracker = this;
		}
		inline ~ARCStringTracker()
		{
			m_lastStringTracker = m_preStringTracker;
		}
#	endif // _USE_CLASS_METHOD_TRACK_INCLUDE_STACK_ 

		inline static const char* GetStringTrackerName() { return m_lastStringTrackerName; }
		inline static void SetStringTrackerName(const char* pTrackString) { m_lastStringTrackerName = pTrackString; }
		static bool DumpTracker(std::ostream& os);
		static bool DumpTrackerToFile(const char* pFileName);
		static void BuildTrackerImage();
		static void ClearTrackerImage();
	private:
		static const char*		m_whoami;
		static const char*		m_lastStringTrackerName;

#	ifdef _USE_CLASS_METHOD_TRACK_INCLUDE_STACK_
		const char* m_pTrackString;
		ARCStringTracker* m_preStringTracker;
		static ARCStringTracker* m_lastStringTracker;
		struct TrackRecord 
		{
			ARCStringTracker* pTracker;
			const char* pString;

			TrackRecord() : pTracker(NULL), pString(NULL) {}
		};
		static std::vector<TrackRecord> m_recs;
#	endif // _USE_CLASS_METHOD_TRACK_INCLUDE_STACK_

	};
#	define PLACE_TRACK_STRING(TrackString)   ARCStringTracker::SetStringTrackerName(TrackString);
#	ifdef _USE_CLASS_METHOD_TRACK_INCLUDE_STACK_
#		define PLACE_TRACK_STRING2(ClassMethodName)   ARCStringTracker loc_ARCStringTracker(ClassMethodName)
#	endif

#	define DUMP_TRACK_TO_STREAM(os)				ARCStringTracker::DumpTracker(os)
#	define DUMP_TRACK_TO_FILE(charFileName)		ARCStringTracker::DumpTrackerToFile(charFileName)
#else
#	define PLACE_TRACK_STRING(TrackString)
#	define DUMP_TRACK_TO_STREAM(os)             true
#	define DUMP_TRACK_TO_FILE(charFileName)     true
#endif // _USE_PLACE_TRACK_STRING_


#ifndef PLACE_TRACK_STRING2
#	define PLACE_TRACK_STRING2(ClassMethodName)
#endif

#define PLACE_METHOD_TRACK_STRING()        PLACE_TRACK_STRING2(__FUNCTION__)

