#include "StdAfx.h"
#include ".\airsidepaxbusparkspot.h"
#include "ALTObjectDisplayProp.h"
#include "Common\SqlScriptMaker.h"
#include "Database\AirsideParkingSpotDBDefine.h"


AirsideParkSpot::AirsideParkSpot()
{
	m_bPushBack = FALSE;
	m_nInConstrainId=-1;
	m_nOutConstrainId = -1;
	m_bLocked = false;
}


void AirsideParkSpot::OnRotate( double  degree, const ARCVector3& point )
{
	DoOffset(-point[VX], -point[VY],0);
	Rotate(-degree);
	DoOffset(point[VX], point[VY],0);	
}

void AirsideParkSpot::DoOffset( DistanceUnit dx, DistanceUnit dy, DistanceUnit dz )
{
	m_servicePoint.DoOffset(dx,dy,dz);
	m_inContrain.DoOffset(dx,dy,dz);
	m_outConstrain.DoOffset(dx,dy,dz);
}

ALTObjectDisplayProp* AirsideParkSpot::NewDisplayProp()
{
	return new ParkSpotDisplayProp();
}

//////////////////////////////////////////////////////////////////////////

CString AirsidePaxBusParkSpot::GetTypeName() const
{
	return _T("PaxBus ParkingSpot");
}

ALTOBJECT_TYPE AirsidePaxBusParkSpot::GetType() const
{
	return ALT_APAXBUSSPOT;
}


ALTObject * AirsidePaxBusParkSpot::NewCopy()
{
	AirsidePaxBusParkSpot* newCopy = new AirsidePaxBusParkSpot();
	newCopy->CopyData(this);
	return newCopy;	
}

CString AirsidePaxBusParkSpot::getPropTable() const
{
	return TB_PAXBUSSPOT_PROP;
}

CString AirsidePaxBusParkSpot::getViewTable() const
{
	return TBVIEW_PAXBUSSPOT;
}

void AirsideParkSpot::ReadObject( int nObjID )
{
	m_nObjID = nObjID;
	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(GetSelectScript(nObjID),nRecordCount,adoRecordset);

	if (!adoRecordset.IsEOF())
	{	

		ALTObject::ReadObject(adoRecordset);
		double ptx,pty,ptz;
		adoRecordset.GetFieldValue(COL_LOCX,ptx);
		adoRecordset.GetFieldValue(COL_LOCY,pty);
		adoRecordset.GetFieldValue(COL_LOCZ,ptz);
		m_servicePoint.setPoint(ptx,pty,ptz);

		adoRecordset.GetFieldValue(COL_INCONSID,m_nInConstrainId);
		/*int nInCount = -1;
		adoRecordset.GetFieldValue(_T("INCONSCOUNT"),nInCount);
		adoRecordset.GetFieldValue(_T("INCONS"),nInCount,m_inContrain);*/

		adoRecordset.GetFieldValue(COL_OUTCONSTID,m_nOutConstrainId);
		/*int nOutCount = -1;
		adoRecordset.GetFieldValue(_T("OUTCONSCOUNT"),nOutCount);
		adoRecordset.GetFieldValue(_T("OUTCONS"),nOutCount,m_outConstrain);
*/

		adoRecordset.GetFieldValue(COL_USEPUSHBACK,m_bPushBack);	

	}

	CADODatabase::ReadPath2008FromDatabase(m_nInConstrainId,m_inContrain);
	CADODatabase::ReadPath2008FromDatabase(m_nOutConstrainId,m_outConstrain);
}
void AirsideParkSpot::InitFromDBRecordset( CADORecordset& adoRecordset )
{
		ALTObject::ReadObject(adoRecordset);
		double ptx,pty,ptz;
		adoRecordset.GetFieldValue(COL_LOCX,ptx);
		adoRecordset.GetFieldValue(COL_LOCY,pty);
		adoRecordset.GetFieldValue(COL_LOCZ,ptz);
		m_servicePoint.setPoint(ptx,pty,ptz);

		adoRecordset.GetFieldValue(COL_INCONSID,m_nInConstrainId);
		/*int nInCount = -1;
		adoRecordset.GetFieldValue(_T("INCONSCOUNT"),nInCount);
		adoRecordset.GetFieldValue(_T("INCONS"),nInCount,m_inContrain);*/

		adoRecordset.GetFieldValue(COL_OUTCONSTID,m_nOutConstrainId);
		/*int nOutCount = -1;
		adoRecordset.GetFieldValue(_T("OUTCONSCOUNT"),nOutCount);
		adoRecordset.GetFieldValue(_T("OUTCONS"),nOutCount,m_outConstrain);
*/

		adoRecordset.GetFieldValue(COL_USEPUSHBACK,m_bPushBack);	

		CADODatabase::ReadPath2008FromDatabase(m_nInConstrainId,m_inContrain);
		CADODatabase::ReadPath2008FromDatabase(m_nOutConstrainId,m_outConstrain);

}

