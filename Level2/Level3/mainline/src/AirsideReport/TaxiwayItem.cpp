#include "stdafx.h"
#include "TaxiwayItem.h"

bool CTaxiwayItem::IsNodeSelected(int nNodeID)
{
	int nItemCount = static_cast<int>(m_vNodeItems.size());
	for (int nItem = 0; nItem < nItemCount; ++nItem)
	{
		if(m_vNodeItems[nItem].m_nNodeID == nNodeID)
			return true;
	}

	return false;
}

int CTaxiwayItem::InitTaxiNodeIndex(int nStartIndex)
{
	int nItemCount = static_cast<int>(m_vNodeItems.size());
	for (int nItem = 0; nItem < nItemCount; ++nItem)
	{
		m_vNodeItems[nItem].m_nIndex = nStartIndex;

		nStartIndex += 1;
	}

	return nStartIndex;
}

bool CTaxiwayItem::fit(const std::vector<CString>& nodeNameList)
{
	if (nodeNameList.size() > m_vNodeItems.size())
	{
		return false;
	}

	std::vector<CString>vList;
	for (int i = 0; i < (int)nodeNameList.size(); i++)
	{
		const CString& sNodeName = nodeNameList[i]; 
		for (int j = 0; j < (int)m_vNodeItems.size(); j++)
		{
			CTaxiwayNodeItem nodeItem = m_vNodeItems[j];
			if (nodeItem.m_strName == sNodeName)
			{
				vList.push_back(sNodeName);
			}
		}
	}
	
	return (nodeNameList == vList);
}
/////////////////////////////////////////////////////////////////////////////////////////////////
CTaxiwayItemList::CTaxiwayItemList()
:m_bAllTaxiway(false)
{

}

CTaxiwayItemList::~CTaxiwayItemList()
{
	ClearTaxiwayItems();
}

std::vector<CTaxiwayItem::CTaxiwayNodeItem > CTaxiwayItemList::getAllNodes()
{
	std::vector<CTaxiwayItem::CTaxiwayNodeItem > vAllNodes;

	int nCount = static_cast<int>(size());
	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		CTaxiwayItem *pTaxiwayItem = at(nItem);
		if(pTaxiwayItem)
			vAllNodes.insert(vAllNodes.end(),pTaxiwayItem->m_vNodeItems.begin(),pTaxiwayItem->m_vNodeItems.end());
	}
	return vAllNodes;
}

CString CTaxiwayItemList::GetNodeName(int nNodeID)
{
	std::vector<CTaxiwayItem::CTaxiwayNodeItem > vAllNodes = getAllNodes();
	for (int nNode = 0; nNode < static_cast<int>(vAllNodes.size()); ++nNode)
	{
		if(vAllNodes[nNode].m_nNodeID == nNodeID)
			return vAllNodes[nNode].m_strName;
	}
	return _T("");
}

//make a index start from 1 to mark node
void CTaxiwayItemList::InitTaxiNodeIndex()
{
	//index
	int nStartIndex = 1;
	int nCount = static_cast<int>(size());
	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		CTaxiwayItem *pTaxiwayItem =at(nItem);

		nStartIndex = pTaxiwayItem->InitTaxiNodeIndex(nStartIndex);
	}
}

int CTaxiwayItemList::GetNodeIndex(int nNodeID)
{
	std::vector<CTaxiwayItem::CTaxiwayNodeItem > vAllNodes = getAllNodes();
	for (int nNode = 0; nNode < static_cast<int>(vAllNodes.size()); ++nNode)
	{
		if(vAllNodes[nNode].m_nNodeID == nNodeID)
			return vAllNodes[nNode].m_nIndex;
	}
	return -1;
}

bool CTaxiwayItemList::IsNodeSelected( int nNodeID )
{
	int nCount = static_cast<int>(size());

	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		CTaxiwayItem *pTaxiwayItem =at(nItem);
		CTaxiwayItem& taxiwayItem = *pTaxiwayItem;

		if(taxiwayItem.IsNodeSelected(nNodeID))
			return true;
	}

	return false;
}

int CTaxiwayItemList::getItemCount()
{
	return static_cast<int>(size());
}

