#include "StdAfx.h"
#include ".\intersectionnodeblocklist.h"
#include "../Database/ADODatabase.h"
#include "IntersectionNode.h"

IntersectionNodeBlock::IntersectionNodeBlock(int nNodeID)
:m_nID(-1)
,m_nNodeID(nNodeID)
,m_bBlock(true)
{
}

IntersectionNodeBlock::~IntersectionNodeBlock(void)
{
}

void IntersectionNodeBlock::SetBlock( bool bBlock )
{
	m_bBlock = bBlock;
}

bool IntersectionNodeBlock::IsBlock()
{
	return m_bBlock;
}

void IntersectionNodeBlock::ReadData( CADORecordset& recordSet )
{
	recordSet.GetFieldValue("ID",m_nID);
	int nBlock = 0;
	recordSet.GetFieldValue("UNBLOCK",nBlock);
	if (nBlock > 0)
		m_bBlock = false;
	else
		m_bBlock = true;

}

void IntersectionNodeBlock::SaveData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO TB_INTERSECTIONNODEBLOCK (NODEID, UNBLOCK)	\
					 VALUES (%d,%d)"), \
					 m_nNodeID,m_bBlock?0:1);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

int IntersectionNodeBlock::GetNodeID()
{
	return m_nNodeID;
}

void IntersectionNodeBlock::SetNodeName( const CString& strName )
{
	m_strNodeName = strName;
}

const CString& IntersectionNodeBlock::GetNodeName()
{
	return m_strNodeName;
}
////////////////////////////////////////////////////////////////////////////
IntersectionNodeBlockList::IntersectionNodeBlockList(void)
{
}

IntersectionNodeBlockList::~IntersectionNodeBlockList(void)
{
	size_t nSize = m_vBlockData.size();
	for (size_t i = 0; i < nSize; i++)
	{
		delete m_vBlockData.at(i);
	}
	m_vBlockData.clear();
}

IntersectionNodeBlock* IntersectionNodeBlockList::GetNodeBlockByIdx( int idx )
{
	if (idx < 0|| idx >= (int)m_vBlockData.size()) 
		return NULL; 

	return m_vBlockData.at(idx);
}

int IntersectionNodeBlockList::GetIntersectionNodeBlockCount()
{
	return (int)m_vBlockData.size();
}

IntersectionNodeBlock* IntersectionNodeBlockList::GetNodeBlockByNodeId( int nID )
{
	size_t nSize = m_vBlockData.size();
	for (size_t i = 0; i < nSize; i++)
	{
		if (m_vBlockData.at(i)->GetNodeID() == nID)
			return m_vBlockData.at(i);

	}
	return NULL;
}

void IntersectionNodeBlockList::InitNodeBlockData( int nAirportID)
{
	std::vector<IntersectionNode> vIntersectionNodes;
	IntersectionNode::ReadNodeList(nAirportID, vIntersectionNodes);

	size_t nSize = vIntersectionNodes.size();
	for (size_t i =0; i < nSize; i++)
	{
		if (vIntersectionNodes.at(i).IsRunwayIntersectionNode())		//not consider runway intersection
			continue;

		if (vIntersectionNodes.at(i).GetTaxiwayIntersectItemList().size() < 2)
			continue;

		IntersectionNodeBlock* pNodeBlock = new IntersectionNodeBlock(vIntersectionNodes.at(i).GetID());
		pNodeBlock->SetNodeName(vIntersectionNodes.at(i).GetName()) ;
		m_vBlockData.push_back(pNodeBlock);
	}

	ReadBlockData();
}

void IntersectionNodeBlockList::ReadBlockData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM TB_INTERSECTIONNODEBLOCK"));

	long nRecordCount = -1;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		int nNodeID = -1;
		adoRecordset.GetFieldValue("NODEID",nNodeID);
		IntersectionNodeBlock* pBlockData = GetNodeBlockByNodeId(nNodeID);
		if (pBlockData)
			pBlockData->ReadData(adoRecordset);

		adoRecordset.MoveNextData();
	}
}

void IntersectionNodeBlockList::SaveData()
{
	DeleteData();

	size_t nSize = m_vBlockData.size();
	for (size_t i = 0; i < nSize; i++)
	{
		if (!m_vBlockData.at(i)->IsBlock())
			m_vBlockData.at(i)->SaveData() ;
	}
}

void IntersectionNodeBlockList::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format("DELETE * FROM TB_INTERSECTIONNODEBLOCK");
	CADODatabase::ExecuteSQLStatement(strSQL);
}