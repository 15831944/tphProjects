#include "StdAfx.h"
#include ".\deicepad.h"
#include "ALTAirport.h"
#include "AirsideImportExportManager.h"
#include "Taxiway.h"
#include "../Common/DistanceLineLine.h"
#include "ALTObjectDisplayProp.h"

DeicePad::DeicePad(void)
{
	m_bUsePushback = false;
	m_bAsStand = false;
	m_dWinSpan = 10000.0;
	m_dLength =10000.0;

	m_nInconsID = -1;
	m_nOutconsID = -1;
}

DeicePad::~DeicePad(void)
{
}

const CPoint2008& DeicePad::GetServicePoint() const
{
	return m_servicePoint;
}

const CPath2008& DeicePad::GetInContrain() const
{
	return m_inContrain;
}

const CPath2008& DeicePad::GetOutConstrain() const
{
	return m_outConstrain;
}

void DeicePad::SetServicePoint( const CPoint2008& _pt )
{
	m_servicePoint = _pt;
}

void DeicePad::SetInContrain( const CPath2008& _path )
{
	m_inContrain = _path;
}

void DeicePad::SetOutConstrain( const CPath2008& _path )
{
	m_outConstrain = _path;
}

const double DeicePad::GetWinSpan() const
{
	return m_dWinSpan;
}

const double DeicePad::GetLength() const
{
	return m_dLength;
}

void DeicePad::SetWinspan( const double & _winspan )
{
	m_dWinSpan = _winspan;
}

bool DeicePad::IsUsePushBack() const
{
	return m_bUsePushback;
}

bool DeicePad::UsePushBack( bool _b )
{
	return m_bUsePushback = _b;
}

