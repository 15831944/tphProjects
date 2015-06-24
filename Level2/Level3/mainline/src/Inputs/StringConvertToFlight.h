// StringConvertToFlight.h: interface for the CStringConvertToFlight class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRINGCONVERTTOFLIGHT_H__D7D22519_E8AC_4346_B8B0_1E84F6E39F04__INCLUDED_)
#define AFX_STRINGCONVERTTOFLIGHT_H__D7D22519_E8AC_4346_B8B0_1E84F6E39F04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class Flight;
class ArctermFile;
class InputTerminal;
class CGateMappingDB;
class ProcessorID;
class CStartDate;

#include "..\Common\ALTObjectID.h"

enum DATAFORMAT
{
	DATAFORMAT_USA = 0,
	DATAFORMAT_EUR
};

class CStringConvertToFlight  
{
private:
	InputTerminal* m_pTerm;
public:
	CStringConvertToFlight( InputTerminal* _pTerm,int nAirportID );
	virtual ~CStringConvertToFlight();
protected:
	bool IsAValidGate( const ProcessorID& _procID )const;
	bool IsAValidBaggageDevice( const ProcessorID& _procID )const;
	bool IsAValidStand( const ALTObjectID& _objID )const;
public:
	// build a flight according to the string _sFormateStr and gate map database
	Flight* ConvertToFlight( const CString _sFormateStr,ArctermFile& _fAgentFile ,const CGateMappingDB& _dbGateMap,
							bool _bArrTimeSec,bool _bDepTimeSec, CStartDate &ConvertStartDate ,CString& strErrMsg, DATAFORMAT dataFormat);
	
	ALTObjectIDList m_vObjectIDList;


};

#endif // !defined(AFX_STRINGCONVERTTOFLIGHT_H__D7D22519_E8AC_4346_B8B0_1E84F6E39F04__INCLUDED_)
