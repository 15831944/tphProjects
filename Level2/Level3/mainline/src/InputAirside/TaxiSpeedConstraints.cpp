#include "StdAfx.h"
#include ".\taxispeedconstraints.h"


#include "ALTObject.h"
#include "IntersectionNode.h"
#include "Database/DBElementCollection_Impl.h"
#include "Taxiway.h"

#include "Common/ARCUnit.h"

// explicit instantiation
template DBElementCollection<TaxiwaySpeedConstraintDataItem>;
template DBElementCollection<TaxiwaySpeedConstraintFlttypeItem>;
template DBElementCollection<TaxiwaySpeedConstraintTaxiwayItem>;

TaxiwaySpeedConstraintDataItem::TaxiwaySpeedConstraintDataItem()
	: m_nFromNodeID(TAXIWAY_NODE_INVALID)
	, m_nToNodeID(TAXIWAY_NODE_INVALID)
	, m_dMaxSpeed(20.0)
{
	InitFromNodeName();
	InitToNodeName();
}

TaxiwaySpeedConstraintDataItem::~TaxiwaySpeedConstraintDataItem()
{

}

void TaxiwaySpeedConstraintDataItem::InitFromDBRecordset( CADORecordset& recordset )
{
	recordset.GetFieldValue(_T("ID"), m_nID);
	recordset.GetFieldValue(_T("FROM_NODE"), m_nFromNodeID);
	recordset.GetFieldValue(_T("TO_NODE"), m_nToNodeID);

	double dSpeedAsCMpS;
	recordset.GetFieldValue(_T("MAX_SPEED"), dSpeedAsCMpS);
	m_dMaxSpeed = ARCUnit::ConvertVelocity(dSpeedAsCMpS,ARCUnit::CMpS,ARCUnit::KNOT);

	InitFromNodeName();
	InitToNodeName();
}

void TaxiwaySpeedConstraintDataItem::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("INSERT INTO TAXIWAY_SPEED_CONSTRAINT_BASIC_DATA ")
		_T("(FROM_NODE, TO_NODE, MAX_SPEED, PARENTID) ")
		_T("VALUES(%d, %d, %f, %d)")
		, m_nFromNodeID
		, m_nToNodeID
		, GetMaxSpeedAsCMpS()
		, nParentID);
}
void TaxiwaySpeedConstraintDataItem::GetUpdateSQL( CString& strSQL ) const
{
	strSQL.Format(_T("UPDATE TAXIWAY_SPEED_CONSTRAINT_BASIC_DATA ")
		_T("SET  FROM_NODE=%d, TO_NODE=%d, MAX_SPEED=%f ")
		_T("WHERE (ID = %d)")
		, m_nFromNodeID
		, m_nToNodeID
		, GetMaxSpeedAsCMpS()
		, m_nID);

}
void TaxiwaySpeedConstraintDataItem::GetDeleteSQL( CString& strSQL ) const
{
	strSQL.Format(_T("DELETE FROM TAXIWAY_SPEED_CONSTRAINT_BASIC_DATA\
					 WHERE (ID = %d)"),m_nID);
}


void TaxiwaySpeedConstraintDataItem::ExportData( CAirsideExportFile& exportFile )
{

}

void TaxiwaySpeedConstraintDataItem::ImportData( CAirsideImportFile& importFile )
{

}

void TaxiwaySpeedConstraintDataItem::InitFromNodeName()
{
	if (TAXIWAY_NODE_INVALID == m_nFromNodeID)
	{
		m_strFromNodeName = _T("(N/A)");
		return;
	}
	IntersectionNode node;
	node.ReadData(m_nFromNodeID);
	m_strFromNodeName = node.GetName();
}

void TaxiwaySpeedConstraintDataItem::InitToNodeName()
{
	if (TAXIWAY_NODE_INVALID == m_nToNodeID)
	{
		m_strToNodeName = _T("(N/A)");
		return;
	}
	IntersectionNode node;
	node.ReadData(m_nToNodeID);
	m_strToNodeName = node.GetName();
}

double TaxiwaySpeedConstraintDataItem::GetMaxSpeedAsCMpS() const
{
	return ARCUnit::ConvertVelocity(m_dMaxSpeed,ARCUnit::KNOT,ARCUnit::CMpS);
}

CString TaxiwaySpeedConstraintDataItem::GetMaxSpeedString() const
{
	CString strText;
	strText.Format(_T("%.3f"), m_dMaxSpeed);
	return strText;
}

