#include "StdAfx.h"
#include "GroundRoute.h"

#include <algorithm>
#include "../InputAirside/AirsideImportExportManager.h"
#include "ALTAirport.h"
#include "HoldPosition.h"
#include "runway.h"
#include "taxiway.h"

GroundRouteItem::GroundRouteItem()
 : m_nID(-1)
 , m_nParentID(-1)
 , m_nGroundID(-1)
 , m_nALTObjID(-1)
 , m_nMarking(-1)
{
	m_ChildItem.clear();
	m_DelChildItem.clear();
}

GroundRouteItem::~GroundRouteItem()
{
	GroundRouteItemIter iter = m_ChildItem.begin();

	for (; iter!=m_ChildItem.end(); iter++)
	{
		delete *iter;
	}
	m_ChildItem.clear();

	iter = m_DelChildItem.begin();
	for (; iter!=m_DelChildItem.end(); iter++)
	{
		delete *iter;
	}
	m_DelChildItem.clear();
}

void GroundRouteItem::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);

	adoRecordset.GetFieldValue(_T("PARENTID"),m_nParentID);

	adoRecordset.GetFieldValue(_T("GROUNDROUTEID"),m_nGroundID);

	adoRecordset.GetFieldValue(_T("RUNWAYMARKING"),m_nMarking);

	adoRecordset.GetFieldValue(_T("OBJID"),m_nALTObjID);

	CString strSQL;
	strSQL.Format(_T("SELECT * \
					 FROM GROUNDROUTEDATA\
					 WHERE (GROUNDROUTEID = %d AND PARENTID = %d)"), m_nGroundID, m_nID);

	CADORecordset itemAdoRecordSet;
	long lItemAffect = 0L;

	CADODatabase::ExecuteSQLStatement(strSQL,lItemAffect,itemAdoRecordSet);
	while (!itemAdoRecordSet.IsEOF())
	{
		GroundRouteItem *pChildItem = new GroundRouteItem;
		pChildItem->ReadData(itemAdoRecordSet);

		m_ChildItem.push_back(pChildItem);

		itemAdoRecordSet.MoveNextData();
	}
}

void GroundRouteItem::SaveData()
{
	if (m_nID != -1)
	{
		return UpdateData();
	}

	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO GROUNDROUTEDATA\
		(GROUNDROUTEID, PARENTID, OBJID, RUNWAYMARKING)\
		VALUES (%d,%d,%d,%d)"),m_nGroundID,m_nParentID,m_nALTObjID, m_nMarking);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	GroundRouteItemIter iter = m_ChildItem.begin();
	for (; iter!=m_ChildItem.end(); iter++)
	{
		(*iter)->SetGroundRouteID(m_nGroundID);
		(*iter)->SetParentID(m_nID);
		(*iter)->SaveData();
	}

	m_DelChildItem.clear();
}

void GroundRouteItem::UpdateData()
{	
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE GROUNDROUTEDATA\
		SET GROUNDROUTEID =%d, PARENTID =%d, OBJID =%d ,RUNWAYMARKING=%d\
		WHERE (ID = %d)"),m_nGroundID,m_nParentID,m_nALTObjID,m_nMarking, m_nID );

	CADODatabase::ExecuteSQLStatement(strSQL);

	GroundRouteItemIter iter = m_ChildItem.begin();
	for (; iter!=m_ChildItem.end(); iter++)
	{
		(*iter)->SetGroundRouteID(m_nGroundID);
		(*iter)->SetParentID(m_nID);
		(*iter)->SaveData();
	}

	for (iter=m_DelChildItem.begin(); iter!=m_DelChildItem.end(); iter++)
	{
		(*iter)->DeleteData();
	}
	m_DelChildItem.clear();
}

