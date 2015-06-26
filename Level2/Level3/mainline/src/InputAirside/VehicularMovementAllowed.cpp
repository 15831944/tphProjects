#include "StdAfx.h"
#include ".\vehicularmovementallowed.h"

CVehicularMovementAllowed::CVehicularMovementAllowed(int nProjID)
:m_nProjID(nProjID)
,m_bDisable(false)
{
	//Init();
}
CVehicularMovementAllowed::~CVehicularMovementAllowed( void )
{
	for (std::vector<CVehicularMovementAllowedTypeItem *>::iterator iter = m_vTypeData.begin();
		iter != m_vTypeData.end(); ++iter)
	{
		delete *iter;
	}
}


void CVehicularMovementAllowed::ReadData( int nParentID )
{
	CString strSelectSQL;
	GetSelectElementSQL(nParentID,strSelectSQL);
	strSelectSQL.Trim();
	if (strSelectSQL.IsEmpty())
		return;

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		CVehicularMovementAllowedTypeItemData element;
		element.InitFromDBRecordset(adoRecordset);

		AddData(element);
		//m_dataList.push_back(element);
		adoRecordset.MoveNextData();
	}

	ReadDisableSettings();
}
void CVehicularMovementAllowed::AddData( CVehicularMovementAllowedTypeItemData& pData )
{
	int nNameID =  pData.GetVehicleNameID();

	int nCount = static_cast<int>(m_vTypeData.size());
	for (int i = 0; i < nCount; ++i)
	{
		if(m_vTypeData[i]->getVehicleNameID() == nNameID)
		{
			m_vTypeData[i]->AddItem(pData);
		}
	}

}

void CVehicularMovementAllowed::SaveData( int nParentID )
{
	for (std::vector<CVehicularMovementAllowedTypeItem *>::iterator iter = m_vTypeData.begin();
		iter != m_vTypeData.end(); ++iter)
	{
		(*iter)->SaveData(nParentID);
	}

	SaveDisbaleSettings();
	//for (std::vector<CVehicularMovementAllowedTypeItemData >::iterator iter = m_vTypeData.begin();
	//	iter != m_vTypeData.end(); ++iter)
	//{
	//	(*iter)->DeleteData();
	//}

	//m_deleteddataList.clear();
}
//void CVehicularMovementAllowed::Init()
//{
//	m_vTypeData.clear();
//	
//	for (int i = 0; i < VehicleType_Count; ++ i)
//	{
//		CVehicularMovementAllowedTypeItem* typeItem = new CVehicularMovementAllowedTypeItem((enumVehicleBaseType)i);
//		m_vTypeData.push_back(typeItem);
//	}
//
//}
//////////////////////////////////////////////////////////////////////////
//CVehicularMovementAllowed
CVehicularMovementAllowedTypeItem::CVehicularMovementAllowedTypeItem( int nVehicleNameID)
{
	m_nVehicleNameID = nVehicleNameID;
}
CVehicularMovementAllowedTypeItem::~CVehicularMovementAllowedTypeItem(void)
{
}

void CVehicularMovementAllowedTypeItem::AddItem( CVehicularMovementAllowedTypeItemData& item )
{
	m_vData.push_back(item);
}

void CVehicularMovementAllowedTypeItem::DelItem( int nIndex )
{
	ASSERT(nIndex >= 0 && nIndex < static_cast<int>(m_vData.size()));

	m_vDeleteData.push_back(m_vData[nIndex]);
	m_vData.erase(m_vData.begin() + nIndex);
}

int CVehicularMovementAllowedTypeItem::getVehicleNameID()
{
	return m_nVehicleNameID;
}

void CVehicularMovementAllowedTypeItem::SaveData( int nParentID )
{
	for (std::vector<CVehicularMovementAllowedTypeItemData >::iterator iter = m_vData.begin();
		iter != m_vData.end(); ++iter)
	{
		(*iter).SaveData(nParentID);
	}

	for (std::vector<CVehicularMovementAllowedTypeItemData >::iterator iter = m_vDeleteData.begin();
		iter != m_vDeleteData.end(); ++iter)
	{
		(*iter).DeleteData();
	}

	m_vDeleteData.clear();
}