TaxiwaySpeedConstraintDataList::TaxiwaySpeedConstraintDataList( void )
{

}

TaxiwaySpeedConstraintDataList::~TaxiwaySpeedConstraintDataList( void )
{

}

void TaxiwaySpeedConstraintDataList::GetSelectElementSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("SELECT * FROM  TAXIWAY_SPEED_CONSTRAINT_BASIC_DATA WHERE (PARENTID = %d)"), nParentID);
}

// void TaxiwaySpeedConstraintDataList::ExportData( CAirsideExportFile& exportFile )
// {
// 
// }
// 
// void TaxiwaySpeedConstraintDataList::ImportData( CAirsideImportFile& importFile )
// {
// 
// }


TaxiwaySpeedConstraintFlttypeItem::TaxiwaySpeedConstraintFlttypeItem()
{
}

TaxiwaySpeedConstraintFlttypeItem::~TaxiwaySpeedConstraintFlttypeItem()
{

}

void TaxiwaySpeedConstraintFlttypeItem::InitFromDBRecordset( CADORecordset& recordset )
{
	recordset.GetFieldValue(_T("ID"), m_nID);
	recordset.GetFieldValue(_T("FLTTYPE"), m_strFltType);
// 	m_flightType.setConstraintWithVersion(m_strFltType);

	m_SpeedConstraints.ReadData(m_nID);
}

void TaxiwaySpeedConstraintFlttypeItem::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("INSERT INTO TAXIWAY_SPEED_CONSTRAINT_FLTTYPE (FLTTYPE, PARENTID) VALUES('%s', %d)")
		, m_strFltType, nParentID);
}

void TaxiwaySpeedConstraintFlttypeItem::GetUpdateSQL( CString& strSQL ) const
{
	strSQL.Format(_T("UPDATE TAXIWAY_SPEED_CONSTRAINT_FLTTYPE ")
		_T("SET  FLTTYPE='%s' WHERE (ID = %d)")
		, m_strFltType
		, m_nID);

}

void TaxiwaySpeedConstraintFlttypeItem::GetDeleteSQL( CString& strSQL ) const
{
	strSQL.Format(_T("DELETE FROM TAXIWAY_SPEED_CONSTRAINT_FLTTYPE\
					 WHERE (ID = %d)"),m_nID);
}

void TaxiwaySpeedConstraintFlttypeItem::SaveData( int nParentID )
{
	DBElement::SaveData(nParentID);
	m_SpeedConstraints.SaveData(m_nID);
}

void TaxiwaySpeedConstraintFlttypeItem::DeleteData()
{
	m_SpeedConstraints.DeleteData();
	m_SpeedConstraints.SaveData(-1/* don't care*/);
	DBElement::DeleteData();
}

void TaxiwaySpeedConstraintFlttypeItem::SetFlightType( const FlightConstraint& val )
{
	m_flightType = val;
	InitFltTypeString();
}

void TaxiwaySpeedConstraintFlttypeItem::SetFltTypeString( CString val )
{
	m_strFltType = val;
	m_flightType.setConstraintWithVersion(val);
}

void TaxiwaySpeedConstraintFlttypeItem::RefreshFlightTypeAirportDB( CAirportDatabase* pAirportDB )
{
	m_flightType.SetAirportDB(pAirportDB);
	m_flightType.setConstraintWithVersion(m_strFltType);
}

void TaxiwaySpeedConstraintFlttypeItem::InitFltTypeString()
{
	TCHAR szFltType[1024] = {0};
	m_flightType.WriteSyntaxStringWithVersion(szFltType);
	m_strFltType = szFltType;
}

CString TaxiwaySpeedConstraintFlttypeItem::GetFltTypeName() const
{
	CString str;
	m_flightType.screenPrint(str);
	return str;
}

