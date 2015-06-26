#include "StdAfx.h"
#include ".\postdeice_boundroute.h"
#include "..\Database\DBElementCollection_Impl.h"
#include "ALTObjectGroup.h"
CPostDeice_BoundRoute::CPostDeice_BoundRoute()
:CIn_OutBoundRoute()
{
}

CPostDeice_BoundRoute::~CPostDeice_BoundRoute()
{
}

void CPostDeice_BoundRoute::GetInsertSQL( CString& strSQL ) const
{
	strSQL.Format(_T("INSERT INTO IN_OUTBOUNDROUTE\
					 (OUTBOUNDROUTEASSIGNMENTID, BEALLRUNWAY, BEALLDEICE, ROUTETYPE) \
					 VALUES (%d, %d, %d, %d)"),\
					 m_nRouteAssignmentID,
					 m_bAllRunway?1:0,
					 m_bAllDeice?1:0,					
					 (int)POSTDEICING);
}

void CPostDeice_BoundRoute::GetUpdateSQL( CString& strSQL ) const
{
	strSQL.Format(_T("UPDATE IN_OUTBOUNDROUTE\
					 SET OUTBOUNDROUTEASSIGNMENTID = %d, BEALLRUNWAY = %d, BEALLDEICE = %d \
					 WHERE (ID =%d)"),
					 m_nRouteAssignmentID,
					 m_bAllRunway?1:0,
					 m_bAllDeice?1:0,
					 m_nID);
}

void CPostDeice_BoundRoute::ReadOriginAndDestination()
{
	if (!m_bAllRunway)
	{
		ReadLogicRunwayList();
	}

	if(!m_bAllDeice)
	{
		ReadDeiceGroupList();
	}
}

void CPostDeice_BoundRoute::SaveOriginAndDestination()
{
	SaveLogicRunwayList();
	SaveDeiceGroupList();
}

void CPostDeice_BoundRoute::DeleteOriginAndDestination()
{
	DeleteLogicRunwayList();
	DeleteDeiceGroupList();
}

void CPostDeice_BoundRoute::SetLogicRunwayIDList( const std::vector<LogicRwyID>& vRwyportIDList )
{
	m_vLogicRunwayIDList.clear();
	m_vLogicRunwayIDList.assign(vRwyportIDList.begin(), vRwyportIDList.end());
}

void CPostDeice_BoundRoute::GetLogicRunwayIDList( std::vector<LogicRwyID>& vRwyportIDList ) const
{
	vRwyportIDList.assign(m_vLogicRunwayIDList.begin(), m_vLogicRunwayIDList.end());
}

void CPostDeice_BoundRoute::ReadLogicRunwayList()
{
	CString strSelectSQL;
	strSelectSQL.Format(_T("SELECT RWYID, MARKID \
						   FROM TB_IN_OUTBOUND_RWYPORT \
						   WHERE (PARENTID = %d)"),
						   m_nID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		int nRwy, nMark;
		adoRecordset.GetFieldValue(_T("RWYID"),nRwy);
		adoRecordset.GetFieldValue(_T("MARKID"),nMark);
		m_vLogicRunwayIDList.push_back(LogicRwyID(nRwy,nMark));

		adoRecordset.MoveNextData();
	}
}

void CPostDeice_BoundRoute::SaveLogicRunwayList()
{
	DeleteLogicRunwayList();

	CString strSQL;

	size_t nSize = m_vLogicRunwayIDList.size();
	for (size_t i =0; i < nSize; i++ )
	{
		strSQL.Format(_T("INSERT INTO TB_IN_OUTBOUND_RWYPORT\
						 (PARENTID, RWYID, MARKID) \
						 VALUES (%d, %d, %d)"),\
						 m_nID,
						 m_vLogicRunwayIDList.at(i).first, m_vLogicRunwayIDList.at(i).second);

		CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
}

void CPostDeice_BoundRoute::DeleteLogicRunwayList()
{
	CString strSQL;
	strSQL.Format(_T("DELETE FROM TB_IN_OUTBOUND_RWYPORT\
					 WHERE (PARENTID = %d)"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

bool CPostDeice_BoundRoute::IsAvailableRoute( const ALTObjectID& padName, int nRwyID, int nMark )
{
	bool bRwyFit = false;
	bool bDeiceFit = false;

	if (m_bAllDeice)
		bDeiceFit = true;
	else
	{
		size_t nSize = m_vDeiceGroupList.size();
		for (size_t i =0; i < nSize; i++)
		{

			ALTObjectGroup deicegroup;
			deicegroup.ReadData( m_vDeiceGroupList[i]);

			if (padName.idFits(deicegroup.getName()))
			{
				bDeiceFit = true;
				break;
			}
		}
	}

	if (m_bAllRunway )
		bRwyFit = true;
	else
	{
		size_t nSize = m_vLogicRunwayIDList.size();
		for (size_t i =0; i < nSize; i++)
		{
			if (m_vLogicRunwayIDList.at(i).first == nRwyID && m_vLogicRunwayIDList.at(i).second == nMark)
			{
				bRwyFit = true;
				break;
			}
		}

	}

	return bDeiceFit&&bRwyFit ;
}