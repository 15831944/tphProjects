// DeiceBayProc.h: interface for the DeiceBayProc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEICEBAYPROC_H__A323EC84_4D74_4FA9_B1F2_BADB06AE81C9__INCLUDED_)
#define AFX_DEICEBAYPROC_H__A323EC84_4D74_4FA9_B1F2_BADB06AE81C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../Main/Processor2.h"
#include "AirfieldProcessor.h"
#include "../Common/ARCPolygon.h"


class DeiceBayProc : public AirfieldProcessor  
{
public:
	DeiceBayProc();
	virtual ~DeiceBayProc();
	// processor type
	virtual int getProcessorType (void) const { return DeiceBayProcessor; }
	// processor name
	virtual const char *getProcessorName (void) const { return "DEICE_STATION"; }
	// processor i/o
	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile) const;

	//number
	void SetNumber(int _nNumber) { m_nNumber = _nNumber; }
	int GetNumber() const { return m_nNumber; }
	virtual void initServiceLocation (int pathCount, const Point *pointList){
		m_location.init (pathCount, pointList);
	}
	void DrawToScene(double dAlt);
	void DrawSelectArea(double dAlt);
	std::vector<CProcessor2*> getInBoundStands(CPROCESSOR2LIST* proclist);

	DistanceUnit GetWinspan()const{ return m_dWinspan; }
	DistanceUnit GetLength()const{ return m_dLength; }

	void SetWindspan(const DistanceUnit& wingspan){ m_dWinspan  =wingspan ;}
	void SetLength(const DistanceUnit & length){ m_dLength = length; }

	ARCVector2 GetDir();
private:
	int m_nNumber;
	DistanceUnit m_dLength;
	DistanceUnit m_dWinspan;

};

#endif // !defined(AFX_DEICEBAYPROC_H__A323EC84_4D74_4FA9_B1F2_BADB06AE81C9__INCLUDED_)
