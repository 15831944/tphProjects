// Floor2.cpp: implementation of the CRenderFloor class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "RenderFloor.h"
#include "termfile.h"
#include "fileman.h"

//////////////////////////////////////////////////////////////////////////

static const char* FLRPREFIX[] = {
	"1ST",
	"2ND",
	"3RD",
};


//////////////////////////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRenderFloor::CRenderFloor(int nLevel)
	: m_nLevel(nLevel)
{
	m_sName = GetDefaultLevelName(nLevel);
	InitDefault();
}

CRenderFloor::CRenderFloor(int nLevel, const CString& sName)
	: m_sName(sName)
	, m_nLevel(nLevel)
{

	InitDefault();
}

CRenderFloor::~CRenderFloor()
{
}

void CRenderFloor::InitDefault()
{
	m_indexlinkToTerminalFloor = -1;
	m_bIsTexValid = FALSE;
	m_bIsVisible = TRUE;
	m_bIsActive = FALSE;
	m_bIsOpaque = FALSE;
	m_bIsShowMap = TRUE;

	m_dRealAltitude = m_dVisualAltitude = m_nLevel * 2000;

	m_vOffset = ARCVector2(0.0,0.0);

	m_Grid.bVisibility = TRUE;
	m_Grid.cAxesColor = RGB(0,0,0);
	m_Grid.cLinesColor = RGB(88,88,88);
	m_Grid.cSubdivsColor = RGB(140,140,140);
	m_Grid.dSizeX = 10000.0;
	m_Grid.dSizeY = 10000.0;
	m_Grid.dLinesEvery = 500.0;
	m_Grid.nSubdivs = 0;


	m_bUseVisibleRegions = FALSE;
	m_bVRDirtyFlag = TRUE;
	m_dThickness = 100.0;

	//picture info
	m_bPicTextureValid = FALSE;
	m_bPicLoaded = FALSE;
	m_picInfo.bShow = TRUE;
	m_picInfo.dRotation = 0;
	m_picInfo.sFileName = _T("");
	m_picInfo.sPathName = _T("");
	m_picInfo.dScale = 1;
	m_picInfo.vOffset = ARCVector2(0,0);
	m_picInfo.vSize = ARCVector2(0,0);

	Point ptList[2];
	ptList[0].setX(0.0);ptList[0].setY(0.0);ptList[0].setZ(0.0);
	ptList[1].setX(0.0);ptList[1].setY(0.0);ptList[1].setZ(0.0);

	m_picInfo.refLine.init(2, ptList);

}

void CPictureInfo::ImportFile(ArctermFile& _file)
{
	char szBuff[256] = {0};
	_file.getField(szBuff,255);
	sFileName = szBuff;
	_file.getField(szBuff,255);
	sPathName = szBuff;

	_file.getFloat(vSize[VX]);
	_file.getFloat(vSize[VY]);

	Point head, tail;
	double x, y, z=0.0;
	_file.getFloat(x);
	_file.getFloat(y);
	head.setPoint( x, y, z);

	_file.getFloat(x);
	_file.getFloat(y);
	tail.setPoint( x, y, z);

	Point ptList[2];
	ptList[0] = head;
	ptList[1] = tail;
	refLine.init( 2, ptList);

	_file.getFloat(vOffset[VX]);
	_file.getFloat(vOffset[VY]);

	_file.getFloat(dScale);
	_file.getFloat(dRotation);

	int nShow = 0;
	_file.getInteger(nShow);
	bShow = nShow ? true:false;
}

void CPictureInfo::ExportFile(ArctermFile& _file)
{
	_file.writeField("Airside Floor Picture");
	_file.writeLine();

	_file.writeField(sFileName);
	_file.writeField(sPathName);

	_file.writeFloat((float)vSize[VX]);
	_file.writeFloat((float)vSize[VY]);

	if (refLine.getCount() < 2)
	{
		_file.writeFloat(0.0);
		_file.writeFloat(0.0);
		_file.writeFloat(0.0);
		_file.writeFloat(0.0);
	}
	else
	{
		_file.writeFloat((float)refLine[0].getX());
		_file.writeFloat((float)refLine[0].getY());

		_file.writeFloat((float)refLine[1].getX());
		_file.writeFloat((float)refLine[1].getY());
	}

	_file.writeFloat((float)vOffset[VX]);
	_file.writeFloat((float)vOffset[VY]);

	_file.writeFloat((float)dScale);
	_file.writeFloat((float)dRotation);

	_file.writeInt(bShow?1:0);
	_file.writeLine();
}

