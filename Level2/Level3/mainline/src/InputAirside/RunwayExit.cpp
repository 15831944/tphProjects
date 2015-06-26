#include "StdAfx.h"

#include "RunwayExit.h"
#include "AirsideImportExportManager.h"
#include "RunwayExitDiscription.h"


RunwayExit::RunwayExit():
m_nID(-1),
m_rwMark(RUNWAYMARK_FIRST),
m_strName("DEFAULT"),
m_side(EXIT_LEFT),
m_dAngle(0.0f),
m_nRunwayID(-1),
m_nRunwayOrTaxiwayID(-1)
{

}

RunwayExit::RunwayExit( const RunwayExit& rwExit)
{
	*this = rwExit;
}

RunwayExit::~RunwayExit()
{

}
void RunwayExit::ReadExitsList( int rwId, RUNWAY_MARK rwMark, std::vector<RunwayExit>& reslt )
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM RUNWAY_EXIT	 WHERE (RUNWAYID = %d) AND (MARKINDEX = %d)"),rwId,(int)rwMark);

	CADORecordset adoRecordset;
	long nItemCount = 0;
	CADODatabase::ExecuteSQLStatement(strSQL,nItemCount,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		RunwayExit runwayExit;
		runwayExit.ReadData(adoRecordset);		
		reslt.push_back(runwayExit);

		adoRecordset.MoveNextData();
	}	
}
void RunwayExit::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("RUNWAYID"),m_nRunwayID);
	int nNodeId;
	adoRecordset.GetFieldValue(_T("INTNODEID"),nNodeId);
	m_intersectNode.ReadData(nNodeId);
	int nMarkIndex = RUNWAYMARK_FIRST;
	adoRecordset.GetFieldValue(_T("MARKINDEX"),nMarkIndex);
	if (nMarkIndex != RUNWAYMARK_FIRST)
		nMarkIndex = RUNWAYMARK_SECOND;
	m_rwMark = (RUNWAY_MARK)nMarkIndex;

	int nExitSide = EXIT_LEFT;
	adoRecordset.GetFieldValue(_T("EXITSIDE"),nExitSide);
	if (nExitSide != EXIT_LEFT)
	{
		nExitSide = EXIT_RIGHT;
	}
	m_side = (ExitSide)nExitSide;

	adoRecordset.GetFieldValue(_T("ANGLE"),m_dAngle);

	adoRecordset.GetFieldValue(_T("NAME"), m_strName );

	adoRecordset.GetFieldValue(_T("TAXIWAYID"),m_nRunwayOrTaxiwayID);
}

void RunwayExit::ReadData(int nRunwayExitID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM RUNWAY_EXIT	 WHERE (ID = %d)"), nRunwayExitID);

	CADORecordset adoRecordset;
	long nItemCount = 0;
	CADODatabase::ExecuteSQLStatement(strSQL,nItemCount,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		ReadData(adoRecordset);

		adoRecordset.MoveNextData();
	}		
}

void RunwayExit::ReadExitsList( int rwId, std::vector<RunwayExit>& reslt )
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM RUNWAY_EXIT	 WHERE (RUNWAYID = %d)"),rwId);

	CADORecordset adoRecordset;
	long nItemCount = 0;
	CADODatabase::ExecuteSQLStatement(strSQL,nItemCount,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		RunwayExit runwayExit;
		runwayExit.ReadData(adoRecordset);
		reslt.push_back(runwayExit);

		adoRecordset.MoveNextData();
	}		
}
void RunwayExit::SaveData(int nRunwayID)
{
	m_nRunwayID = nRunwayID;
	if (m_nID >=0 )
	{
		return UpdateData();
	}
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO RUNWAY_EXIT\
					 (RUNWAYID, MARKINDEX, INTNODEID, EXITSIDE, NAME, ANGLE, TAXIWAYID)\
					 VALUES (%d,%d,%d,%d,'%s',%f,%d)"),
					 nRunwayID,(int)m_rwMark,m_intersectNode.GetID(),(int)m_side,m_strName,m_dAngle, m_nRunwayOrTaxiwayID);


	m_nID =  CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

}
void RunwayExit::ImportRunwayExits(CAirsideImportFile& importFile)
{
	if( importFile.getVersion() < 1038)
	{
		return;
	}
	while (!importFile.getFile().isBlank())
	{
		RunwayExit runwayExit;
		runwayExit.ImportRunwayExit(importFile);
		//importFile.getFile().getLine();
	}
}
void RunwayExit::ImportRunwayExit(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);
	int nOldRunwayID = -1;
	importFile.getFile().getInteger(nOldRunwayID);

	int nMarkIndex = 0;
	importFile.getFile().getInteger(nMarkIndex);
	if (nMarkIndex == RUNWAYMARK_FIRST)
		m_rwMark = RUNWAYMARK_FIRST;
	else
		m_rwMark = RUNWAYMARK_SECOND;

	int nOldIntNodeID = -1;
	importFile.getFile().getInteger(nOldIntNodeID);
	m_intersectNode.SetID( importFile.GetIntersectionIndexMap(nOldIntNodeID) );
	int nExitSide = 0;
	importFile.getFile().getInteger(nExitSide);
	if (nExitSide == EXIT_LEFT)
		m_side = EXIT_LEFT;
	else
		m_side = EXIT_RIGHT;

	char sz[1024];
	importFile.getFile().getField(sz,1024);
	m_strName = sz;

	importFile.getFile().getFloat(m_dAngle);

	int nTaxiID = -1;
	importFile.getFile().getInteger(nTaxiID);
	m_nRunwayOrTaxiwayID = importFile.GetObjectNewID(nTaxiID);

	SaveData(importFile.GetObjectNewID(nOldRunwayID));

	importFile.AddRunwayExitIndexMap(nOldID, m_nID);

	importFile.getFile().getLine();
}

