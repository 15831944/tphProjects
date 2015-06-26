#include "stdafx.h"
#include "VehicleTag.h"
#include "..\Database\DBElementCollection_Impl.h"

//CVehicleTag::CVehicleTag(void)
//{
//	m_nProjID = -1;
//}

CVehicleTag::CVehicleTag():m_nProjID(-1)
{
	m_nCurSelNode = -1;
}

CVehicleTag::~CVehicleTag(void)
{
	Clear();
}

void CVehicleTag::Clear(void)
{
	for(std::vector <CVehicleTagNode *>::iterator itrTagNode = m_vrTagNode.begin();\
		itrTagNode != m_vrTagNode.end();++ itrTagNode){
			if(*itrTagNode)
				delete (*itrTagNode);
		}
	m_vrTagNode.clear();
	m_nCurSelNode = -1;
}

void CVehicleTag::ReadData(int nProjID)
{
	m_nProjID = nProjID;
	std::vector<int> vrTagNodeID;
	CVehicleTagNode::GetVehicleTagNodeList(nProjID,vrTagNodeID);
	Clear();
	if(vrTagNodeID.empty())
	{
		CVehicleTagNode * pVehicleTagNode = new CVehicleTagNode(nProjID);
		if(!pVehicleTagNode){
			Clear();
			return;
		}
		CVehicleTagItem * pVehicleTagItemDefault = new CVehicleTagItem(nProjID);
		if(!pVehicleTagItemDefault){
			Clear();
			return;
		}
		pVehicleTagItemDefault->m_bShowTags = FALSE;
		pVehicleTagItemDefault->m_dwTagInfo = 0x0;
		pVehicleTagNode->AddItem(pVehicleTagItemDefault);
		m_vrTagNode.push_back(pVehicleTagNode);
		m_nCurSelNode = 0;
		return;
	}
	for (std::vector<int>::const_iterator citrTagNodeID = vrTagNodeID.begin();\
		citrTagNodeID != vrTagNodeID.end(); ++citrTagNodeID) {
			CVehicleTagNode * pVehicleTagNode = new CVehicleTagNode(nProjID);
			if(!pVehicleTagNode){
				Clear();
				return;
			}
			pVehicleTagNode->ReadData(*citrTagNodeID);
			m_vrTagNode.push_back(pVehicleTagNode);
	}
	m_nCurSelNode = 0;
}

void CVehicleTag::SaveData(void)
{
	if(m_nProjID == -1)return;
	for(std::vector <CVehicleTagNode *>::iterator itrTagNode = m_vrTagNode.begin();\
		itrTagNode != m_vrTagNode.end();++ itrTagNode){
			if(*itrTagNode)
				(*itrTagNode)->SaveData();
		}
}

void CVehicleTag::DeleteData(void)
{
	for(std::vector <CVehicleTagNode *>::iterator itrTagNode = m_vrTagNode.begin();\
		itrTagNode != m_vrTagNode.end();++ itrTagNode){
			if(*itrTagNode)
				(*itrTagNode)->DeleteData();
		}
	Clear();
	m_nProjID = -1;
}

void CVehicleTag::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nProjID);
	exportFile.getFile().writeInt((int)m_vrTagNode.size());
	for(std::vector <CVehicleTagNode *>::iterator itrTagNode = m_vrTagNode.begin();\
		itrTagNode != m_vrTagNode.end();++ itrTagNode){
			if(*itrTagNode)
				(*itrTagNode)->ExportData(exportFile);
		}
	exportFile.getFile().writeLine();
}

void CVehicleTag::ImportData(CAirsideImportFile& importFile)
{
	importFile.getFile().getInteger(m_nProjID);
	m_nProjID = importFile.getNewProjectID();

	int nCount = -1;
	importFile.getFile().getInteger(nCount);
	Clear();
	for (int i = 0;i < nCount;i++) {
		CVehicleTagNode * pVehicleTagNode = new CVehicleTagNode(m_nProjID);
		if(!pVehicleTagNode){
			Clear();
			return;
		}
		pVehicleTagNode->ImportData(importFile);
		m_vrTagNode.push_back(pVehicleTagNode);
	}
	m_nCurSelNode = 0;

	importFile.getFile().getLine();
}

void CVehicleTag::ExportVehicleTag(CAirsideExportFile& exportFile)
{
	CVehicleTag exvehicleTag;
	exvehicleTag.ReadData(exportFile.GetProjectID());
	exportFile.getFile().writeField(_T("VEHICLETAG"));
	exportFile.getFile().writeLine();
	exvehicleTag.ExportData(exportFile);
	exportFile.getFile().endFile();
}

void CVehicleTag::ImportVehicleTag(CAirsideImportFile& importFile)
{
	CVehicleTag imvehicleTag;
	imvehicleTag.ImportData(importFile);
	imvehicleTag.SaveData();
}

