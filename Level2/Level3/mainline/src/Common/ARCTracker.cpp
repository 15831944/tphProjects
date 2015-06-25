#include "stdafx.h"
#include "ARCTracker.h"




#ifdef _USE_PLACE_TRACK_STRING_


const char*		ARCStringTracker::m_whoami = "ARCStringTracker";
const char*		ARCStringTracker::m_lastStringTrackerName = NULL;

#	ifdef _USE_CLASS_METHOD_TRACK_INCLUDE_STACK_   // if we also want track in the stack
ARCStringTracker*		 ARCStringTracker::m_lastStringTracker = NULL;
std::vector<ARCStringTracker::TrackRecord> ARCStringTracker::m_recs;
#	endif // _USE_CLASS_METHOD_TRACK_INCLUDE_STACK_ 

#include <time.h>
#include <string>

static std::string convHex(unsigned long v)
{
	const char digits[] = "0123456789abcdef";
	char result[12]; // "0x12345678"
	result[0] = '0';
	result[1] = 'x';
	char* pLast = result + 10;
	*pLast = '\x00';
	for(int i=0;i<8;i++)
	{
		*(--pLast) = digits[v % 16];
		v /= 16;
	}
	return result;
}

static bool checkReadable(const void* p, int nSize)
{
	return TRUE == ::IsBadReadPtr(p, nSize);
}

static std::string readStringCarefully(const char* p)
{
	const char* pEnd = p;
	while (1)
	{
		if (::IsBadReadPtr(pEnd, 1))
			throw std::string(p, pEnd);

		if ('\x00' == *pEnd)
			break;

		pEnd++;
	}

	return std::string(p);
}


void ARCStringTracker::BuildTrackerImage()
{
#	ifdef _USE_CLASS_METHOD_TRACK_INCLUDE_STACK_
	m_recs.clear();
	TrackRecord rec;
	ARCStringTracker* pTracker = m_lastStringTracker;
	while (pTracker)
	{
		if (checkReadable(pTracker, sizeof(ARCStringTracker)))
		{
			rec.pTracker = pTracker;
			rec.pString = "Invalid Tracker(N/A)";
			m_recs.push_back(rec);
			break;
		}
		rec.pTracker = pTracker;
		rec.pString = pTracker->m_pTrackString;
		m_recs.push_back(rec);
		pTracker = pTracker->m_preStringTracker;
	}
#	endif // _USE_CLASS_METHOD_TRACK_INCLUDE_STACK_
}


void ARCStringTracker::ClearTrackerImage()
{
#	ifdef _USE_CLASS_METHOD_TRACK_INCLUDE_STACK_
	m_recs.clear();
#	endif // _USE_CLASS_METHOD_TRACK_INCLUDE_STACK_
}

bool ARCStringTracker::DumpTracker(std::ostream& os)
{
	bool bNoError = true;

	os.setf(std::ios::hex);

	os << "Dumping ARCStringTracker tracker..." << std::endl;

	//------------------------------------------------------------
	// 
	try
	{
		os << "\t" << "The tracker is (" << convHex((unsigned long)&m_whoami) << " -> " << convHex((unsigned long)m_whoami) << ")\"" << readStringCarefully(m_whoami) << "\"." << std::endl;
	}
	catch (std::string& e)
	{
		os << std::endl << "Dumping failed(" << e << ")." << std::endl;
		bNoError = false;
	}
	//------------------------------------------------------------


	//------------------------------------------------------------
	// 
	try
	{
		os << "\t" << "The last track string is (" << convHex((unsigned long)m_lastStringTrackerName) << ")\"" << readStringCarefully(m_lastStringTrackerName) << "\"." << std::endl;
	}
	catch (std::string& e)
	{
		os << std::endl << "Dumping failed(" << e << ")." << std::endl;
		bNoError = false;
	}
	//------------------------------------------------------------


	//------------------------------------------------------------
	// 
#	ifdef _USE_CLASS_METHOD_TRACK_INCLUDE_STACK_
	os << "Dumping stack tracker..." << std::endl;
	try
	{
		ARCStringTracker* pTracker = m_lastStringTracker;
		while (pTracker)
		{
			if (checkReadable(pTracker, sizeof(ARCStringTracker)))
				throw std::string("ARCStringTracker corrupted");

			os << "\t(" << convHex((unsigned long)pTracker) << " -> " << convHex((unsigned long)pTracker->m_pTrackString) << ")" << readStringCarefully(pTracker->m_pTrackString) << std::endl;
			pTracker = pTracker->m_preStringTracker;
		}
	}
	catch (std::string& e)
	{
		os << std::endl << "Dumping failed(" << e << ")." << std::endl;
		bNoError = false;
	}
	os << "Dumping stack tracker finished." << std::endl;

	os << "Dumping stack tracker stored image..." << std::endl;
	std::vector<TrackRecord>::iterator ite = m_recs.begin();
	std::vector<TrackRecord>::iterator iteEn = m_recs.end();
	for (;ite!=iteEn;ite++)
	{
		const TrackRecord& rec = *ite;
		os << "\t(" << convHex((unsigned long)rec.pTracker) << " -> " << convHex((unsigned long)rec.pString) << ")" << readStringCarefully(rec.pString) << std::endl;
	}
	os << "Dumping stack tracker stored image finished." << std::endl;

#	endif // _USE_CLASS_METHOD_TRACK_INCLUDE_STACK_
	//------------------------------------------------------------

	os << std::endl << "All dumps finished." << std::endl << std::endl;

	__time64_t now;
	_time64( &now );
	os << "UTC time:   " << asctime( _gmtime64( &now ) )    << std::endl;
	os << "Local time: " << asctime( _localtime64( &now ) ) << std::endl;
	return bNoError;
}

#include <fstream>

bool ARCStringTracker::DumpTrackerToFile( const char* pFileName )
{
	std::ofstream os;
	os.open(pFileName);
	if (os.is_open())
		 return ARCStringTracker::DumpTracker(os);
	return false;
}

#endif // _USE_PLACE_TRACK_STRING_

