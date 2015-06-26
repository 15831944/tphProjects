#include "stdafx.h"

#include "HoldShortLines.h"
#include "AirsideImportExportManager.h"
#include "../Database/DBElementCollection_Impl.h"

#include <boost/bind.hpp>

const GUID& CTaxiInterruptLine::getTypeGUID()
{
	// {745996C7-EECE-4676-98DE-11C6B396FA87}
	static const GUID name = 
	{ 0x745996c7, 0xeece, 0x4676, { 0x98, 0xde, 0x11, 0xc6, 0xb3, 0x96, 0xfa, 0x87 } };

	return name;
}

const CGuid& CTaxiInterruptLine::getGuid() const
{
	return m_guid;
}

CTaxiInterruptLine::CTaxiInterruptLine(void)
	: m_guid(CGuid::Create())
{
	m_nFrontInterNodeID = -1;
	m_nBackInterNodeID = -1;
	m_dDistToFrontInterNode = 0.0;
	m_strName = _T("");
	m_strDescription = _T("");
	m_nTaxiwayID = -1;
}

CTaxiInterruptLine::~CTaxiInterruptLine(void)
{
}

void CTaxiInterruptLine::InitFromDBRecordset(CADORecordset& recordset)
{
	if(!recordset.IsEOF())
	{
		recordset.GetFieldValue(_T("ID"),m_nID);
		recordset.GetFieldValue(_T("FRONTINTERNODEID"),m_nFrontInterNodeID);
		recordset.GetFieldValue(_T("BACKINTERNODEID"),m_nBackInterNodeID);
		int nValue =0;
		recordset.GetFieldValue(_T("DISTTOFRONTINTERNODE"),nValue);
		m_dDistToFrontInterNode = nValue;
		recordset.GetFieldValue(_T("NAME"),m_strName);
		recordset.GetFieldValue(_T("DESCRIPTION"),m_strDescription);


		recordset.GetFieldValue(_T("POS_X"),nValue);
		m_Position.setX(nValue);

		recordset.GetFieldValue(_T("POS_Y"),nValue);
		m_Position.setY(nValue);

		recordset.GetFieldValue(_T("TAXIWAYID"), nValue);
		m_nTaxiwayID = nValue;
	}
}

void CTaxiInterruptLine::GetInsertSQL(int nParentID,CString& strSQL)  const
{
	strSQL.Empty();
	strSQL.Format(_T("INSERT INTO AS_TAXIINTERRUPTLINES (FRONTINTERNODEID,BACKINTERNODEID,DISTTOFRONTINTERNODE,NAME,DESCRIPTION,POS_X,POS_Y,TAXIWAYID)	\
		VALUES (%d, %d, %d,'%s','%s',%d, %d, %d);"),m_nFrontInterNodeID, m_nBackInterNodeID,(int)m_dDistToFrontInterNode,m_strName,m_strDescription,(int)m_Position.getX(),(int)m_Position.getY(), m_nTaxiwayID);
	return ;
}

void CTaxiInterruptLine::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Empty();
	strSQL.Format(_T("UPDATE AS_TAXIINTERRUPTLINES SET FRONTINTERNODEID = %d, BACKINTERNODEID = %d,DISTTOFRONTINTERNODE = %d, NAME = '%s', DESCRIPTION = '%s', POS_X = %d, POS_Y = %d, TAXIWAYID = %d	\
					  WHERE (ID = %d);"),m_nFrontInterNodeID, m_nBackInterNodeID,(int)m_dDistToFrontInterNode,m_strName,m_strDescription,(int)m_Position.getX(),(int)m_Position.getY(),m_nTaxiwayID, m_nID);
	return ;
}

void CTaxiInterruptLine::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Empty();
	strSQL.Format(_T("DELETE FROM AS_TAXIINTERRUPTLINES WHERE (ID = %d);"),m_nID);
	return ;
}

void CTaxiInterruptLine::GetSelectSQL(int nID,CString& strSQL)const
{
	strSQL.Empty();
	strSQL.Format(_T("SELECT * FROM AS_TAXIINTERRUPTLINES WHERE (ID = %d);"),nID);
	return ;
}

void CTaxiInterruptLine::UpdateData()
{
	CString strSQL;
	GetUpdateSQL(strSQL);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

int CTaxiInterruptLine::GetFrontIntersectionNodeID()
{
	return m_nFrontInterNodeID;
}

int CTaxiInterruptLine::GetBackIntersectionNodeID()
{
	return m_nBackInterNodeID;
}

double CTaxiInterruptLine::GetDistToFrontIntersectionNode()
{
	return m_dDistToFrontInterNode;
}

CPoint2008 CTaxiInterruptLine::GetPosition()
{
	return m_Position;
}

CString CTaxiInterruptLine::GetName()
{
	return m_strName;
}

int CTaxiInterruptLine::GetTaxiwayID()
{
	return m_nTaxiwayID;
}

void CTaxiInterruptLine::SetDistToFrontIntersectionNode( double dDistToFrontInterNode )
{
	m_dDistToFrontInterNode = dDistToFrontInterNode;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
TaxiInterruptLineList::TaxiInterruptLineList(void)
{
}

TaxiInterruptLineList::~TaxiInterruptLineList(void)
{
}

void TaxiInterruptLineList::GetSelectElementSQL(int nParentID,CString& strSQL) const
{
	strSQL.Empty();
	strSQL.Format(_T("SELECT * FROM AS_TAXIINTERRUPTLINES;"));
	return;
}

void TaxiInterruptLineList::DoNotCall()
{
// 	TaxiInterruptLineList holdlines;
// 	holdlines.AddNewItem(NULL);
}

template CTaxiInterruptLine* DBElementCollection<CTaxiInterruptLine>::AddNewItem(CTaxiInterruptLine* dbElement);
template CTaxiInterruptLine* DBElementCollection<CTaxiInterruptLine>::GetItem(size_t nIndex);

CTaxiInterruptLine* TaxiInterruptLineList::GetTaxiInterruptLine( const CGuid& guid ) const
{
	std::vector<CTaxiInterruptLine*>::const_iterator ite = std::find_if(
		m_dataList.begin(), m_dataList.end(), boost::BOOST_BIND(&CTaxiInterruptLine::getGuid, _1) == guid
		);

	return ite == m_dataList.end() ? NULL : *ite;
}

CTaxiInterruptLine* TaxiInterruptLineList::GetTaxiInterruptLIneByID( int nID )
{
	for (size_t i = 0; i < GetElementCount(); i++)
	{
		CTaxiInterruptLine* pLine = GetItem(i);
		if (pLine->GetID() == nID)
		{
			return pLine;
		}
	}
	return NULL;
}