void RunwayExit::ExportRuwayExits(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField(_T("ID,RUNWAYID, MARKINDEX, INTNODEID, EXITSIDE, NAME, ANGLE"));
	exportFile.getFile().writeLine();

	CString strSQL;
	strSQL.Format(_T("SELECT  *	 FROM  RUNWAY_EXIT	 WHERE  RUNWAYID IN\
					 (SELECT  ID\
					 FROM  ALTOBJECT\
					 WHERE   (TYPE = %d) AND (APTID IN\
					 (SELECT ID\
					 FROM    ALTAIRPORT\
					 WHERE   (PROJID = %d))))"),
					 (int)ALT_RUNWAY,exportFile.GetProjectID());
	CADORecordset adoRecordset;
	long nItemCount = 0;
	CADODatabase::ExecuteSQLStatement(strSQL,nItemCount,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		RunwayExit runwayExit;
		runwayExit.ReadData(adoRecordset);
		int nRunwayID = -1;
		adoRecordset.GetFieldValue(_T("RUNWAYID"),nRunwayID);
		runwayExit.ExportRunwayExit(exportFile,nRunwayID);
		adoRecordset.MoveNextData();
	}

	exportFile.getFile().endFile();

}
void RunwayExit::ExportRunwayExit(CAirsideExportFile& exportFile,int nRunwayID)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(nRunwayID);
	exportFile.getFile().writeInt((int)m_rwMark);
	exportFile.getFile().writeInt(m_intersectNode.GetID());
	exportFile.getFile().writeInt((int)m_side);
	exportFile.getFile().writeField(m_strName);
	exportFile.getFile().writeDouble(m_dAngle);
	exportFile.getFile().writeInt(m_nRunwayOrTaxiwayID);
	exportFile.getFile().writeLine();

}

RunwayExit& RunwayExit::operator =(const RunwayExit& rwExit)
{
	m_nID = rwExit.m_nID;
	m_side = rwExit.m_side;
	m_rwMark = rwExit.m_rwMark;
	m_strName = rwExit.m_strName;
	m_dAngle = rwExit.m_dAngle;
	m_intersectNode  = rwExit.m_intersectNode;
	m_nRunwayID = rwExit.m_nRunwayID;
	m_nRunwayOrTaxiwayID = rwExit.m_nRunwayOrTaxiwayID;
	return *this;
}

void RunwayExit::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE RUNWAY_EXIT\
					 SET MARKINDEX =%d, INTNODEID =%d, EXITSIDE =%d, NAME ='%s', ANGLE = %f, TAXIWAYID = %d\
					 WHERE (ID = %d)"),
					 (int)m_rwMark,m_intersectNode.GetID(),(int)m_side,m_strName,m_dAngle,m_nRunwayOrTaxiwayID, m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
void RunwayExit::DeleteData()
{

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM RUNWAY_EXIT WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

}


IntersectionNode& RunwayExit::GetIntesectionNode()
{
	return m_intersectNode;
}

IntersectionNode RunwayExit::GetIntesectionNode() const
{
	return m_intersectNode;
}
int RunwayExit::GetTaxiwayID()const
{
	return m_nRunwayOrTaxiwayID;
}

CString RunwayExit::GetName() const
{
	return m_strName;
}

bool RunwayExit::IsIdent( const RunwayExit& otherExit ) const
{
	if( GetRunwayMark()!= otherExit.GetRunwayMark() )
		return false;
	if( GetIntersectNodeID()!= otherExit.GetIntersectNodeID() )
		return false;
	if( GetRunwayID()!= otherExit.GetRunwayID() )
		return false;
	if( GetTaxiwayID()!= otherExit.GetTaxiwayID() )
		return false;
	if( GetSideType()!= otherExit.GetSideType() )
		return false;

	return true;
}

double RunwayExit::GetAngle() const
{
	return m_dAngle;
}

int RunwayExit::GetRunwayID() const
{
	return m_nRunwayID;
}

RunwayExit::ExitSide RunwayExit::GetSideType() const
{
	return m_side;
}

int RunwayExit::GetIntersectNodeID() const
{
	return m_intersectNode.GetID();
}

RUNWAY_MARK RunwayExit::GetRunwayMark() const
{
	return m_rwMark;
}

int RunwayExit::GetID() const
{
	return m_nID;
}

void RunwayExit::SetName( CString strName )
{
	m_strName = strName;
}

void RunwayExit::SetExitSide( ExitSide side )
{
	m_side = side;
}

void RunwayExit::SetRunwayMark( RUNWAY_MARK rwMark )
{
	m_rwMark = rwMark;
}

void RunwayExit::SetAngle( double dAngle )
{
	m_dAngle = dAngle;
}

void RunwayExit::SetRunwayID( int nID )
{
	m_nRunwayID = nID;
}

void RunwayExit::SetIntersectRunwayOrTaxiwayID( int nID )
{
	m_nRunwayOrTaxiwayID = nID;
}

void RunwayExit::SetIntersectNode( const IntersectionNode& node )
{
	m_intersectNode = node;
}

void RunwayExit::SetID( int nID )
{
	m_nID = nID;
}

bool RunwayExit::EqualToRunwayExitDescription(const RunwayExitDescription& runwayDes)const
{
	if (m_nRunwayID == runwayDes.GetRunwayID() &&
		m_nRunwayOrTaxiwayID == runwayDes.GetTaxiwayID()&&
		m_side == runwayDes.GetSideType()&&
		m_rwMark == runwayDes.GetRunwayMark()&&
		m_intersectNode.GetIndex() == runwayDes.GetIndex())
	{
		return true;
	}
	return false;
}