void GroundRouteItem::DeleteData()
{
	if (m_nID == -1)
		return;

	ASSERT(m_ChildItem.size()==0);

	GroundRouteItemIter iter = m_ChildItem.begin();
	for (iter=m_DelChildItem.begin(); iter!=m_DelChildItem.end(); iter++)
	{
		(*iter)->DeleteData();
	}
	m_DelChildItem.clear();

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM GROUNDROUTEDATA\
		WHERE (ID = %d)"),m_nID );

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void GroundRouteItem::AddChildItem(GroundRouteItem* pGroundRouteItem)
{
	ASSERT(pGroundRouteItem);

	m_ChildItem.push_back(pGroundRouteItem);
}

GroundRouteItemVector GroundRouteItem::GetChildItems()
{
	return m_ChildItem;
}

GroundRouteItemVector GroundRouteItem::GetDelChildItems()
{
	return m_DelChildItem;
}

void GroundRouteItem::DeleteChildItem(GroundRouteItem* pGroundRouteItem)
{
	GroundRouteItemIter iter = m_ChildItem.begin();
	for (; iter!=m_ChildItem.end(); iter++)
	{
		if ((*iter) == pGroundRouteItem)
		{
			(*iter)->DeleteAllChild();
			m_DelChildItem.push_back(*iter);
			m_ChildItem.erase(iter);			

			return;
		}
	}
}

void GroundRouteItem::DeleteAllChild()
{
	GroundRouteItemIter iter = m_ChildItem.begin();
	for (; iter!=m_ChildItem.end(); iter++)
	{
		(*iter)->DeleteAllChild();
		m_DelChildItem.push_back(*iter);
	}

	m_ChildItem.clear();
}

GroundRouteItemVector GroundRouteItem::GetTailItems()
{
	GroundRouteItemVector GRTailItems;

	if (m_ChildItem.size() == 0)
	{
		GRTailItems.push_back(this);
	}

	GroundRouteItemIter iter = m_ChildItem.begin();
	for (; iter!=m_ChildItem.end(); iter++)
	{
		GroundRouteItemVector GRChildTailItems = (*iter)->GetTailItems();

		GroundRouteItemIter childIter = GRChildTailItems.begin();
		for (; childIter!=GRChildTailItems.end(); childIter++)
		{
			GRTailItems.push_back(*childIter);
		}		
	}

	return GRTailItems;
}

void GroundRouteItem::VerifyData()
{
	GroundRouteItemIter iter = m_ChildItem.begin();
	for (; iter!=m_ChildItem.end(); iter++)
	{
		GroundRouteItem *pGroundRouteItem = *iter;
		int nAltObjID = pGroundRouteItem->GetALTObjectID();
		ALTObject altObj;
		ALTObject *pALTObj = altObj.ReadObjectByID(nAltObjID);

		if (NULL == pALTObj)
		{
			pGroundRouteItem->DeleteAllChild();
			m_DelChildItem.push_back(pGroundRouteItem);
			iter = m_ChildItem.erase(iter);
			iter--;
		}
		else
		{
			pGroundRouteItem->VerifyData();
		}
	}
}
void GroundRouteItem::ExportGroundRouteItem(CAirsideExportFile& exportFile)
{
	//	int m_nParentID;
	//	int m_nGroundID;	
	//	int m_nALTObjID;
	//	int m_nMarking;

	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nParentID);
	exportFile.getFile().writeInt(m_nGroundID);
	exportFile.getFile().writeInt(m_nALTObjID);
	exportFile.getFile().writeInt(m_nMarking);
	exportFile.getFile().writeInt(static_cast<int>(m_ChildItem.size()));
	exportFile.getFile().writeLine();

	GroundRouteItemIter iter = m_ChildItem.begin();
	for (; iter!=m_ChildItem.end(); iter++)
	{
		(*iter)->ExportGroundRouteItem(exportFile);
	}

}
void GroundRouteItem::ImportGroundRouteItem(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);
	int nOldParentID = -1;
	importFile.getFile().getInteger(nOldParentID);

	int nOldGroundRouteID = -1;
	importFile.getFile().getInteger(nOldGroundRouteID);

	int nOldObjID = -1;
	importFile.getFile().getInteger(nOldObjID);

	m_nALTObjID = importFile.GetObjectNewID(nOldObjID);

	importFile.getFile().getInteger(m_nMarking);

	int nChildCount = 0;
	importFile.getFile().getInteger(nChildCount);
	importFile.getFile().getLine();

	SaveData();

	for (int i =0; i < nChildCount; ++ i)
	{
		GroundRouteItem groundRouteItem;
		groundRouteItem.SetGroundRouteID(m_nGroundID);
		groundRouteItem.SetParentID(m_nID);
		groundRouteItem.ImportGroundRouteItem(importFile);
	}



}
//////////////////////////////////////////////////////////////////////////
//GroundRoute
GroundRoute::GroundRoute(int nAirportID)
: m_pFirstRouteItem(NULL)
{
	m_DelFirstRouteItemCol.clear();
	m_nAptID = nAirportID;
}