BOOL TaxiwaySpeedConstraintFlttypeItem::CheckAllRoutesValid( const IntersectionNodeList& sortedNodeList, CString& strError )
{
	size_t nCount = m_SpeedConstraints.GetElementCount();
	for(size_t i=0;i<nCount;i++)
	{
		if (m_SpeedConstraints.GetItem(i)->GetMaxSpeed()<=0.0f)
		{
			strError = _T("Maximum speed can not be negtive or zero!");
			return FALSE;
		}

		int nStart1;
		int nEnd1;

		if (   FindNodeIndex(sortedNodeList, nStart1, m_SpeedConstraints.GetItem(i)->GetFromNodeID())
			&& FindNodeIndex(sortedNodeList, nEnd1, m_SpeedConstraints.GetItem(i)->GetToNodeID())
			)
		{
			if (nStart1>=nEnd1)
			{
				strError = _T("The To-Node must be after From-Node!");
				return FALSE;
			}
		}
		else
		{
			strError = _T("There is some node that becomes no longer valid!");
			return FALSE;
		}
		for(size_t j=0;j<nCount;j++)
		{
			if (i!=j && IsConflicted(sortedNodeList, m_SpeedConstraints.GetItem(i), m_SpeedConstraints.GetItem(j), strError))
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL TaxiwaySpeedConstraintFlttypeItem::IsConflicted( const IntersectionNodeList& sortedNodeList,
													 TaxiwaySpeedConstraintDataItem* pConstraintDataItem1,
													 TaxiwaySpeedConstraintDataItem* pConstraintDataItem2,
													 CString& strError )
{
	int nStart1;
	int nEnd1;
	int nStart2;
	int nEnd2;

	if (   FindNodeIndex(sortedNodeList, nStart1, pConstraintDataItem1->GetFromNodeID())
		&& FindNodeIndex(sortedNodeList, nEnd1, pConstraintDataItem1->GetToNodeID())
		&& FindNodeIndex(sortedNodeList, nStart2, pConstraintDataItem2->GetFromNodeID())
		&& FindNodeIndex(sortedNodeList, nEnd2, pConstraintDataItem2->GetToNodeID())
		)
	{
		if (nStart1>=nEnd1 || nStart2>=nEnd2)
		{
			strError = _T("The To-Node must be after From-Node!");
			return TRUE;
		}
		if ((nStart1<nStart2 && nEnd1<=nStart2) || (nStart2<nStart1 && nEnd2<=nStart1))
		{
			return FALSE;
		}
		strError = _T("There is conflict between constraint items.");
		return TRUE;
	}
	strError = _T("There is some node that becomes no longer valid!");
	return TRUE;
}

BOOL TaxiwaySpeedConstraintFlttypeItem::FindNodeIndex(
	const IntersectionNodeList& sortedNodeList, int& nNodeIndex, int nNodeID )
{
	if (TAXIWAY_NODE_INVALID == nNodeID)
	{
		return FALSE;
	}
	size_t nCount = sortedNodeList.size();
	for(size_t i=0;i<nCount;i++)
	{
		if (sortedNodeList[i].GetID() == nNodeID)
		{
			nNodeIndex = (int)i;
			return TRUE;
		}
	}
	return FALSE;
}
TaxiwaySpeedConstraintFlttypeList::TaxiwaySpeedConstraintFlttypeList( void )
{

}

TaxiwaySpeedConstraintFlttypeList::~TaxiwaySpeedConstraintFlttypeList( void )
{

}

void TaxiwaySpeedConstraintFlttypeList::GetSelectElementSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("SELECT * FROM TAXIWAY_SPEED_CONSTRAINT_FLTTYPE WHERE PARENTID = %d")
		, nParentID);
}

void TaxiwaySpeedConstraintFlttypeList::RefreshAirpotDB( CAirportDatabase* pAirportDB )
{
	for(size_t i=0;i<m_dataList.size();i++)
	{
		TaxiwaySpeedConstraintFlttypeItem* pItem = m_dataList.at(i);
		pItem->RefreshFlightTypeAirportDB(pAirportDB);
	}
}
// void TaxiwaySpeedConstraintFlttypeList::ExportData( CAirsideExportFile& exportFile )
// {
// 
// }
// 
// void TaxiwaySpeedConstraintFlttypeList::ImportData( CAirsideImportFile& importFile )
// {
// 
// }

TaxiwaySpeedConstraintTaxiwayItem::TaxiwaySpeedConstraintTaxiwayItem()
{

}

TaxiwaySpeedConstraintTaxiwayItem::~TaxiwaySpeedConstraintTaxiwayItem()
{

}

void TaxiwaySpeedConstraintTaxiwayItem::InitStandString()
{
	ALTObject obj;
	obj.ReadObject(m_nTaxiwayID);
	m_strTaxiwayString = obj.GetObjectName().GetIDString();
}

void TaxiwaySpeedConstraintTaxiwayItem::InitFromDBRecordset( CADORecordset& recordset )
{
	recordset.GetFieldValue(_T("ID"), m_nID);
	recordset.GetFieldValue(_T("TAXIWAY_ID"), m_nTaxiwayID);

	InitStandString();
	m_ConstraintFltType.ReadData(m_nID);
}

void TaxiwaySpeedConstraintTaxiwayItem::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("INSERT INTO TAXIWAY_SPEED_CONSTRAINT_TAXIWAY ")
		_T("(TAXIWAY_ID, PROJID) ")
		_T("VALUES(%d, %d)")
		, m_nTaxiwayID
		, nParentID);
}