CTaxiwayItem* CTaxiwayItemList::getItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < getItemCount());

	if (nIndex >=0 && nIndex < getItemCount())
	{
		return at(nIndex);
	}

	return NULL;
}

bool CTaxiwayItemList::IfNodeListInSameTaxiwayAndGetTaxiwayList(const std::vector<CString>& nodeNameList,CTaxiwayItemList& taxiwayList)
{
	if(nodeNameList.size() <= 1)
		return false;

	for (int i = 0; i < getItemCount(); i++)
	{
		CTaxiwayItem* item = getItem(i);
		if (item->fit(nodeNameList))
		{
			taxiwayList.push_back(new CTaxiwayItem(*item));
		}
	}

	if (taxiwayList.empty())
	{
		return false;
	}
	
	return true;
}

void CTaxiwayItemList::AddItem(const CTaxiwayItem& taxiwayItem)
{
	CTaxiwayItem *pItem = new CTaxiwayItem;
	*pItem = taxiwayItem;

	push_back(pItem);
}

void CTaxiwayItemList::ClearTaxiwayItems()
{ 
	std::vector<CTaxiwayItem *>::iterator iter = begin();
	for (;iter != end(); ++iter)
	{
		delete *iter;
	}
	clear();
}

void CTaxiwayItemList::ReadParameter(ArctermFile& _file)
{
	int nCount = 0;
	_file.getInteger(nCount);
	_file.getLine();

	for (int nItem = 0; nItem < nCount; ++nItem)
	{

		CTaxiwayItem taxiwayItem;

		_file.getInteger(taxiwayItem.m_nTaxiwayID);
		_file.getField(taxiwayItem.m_strTaxiwayName.GetBuffer(1024),1024);

		taxiwayItem.m_strTaxiwayName.ReleaseBuffer();

		_file.getInteger(taxiwayItem.m_nodeFrom.m_nNodeID);
		_file.getField(taxiwayItem.m_nodeFrom.m_strName.GetBuffer(1024),1024);
		taxiwayItem.m_nodeFrom.m_strName.ReleaseBuffer();

		_file.getInteger(taxiwayItem.m_nodeTo.m_nNodeID);
		_file.getField(taxiwayItem.m_nodeTo.m_strName.GetBuffer(1024),1024);
		taxiwayItem.m_nodeTo.m_strName.ReleaseBuffer();


		int nNodeCount = 0;
		_file.getInteger(nNodeCount);
		for (int nNode = 0; nNode < nNodeCount; ++ nNode)
		{
			CTaxiwayItem::CTaxiwayNodeItem nodeItem;
			_file.getInteger(nodeItem.m_nNodeID);
			_file.getField(nodeItem.m_strName.GetBuffer(1024),1024);

			taxiwayItem.m_vNodeItems.push_back(nodeItem);
		}

		AddItem(taxiwayItem);

		_file.getLine();
	}
	m_bAllTaxiway = 0 == size();
}

void CTaxiwayItemList::WriteParameter(ArctermFile& _file)
{
	int nCount = m_bAllTaxiway ? 0 : static_cast<int>(size());
	
	_file.writeInt(nCount);
	_file.writeLine();
	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		CTaxiwayItem *pTaxiwayItem =at(nItem);
		CTaxiwayItem& taxiwayItem = *pTaxiwayItem;

		_file.writeInt(taxiwayItem.m_nTaxiwayID);
		_file.writeField(taxiwayItem.m_strTaxiwayName);
		_file.writeInt(taxiwayItem.m_nodeFrom.m_nNodeID);
		_file.writeField(taxiwayItem.m_nodeFrom.m_strName);
		_file.writeInt(taxiwayItem.m_nodeTo.m_nNodeID);
		_file.writeField(taxiwayItem.m_nodeTo.m_strName);

		int nNodeCount = static_cast<int>(taxiwayItem.m_vNodeItems.size());
		_file.writeInt(nNodeCount);
		for (int nNode = 0; nNode < nNodeCount; ++ nNode)
		{
			_file.writeInt(taxiwayItem.m_vNodeItems[nNode].m_nNodeID);
			_file.writeField(taxiwayItem.m_vNodeItems[nNode].m_strName);
		}

		_file.writeLine();
	}

	_file.writeField(_T("useless"));
	_file.writeLine();
}