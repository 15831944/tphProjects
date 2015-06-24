#include "StdAfx.h"
#include ".\airsidelevellist.h"
#include "../Database/ADODatabase.h"
CAirsideLevelList::CAirsideLevelList(int nAirportID)
:m_nAirportID(nAirportID)
{
}

CAirsideLevelList::CAirsideLevelList()
{
	m_nAirportID = -1;
}
CAirsideLevelList::~CAirsideLevelList(void)
{
	for(int i=0 ; i< (int)m_vFullLevel.size() ;i ++)
	{
		delete m_vFullLevel[i];
	}
}
void CAirsideLevelList::ReadLevelList()
{
	m_vAirsideLevel.clear();

	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, BARPLEVEL, NAME\
		FROM ASGROUND\
		WHERE (APTID = %d)\
		ORDER BY LORDER"),m_nAirportID);
	long nCount = 0L;
	CADORecordset adRes;
	CADODatabase::ExecuteSQLStatement(strSQL,nCount,adRes);

	while(!adRes.IsEOF())
	{
		LevelInfo levelInfo;

		adRes.GetFieldValue(_T("ID"),levelInfo.nLevelID);
		adRes.GetFieldValue(_T("BARPLEVEL"),levelInfo.bMainLevel);
		adRes.GetFieldValue(_T("NAME"),levelInfo.strName);

		m_vAirsideLevel.push_back(levelInfo);
		adRes.MoveNextData();
	}

	// default ARP level
	if(m_vAirsideLevel.size() == 0)
	{
		CAirsideGround asGround;
		asGround.setMainLevel(true);
		asGround.SaveData(m_nAirportID);
		asGround.SaveGridInfo(asGround.getID());
		asGround.SaveCADInfo(asGround.getID());	

		CADODatabase::ExecuteSQLStatement(strSQL,nCount,adRes);

		while(!adRes.IsEOF())
		{
			LevelInfo levelInfo;

			adRes.GetFieldValue(_T("ID"),levelInfo.nLevelID);
			adRes.GetFieldValue(_T("BARPLEVEL"),levelInfo.bMainLevel);
			adRes.GetFieldValue(_T("NAME"),levelInfo.strName);

			m_vAirsideLevel.push_back(levelInfo);
			adRes.MoveNextData();
		}
	}

}
void CAirsideLevelList::ReadFullLevelList()
{
	ReadLevelList();
	for(int i=0;i< (int)m_vAirsideLevel.size();i++)
	{
		int nID = m_vAirsideLevel[i].nLevelID;
		CAirsideGround * pGround = GetLevelByID( nID );
		if(!pGround)
		{
			pGround = new CAirsideGround();
			m_vFullLevel.push_back(pGround);
		}
		
		if(pGround)
		{
			pGround->ReadData(nID);
			pGround->ReadCADInfo(nID);
			pGround->ReadOverlayInfo(nID);
			pGround->ReadGridInfo(nID);
		}
	}

	//remove unexsit list TO DO
	
	
	
}
void CAirsideLevelList::SaveLevelList()
{
	
	try
	{	
	   CADODatabase::BeginTransaction() ;
		CString strSQL = _T("");
		for (std::vector<LevelInfo>::size_type i=0; i < m_vAirsideLevel.size(); ++i)
		{
			strSQL.Format(_T("UPDATE ASGROUND\
							SET LORDER =%d\
							WHERE (ID = %d)"),i,m_vAirsideLevel[i].nLevelID);
			CADODatabase::ExecuteSQLStatement(strSQL);
		}
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
}
void CAirsideLevelList::AddLevelBeforeARP()
{

}
void CAirsideLevelList::AddLevelAfterARP()
{

}

CAirsideLevelList::LevelInfo CAirsideLevelList::GetItem(int nIndex)
{
	ASSERT(nIndex>=0 && nIndex<(int)m_vAirsideLevel.size());
	return m_vAirsideLevel[nIndex];
}

CAirsideGround * CAirsideLevelList::GetLevelByID( int id )
{
	for(int i=0;i< (int)m_vFullLevel.size();i++)
	{
		if( m_vFullLevel.at(i)->getID() == id )
			return m_vFullLevel.at(i);
	}
	return NULL;
}