DistanceUnit CRenderFloor::GetBoundRadius() const
{
	return sqrt( m_Grid.dSizeX * m_Grid.dSizeX + m_Grid.dSizeY * m_Grid.dSizeY );
}

void CRenderFloor::SetPictureFileName( const CString& filename )
{
	if( filename.CompareNoCase(m_picInfo.sFileName) == 0 )
	{
		return;	
	}

	m_picInfo.sFileName = filename;
	m_bPicTextureValid = FALSE;
	m_bPicLoaded = FALSE;
}

void CRenderFloor::SetPictureFilePath( const CString& filePath )
{
	if( filePath.CompareNoCase(m_picInfo.sPathName)==0 )
	{
		return ;
	}
	m_picInfo.sPathName = filePath;
	m_bPicTextureValid  = FALSE;
	m_bPicLoaded  = FALSE;
}


CString CRenderFloor::FindExtension(const CString& name)
{
	int len = name.GetLength();
	int i;
	for (i = len-1; i >= 0; i--){
		if (name[i] == '.'){
			return name.Mid(i+1);
		}
	}
	return CString(_T(""));
}

CString CRenderFloor::RemoveExtension(const CString& name)
{
	int len = name.GetLength();
	int i;
	for (i = len-1; i >= 0; i--){
		if (name[i] == '.'){
			return name.Mid(0,i);
		}
	}
	return name;
}