int CVehicularMovementAllowedTypeItem::GetItemCount()
{
	return static_cast<int >(m_vData.size());
}

CVehicularMovementAllowedTypeItemData& CVehicularMovementAllowedTypeItem::GetItem( int nIndex )
{
	ASSERT(nIndex >=0 && nIndex < static_cast<int >(m_vData.size()));
	return m_vData[nIndex];
}

int CVehicularMovementAllowedTypeItem::IsDataValid()
{
	int nCount =  static_cast<int >(m_vData.size());
	for(int nIndex =0; nIndex < nCount ; ++ nIndex)
	{
		if(m_vData[nIndex].IsDataValid() == false)
			return nIndex;
	}

	return -1;
}
void CVehicularMovementAllowed::GetSelectElementSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("SELECT  ID , PROJID , VEHICLETYPE , TAXIWAYID , NODEFROM , NODETO \
		FROM IN_VEHICULARMOVEMENTALLOWED\
		WHERE PROJID = %d"),nParentID);
	
}

int CVehicularMovementAllowed::GetItemCount()
{
	return static_cast<int>(m_vTypeData.size());
}

//CString CVehicularMovementAllowed::GetItemName(int nIndex)
//{
//	ASSERT(nIndex >= 0 && nIndex < static_cast<int>(m_vTypeData.size()));
//
//	int nNameID  = m_vTypeData[nIndex]->getVehicleNameID();
//	return VehicleDefaultName[nNameID];
//}

int CVehicularMovementAllowed::GetItemNameID( int nIndex )
{
	ASSERT(nIndex >= 0 && nIndex < static_cast<int>(m_vTypeData.size()));
	if (nIndex <0 || nIndex > static_cast<int>(m_vTypeData.size())-1)
		return -1;

	return m_vTypeData[nIndex]->getVehicleNameID();;
}

CVehicularMovementAllowedTypeItem* CVehicularMovementAllowed::GetItem( int nVehicleNameID )
{
	int nCount = static_cast<int>(m_vTypeData.size());
	for(int nIndex = 0; nIndex < nCount; ++nIndex)
	{
		int nNameID  = m_vTypeData[nIndex]->getVehicleNameID();
		if(nVehicleNameID == nNameID)
			return m_vTypeData[nIndex];
	}
	
	return NULL;
}

void CVehicularMovementAllowed::SetDisable( bool bDisable )
{
	m_bDisable = bDisable;
}

bool CVehicularMovementAllowed::GetDiable()
{
	return m_bDisable;
}

void CVehicularMovementAllowed::ReadDisableSettings()
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID	,PROJID	,BDISABLE FROM IN_VEHICULARMOVEMENTALLOWEDSETTINGS"));
	
	strSQL.Trim();
	if (strSQL.IsEmpty())
		return;

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);

	if (!adoRecordset.IsEOF())
	{
		int nDisable = 0;
		m_bDisable = false;
		adoRecordset.GetFieldValue(_T("BDISABLE"),nDisable);
		if(nDisable > 0)
			m_bDisable = true;

	}
	else
	{
		strSQL.Empty();

		strSQL.Format(_T("INSERT INTO IN_VEHICULARMOVEMENTALLOWEDSETTINGS\
			(PROJID,BDISABLE) VALUES(%d ,%d)"
			),m_nProjID,m_bDisable?1:0);

		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}

