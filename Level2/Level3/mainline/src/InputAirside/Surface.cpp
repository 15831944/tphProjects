#include "StdAfx.h"
#include ".\surface.h"
#include "Topography.h"
#include "AirsideImportExportManager.h"
#include "ALTObjectDisplayProp.h"


Surface::Surface(void)
{
}

Surface::~Surface(void)
{
}
// NAME_L1, NAME_L2, NAME_L3, NAME_L4, PATHID, POINTCOUNT, PATH,TEXTURE, TSTAMP
void Surface::ReadObject(int nObjID)
{
	setID(nObjID);
	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(GetSelectScript(nObjID),nRecordCount,adoRecordset);

	if (!adoRecordset.IsEOF())
	{	

		ALTObject::ReadObject(adoRecordset);


		adoRecordset.GetFieldValue(_T("PATHID"),m_nPathID);
		int nPointCount = -1;
		adoRecordset.GetFieldValue(_T("POINTCOUNT"),nPointCount);
		adoRecordset.GetFieldValue(_T("PATH"),nPointCount,m_path);

		adoRecordset.GetFieldValue(_T("TEXTURE"),m_strTexture);
	}

}
int Surface::SaveObject(int nAirportID)
{
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_SURFACE);

	m_nPathID = CADODatabase::SavePath2008IntoDatabase(m_path);

	CString strSQL = GetInsertScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	return nObjID;
}
void Surface::UpdatePath()
{	
	CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);
}
void Surface::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	CString strUpdateScript = GetUpdateScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strUpdateScript);
}
void Surface::DeleteObject(int nObjID)
{
	ALTObject::DeleteObject(nObjID);

	CADODatabase::DeletePathFromDatabase(m_nPathID);

	CString strDeleteScript = GetDeleteScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strDeleteScript);
}
CString Surface::GetSelectScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT APTID,NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOCKED, PATHID, POINTCOUNT, PATH,TEXTURE, TSTAMP\
		FROM SURFACE_VIEW WHERE ID = %d"),nObjID);

	return strSQL;
}
CString Surface::GetInsertScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO SURFACEPROP \
		(OBJID, PATH, TEXTURE) \
		VALUES (%d,%d,'%s')"),nObjID,m_nPathID,m_strTexture.c_str());

	return strSQL;
}
CString Surface::GetUpdateScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE SURFACEPROP\
		SET TEXTURE ='%s'\
		WHERE OBJID = %d"),m_strTexture.c_str(),nObjID);

	return strSQL;
}
CString Surface::GetDeleteScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM SURFACEPROP \
		WHERE (OBJID = 1)"),nObjID);

	return strSQL;
}

ALTObject * Surface::NewCopy()
{
	Surface * reslt = new Surface;
	*reslt = *this;
	return reslt; 
}
void Surface::ExportSurfaces(CAirsideExportFile& exportFile)
{
	ALTObjectUIDList vSurfaceID;
	Topography::GetSurfaceList(exportFile.GetTopographyID(),vSurfaceID);
	for (int i =0; i < static_cast<int>(vSurfaceID.size()); ++i)
	{
		Surface surfaceObj;
		surfaceObj.ReadObject(vSurfaceID[i]);
		surfaceObj.ExportSurface(exportFile);
	}

}
void Surface::ExportSurface(CAirsideExportFile& exportFile)
{
	ALTObject::ExportObject(exportFile);
	exportFile.getFile().writeInt(m_nAptID);

	int nPathCount = m_path.getCount();	
	exportFile.getFile().writeInt(nPathCount);
	for (int i =0; i < nPathCount; ++i)
	{
		exportFile.getFile().writePoint2008(m_path.getPoint(i));
	}

	exportFile.getFile().writeField(m_strTexture.c_str());
	exportFile.getFile().writeLine();
	
	
	SurfaceDisplayProp dspProp;
	dspProp.ReadData(m_nObjID);
	dspProp.ExportDspProp(exportFile);


}
void Surface::ImportObject(CAirsideImportFile& importFile)
{
	ALTObject::ImportObject(importFile);
	int nOldTopoID = -1;
	importFile.getFile().getInteger(nOldTopoID);
	m_nAptID = importFile.getNewTopographyID();

	//path
	//m_path.a

	int nPointCount = 0;
	importFile.getFile().getInteger(nPointCount);
	m_path.init(nPointCount);
	CPoint2008 *pPoint =  m_path.getPointList();

	for (int i = 0; i < nPointCount; ++i)
	{
		importFile.getFile().getPoint2008(pPoint[i]);
	}

	char chTexture[512];
	importFile.getFile().getField(chTexture,512);
	m_strTexture = chTexture;

	importFile.getFile().getLine();

	int nOldObjectID = m_nObjID;
	int nNewObjID = SaveObject(m_nAptID);
	importFile.AddObjectIndexMap(nOldObjectID,nNewObjID);

	SurfaceDisplayProp dspProp;
	dspProp.ImportDspProp(importFile,m_nObjID);

}

const GUID& Surface::getTypeGUID()
{
	// {FF61F7CB-DF2E-42b4-B207-6F1018A05C85}
	static const GUID name = 
	{ 0xff61f7cb, 0xdf2e, 0x42b4, { 0xb2, 0x7, 0x6f, 0x10, 0x18, 0xa0, 0x5c, 0x85 } };

	return name;
}

ALTObjectDisplayProp* Surface::NewDisplayProp()
{
	return new SurfaceDisplayProp();
}
