void CVehicleTag::AddNode(CVehicleTagNode * pVehicleTagNodeAdding)
{
	if(!pVehicleTagNodeAdding)return;
	m_vrTagNode.push_back(pVehicleTagNodeAdding);
}

bool CVehicleTag::AddNode(CString strVehicleTagNodeName)
{
	if(strVehicleTagNodeName.IsEmpty())return (false);
	CVehicleTagNode * pVehicleTagNodeAdding = 0;
	pVehicleTagNodeAdding = new CVehicleTagNode(m_nProjID);
	if(!pVehicleTagNodeAdding)return (false);
	pVehicleTagNodeAdding->m_strVehicleTagNodeName = strVehicleTagNodeName;
	AddNode(pVehicleTagNodeAdding);
	return (true);
}

bool CVehicleTag::AddTagItemToNode(int nNodeIndex,int nVehicleSpeID,BOOL bShowTags,DWORD dwTagInfo)
{
	if(nNodeIndex < 0 || nNodeIndex >= (int)m_vrTagNode.size())return (false);
	CVehicleTagNode * pVehicleTagNode = 0;
	pVehicleTagNode = GetNode(nNodeIndex);
	if(!pVehicleTagNode)return (false);

	CVehicleTagItem * pTagItem = new CVehicleTagItem(m_nProjID);
	if(!pTagItem)
		return (false);
	if(nVehicleSpeID != -1)
		pTagItem->m_vehicleItem.ReadData(nVehicleSpeID);
	pTagItem->m_bShowTags = bShowTags;
	pTagItem->m_dwTagInfo = dwTagInfo;
	pVehicleTagNode->AddItem(pTagItem);
	return (true);
}

CVehicleTagItem *   CVehicleTag::DeleteItemInNode(int nNodeIndex,int nItemIndex)
{
	CVehicleTagItem * pVehicleTagItemDeleted = 0;
	if(nNodeIndex < 0 || nNodeIndex >= (int)m_vrTagNode.size())return (pVehicleTagItemDeleted);
	CVehicleTagNode * pVehicleTagNode = 0;
	pVehicleTagNode = GetNode(nNodeIndex);
	if(!pVehicleTagNode)return (pVehicleTagItemDeleted);

	pVehicleTagItemDeleted = pVehicleTagNode->DeleteItem(nItemIndex);
	return (pVehicleTagItemDeleted);
}

bool CVehicleTag::GetItemVehicleSpeInNode(int nNodeIndex,int nItemIndex,CVehicleSpecificationItem & vehicleSpeItem)
{
	if(nNodeIndex < 0 || nNodeIndex >= (int)m_vrTagNode.size())return (false);
	CVehicleTagNode * pVehicleTagNode = 0;
	pVehicleTagNode = GetNode(nNodeIndex);
	if(!pVehicleTagNode)return (false);

	CVehicleTagItem * pTagItem = pVehicleTagNode->GetItem(nItemIndex);
	if(!pTagItem)return (false);
	vehicleSpeItem = pTagItem->m_vehicleItem;
	return (true);
}

BOOL CVehicleTag::GetItemShowTagsFlagInNode(int nNodeIndex,int nItemIndex,BOOL &bShowTags)
{
	bShowTags = FALSE;
	if(nNodeIndex < 0 || nNodeIndex >= (int)m_vrTagNode.size())return (false);
	CVehicleTagNode * pVehicleTagNode = 0;
	pVehicleTagNode = GetNode(nNodeIndex);
	if(!pVehicleTagNode)return (false);

	CVehicleTagItem * pTagItem = pVehicleTagNode->GetItem(nItemIndex);
	if(!pTagItem)return (false);
	bShowTags = pTagItem->m_bShowTags;
	return (true);
}

BOOL CVehicleTag::GetItemTagInfoInNode(int nNodeIndex,int nItemIndex,DWORD & dwTagInfo)
{
	dwTagInfo = -1;
	if(nNodeIndex < 0 || nNodeIndex >= (int)m_vrTagNode.size())return (false);
	CVehicleTagNode * pVehicleTagNode = 0;
	pVehicleTagNode = GetNode(nNodeIndex);
	if(!pVehicleTagNode)return (false);

	CVehicleTagItem * pTagItem = pVehicleTagNode->GetItem(nItemIndex);
	if(!pTagItem)return (false);
	dwTagInfo = pTagItem->m_dwTagInfo;
	return pTagItem->m_bShowTags;
	//return (true);
}

bool CVehicleTag::UpdateItemVehicleSpeInNode(int nNodeIndex,int nItemIndex,int nVehicleSpeID)
{
	if(nNodeIndex < 0 || nNodeIndex >= (int)m_vrTagNode.size())return (false);
	CVehicleTagNode * pVehicleTagNode = 0;
	pVehicleTagNode = GetNode(nNodeIndex);
	if(!pVehicleTagNode)return (false);

	CVehicleTagItem * pTagItem = pVehicleTagNode->GetItem(nItemIndex);
	if(!pTagItem)return (false);
	pTagItem->m_vehicleItem.ReadData(nVehicleSpeID);
	return (true);
}