void DeicePad::SetLength( const double & _length )
{
	m_dLength = _length;
}
//NAME_L1, NAME_L2, NAME_L3, NAME_L4, USEPB, WINGSPAN, LENGTH, \
//SVCLOCTID, INCONSCOUNT, INCONS, INCONSID, OUTCONS, OUTCONSCOUNT, \
//OUTCONSID, SVCLOCT, SVCOUNT
void DeicePad::ReadObject(int nObjID)
{
	m_nObjID = nObjID;
	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(GetSelectScript(nObjID),nRecordCount,adoRecordset);

	if (!adoRecordset.IsEOF())
	{	

		ALTObject::ReadObject(adoRecordset);


		double ptx,pty,ptz;
		adoRecordset.GetFieldValue(_T("LCTX"),ptx);

		adoRecordset.GetFieldValue(_T("LCTY"),pty);

		adoRecordset.GetFieldValue(_T("LCTZ"),ptz);
		m_servicePoint.setPoint(ptx,pty,ptz);

		adoRecordset.GetFieldValue(_T("INCONSID"),m_nInconsID);
		int nInCount = -1;
		adoRecordset.GetFieldValue(_T("INCONSCOUNT"),nInCount);
		adoRecordset.GetFieldValue(_T("INCONS"),nInCount,m_inContrain);

		adoRecordset.GetFieldValue(_T("OUTCONSID"),m_nOutconsID);
		int nOutCount = -1;
		adoRecordset.GetFieldValue(_T("OUTCONSCOUNT"),nOutCount);
		adoRecordset.GetFieldValue(_T("OUTCONS"),nOutCount,m_outConstrain);


		adoRecordset.GetFieldValue(_T("USEPB"),m_bUsePushback);

		adoRecordset.GetFieldValue(_T("WINGSPAN"),m_dWinSpan);
		adoRecordset.GetFieldValue(_T("LENGTH"),m_dLength);
		int asstand = 0 ;
		adoRecordset.GetFieldValue(_T("ASSTAND"),asstand); 
		m_bAsStand = asstand == 1 ? TRUE:FALSE ;

	}
	
}
int DeicePad::SaveObject(int nAirportID)
{
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_DEICEBAY);
	
	if(m_inContrain.getCount())
		m_nInconsID = CADODatabase::SavePath2008IntoDatabase(m_inContrain);
	if(m_outConstrain.getCount())
		m_nOutconsID = CADODatabase::SavePath2008IntoDatabase(m_outConstrain);

	CADODatabase::ExecuteSQLStatement(GetInsertScript(nObjID));

	return nObjID;
}
void DeicePad::UpdateInConstraint()
{
	if (m_nInconsID == -1)
		m_nInconsID = CADODatabase::SavePath2008IntoDatabase(m_inContrain);
	else
		CADODatabase::UpdatePath2008InDatabase(m_inContrain, m_nInconsID);
}
void DeicePad::UpdateOutConstraint()
{
	if (m_nOutconsID == -1)
		m_nOutconsID = CADODatabase::SavePath2008IntoDatabase(m_outConstrain);
	else
		CADODatabase::UpdatePath2008InDatabase(m_outConstrain,m_nOutconsID);
}
void DeicePad::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	CADODatabase::ExecuteSQLStatement(GetUpdateScript(nObjID));
}
void DeicePad::DeleteObject(int nObjID)
{	
	ALTObject::DeleteObject(nObjID);

	CADODatabase::DeletePathFromDatabase(m_nInconsID);
	CADODatabase::DeletePathFromDatabase(m_nOutconsID);

	CADODatabase::ExecuteSQLStatement(GetDeleteScript(nObjID));

}
CString DeicePad::GetSelectScript(int nObjID) const
{
	CString strSQL = _T("");

	strSQL.Format(_T("SELECT APTID, NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOCKED, LCTX, LCTY, LCTZ, USEPB, \
		WINGSPAN, LENGTH, INCONSID, INCONSCOUNT, INCONS, OUTCONSID, OUTCONS, \
		OUTCONSCOUNT,ASSTAND\
		FROM DEICEPAD_VIEW\
		WHERE ID = %d"),nObjID);

	return strSQL;

}
CString DeicePad::GetInsertScript(int nObjID) const
{
	CString strSQL = _T("");

	strSQL.Format(_T("INSERT INTO DEICEPADPROP \
		(OBJID, LCTX, LCTY, LCTZ, INCONS, OUTCONS, USEPB, WINGSPAN, LENGTH,ASSTAND) \
		VALUES (%d,%f,%f,%f,%d,%d,%d,%f,%f,%d)"),
		nObjID,
		m_servicePoint.getX(),m_servicePoint.getY(),m_servicePoint.getZ() ,
		m_nInconsID,m_nOutconsID,m_bUsePushback?1:0,m_dWinSpan,m_dLength, m_bAsStand?1:0);

	return strSQL;

}
CString DeicePad::GetUpdateScript(int nObjID) const
{
	CString strSQL = _T("");

	strSQL.Format(_T("UPDATE DEICEPADPROP\
		SET LCTX =%f, LCTY =%f, LCTZ =%f,USEPB =%d, WINGSPAN =%f, LENGTH =%f,ASSTAND =%d\
		WHERE OBJID = %d"),m_servicePoint.getX(),m_servicePoint.getY(),m_servicePoint.getZ() ,
		m_bUsePushback?1:0,m_dWinSpan,m_dLength,m_bAsStand?1:0, nObjID);

	return strSQL;


}
CString DeicePad::GetDeleteScript(int nObjID) const
{
	CString strSQL = _T("");

	strSQL.Format(_T("DELETE FROM DEICEPADPROP \
		WHERE OBJID = %d"),
		nObjID);

	return strSQL;
}

ALTObject * DeicePad::NewCopy()
{
	DeicePad * reslt = new DeicePad;
	reslt->CopyData(this);
	return reslt;
}

ARCVector2 DeicePad::GetDir() const
{
	int nCont;ARCVector2 vdir;
	if( ( nCont = m_inContrain.getCount() ) >  0 ){
		vdir  = ARCVector2( m_servicePoint.getX(),m_servicePoint.getY() );
		vdir -= ARCVector2(m_inContrain.getPoint(nCont-1) );		
	}
	
	return vdir.Normalize();
}