GroundRoute::~GroundRoute(void)
{
	if (NULL != m_pFirstRouteItem)
	{
		delete m_pFirstRouteItem;
	}

	GroundRouteItemIter iter = m_DelFirstRouteItemCol.begin();
	for (; iter!=m_DelFirstRouteItemCol.end(); iter++)
	{
		delete *iter;
	}
	m_DelFirstRouteItemCol.clear();
}
//2 steps
//read the object id and name
//read the tree data
void GroundRoute::ReadObject(int nObjID)
{
	if (nObjID <0)
		throw CADOException("The object id is not valid, less than 0.");
	
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT *\
		FROM ALTOBJECT\
		WHERE (ID = %d)"),nObjID);


	CADORecordset adoRecordset;
	long lItemAffect = 0L;

	CADODatabase::ExecuteSQLStatement(strSQL,lItemAffect,adoRecordset);
	if (!adoRecordset.IsEOF())
	{
		ALTObject::ReadObject(adoRecordset);
		ALTObject::setID(nObjID);

		ReadData();
	}
}

void GroundRoute::ReadData()
{
	CString strSQL;
	strSQL.Format(_T("SELECT * \
		FROM GROUNDROUTEDATA\
		WHERE (GROUNDROUTEID = %d AND PARENTID = -1)"), m_nObjID);

	CADORecordset adoRecordset;
	long lItemAffect = 0L;

	CADODatabase::ExecuteSQLStatement(strSQL,lItemAffect,adoRecordset);
	while (!adoRecordset.IsEOF())
	{
		m_pFirstRouteItem = new GroundRouteItem;
		m_pFirstRouteItem->ReadData(adoRecordset);

		adoRecordset.MoveNextData();
	}
}

int GroundRoute::SaveObject(int AirportID)
{	
	int nObjID;

	if (m_nObjID < 0)
	{
		nObjID = ALTObject::SaveObject(AirportID,ALT_GROUNDROUTE);
		SaveData();
	}
	else
	{
		ALTObject::UpdateObject(m_nObjID);
		SaveData();
		nObjID = m_nObjID;
	}	

	return nObjID;
}

void GroundRoute::SaveData()
{
	if (NULL != m_pFirstRouteItem)
	{
		m_pFirstRouteItem->SetGroundRouteID(m_nObjID);
		m_pFirstRouteItem->SaveData();
	}

	GroundRouteItemIter iter = m_DelFirstRouteItemCol.begin();
	for (; iter!=m_DelFirstRouteItemCol.end(); iter++)
	{
		(*iter)->DeleteData();
	}
	m_DelFirstRouteItemCol.clear();
}

void GroundRoute::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);
	SaveData();
}

void GroundRoute::DeleteObject(int nObjID)
{
	m_DelFirstRouteItemCol.clear();
	DeleteData();
	ALTObject::DeleteObject(nObjID);
}

void GroundRoute::DeleteData()
{
	if (m_nObjID == -1)
		return;

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM GROUNDROUTEDATA\
		WHERE (GROUNDROUTEID = %d)"),m_nObjID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	delete m_pFirstRouteItem;
}

void GroundRoute::SetName(LPCTSTR lpszName)
{
	m_str4Name.m_val[0] = std::string(lpszName);
}

GroundRouteItem* GroundRoute::GetFirstItem()
{
	return m_pFirstRouteItem;
}

GroundRouteItem* GroundRoute::GetItemWithGivenIntersectNodeID(int nIntersectNodeID)
{
	return GetItemWithGivenIntersectNodeID(m_pFirstRouteItem, nIntersectNodeID);
}

