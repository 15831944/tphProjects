#include "StdAfx.h"
#include ".\modelingsequenceimpl.h"
#include "resource.h"
#include "termplan.h"
#include "Placement.h"
#include "..\inputs\AreasPortals.h"
#include "Floors.h"
#include "Processor2.h" 
#include <CommonData/Shape.h>
#include "TVNode.h"
#include "TermPlanDoc.h"

CModelingSequenceImpl::CModelingSequenceImpl()
{
	m_pTermPlanDoc=NULL;
}

CModelingSequenceImpl::~CModelingSequenceImpl()
{
}

void CModelingSequenceImpl::BuildTVNodeTree()
{
}

void CModelingSequenceImpl::SetTermPlanDoc(CTermPlanDoc *pDoc)
{
	ASSERT(pDoc!=NULL);
	m_pTermPlanDoc=pDoc;
}

CTVNode* CModelingSequenceImpl::GetTVRootNode()
{
	return &m_tvRootNode;
}
/*
void CModelingSequenceImpl::AddProcessorGroup(int iType, CTVNode* pGroupNode)
{
	ASSERT(m_pTermPlanDoc != NULL);
	if (pGroupNode == NULL)
		pGroupNode = FindProcTypeNode(iType);
	ASSERT(pGroupNode != NULL);

	Terminal& terminal = m_pTermPlanDoc->GetTerminal();
	pGroupNode->DeleteAllChildren();
	ProcessorID id, *pid = NULL;
	id.SetStrDict(terminal.inStrDict);
	CString sID;
	StringList strListL1;
	terminal.procList->getAllGroupNames(strListL1, iType);

	//level 1
	for(int i = 0; i < strListL1.getCount(); i++)
	{

		CString strTemp1 = strListL1.getString(i);
		strTemp1.TrimLeft(); strTemp1.TrimRight();
		if (strTemp1.IsEmpty())
		{
			continue;
		}
		CTVNode*  pL1Node= new CTVNode(strTemp1);
		pGroupNode->AddChild(pL1Node);
		sID = strTemp1;
		id.setID((LPCSTR) sID);
		StringList strListL2;
		terminal.procList->getMemberNames(id, strListL2, -1);
		terminal.procList->getMemberNames(id, strListL2, iType);
		if(strListL2.IsEmpty()) 
		{
			pL1Node->m_nIDResource = IDR_CTX_PROCESSOR;
			//get CProcessor2* associated with this id
			CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id);
			ASSERT(pP2 != NULL);
			pL1Node->m_dwData = (DWORD) pP2;
		}
		// Level 2
		for( int j = 0; j < strListL2.getCount(); j++ )
		{
			CString strTemp2 = strListL2.getString(j);
			strTemp2.TrimLeft();  strTemp2.TrimRight();
			if (strTemp2.IsEmpty())
			{
				continue;
			}
			//CTVNode* pL2Node = new CTVNode(strTemp1+"-"+strTemp2);
			CTVNode* pL2Node = new CTVNode(strTemp2, IDR_CTX_PROCESSORGROUP);
			pL1Node->AddChild(pL2Node);
			sID = strTemp1+"-"+strTemp2;
			id.setID((LPCSTR) sID);
			StringList strListL3;
			terminal.procList->getMemberNames( id, strListL3, -1 );

			terminal.procList->getMemberNames( id, strListL3, RunwayProcessor );
			if(strListL3.IsEmpty())
			{
				pL2Node->m_nIDResource = IDR_CTX_PROCESSOR;
				//get CProcessor2* associated with this id
				CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id);
				ASSERT(pP2 != NULL);
				pL2Node->m_dwData = (DWORD) pP2;
			}
			// Level 3
			for( int k = 0; k < strListL3.getCount(); k++ ) 
			{
				CString strTemp3 = strListL3.getString(k);
				strTemp3.TrimLeft(); strTemp3.TrimRight();
				if (strTemp3.IsEmpty())
				{
					continue;
				}
				//CTVNode* pL3Node = new CTVNode(strTemp1+"-"+strTemp2+"-"+strTemp3);
				CTVNode* pL3Node = new CTVNode(strTemp3, IDR_CTX_PROCESSORGROUP);
				pL2Node->AddChild(pL3Node);
				sID = strTemp1+"-"+strTemp2+"-"+strTemp3;
				id.setID((LPCSTR) sID);
				StringList strListL4;
				terminal.procList->getMemberNames( id, strListL4, -1 );
				terminal.procList->getMemberNames( id, strListL4, RunwayProcessor );

				if(strListL4.IsEmpty())
				{
					pL3Node->m_nIDResource = IDR_CTX_PROCESSOR;
					//get CProcessor2* associated with this id
					CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id);
					ASSERT(pP2 != NULL);
					pL3Node->m_dwData = (DWORD) pP2;
				}
				// Level 4
				for( int l = 0; l < strListL4.getCount(); l++ ) 
				{
					CString strTemp4 = strListL4.getString(l);
					strTemp4.TrimLeft(); strTemp4.TrimRight();
					if (strTemp4.IsEmpty())
					{				
						continue;
					}
					//CTVNode* pL4Node = new CTVNode(strTemp1+"-"+strTemp2+"-"+strTemp3+"-"+strTemp4, IDR_CTX_PROCESSOR);
					CTVNode* pL4Node = new CTVNode(strTemp4, IDR_CTX_PROCESSOR);
					pL3Node->AddChild(pL4Node);
					//get CProcessor2* associated with this id
					sID = strTemp1+"-"+strTemp2+"-"+strTemp3+"-"+strTemp4;
					id.setID((LPCSTR) sID);
					CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id);
					ASSERT(pP2 != NULL);
					pL4Node->m_dwData = (DWORD) pP2;
				}
			}
		}
	}
}

CTVNode *CModelingSequenceImpl::FindProcTypeNode(int iType)
{
	ASSERT(m_pTermPlanDoc != NULL);
	CTVNode* pRetNode = NULL;
	CTVNode* pAirSideRootNode = m_pTermPlanDoc->m_msManager.GetAirSideRootTVNode();

	if (pAirSideRootNode->GetChildCount() == 0)
		return NULL;

	ProcessorClassList ProcType = (ProcessorClassList) iType;

	UINT nProcTypeNameResID = 0;
	switch(ProcType)
	{
	case ArpProcessor:
		nProcTypeNameResID = IDS_TVNN_AIRPORTREFERENCEPOINTS;
		break;

	case RunwayProcessor:
		nProcTypeNameResID = IDS_TVNN_RUNWAYS;
		break;

	case TaxiwayProcessor:
		nProcTypeNameResID = IDS_TVNN_TAIWAYS;
		break;

	case StandProcessor:
		nProcTypeNameResID = IDS_TVNN_AIRCRAFTSTANDS;
		break;

	case NodeProcessor:
		nProcTypeNameResID = IDS_TVNN_AIRFIELDNETWORDNODES;
		break;

	case DeiceBayProcessor:
		nProcTypeNameResID = IDS_TVNN_DEICEPADS;
		break;

	case FixProcessor:
		nProcTypeNameResID = IDS_TVNN_FIXES;
		break;
	}

	CString strProcTypeName;
	if (nProcTypeNameResID != 0)
	{
		if (strProcTypeName.LoadString(nProcTypeNameResID))
			pRetNode =  m_pTermPlanDoc->m_msManager.FindNodeByName(strProcTypeName);
	}

	return pRetNode;
}
*/