bool CVehicleTag::UpdateItemShowTagsFlagInNode(int nNodeIndex,int nItemIndex,BOOL bShowTags)
{
	if(nNodeIndex < 0 || nNodeIndex >= (int)m_vrTagNode.size())return (false);
	CVehicleTagNode * pVehicleTagNode = 0;
	pVehicleTagNode = GetNode(nNodeIndex);
	if(!pVehicleTagNode)return (false);

	CVehicleTagItem * pTagItem = pVehicleTagNode->GetItem(nItemIndex);
	if(!pTagItem)return (false);
	pTagItem->m_bShowTags = bShowTags;
	return (true);
}

bool CVehicleTag::UpdateItemTagInfoInNode(int nNodeIndex,int nItemIndex,DWORD dwTagInfo)
{
	if(nNodeIndex < 0 || nNodeIndex >= (int)m_vrTagNode.size())return (false);
	CVehicleTagNode * pVehicleTagNode = 0;
	pVehicleTagNode = GetNode(nNodeIndex);
	if(!pVehicleTagNode)return (false);

	CVehicleTagItem * pTagItem = pVehicleTagNode->GetItem(nItemIndex);
	if(!pTagItem)return (false);
	pTagItem->m_dwTagInfo = dwTagInfo;
	return (true);
}

bool CVehicleTag::UpdateNodeName(int nNodeIndex,CString strNewName)
{
	if(nNodeIndex < 0 || nNodeIndex >= (int)m_vrTagNode.size() || strNewName.IsEmpty())return (false);
	CVehicleTagNode * pVehicleTagNode = 0;
	pVehicleTagNode = GetNode(nNodeIndex);
	if(!pVehicleTagNode)return (false);
	pVehicleTagNode->m_strVehicleTagNodeName = strNewName;
	return (true);
}

CString CVehicleTag::GetNodeName(int nIndex)
{
	if(nIndex < 0 || nIndex >= (int)m_vrTagNode.size())return (_T(""));
	CVehicleTagNode * pVehicleTagNode = 0;
	pVehicleTagNode = GetNode(nIndex);
	if(!pVehicleTagNode)return (_T(""));
	return (pVehicleTagNode->m_strVehicleTagNodeName);
}

CVehicleTagNode *  CVehicleTag::DeleteNode(int nIndex)
{
	CVehicleTagNode * pVehicleTagNodeDeleted = 0;
	if(nIndex < 0 || nIndex >= (int)m_vrTagNode.size())return (pVehicleTagNodeDeleted);
	std::vector <CVehicleTagNode *>::iterator itrTagNodeDeleted = m_vrTagNode.begin() + nIndex;
	pVehicleTagNodeDeleted = (*itrTagNodeDeleted);
	m_vrTagNode.erase(itrTagNodeDeleted);
	if(m_nCurSelNode >= nIndex)
	{
		if(m_nCurSelNode - 1 >= 0)
			m_nCurSelNode--;
		else
			m_nCurSelNode = -1;
	}
	return (pVehicleTagNodeDeleted);
}

CVehicleTagNode * CVehicleTag::GetNode(int nIndex)
{
	if(nIndex < 0 || nIndex >= (int)m_vrTagNode.size())return (0);
	std::vector <CVehicleTagNode *>::iterator itrTagNode = m_vrTagNode.begin() + nIndex;
	return (*itrTagNode);
}

int CVehicleTag::GetNodeCount(void)
{
	return (int(m_vrTagNode.size()));
}

int CVehicleTag::GetCurSelNodeIndex(void)
{
	return (m_nCurSelNode);
}

int CVehicleTag::GetCurSelItemIndexInCurSelNode(void)
{
	CVehicleTagNode * pCurSelNode = 0;
	pCurSelNode = GetNode(m_nCurSelNode);
	if(!pCurSelNode)
		return (-1);
	else
		return(pCurSelNode->GetCurSelItemIndex());
}

int CVehicleTag::GetItemCountInCurSelNode(void)
{
	CVehicleTagNode * pCurSelNode = 0;
	pCurSelNode = GetNode(m_nCurSelNode);
	if(!pCurSelNode)
		return (-1);
	else
		return(pCurSelNode->GetItemCount());
}

bool CVehicleTag::SetCurSelNodeIndex(int nSelNodeIndex)
{
	if(nSelNodeIndex < 0 || nSelNodeIndex >= (int)m_vrTagNode.size())return (false);
	m_nCurSelNode = nSelNodeIndex;
	return (true);
}

bool CVehicleTag::SetCurSelItemIndexInCurSelNode(int nSelItemIndex)
{
	CVehicleTagNode * pCurSelNode = 0;
	pCurSelNode = GetNode(m_nCurSelNode);
	if(!pCurSelNode)
		return (false);
	else
		return(pCurSelNode->SetCurSelItemIndex(nSelItemIndex));
}