void TaxiwaySpeedConstraintTaxiwayItem::GetUpdateSQL( CString& strSQL ) const
{
	strSQL.Format(_T("UPDATE TAXIWAY_SPEED_CONSTRAINT_TAXIWAY ")
		_T("SET  TAXIWAY_ID=%d ")
		_T("WHERE (ID = %d)")
		, m_nTaxiwayID
		, m_nID);
}

void TaxiwaySpeedConstraintTaxiwayItem::GetDeleteSQL( CString& strSQL ) const
{
	strSQL.Format(_T("DELETE FROM TAXIWAY_SPEED_CONSTRAINT_TAXIWAY\
					 WHERE (ID = %d)"),m_nID);
}

void TaxiwaySpeedConstraintTaxiwayItem::SaveData( int nParentID )
{
	DBElement::SaveData(nParentID);
	m_ConstraintFltType.SaveData(m_nID);
}

void TaxiwaySpeedConstraintTaxiwayItem::DeleteData()
{
	m_ConstraintFltType.DeleteData();
	m_ConstraintFltType.SaveData(-1/* don't care*/);
	DBElement::DeleteData();

}

BOOL TaxiwaySpeedConstraintTaxiwayItem::CheckAllRoutesValid( CString& strError )
{
	Taxiway taxiway;
	taxiway.ReadObject(m_nTaxiwayID);
	if (taxiway.GetObjectName().GetIDString() == _T(""))
	{
		strError.Format(_T("Taxiway with ID %d is no longer valid."), m_nTaxiwayID);
		return FALSE;
	}

	IntersectionNodeList nodeList = taxiway.GetIntersectNodes();
	std::sort(nodeList.begin(), nodeList.end(), TaxiwayNodeSorter(m_nTaxiwayID));

	size_t nCount = m_ConstraintFltType.GetElementCount();
	for(size_t i=0;i<nCount;i++)
	{
		TaxiwaySpeedConstraintFlttypeItem* pConstraintFlttypeItem = m_ConstraintFltType.GetItem(i);
		if (FALSE == pConstraintFlttypeItem->CheckAllRoutesValid(nodeList, strError))
		{
			return FALSE;
		}
	}
	return TRUE;
}


bool TaxiwaySpeedConstraintTaxiwayItem::TaxiwayNodeSorter::operator()(
	const IntersectionNode& lhs, const IntersectionNode& rhs )
{
	return lhs.GetDistance(m_nTaxiwayID) < rhs.GetDistance(m_nTaxiwayID);
}


TaxiSpeedConstraints::TaxiSpeedConstraints( void )
{

}

TaxiSpeedConstraints::~TaxiSpeedConstraints( void )
{

}

void TaxiSpeedConstraints::GetSelectElementSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("SELECT * FROM  TAXIWAY_SPEED_CONSTRAINT_TAXIWAY WHERE (PROJID = %d)"), nParentID);
}

void TaxiSpeedConstraints::RefreshAirpotDB( CAirportDatabase* pAirportDB )
{
	for(size_t i=0;i<m_dataList.size();i++)
	{
		TaxiwaySpeedConstraintTaxiwayItem* pItem = m_dataList.at(i);
		pItem->RefreshAirpotDB(pAirportDB);
	}
}

BOOL TaxiSpeedConstraints::CheckAllRoutesValid( CString& strError )
{
	size_t nCount = GetElementCount();
	for(size_t i=0;i<nCount;i++)
	{
		TaxiwaySpeedConstraintTaxiwayItem* pConstraintTaxiwayItem = GetItem(i);
		if (FALSE == pConstraintTaxiwayItem->CheckAllRoutesValid(strError))
		{
			return FALSE;
		}
	}
	return TRUE;
}

void TaxiSpeedConstraints::ReadData( int nProjID )
{
	CleanData();
	for(size_t i=0;i< m_deleteddataList.size();i++){
		delete m_deleteddataList[i];
	}
	m_deleteddataList.clear();

	DBElementCollection<TaxiwaySpeedConstraintTaxiwayItem>::ReadData(nProjID);
}

void TaxiSpeedConstraints::ReadData( int nProjID, CAirportDatabase* pAirportDB )
{
	ReadData(nProjID);
	RefreshAirpotDB(pAirportDB);
}