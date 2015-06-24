// GateLagTimeDB.h: interface for the CGateLagTimeDB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GATELAGTIMEDB_H__0407246A_C07E_4653_99EB_033B409B74F5__INCLUDED_)
#define AFX_GATELAGTIMEDB_H__0407246A_C07E_4653_99EB_033B409B74F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PROCDB.H"


class CGateLagTimeDB : public ProcessorDatabase  
{
public:
	CGateLagTimeDB();
	virtual ~CGateLagTimeDB();
public:
	void WriteToFile( ArctermFile& _file )const;
	void ReadFromFile( ArctermFile& _file , InputTerminal* _pInTerm);

};

#endif // !defined(AFX_GATELAGTIMEDB_H__0407246A_C07E_4653_99EB_033B409B74F5__INCLUDED_)