void DeicePad::ExportDeicePads(int nAirportID,CAirsideExportFile& exportFile)
{
	std::vector<int> vDeicepadID;
	ALTAirport::GetDeicePadsIDs(nAirportID,vDeicepadID);
	std::vector<int>::iterator iter = vDeicepadID.begin();
	for (;iter!= vDeicepadID.end(); ++ iter)
	{
		DeicePad deicpadObj;
		deicpadObj.ReadObject(*iter);
		deicpadObj.ExportDeicePad(exportFile);
	}
}
void DeicePad::ExportDeicePad(CAirsideExportFile& exportFile)
{
	ALTObject::ExportObject(exportFile);
	//airport id
	exportFile.getFile().writeInt(m_nAptID);
	//service location
	exportFile.getFile().writePoint2008(m_servicePoint);
	//in constrain
	int nInConsCount = m_inContrain.getCount();
	exportFile.getFile().writeInt(nInConsCount);
	for (int i =0; i < nInConsCount; ++i)
	{
		exportFile.getFile().writePoint2008(m_inContrain.getPoint(i));
	}
	//out constrain
	int nOutConstrainCount = m_outConstrain.getCount();
	exportFile.getFile().writeInt(nOutConstrainCount);
	for (int i =0; i < nOutConstrainCount; ++ i)
	{
		exportFile.getFile().writePoint2008(m_outConstrain.getPoint(i));
	}
	//use push back
	exportFile.getFile().writeInt(m_bUsePushback?1:0);
	
	//wingspan
	exportFile.getFile().writeDouble(m_dWinSpan);
	//length
	exportFile.getFile().writeDouble(m_dLength);




	exportFile.getFile().writeLine();

	DeicePadDisplayProp dspProp;
	dspProp.ReadData(m_nObjID);
	dspProp.ExportDspProp(exportFile);


}
void DeicePad::ImportObject(CAirsideImportFile& importFile)
{

	ALTObject::ImportObject(importFile);

	int nAirportID = -1;
	importFile.getFile().getInteger(nAirportID);
	m_nAptID = importFile.GetAirportNewIndex(nAirportID);
	//service location
	importFile.getFile().getPoint2008(m_servicePoint);	
	//in constrain
	int nInconsCount = 0;
	importFile.getFile().getInteger(nInconsCount);
	m_inContrain.init(nInconsCount);

	CPoint2008* pPointIncons = m_inContrain.getPointList();
	for (int i =0; i < nInconsCount; ++i)
	{
		importFile.getFile().getPoint2008(pPointIncons[i]);
	}

	//out cons
	int nOutConsCount = 0;
	importFile.getFile().getInteger(nOutConsCount);
	m_outConstrain.init(nOutConsCount);
	CPoint2008* pPointOutCons = m_outConstrain.getPointList();
	for (int i =0; i < nOutConsCount; ++i)
	{
		importFile.getFile().getPoint2008(pPointOutCons[i]);
	}
	int nUsePushBack = 0;
	importFile.getFile().getInteger(nUsePushBack);
	m_bUsePushback = (nUsePushBack == 0)?true:false ;

	//wingspan
	importFile.getFile().getFloat(m_dWinSpan);
	
	//length
	importFile.getFile().getFloat(m_dLength);

	importFile.getFile().getLine();


	int nOldObjectID = m_nObjID;
	int nNewObjID = SaveObject(m_nAptID);
	importFile.AddObjectIndexMap(nOldObjectID,nNewObjID);


	DeicePadDisplayProp dspProp;
	dspProp.ImportDspProp(importFile,m_nObjID);
}

bool DeicePad::CopyData( const ALTObject* pObj )
{
	if(this==pObj)
		return true;

	if( pObj->GetType() == GetType() )
	{
		DeicePad * pOtherDeice = (DeicePad*)pObj;
		m_servicePoint = pOtherDeice->GetServicePoint();
		m_inContrain = pOtherDeice->GetInContrain();
		m_outConstrain = pOtherDeice->GetOutConstrain();
		m_bUsePushback = pOtherDeice->IsUsePushBack();
		m_dWinSpan = pOtherDeice->GetWinSpan();
		m_dLength = pOtherDeice->GetLength();		
	}
	return __super::CopyData(pObj);
}

void DeicePad::SetAsStand(bool bAsStand)
{
	m_bAsStand = bAsStand;
}

bool DeicePad::IsAsStand()
{
	return m_bAsStand;
}

const GUID& DeicePad::getTypeGUID()
{
	// {89FA9E6B-60F0-408a-9D02-2922F65C7358}
	static const GUID name = 
	{ 0x89fa9e6b, 0x60f0, 0x408a, { 0x9d, 0x2, 0x29, 0x22, 0xf6, 0x5c, 0x73, 0x58 } };

	return name;
}

ALTObjectDisplayProp* DeicePad::NewDisplayProp()
{
	return new DeicePadDisplayProp();
}