GroundRouteItem* GroundRoute::GetItemWithGivenIntersectNodeID(GroundRouteItem* pGRItem, int nIntersectNodeID)
{
	ASSERT(pGRItem);

	int nALTObjID = pGRItem->GetALTObjectID();
	ALTObject *pALTObj = ALTObject::ReadObjectByID(nALTObjID);
	ASSERT(pALTObj);

	IntersectionNodeList iNodeList;

	switch(pALTObj->GetType())
	{
	case ALT_TAXIWAY:
		{
			Taxiway *pTW = (Taxiway*)pALTObj;

			iNodeList = pTW->GetIntersectNodes();
		}
		break;

	case ALT_RUNWAY:
		{
			Runway *pRw = (Runway*)pALTObj;

			iNodeList = pRw->GetIntersectNodes(); 
		}
		break;

	default:
		ASSERT(FALSE);
		break;
	}

	//if fine the node return true
	for (int i=0; i<(int)iNodeList.size(); i++)
	{
		IntersectionNode iNode = iNodeList.at(i);

		if (iNode.GetID() == nIntersectNodeID)
		{
			return pGRItem;
		}
	}

	//find in child
	for (int ii=0; ii<pGRItem->GetChildCount(); ii++)
	{
		GroundRouteItem *pChildItem = pGRItem->GetChildItem(ii);

		GroundRouteItem *pResultItem = GetItemWithGivenIntersectNodeID(pChildItem, nIntersectNodeID);

		if (NULL != pResultItem)
		{
			return pResultItem;
		}
	}

	return NULL;
}

void GroundRoute::SetFirstItem(GroundRouteItem* pGroundRouteItem)
{
	ASSERT(pGroundRouteItem);
	m_pFirstRouteItem = pGroundRouteItem;
}

GroundRouteItemVector GroundRoute::GetTailItems()
{
	ASSERT(m_pFirstRouteItem);

	return m_pFirstRouteItem->GetTailItems();
}

GroundRouteItemVector GroundRoute::GetExitItems()
{
	GroundRouteItemVector vItems;
	vItems.clear();
	vItems = GetTailItems();
	for (int i = 0; i < (int)vItems.size(); i++)
	{
		if ((vItems.at(i))->GetChildCount() > 0)
		{
			vItems.erase(vItems.begin() +i);
			i--;
		}
	}
	return vItems;
}

void GroundRoute::DeleteContent()
{
	if (NULL != m_pFirstRouteItem)
	{
		m_pFirstRouteItem->DeleteAllChild();
	}

	m_DelFirstRouteItemCol.push_back(m_pFirstRouteItem);
	m_pFirstRouteItem = NULL;
}

void GroundRoute::VerifyData()
{
	if (NULL == m_pFirstRouteItem)
	{
		return;
	}

	int nObjID = m_pFirstRouteItem->GetALTObjectID();

	ALTObject altObj;
	ALTObject *pALTObj = altObj.ReadObjectByID(nObjID);

	if (NULL == pALTObj)
	{
		m_pFirstRouteItem->DeleteAllChild();

		m_DelFirstRouteItemCol.push_back(m_pFirstRouteItem);
		m_pFirstRouteItem = NULL;
	}
	else
	{
		m_pFirstRouteItem->VerifyData();
	}
}
void GroundRoute::ExportGroundRoutes(int nAirportID,CAirsideExportFile& exportFile)
{

	std::vector<int> vGroundRouteID;
	ALTAirport::GetGroundRouteIDs(nAirportID,vGroundRouteID);

	int nGroundRouteCount = static_cast<int>(vGroundRouteID.size());

	//export runway
	for (int i =0; i < nGroundRouteCount;++i)
	{
		GroundRoute groundRoute(nAirportID);
		groundRoute.ReadObject(vGroundRouteID[i]);
		groundRoute.ExportGroundRoute(exportFile);
	}

}
void GroundRoute::ExportGroundRoute(CAirsideExportFile& exportFile)
{
	ALTObject::ExportObject(exportFile);
	exportFile.getFile().writeInt(m_nAptID);
	exportFile.getFile().writeLine();
	m_pFirstRouteItem->ExportGroundRouteItem(exportFile);

}
void GroundRoute::ImportObject(CAirsideImportFile& importFile)
{
	ALTObject::ImportObject(importFile);
	m_nObjID = -1;
	int nOldAirportID = -1;
	importFile.getFile().getInteger(nOldAirportID);
	m_nAptID = importFile.GetAirportNewIndex(nOldAirportID);
	importFile.getFile().getLine();

	SaveObject(m_nAptID);
	if (!importFile.getFile().isBlank())
	{
		GroundRouteItem groundRouteItem;
		groundRouteItem.SetGroundRouteID(m_nObjID);
		groundRouteItem.ImportGroundRouteItem(importFile);
	}
}



