int AirsideParkSpot::SaveObject( int nAirportID )
{
	int nObjID = ALTObject::SaveObject(nAirportID, GetType() );

	SaveInOutConstrain();
	CADODatabase::ExecuteSQLStatement(GetInsertScript(nObjID));

	return nObjID;
}

void AirsideParkSpot::UpdateObject( int nObjID )
{
	ALTObject::UpdateObject(nObjID);
	SaveInOutConstrain();
	CADODatabase::ExecuteSQLStatement(GetUpdateScript(nObjID));
}

void AirsideParkSpot::DeleteObject( int nObjID )
{
	ALTObject::DeleteObject(nObjID);

	CADODatabase::DeletePathFromDatabase(m_nInConstrainId);
	CADODatabase::DeletePathFromDatabase(m_nOutConstrainId);
	CADODatabase::ExecuteSQLStatement(GetDeleteScript(nObjID));
}

CString AirsideParkSpot::GetSelectScript( int nObjID ) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM %s WHERE ID = %d"),getViewTable().GetString(), nObjID);
	return strSQL;
}

CString AirsideParkSpot::GetInsertScript( int nObjID ) const
{
	SqlInsertScriptMaker script(getPropTable());
	script.AddColumn(COL_ID, nObjID);
	script.AddColumn(COL_LOCX, m_servicePoint.getX());
	script.AddColumn(COL_LOCY, m_servicePoint.getY());
	script.AddColumn(COL_LOCZ, m_servicePoint.getZ());
	script.AddColumn(COL_INCONSID, m_nInConstrainId);
	script.AddColumn(COL_OUTCONSTID, m_nOutConstrainId);
	script.AddColumn(COL_USEPUSHBACK, m_bPushBack);
	return script.GetScript();
}

CString AirsideParkSpot::GetUpdateScript( int nObjID ) const
{
	SqlUpdateScriptMaker script(getPropTable());
	script.AddColumn(COL_ID, nObjID);
	script.AddColumn(COL_LOCX, m_servicePoint.getX());
	script.AddColumn(COL_LOCY, m_servicePoint.getY());
	script.AddColumn(COL_LOCZ, m_servicePoint.getZ());
	script.AddColumn(COL_INCONSID, m_nInConstrainId);
	script.AddColumn(COL_OUTCONSTID, m_nOutConstrainId);
	script.AddColumn(COL_USEPUSHBACK, m_bPushBack);

	CString sCond;
	sCond.Format(_T("%s = %d"), COL_ID, nObjID);
	script.SetCondition(sCond);
	return script.GetScript();
}

CString AirsideParkSpot::GetDeleteScript( int nObjID ) const
{
	SqlDeleteScriptMaker script(getPropTable());
	CString sCond;
	sCond.Format(_T("%s = %d"), COL_ID, nObjID);
	script.SetCondition(sCond);	
	return script.GetScript();
}

void AirsideParkSpot::SaveInOutConstrain()
{
	if(m_nInConstrainId<0)
	{
		m_nInConstrainId = CADODatabase::SavePath2008IntoDatabase(m_inContrain);
	}
	else
	{
		CADODatabase::UpdatePath2008InDatabase(m_inContrain, m_nInConstrainId);
	}

	if(m_nOutConstrainId<0)
	{
		m_nOutConstrainId = CADODatabase::SavePath2008IntoDatabase(m_outConstrain);
	}
	else
	{
		CADODatabase::UpdatePath2008InDatabase(m_outConstrain, m_nOutConstrainId);
	}
	
		
}

bool AirsideParkSpot::CopyData( const ALTObject* pObj )
{
	if(pObj->GetType()==GetType())
	{
		const AirsideParkSpot* pOther =(const AirsideParkSpot*)pObj;
		m_servicePoint = pOther->m_servicePoint;
		m_bPushBack = pOther->m_bPushBack;
		m_inContrain = pOther->m_inContrain;
		m_outConstrain = pOther->m_outConstrain;
	}
	return __super::CopyData(pObj);
}

void AirsideParkSpot::Rotate( double degree )
{
	m_servicePoint.rotate(degree);
	m_inContrain.Rotate(degree);
	m_outConstrain.Rotate(degree);
}




