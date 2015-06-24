// Sector.h: interface for the CSector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECTOR_H__20229026_1F3C_464C_9BCA_880033302826__INCLUDED_)
#define AFX_SECTOR_H__20229026_1F3C_464C_9BCA_880033302826__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4786 )
#include <vector>
#include "Airport.h"

class CSector : public CObject
{
	DECLARE_SERIAL(CSector)
public:
	CSector();
	virtual ~CSector();
	virtual void Serialize(CArchive& ar);

	void AddFilter(CAIRPORTFILTER airportFilter);

	void GetAirportList(CAIRPORTLIST& APList);

	CString m_sName;
	std::vector<CAIRPORTFILTER> m_vAirportFilters;

protected:
};

typedef std::vector<CSector*> CSECTORLIST;

#endif // !defined(AFX_SECTOR_H__20229026_1F3C_464C_9BCA_880033302826__INCLUDED_)
