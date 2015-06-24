#include "StdAfx.h"
#include ".\airsideground.h"
#include "../Inputs/IN_TERM.H"
#include "../Database/ADODatabase.h"
#include "../inputairside/AirsideImportExportManager.h"
CAirsideGround::CAirsideGround()
:CFloor2(0)
,m_nID(-1)
,m_nAirportID(-1)
,m_nOrder(0)
,m_bMainLevel(0)

{
	m_Grid.bVisibility = TRUE;
	m_sName = _T("ARP Level");
	m_Grid.dSizeX *= 50;
	m_Grid.dSizeY *= 50;
	m_Grid.dLinesEvery *= 20;
	//setEnvironType(EnvMode_AirSide) ;
}
CAirsideGround::CAirsideGround( const CString& sName)
:CFloor2(0,sName)
,m_nID(-1)
,m_nAirportID(-1)
,m_nOrder(0)
,m_bMainLevel(0)
{
	m_Grid.dSizeX *=50;
	m_Grid.dSizeY *= 50;
	m_Grid.dLinesEvery *= 20;
	//setEnvironType(EnvMode_AirSide) ;
}

CAirsideGround::~CAirsideGround(void)
{
}

void CAirsideGround::ReadData(int nLevelID)
{

	m_nID = nLevelID;

	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID,APTID, LORDER, BARPLEVEL, NAME, VIEWALT, REALALT, THIICKNESS, OFFSETX, OFFSETY, BMONO, \
		MONOCOLOR, BVISIBLE, BOPAQUA, BMARKER, MARKERX, MARKERY, BSHOWMAP, \
		TSTAMP\
		FROM ASGROUND\
		WHERE ID = %d"),nLevelID);

		long nCount = 0L;
	CADORecordset adRes;
	CADODatabase::ExecuteSQLStatement(strSQL,nCount,adRes);

	if (!adRes.IsEOF())
	{
		ReadData(adRes);
	}

	ReadOverlayInfo(nLevelID);

}
void CAirsideGround::ReadData(CADORecordset& adRes)
{
	adRes.GetFieldValue(_T("ID"),m_nID);
	adRes.GetFieldValue(_T("APTID"),m_nAirportID);
	adRes.GetFieldValue(_T("LORDER"),m_nOrder);
	adRes.GetFieldValue(_T("BARPLEVEL"),m_bMainLevel);

	adRes.GetFieldValue(_T("NAME"),m_sName);
	adRes.GetFieldValue(_T("VIEWALT"),m_dVisualAltitude);
	adRes.GetFieldValue(_T("REALALT"),m_dRealAltitude);
	adRes.GetFieldValue(_T("THIICKNESS"),m_dThickness);
	adRes.GetFieldValue(_T("OFFSETX"),m_vOffset[0]);
	adRes.GetFieldValue(_T("OFFSETY"),m_vOffset[1]);

	CString strMono;
	adRes.GetFieldValue(_T("BMONO"),strMono);
	m_bIsMonochrome = atoi(strMono);

	unsigned long lColor = 0L;
	adRes.GetFieldValue(_T("MONOCOLOR"),lColor);
	m_cMonochromeColor = (COLORREF)(DWORD)lColor;

	bool bValue = false;
	adRes.GetFieldValue(_T("BVISIBLE"),bValue);
	m_bIsVisible = bValue;

	bValue = false;
	adRes.GetFieldValue(_T("BOPAQUA"),bValue);
	m_bIsOpaque = bValue;
	bValue = false;
	adRes.GetFieldValue(_T("BMARKER"),bValue);
	m_bUseMarker = bValue;


	adRes.GetFieldValue(_T("MARKERX"),m_vMarkerLoc[0]);
	adRes.GetFieldValue(_T("MARKERY"),m_vMarkerLoc[1]);

	bValue = false;
	adRes.GetFieldValue(_T("BSHOWMAP"),bValue);
	m_bIsShowMap = bValue;
}
void CAirsideGround::SaveData(int nAirportID,int nOrder)
{
	m_nOrder = nOrder;
	SaveData(nAirportID);

}
void CAirsideGround::Rename(const CString& strName)
{
	m_sName = strName;
	CString strSQL;
	strSQL.Format(_T("UPDATE ASGROUND SET NAME ='%s' WHERE ID = %d"),strName,m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}
void CAirsideGround::SaveData(int nAirportID)
{
	try
	{
		CADODatabase::BeginTransaction() ;
		if (m_nID >= 0)
		{
			return UpdateData(nAirportID);
		}

		m_nAirportID = nAirportID;
		CString strSQL = _T("");
		strSQL.Format(_T("INSERT INTO ASGROUND\
						 ( APTID, LORDER, BARPLEVEL, NAME, VIEWALT, REALALT, THIICKNESS, OFFSETX, OFFSETY, BMONO, \
						 MONOCOLOR, BVISIBLE, BOPAQUA, BMARKER, MARKERX, MARKERY, BSHOWMAP)\
						 VALUES (%d,%d,%d,'%s',%f,%f,%f,%f,%f,%d,%d,%d,%d,%d,%f,%f,%d)"),
						 m_nAirportID,m_nOrder,m_bMainLevel,m_sName,m_dVisualAltitude,m_dRealAltitude,m_dThickness,m_vOffset[0],m_vOffset[1],
						 m_bIsMonochrome?1:0,(unsigned long)(DWORD)(COLORREF)m_cMonochromeColor,m_bIsVisible?1:0,
						 m_bIsOpaque?1:0,m_bUseMarker?1:0,m_vMarkerLoc[0],m_vMarkerLoc[1],m_bIsShowMap?1:0
						 );

		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}

}

void CAirsideGround::UpdateData(int nLeveID)
{
	//m_nAirportID = nAirportID;

	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE ASGROUND\
		SET LORDER =%d, BARPLEVEL =%d, NAME ='%s', VIEWALT =%f, REALALT =%f, THIICKNESS =%f, OFFSETX =%f, OFFSETY =%f, BMONO =%d, \
		MONOCOLOR =%d, BVISIBLE =%d, BOPAQUA =%d, BMARKER =%d, MARKERX =%f, MARKERY =%f, \
		BSHOWMAP =%d\
		WHERE ID = %d"),m_nOrder,m_bMainLevel,m_sName,m_dVisualAltitude,m_dRealAltitude,m_dThickness,m_vOffset[0],m_vOffset[1],
		m_bIsMonochrome?1:0,(unsigned long)(DWORD)(COLORREF)m_cMonochromeColor,m_bIsVisible?1:0,
		m_bIsOpaque?1:0,m_bUseMarker?1:0,m_vMarkerLoc[0],m_vMarkerLoc[1],m_bIsShowMap?1:0,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}


void CAirsideGround::DeleteData(int nID)
{
	try
	{
		CADODatabase::BeginTransaction() ;
		CString strSQL = _T("");
		strSQL.Format(_T("DELETE FROM ASGROUND\
			WHERE ID = %d"),nID);
			CADODatabase::ExecuteSQLStatement(strSQL);

		DelGridInfo(nID);
		DelCADInfo(nID);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}

}

void CAirsideGround::ReadGridInfo(int nLevelID)
{
	m_nID = nLevelID;
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ACOLOR, LCOLOR, SCOLOR, SIZEX, SIZEY, LINESEVERY, SUBDIVS, BVISIBLE, \
		TSTAMP\
		FROM ASGROUND_GRID\
		WHERE ID = %d"),nLevelID);
		long nCount = 0L;
	CADORecordset adRes;
	CADODatabase::ExecuteSQLStatement(strSQL,nCount,adRes);

	if (!adRes.IsEOF())
	{
		unsigned long lColor = 0L;
		adRes.GetFieldValue(_T("ACOLOR"),lColor);
		m_Grid.cAxesColor = (COLORREF)(DWORD)lColor;

		adRes.GetFieldValue(_T("LCOLOR"),lColor);
		m_Grid.cLinesColor = (COLORREF)(DWORD)lColor;


		adRes.GetFieldValue(_T("SCOLOR"),lColor);
		m_Grid.cSubdivsColor= (COLORREF)(DWORD)lColor;

		adRes.GetFieldValue(_T("SIZEX"),m_Grid.dSizeX);
		adRes.GetFieldValue(_T("SIZEY"),m_Grid.dSizeY);
		adRes.GetFieldValue(_T("SUBDIVS"),m_Grid.nSubdivs);
		bool bVisible;
		adRes.GetFieldValue(_T("BVISIBLE"),bVisible);
		m_Grid.bVisibility = bVisible;
		adRes.GetFieldValue(_T("LINESEVERY"),m_Grid.dLinesEvery);

	}

}
void CAirsideGround::SaveGridInfo(int nAirportID)
{
	try
	{
		CADODatabase::BeginTransaction() ;
		CString strSQL = _T("");
		strSQL.Format(_T("INSERT INTO ASGROUND_GRID\
			(ID, ACOLOR, LCOLOR, SCOLOR, SIZEX, SIZEY, LINESEVERY, SUBDIVS, BVISIBLE)\
			VALUES (%d,%d,%d,%d,%f,%f,%f,%d,%d)"),
			nAirportID,m_Grid.cAxesColor,m_Grid.cLinesColor,m_Grid.cSubdivsColor,
			m_Grid.dSizeX,m_Grid.dSizeY,m_Grid.dLinesEvery,m_Grid.nSubdivs,m_Grid.bVisibility?1:0);
		CADODatabase::ExecuteSQLStatement(strSQL);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}

}
void CAirsideGround::UpdateGridInfo(int nAirportID)
{
	try
	{
		CADODatabase::BeginTransaction() ;
		CString strSQL = _T("");
		strSQL.Format(_T("UPDATE ASGROUND_GRID\
			SET ACOLOR =%d, LCOLOR =%d, SCOLOR =%d, SIZEX =%f, SIZEY =%f, LINESEVERY = %f, SUBDIVS =%d, \
			BVISIBLE =%d\
			WHERE ID = %d"),
			m_Grid.cAxesColor,m_Grid.cLinesColor,m_Grid.cSubdivsColor,
			m_Grid.dSizeX,m_Grid.dSizeY,m_Grid.dLinesEvery,m_Grid.nSubdivs,m_Grid.bVisibility?1:0,nAirportID);

		CADODatabase::ExecuteSQLStatement(strSQL);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}

}

void CAirsideGround::DelGridInfo(int nAirportID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM ASGROUND_GRID\
		WHERE ID = %d"),nAirportID);
		CADODatabase::ExecuteSQLStatement(strSQL);
}

void CAirsideGround::ReadCADInfo(int nLevelID)
{
	m_nID = nLevelID;

	CString strSQL = _T("");
	strSQL.Format(_T("SELECT FILENAME, PATHNAME, OFFSETX, OFFSETY, SCALE, ROTATION, TSTAMP\
		FROM ASGROUND_CADINFO\
		WHERE ID = %d"),nLevelID);
		long nCount = 0L;
	CADORecordset adRes;
	CADODatabase::ExecuteSQLStatement(strSQL,nCount,adRes);

	if (!adRes.IsEOF())
	{
		adRes.GetFieldValue(_T("FILENAME"),m_CadFile.sFileName);
		adRes.GetFieldValue(_T("PATHNAME"),m_CadFile.sPathName);
		adRes.GetFieldValue(_T("OFFSETX"),m_CadFile.vOffset[0]);
		adRes.GetFieldValue(_T("OFFSETY"),m_CadFile.vOffset[1]);
		adRes.GetFieldValue(_T("SCALE"),m_CadFile.dScale);
		adRes.GetFieldValue(_T("ROTATION"),m_CadFile.dRotation);

		m_CadFile.sPathName.Trim();
		if (m_CadFile.sFileName.CompareNoCase(_T("")) != 0 && m_CadFile.sPathName.GetLength() >0)
		{
			m_bIsMapValid = TRUE;
		}
		else
			m_bIsMapValid = FALSE;
	}

}
void CAirsideGround::SaveCADInfo(int nLevelID)
{
	try
	{
		CADODatabase::BeginTransaction() ;
		CString strSQL = _T("");
		strSQL.Format(_T("INSERT INTO ASGROUND_CADINFO\
			(ID, FILENAME, PATHNAME, OFFSETX, OFFSETY, SCALE, ROTATION)\
			VALUES (%d,'%s','%s',%f,%f,%f,%f)"),
			nLevelID,m_CadFile.sFileName,m_CadFile.sPathName,m_CadFile.vOffset[0],m_CadFile.vOffset[1],
			m_CadFile.dScale,m_CadFile.dRotation);
		CADODatabase::ExecuteSQLStatement(strSQL);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}

}
void CAirsideGround::UpdateCADInfo(int nLevelID)
{
	try
	{
		CADODatabase::BeginTransaction() ;
		CString strSQL = _T("");
		strSQL.Format(_T("UPDATE ASGROUND_CADINFO\
			SET FILENAME ='%s', PATHNAME ='%s', OFFSETX =%f, OFFSETY =%f, SCALE =%f, ROTATION =%f\
			WHERE ID = %d"),m_CadFile.sFileName,m_CadFile.sPathName,m_CadFile.vOffset[0],m_CadFile.vOffset[1],
			m_CadFile.dScale,m_CadFile.dRotation,nLevelID);
		CADODatabase::ExecuteSQLStatement(strSQL);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}

}
void CAirsideGround::DelCADInfo(int nAirportID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM ASGROUND_CADINFO\
		WHERE ID = %d"),nAirportID);
		CADODatabase::ExecuteSQLStatement(strSQL);

}

void CAirsideGround::ReadOverlayInfo(int nLevelID)
{
	m_nID = nLevelID;

	CString strSQL = _T("");
	strSQL.Format(_T("SELECT FILENAME, REFLINEHEADX,REFLINEHEADY,REFLINETAILX,REFLINETAILY, OFFSETX, OFFSETY, SCALE, ROTATION, TSTAMP\
		FROM ASGROUND_PICTUREINFO\
		WHERE LEVELID = %d"),nLevelID);
		long nCount = 0L;
	CADORecordset adRes;
	CADODatabase::ExecuteSQLStatement(strSQL,nCount,adRes);

	if (!adRes.IsEOF())
	{
		adRes.GetFieldValue(_T("FILENAME"),m_picInfo.sFileName);

		Point head, tail;
		double x, y, z=.0;

		adRes.GetFieldValue(_T("REFLINEHEADX"), x);
		adRes.GetFieldValue(_T("REFLINEHEADY"), y);
		head.setPoint( x, y, z);

		adRes.GetFieldValue(_T("REFLINETAILX"), x);
		adRes.GetFieldValue(_T("REFLINETAILY"), y);
		tail.setPoint( x, y, z);

		Point ptList[2];
		ptList[0] = head;
		ptList[1] = tail;
		m_picInfo.refLine.init( 2, ptList);


		adRes.GetFieldValue(_T("OFFSETX"),m_picInfo.vOffset[0]);
		adRes.GetFieldValue(_T("OFFSETY"),m_picInfo.vOffset[1]);
		adRes.GetFieldValue(_T("SCALE"),m_picInfo.dScale);
		adRes.GetFieldValue(_T("ROTATION"),m_picInfo.dRotation);
		int nStamp = 0;
		adRes.GetFieldValue(_T("TSTAMP"),nStamp);
		m_picInfo.bShow = (nStamp == 1 ? true : false);


		//m_CadFile.sPathName.Trim();
		//if (m_CadFile.sFileName.CompareNoCase(_T("")) != 0 && m_CadFile.sPathName.GetLength() >0)
		//{
		//	m_bIsMapValid = TRUE;
		//}
		//else
		//	m_bIsMapValid = FALSE;
	}
}

void CAirsideGround::UpdateOverlayInfo(int nLevelID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE ASGROUND_PICTUREINFO\
		SET FILENAME ='%s',REFLINEHEADX=%f,REFLINEHEADY=%f,REFLINETAILX=%f,REFLINETAILY=%f, OFFSETX =%f, OFFSETY =%f, SCALE =%f, ROTATION =%f, TSTAMP=%d\
		WHERE LEVELID = %d"),m_picInfo.sFileName,
		m_picInfo.refLine.getPoint(0).getX(),m_picInfo.refLine.getPoint(0).getY(),
		m_picInfo.refLine.getPoint(1).getX(),m_picInfo.refLine.getPoint(1).getY(),

		m_picInfo.vOffset[0],m_picInfo.vOffset[1],
		m_picInfo.dScale,m_picInfo.dRotation,m_picInfo.bShow ? 1 : 0, nLevelID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CAirsideGround::SaveOverlayInfo(int nLevelID)
{
	try
	{
		CADODatabase::BeginTransaction() ;
		CString strSQL = _T("");
		strSQL.Format(_T("SELECT * FROM ASGROUND_PICTUREINFO\
						 WHERE LEVELID = %d"),nLevelID);
		long nCount = 0L;
		CADORecordset adRes;
		CADODatabase::ExecuteSQLStatement(strSQL,nCount,adRes);

		if (!adRes.IsEOF())
		{
			UpdateOverlayInfo(nLevelID);
			return;
		}

		strSQL.Empty();
		strSQL.Format(_T("INSERT INTO ASGROUND_PICTUREINFO\
						 (LEVELID, FILENAME, REFLINEHEADX,REFLINEHEADY,REFLINETAILX,REFLINETAILY, OFFSETX, OFFSETY, SCALE, ROTATION, TSTAMP)\
						 VALUES (%d,'%s',%f,%f,%f,%f,%f,%f,%f,%f,%d)"),
						 nLevelID,m_picInfo.sFileName,

						 m_picInfo.refLine.getPoint(0).getX(),m_picInfo.refLine.getPoint(0).getY(),
						 m_picInfo.refLine.getPoint(1).getX(),m_picInfo.refLine.getPoint(1).getY(),

						 m_picInfo.vOffset[0],m_picInfo.vOffset[1],
						 m_picInfo.dScale,m_picInfo.dRotation, m_picInfo.bShow ? 1 : 0);

		CADODatabase::ExecuteSQLStatement(strSQL);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}

}

void CAirsideGround::DelOverlayInfo(int nLevelID)
{
	try
	{
		CADODatabase::BeginTransaction() ;
		CString strSQL = _T("");
		strSQL.Format(_T("DELETE FROM ASGROUND_PICTUREINFO\
			WHERE LEVELID = %d"),nLevelID);
			CADODatabase::ExecuteSQLStatement(strSQL);
		CADODatabase::CommitTransaction() ;
	}
	catch (CException*)
	{
		CADODatabase::RollBackTransation() ;
	}

}



//export
void CAirsideGround::ExportAirsideGrounds(CAirsideExportFile& exportFile)
{


	exportFile.getFile().writeField(_T("ID,APTID, LORDER, BARPLEVEL, NAME, VIEWALT, REALALT, THIICKNESS, OFFSETX, OFFSETY, BMONO, \
		MONOCOLOR, BVISIBLE, BOPAQUA, BMARKER, MARKERX, MARKERY, BSHOWMAP"));
		exportFile.getFile().writeLine();

	std::vector<int> vAirportID = exportFile.GetAirportIDList();

	for (int i=0; i < static_cast<int>(vAirportID.size());++i)
	{
		CString strSQL = _T("");
		strSQL.Format(_T("SELECT ID,APTID, LORDER, BARPLEVEL, NAME, VIEWALT, REALALT, THIICKNESS, OFFSETX, OFFSETY, BMONO, \
			MONOCOLOR, BVISIBLE, BOPAQUA, BMARKER, MARKERX, MARKERY, BSHOWMAP, \
			TSTAMP\
			FROM ASGROUND\
			WHERE APTID = %d"),vAirportID[i]);

			long nCount = 0L;
		CADORecordset adRes;
		CADODatabase::ExecuteSQLStatement(strSQL,nCount,adRes);

		while (!adRes.IsEOF())
		{

			CAirsideGround airsideGround;
			airsideGround.ReadData(adRes);

			airsideGround.ExportBaseInfo(exportFile,airsideGround.getID());
			airsideGround.ExportGridInfo(exportFile,airsideGround.getID());
			airsideGround.ExportCADInfo(exportFile,airsideGround.getID());
			airsideGround.ExportOverlayInfo(exportFile,airsideGround.getID());

			adRes.MoveNextData();
		}
	}
	//CAirsideGround airsideGround;
	//airsideGround.ExportBaseInfo(exportFile,nLevelID);
	//airsideGround.ExportGridInfo(exportFile,nLevelID);
	//airsideGround.ExportCADInfo(exportFile,nLevelID);

}
void CAirsideGround::ExportBaseInfo(CAirsideExportFile& exportFile,int nLevelID)
{
	//ReadData(nLevelID);

	exportFile.getFile().writeInt(nLevelID);
	exportFile.getFile().writeInt(m_nAirportID);
	exportFile.getFile().writeInt(m_nOrder);
	exportFile.getFile().writeInt(m_bMainLevel);


	exportFile.getFile().writeField(m_sName);

	exportFile.getFile().writeDouble(m_dVisualAltitude);
	exportFile.getFile().writeDouble(m_dRealAltitude);
	exportFile.getFile().writeDouble(m_dThickness);
	exportFile.getFile().writeDouble(m_vOffset[0]);
	exportFile.getFile().writeDouble(m_vOffset[1]);
	exportFile.getFile().writeInt(m_bIsMonochrome);

	exportFile.getFile().writeInt((long)m_cMonochromeColor);
	exportFile.getFile().writeInt(m_bIsVisible);
	exportFile.getFile().writeInt(m_bIsOpaque);
	exportFile.getFile().writeInt(m_bUseMarker);

	exportFile.getFile().writeDouble(m_vMarkerLoc[0]);
	exportFile.getFile().writeDouble(m_vMarkerLoc[1]);

	exportFile.getFile().writeInt(m_bIsShowMap);
	exportFile.getFile().writeLine();

}
void CAirsideGround::ExportGridInfo(CAirsideExportFile& exportFile,int nLevelID)
{
	ReadGridInfo(nLevelID);

	exportFile.getFile().writeInt(nLevelID);
	exportFile.getFile().writeInt( (long)m_Grid.cAxesColor );
	exportFile.getFile().writeInt( (long)m_Grid.cLinesColor );
	exportFile.getFile().writeInt( (long)m_Grid.cSubdivsColor );
	exportFile.getFile().writeFloat( (float)m_Grid.dSizeX );
	exportFile.getFile().writeFloat( (float)m_Grid.dSizeY );
	exportFile.getFile().writeFloat( (float)m_Grid.dLinesEvery );
	exportFile.getFile().writeInt( m_Grid.nSubdivs );
	exportFile.getFile().writeInt( m_Grid.bVisibility );
	exportFile.getFile().writeLine();
}
void CAirsideGround::ExportCADInfo(CAirsideExportFile& exportFile,int nLevelID)
{
	ReadCADInfo(nLevelID);
	exportFile.getFile().writeInt(nLevelID);
	exportFile.getFile().writeField( MapFileName() );
	exportFile.getFile().writeField( MapPathName() );
	ARCVector2 v2D = MapOffset();
	exportFile.getFile().writeFloat( (float)v2D[0] );
	exportFile.getFile().writeFloat( (float)v2D[1] );
	exportFile.getFile().writeFloat( (float)MapRotation() );
	exportFile.getFile().writeFloat( (float)MapScale() );
	exportFile.getFile().writeLine();

}

void CAirsideGround::ExportOverlayInfo(CAirsideExportFile& exportFile,int nLevelID)
{
	ReadOverlayInfo(nLevelID);

	exportFile.getFile().writeInt(nLevelID);
	exportFile.getFile().writeField( m_picInfo.sFileName );

	if(m_picInfo.refLine.getCount() <2)
	{
		exportFile.getFile().writeFloat(0.0);
		exportFile.getFile().writeFloat(0.0);
		exportFile.getFile().writeFloat(0.0);
		exportFile.getFile().writeFloat(0.0);
	}
	else
	{ 
		exportFile.getFile().writeFloat( (float)m_picInfo.refLine.getPoint(0).getX());
		exportFile.getFile().writeFloat( (float)m_picInfo.refLine.getPoint(0).getY());
		exportFile.getFile().writeFloat( (float)m_picInfo.refLine.getPoint(1).getX());
		exportFile.getFile().writeFloat( (float)m_picInfo.refLine.getPoint(1).getY());
	}



	exportFile.getFile().writeFloat( (float)m_picInfo.vOffset[0]);
	exportFile.getFile().writeFloat( (float)m_picInfo.vOffset[1]);
	exportFile.getFile().writeFloat( (float)m_picInfo.dRotation);
	exportFile.getFile().writeFloat( (float)m_picInfo.dScale);
	exportFile.getFile().writeInt( m_picInfo.bShow ? 1 : 0);


	exportFile.getFile().writeLine();

}

//import
void CAirsideGround::ImportAirsideGrounds(CAirsideImportFile& importFile)
{
	while (!importFile.getFile().isBlank())
	{
		CAirsideGround airsideGround;
		airsideGround.ImportAirsideGround(importFile);
	}

}	
void CAirsideGround::ImportAirsideGround(CAirsideImportFile& importFile)
{
	ImportBaseInfo(importFile);
	ImportGridInfo(importFile);
	ImportCADInfo(importFile);

	if(importFile.getVersion() > 1028)
		ImportOverlayInfo(importFile);
}
void CAirsideGround::ImportBaseInfo(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);
	int nOldAirportID = -1;
	importFile.getFile().getInteger(nOldAirportID);
	m_nAirportID = importFile.GetAirportNewIndex(nOldAirportID);

	importFile.getFile().getInteger(m_nOrder);
	importFile.getFile().getInteger(m_bMainLevel);

	char chname[512];
	importFile.getFile().getField(chname,512);
	m_sName = chname;

	importFile.getFile().getFloat(m_dVisualAltitude);
	importFile.getFile().getFloat(m_dRealAltitude);
	importFile.getFile().getFloat(m_dThickness);
	importFile.getFile().getFloat(m_vOffset[0]);
	importFile.getFile().getFloat(m_vOffset[1]);
	importFile.getFile().getInteger(m_bIsMonochrome);

	long lMonochromeColor = 0;
	importFile.getFile().getInteger(lMonochromeColor);
	m_cMonochromeColor =(unsigned long)lMonochromeColor;

	importFile.getFile().getInteger(m_bIsVisible);
	importFile.getFile().getInteger(m_bIsOpaque);
	importFile.getFile().getInteger(m_bUseMarker);

	importFile.getFile().getFloat(m_vMarkerLoc[0]);
	importFile.getFile().getFloat(m_vMarkerLoc[1]);

	importFile.getFile().getInteger(m_bIsShowMap);
	importFile.getFile().getLine();

	SaveData(m_nAirportID);

}
void CAirsideGround::ImportGridInfo(CAirsideImportFile& importFile)
{

	int nOldlevelID = -1;
	importFile.getFile().getInteger(nOldlevelID);
	//nNewAirportID = importFile.GetAirportNewIndex(nOldAirportID);

	long lVal;
	importFile.getFile().getInteger( lVal );
	m_Grid.cAxesColor = lVal;
	importFile.getFile().getInteger( lVal );
	m_Grid.cLinesColor = lVal;
	importFile.getFile().getInteger( lVal );
	m_Grid.cSubdivsColor = lVal;
	importFile.getFile().getFloat( m_Grid.dSizeX );
	importFile.getFile().getFloat( m_Grid.dSizeY );
	importFile.getFile().getFloat( m_Grid.dLinesEvery );
	int nVal;
	importFile.getFile().getInteger( nVal );
	m_Grid.nSubdivs = nVal;
	importFile.getFile().getInteger( nVal );
	m_Grid.bVisibility = nVal;

	importFile.getFile().getLine();
	SaveGridInfo(m_nID);
}
void CAirsideGround::ImportCADInfo(CAirsideImportFile& importFile)
{

	int nOldlevelID = -1;
	importFile.getFile().getInteger(nOldlevelID);


	char chFileName[512];
	importFile.getFile().getField(chFileName,512);
	m_CadFile.sFileName = chFileName;

	char chPathName[512];
	importFile.getFile().getField( chPathName,512 );
	m_CadFile.sPathName = chPathName;

	importFile.getFile().getFloat( m_CadFile.vOffset[0] );
	importFile.getFile().getFloat(m_CadFile.vOffset[1] );
	importFile.getFile().getFloat(m_CadFile.dRotation );
	importFile.getFile().getFloat(m_CadFile.dScale );
	importFile.getFile().getLine();

	SaveCADInfo(m_nID);

}

void CAirsideGround::ImportOverlayInfo(CAirsideImportFile& importFile)
{
	int nOldlevelID = -1;
	importFile.getFile().getInteger(nOldlevelID);


	char chFileName[512];
	importFile.getFile().getField(chFileName,512);
	m_picInfo.sFileName = chFileName;

	Point head, tail;
	double x, y, z=.0;
	importFile.getFile().getFloat( x );
	importFile.getFile().getFloat( y );
	head.setPoint( x, y, z);

	importFile.getFile().getFloat( x );
	importFile.getFile().getFloat( y );
	tail.setPoint( x, y, z);

	Point ptList[2];
	ptList[0] = head;
	ptList[1] = tail;
	m_picInfo.refLine.init( 2, ptList);

	importFile.getFile().getFloat( m_picInfo.vOffset[0] );
	importFile.getFile().getFloat(m_picInfo.vOffset[1] );
	importFile.getFile().getFloat(m_picInfo.dRotation );
	importFile.getFile().getFloat(m_picInfo.dScale );
	int nStamp = 0;
	importFile.getFile().getInteger(nStamp);
	m_picInfo.bShow = nStamp == 0 ? false : true;

	importFile.getFile().getLine();

	SaveOverlayInfo(m_nID);
}

int CAirsideGround::InsertDefaultLevel(int nAirportID, CString strName, int nOrder)
{
	m_sName = strName;
	SaveData(nAirportID, nOrder);
	SaveCADInfo(m_nID);
	SaveGridInfo(m_nID);
	SaveOverlayInfo(m_nID);
	return m_nID;
}
//////////////////////////////////////////////////////////////////////////
//Added by cjchen
//4/24/2008
//save floorlayer data to DB ,
// FOR  Task #142
//////////////////////////////////////////////////////////////////////////
void CAirsideGround::SaveFloorLayerToDB(int nLevelID)
{
   CFloorLayerList_Iter iter = m_vLayers.begin() ;
   try
   {
	   CADODatabase::BeginTransaction() ;
	   for ( ; iter != m_vLayers.end() ; ++iter)
	   {
		   CString SQL ;
		   SQL.Format(_T("INSERT INTO ASGROUND_FLOORLAYERS\
						 (LAYID, CCOLOR, BISON,SNAME,BISMONOCHROME)\
						 VALUES (%d,%d,%d,'%s',%d)") ,
						 nLevelID ,
						 (COLORREF)(*iter)->cColor,
						 (*iter)->bIsOn ,
						 (*iter)->sName ,
						 (*iter)->bIsMonochrome) ;
		   CADODatabase::ExecuteSQLStatement(SQL) ;
	   }
	   CADODatabase::CommitTransaction() ;
   }
   catch (CADOException& e)
   {
   	   e.ErrorMessage() ;
	   CADODatabase::RollBackTransation() ;
   }
   
}
void CAirsideGround::ReadFloorLayerFromDB(int nLevelID)
{
	CString SQL ;
	SQL.Format(_T("SELECT * FROM ASGROUND_FLOORLAYERS WHERE LAYID = %d") , nLevelID) ;
	CADORecordset adRes;
	long cout = 0 ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,cout ,adRes) ;
	}
	catch (CADOException& e)
	{
		e.ErrorMessage() ;
		return ;
	}
	while (!adRes.IsEOF())
	{
		CCADLayer*  layer = new CCADLayer ;
		double color = 0 ;
		adRes.GetFieldValue(_T("CCOLOR") ,color) ;
		layer->cColor = ARCColor3( static_cast<COLORREF>(color) );
		adRes.GetFieldValue(_T("BISON"),layer->bIsOn) ;
		adRes.GetFieldValue(_T("SNAME"),layer->sName) ;
		adRes.GetFieldValue(_T("bIsMonochrome"),layer->bIsMonochrome) ;
		m_vLayers.push_back(layer) ;
		adRes.MoveNextData();
	}
}
void CAirsideGround::DeleteFloorLayerFromDB(int nLevelID)
{
	CString SQL ;
	SQL.Format(_T("DELETE FROM ASGROUND_FLOORLAYERS  WHERE LAYID = %d") , nLevelID) ;
	try{
		CADODatabase::BeginTransaction() ;
		CADODatabase::ExecuteSQLStatement(SQL) ;
		CADODatabase::CommitTransaction() ;
	}catch(CADOException& e)
	{
		e.ErrorMessage() ;
		CADODatabase::RollBackTransation() ;
	}
}

//////////////////////////////////////////////////////////////////////////