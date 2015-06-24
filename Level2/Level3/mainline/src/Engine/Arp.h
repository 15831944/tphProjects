// Arp.h: interface for the Arp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARP_H__0029896F_53A8_4C45_9AE3_749B74CA8F3A__INCLUDED_)
#define AFX_ARP_H__0029896F_53A8_4C45_9AE3_749B74CA8F3A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AirfieldProcessor.h"

using namespace std;

class ArpProc : public AirfieldProcessor  
{
public:
	ArpProc();
	virtual ~ArpProc();
	//processor type
	virtual int getProcessorType (void) const { return ArpProcessor; }
	//processor name
	virtual const char *getProcessorName (void) const { return "ARP"; }
	//processor i/o
	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile) const;

	//Latitude & longitude
	enum Latlong{ Latitude, Longitude };
/*
	void SetLatlong(char _cLatitude, char _cLongitude) 
	{ m_sLatlong[0]= _toupper(_cLatitude); m_sLatlong[1]= _toupper(_cLongitude);}

	LPCSTR GetLatlong() const { return m_sLatlong; }

private:
	char  m_sLatlong[3];// [0] = 'E'|'W'|'\0' , [1] = 'S'|'N'|'\0', [2]='\0'
*/
	void SetLatlong(Latlong _flag, string _sLaglong)
	{
		m_sLatlong[_flag] = _sLaglong;
	}
	const string& GetLatlong(Latlong _flag) const
	{
		return m_sLatlong[_flag];
	}

private:
	string m_sLatlong[2];

	
};

#endif // !defined(AFX_ARP_H__0029896F_53A8_4C45_9AE3_749B74CA8F3A__INCLUDED_)
