#ifndef __LINE_PARKING_PROCESSOR_DEF
#define __LINE_PARKING_PROCESSOR_DEF


#include "./LandfieldProcessor.h"


class StretchProc;
class LineParkingProc : 
	public LandfieldProcessor
{
public :
	LineParkingProc();
	~LineParkingProc();
	//processor type
	virtual int getProcessorType (void) const { return LineParkingProcessor; }
	//processor name
	virtual const char *getProcessorName (void) const { return m_strTypeName; }
	//processor i/o	

	virtual LandfieldProcessor * GetCopy();

	virtual bool HasProperty(ProcessorProperty propType)const;

	void GetParkingSpot(std::vector<ParkingSpot>& spots);

	void GetParkingPath(ARCPath3& closeStretchSide, ARCPath3& farToStretchSide)const;

	StretchProc * GetLinkProc();

public:
	const static CString m_strTypeName; // "LineParking"	

};











#endif