void CVehicularMovementAllowed::SaveDisbaleSettings()
{

	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_VEHICULARMOVEMENTALLOWEDSETTINGS	SET	BDISABLE = %d"),m_bDisable?1:0);

	CADODatabase::ExecuteSQLStatement(strSQL);

}
//////////////////////////////////////////////////////////////////////////
//CVehicularMovementAllowedTypeItemData
CVehicularMovementAllowedTypeItemData::CVehicularMovementAllowedTypeItemData()
{
	m_nVehicleNameID = -1;
	m_nTaxiwayID = -1;
	m_NodeFrom = -1;
	m_NodeTo = -1;
}
CVehicularMovementAllowedTypeItemData::CVehicularMovementAllowedTypeItemData(int nVehicleNameID)
{
	m_nVehicleNameID = nVehicleNameID;
	m_nTaxiwayID = -1;
	m_NodeFrom = -1;
	m_NodeTo = -1;
}

CVehicularMovementAllowedTypeItemData::~CVehicularMovementAllowedTypeItemData()
{

}
void CVehicularMovementAllowedTypeItemData::SetTaxiwayID( int nTaxiWayID )
{
	m_nTaxiwayID = nTaxiWayID;
}

int CVehicularMovementAllowedTypeItemData::GetTaxiwayID()
{
	return m_nTaxiwayID;
}

void CVehicularMovementAllowedTypeItemData::SetNodeFrom( int nNodeFrom )
{
	m_NodeFrom = nNodeFrom;
}

int CVehicularMovementAllowedTypeItemData::GetNodeFrom()
{
	return m_NodeFrom;
}

void CVehicularMovementAllowedTypeItemData::SetNodeTo( int nNodeTo )
{
	m_NodeTo = nNodeTo;
}

int CVehicularMovementAllowedTypeItemData::GetNodeTo()
{
	return m_NodeTo;
}
void CVehicularMovementAllowedTypeItemData::SetVehicleNameID( int nNameID )
{
	m_nVehicleNameID = nNameID;
}

int CVehicularMovementAllowedTypeItemData::GetVehicleNameID()
{
	return m_nVehicleNameID;
}
void CVehicularMovementAllowedTypeItemData::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	strSQL.Format(_T("INSERT INTO  IN_VEHICULARMOVEMENTALLOWED \
		( PROJID \
		, VEHICLETYPE \
		, TAXIWAYID \
		, NODEFROM \
		, NODETO )\
		VALUES\
		(%d,%d,%d,%d,%d)"),nParentID,m_nVehicleNameID,m_nTaxiwayID,m_NodeFrom,m_NodeTo);
}

void CVehicularMovementAllowedTypeItemData::InitFromDBRecordset( CADORecordset& adoRecordset )
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);

	int nType = 0;
	adoRecordset.GetFieldValue(_T("VEHICLETYPE"),m_nVehicleNameID);
	adoRecordset.GetFieldValue(_T("TAXIWAYID"),m_nTaxiwayID);
	adoRecordset.GetFieldValue(_T("NODEFROM"),m_NodeFrom);
	adoRecordset.GetFieldValue(_T("NODETO"),m_NodeTo);

}

void CVehicularMovementAllowedTypeItemData::GetUpdateSQL( CString& strSQL ) const
{
	strSQL.Format(_T("UPDATE  IN_VEHICULARMOVEMENTALLOWED \
		SET  TAXIWAYID  =%d\
		, NODEFROM  = %d\
		, NODETO  = %d\
		WHERE ID = %d"),m_nTaxiwayID,m_NodeFrom,m_NodeTo,m_nID);

}

void CVehicularMovementAllowedTypeItemData::GetDeleteSQL( CString& strSQL ) const
{
	strSQL.Format(_T("DELETE FROM IN_VEHICULARMOVEMENTALLOWED\
		WHERE ID = %d"),m_nID);
}

void CVehicularMovementAllowedTypeItemData::GetSelectSQL( int nID,CString& strSQL ) const
{
	strSQL.Format(_T(""));
}

bool CVehicularMovementAllowedTypeItemData::IsDataValid()
{
	if(m_nTaxiwayID >= 0 && m_NodeFrom >=0 && m_NodeTo >=0 && m_nVehicleNameID >= 0)
		return true;

	return false;


}