void CRenderFloor::ReadMarkerLocationFromDB(int cur_Envir_type ,int m_nLevel,ARCVector2& marker)
{
	CString SQL ;
	SQL.Format(_T("SELECT MARKER_X,MARKER_Y FROM ALIGHT_MARKER WHERE LAYERID = %d and ENVIRONTYPE = %d "),
		m_nLevel,
		cur_Envir_type) ;
	CADORecordset res_set ;
	long cout = 0 ;
	try{
		CADODatabase::ExecuteSQLStatement(SQL,cout,res_set);
	}catch(CADOException& e)
	{
		e.ErrorMessage() ;
		return ;
	}
	while (!res_set.IsEOF())
	{
		res_set.GetFieldValue(_T("MARKER_X"),marker[0]);
		res_set.GetFieldValue(_T("MARKER_Y"),marker[0]);
		res_set.MoveNextData() ;
	}
}
void CRenderFloor::DeleteMarkerLocationFromDB(int cur_Envir_type ,int m_nLevel)
{

	CString SQL ;
	SQL.Format(_T("DELETE ALIGHT_MARKER WHERE ENVIRONTYPE = %d and LAYERID = %d"),
		cur_Envir_type,
		m_nLevel) ;
	try{
		CADODatabase::BeginTransaction() ;
		CADODatabase::ExecuteSQLStatement(SQL) ;
		CADODatabase::CommitTransaction() ;
	}catch(CADOException& e)
	{
		e.ErrorMessage() ;
		CADODatabase::RollBackTransation() ;
		return ;
	}

}
void CRenderFloor::SaveMarkerLocationToDB(int cur_Envir_type , int m_nLevel,	 ARCVector2& marker)
{
	CString SQL ;
	CADORecordset res_set ;
	long cout = 0 ;
	int no_use = 0 ;
	int use = 1 ;
	SQL.Format(_T("SELECT * FROM ALIGHT_MARKER WHERE LAYERID = %d and ENVIRONTYPE = %d "),
		m_nLevel,
		cur_Envir_type
		) ;

	try{
		CADODatabase::BeginTransaction() ;
		CADODatabase::ExecuteSQLStatement(SQL,cout,res_set) ;
		if (res_set.IsEOF())
		{
			SQL.Format(_T("INSERT INTO ALIGHT_MARKER\
							(LAYERID ,ENVIRONTYPE ,MARKER_ISUSE ,ALOGNLINE_ISUSE ,MARKER_X ,MARKER_Y ,ALIGNLINE_BEGX,ALIGNLINE_BEGY,ALIGNLINE_ENDX,ALIGNLINE_ENDY)\
							VALUES(%d,%d,%d,%d,%f,%f,%f,%f,%f,%f)"),
							m_nLevel,
							cur_Envir_type,
							use,
							no_use,
							marker[0],
							marker[1],
							0,
							0,
							0,
							0);
		}else
		{
			SQL.Format(_T("UPDATE ALIGHT_MARKER SET MARKER_X = %f , MARKER_Y = %f ,MARKER_ISUSE = %d \
							WHERE LAYERID = %d and ENVIRONTYPE = %d "),
							marker[0] ,
							marker[1] ,
							1 ,
							m_nLevel,
							cur_Envir_type) ;
		}
		CADODatabase::ExecuteSQLStatement(SQL) ;
		CADODatabase::CommitTransaction() ;
	}catch(CADOException& e)
	{
		e.ErrorMessage() ;
		CADODatabase::RollBackTransation() ;
		return ;
	}

}
BOOL CRenderFloor::readMarketStateFromDB(int sys_mode, int m_nLevel,BOOL& state)
{
	CString SQL ;
	SQL.Format(_T("SELECT MARKER_ISUSE FROM ALIGHT_MARKER WHERE LAYERID = %d and ENVIRONTYPE = %d "),
		m_nLevel,
		sys_mode) ;
	CADORecordset res_set ;
	long cout = 0 ;
	try{
		CADODatabase::ExecuteSQLStatement(SQL,cout,res_set);
	}catch(CADOException& e)
	{
		e.ErrorMessage() ;
		state = FALSE;
	}
	if(cout == 0 )
		return FALSE;
	while (!res_set.IsEOF())
	{
		res_set.GetFieldValue(_T("MARKER_ISUSE"),state);
		res_set.MoveNextData() ;
	}
	return state ;
}
void CRenderFloor::SaveMarketStateToDB(int sys_mode, int m_nLevel,BOOL state)
{

	CString SQL ;
	SQL.Format(_T("UPDATE ALIGHT_MARKER SET MARKER_ISUSE = %d  WHERE LAYERID = %d and ENVIRONTYPE = %d "),
		state,
		m_nLevel,
		sys_mode) ;
	try{
		CADODatabase::BeginTransaction();
		CADODatabase::ExecuteSQLStatement(SQL);
		CADODatabase::CommitTransaction();
	}catch(CADOException& e)
	{
		e.ErrorMessage() ;
		CADODatabase::RollBackTransation() ;
		return ;
	}

}
void CRenderFloor::ReadAlignLineFromDB(int sys_mode, int m_nLevel,CAlignLine& line) 
{
	CString SQL ;
	ARCVector2 beg ;
	ARCVector2 end ; 
	SQL.Format(_T("SELECT ALIGNLINE_BEGX ,ALIGNLINE_BEGY,ALIGNLINE_ENDX,ALIGNLINE_ENDY FROM ALIGHT_MARKER WHERE LAYERID = %d and ENVIRONTYPE = %d "),
		m_nLevel,
		sys_mode) ;
	CADORecordset res_set ;
	long cout = 0 ;
	try{
		CADODatabase::ExecuteSQLStatement(SQL,cout,res_set);
	}catch(CADOException& e)
	{
		e.ErrorMessage() ;
		return ;
	}
	while (!res_set.IsEOF())
	{
		res_set.GetFieldValue(_T("ALIGNLINE_BEGX"),beg[0]);
		res_set.GetFieldValue(_T("ALIGNLINE_BEGX"),beg[1]);
		res_set.GetFieldValue(_T("ALIGNLINE_ENDX"),end[0]);
		res_set.GetFieldValue(_T("ALIGNLINE_ENDY"),end[1]);
		res_set.MoveNextData() ;
		line.setBeginPoint(beg) ;
		line.setEndPoint(end) ;
	}
}
void CRenderFloor::SaveAlignLineFromDB(int sys_mode, int m_nLevel, CAlignLine& line) 
{
	CString SQL ;
	CADORecordset res_set ;
	long cout = 0 ;
	int no_use = 0 ;
	int use = 1 ;

	SQL.Format(_T("SELECT * FROM ALIGHT_MARKER WHERE LAYERID = %d and ENVIRONTYPE = %d "),
		m_nLevel,
		sys_mode
		) ;
	try{
		CADODatabase::BeginTransaction() ;
		CADODatabase::ExecuteSQLStatement(SQL,cout ,res_set) ;
		if (res_set.IsEOF())
		{

			SQL.Format(_T("INSERT INTO ALIGHT_MARKER\
							(LAYERID ,ENVIRONTYPE ,MARKER_ISUSE,ALOGNLINE_ISUSE,ALIGNLINE_BEGX ,ALIGNLINE_BEGY ,ALIGNLINE_ENDX,ALIGNLINE_ENDY,MARKER_X,MARKER_Y)\
							VALUES(%d,%d,%d,%d ,%f,%f,%f,%f,%f,%f)"),
							m_nLevel,
							sys_mode,
							no_use,
							use ,
							line.getBeginPoint()[0],
							line.getBeginPoint()[1] ,
							line.getEndPoint()[0],
							line.getEndPoint()[1],
							0,
							0);
		}else
		{
			SQL.Format(_T("UPDATE ALIGHT_MARKER SET ALIGNLINE_BEGX = %f , ALIGNLINE_BEGY = %f,\
							ALIGNLINE_ENDX = %f ,ALIGNLINE_ENDY = %f ,ALOGNLINE_ISUSE = %d WHERE LAYERID = %d and ENVIRONTYPE = %d "),
							(line.getBeginPoint())[0],
							(line.getBeginPoint())[1] ,
							(line.getEndPoint())[0],
							(line.getEndPoint())[1],
							1 ,
							m_nLevel,
							sys_mode) ;
		}
		CADODatabase::ExecuteSQLStatement(SQL) ;
		CADODatabase::CommitTransaction() ;
	}catch(CADOException& e)
	{
		e.ErrorMessage() ;
		CADODatabase::RollBackTransation() ;
		return ;
	}

}
void CRenderFloor::DeleteAlignLineFromDB(int cur_Envir_type ,int m_nLevel,int index) 
{
	DeleteMarkerLocationFromDB(cur_Envir_type,m_nLevel) ;
}
void CRenderFloor::SaveAlignLineStateToDB(int sys_mode, int m_nLevel,BOOL state)
{
	CString SQL ;
	SQL.Format(_T("UPDATE ALIGHT_MARKER SET ALOGNLINE_ISUSE = %d  WHERE LAYERID = %d and ENVIRONTYPE = %d "),
		state,
		m_nLevel,
		sys_mode) ;

	try{
		CADODatabase::BeginTransaction() ;
		CADODatabase::ExecuteSQLStatement(SQL);
		CADODatabase::CommitTransaction();
	}catch(CADOException& e)
	{
		e.ErrorMessage() ;
		CADODatabase::RollBackTransation();
		return ;
	}

}
BOOL CRenderFloor::ReadAlignLineStateFromDB(int sys_mode, int m_nLevel,BOOL& state)
{
	CString SQL ;
	SQL.Format(_T("SELECT ALOGNLINE_ISUSE FROM ALIGHT_MARKER WHERE LAYERID = %d and ENVIRONTYPE = %d "),
		m_nLevel,
		sys_mode) ;
	CADORecordset res_set ;
	long cout = 0 ;
	try{
		CADODatabase::ExecuteSQLStatement(SQL,cout,res_set);
	}catch(CADOException& e)
	{
		e.ErrorMessage() ;
		return  FALSE ;
	}
	if(cout == 0 )
		state = FALSE ;
	while (!res_set.IsEOF())
	{
		res_set.GetFieldValue(_T("ALOGNLINE_ISUSE"),state);
		res_set.MoveNextData() ;

	}
	return state ;

}

void CRenderFloor::SetMapFileName( const CString& sFileName )
{
	CBaseFloor::MapFileName(sFileName);
	if(!m_bIsCADLoaded)
		m_bIsTexValid = FALSE;
}

void CRenderFloor::SetMapPathName( const CString& sPathName )
{
	CBaseFloor::MapPathName(sPathName);
	if(!m_bIsCADLoaded)
		m_bIsTexValid = FALSE;
}

CString CRenderFloor::GetDefaultLevelName( int nLevel, CString prefix /*= "Floor"*/ )
{
	CString sName;
	if(nLevel < 3)
		sName.Format("%s %s", FLRPREFIX[nLevel],prefix);
	else
		sName.Format("%dTH %s", nLevel+1,prefix );
	return sName;
}


