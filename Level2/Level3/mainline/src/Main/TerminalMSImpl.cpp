#include "StdAfx.h"
#include "TerminalMSImpl.h"
#include "Floor2.h"
#include "resource.h"
#include "TermPlanDoc.h"
#include <CommonData/Shape.h>
#include "..\Inputs\Structurelist.h"
#include "..\Inputs\WallShapeList.h"
#include "Placement.h"
#include "TVN.h"

CTerminalMSImpl::CTerminalMSImpl()
{
}

CTerminalMSImpl::~CTerminalMSImpl()
{
}

void CTerminalMSImpl::BuildTVNodeTree()
{
	ASSERT(m_pTermPlanDoc != NULL);
	Terminal& terminal = m_pTermPlanDoc->GetTerminal();
//	CTVNode* pRootNode = m_pTermPlanDoc->GetTerminalRootTVNode();
	CTVNode* pRootNode=GetTVRootNode();
	//Build Node structure
	CString s;
	//add layout node to root
	s.LoadString(IDS_TVNN_LAYOUT);
	CTVNode* pLayoutNode = new CTVNode(s);
	pLayoutNode->m_eState = CTVNode::expanded;
	m_tvRootNode.AddChild(pLayoutNode);

	CTVNode* pNode = NULL;

	//add floors node to layout
	s.LoadString(IDS_TVNN_FLOORS);
	CTVNode* pFloorsNode = new CTVNode(s, IDR_CTX_FLOORS);
	pLayoutNode->AddChild(pFloorsNode);
	//add each floor to floors
	for (int i=m_pTermPlanDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors.size()-1; i>=0; i--)
	{
		CTVNode* pFloorNode = new CTVNode(m_pTermPlanDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[i]->FloorName(), IDR_CTX_FLOOR);
		pFloorNode->m_iImageIdx = (i==m_pTermPlanDoc->m_nActiveFloor?ID_NODEIMG_FLOORACTIVE:ID_NODEIMG_FLOORNOTACTIVE);
		pFloorNode->m_dwData = (DWORD) i;
		pFloorsNode->AddChild(pFloorNode);
	}

	//add under construction node
	if(m_pTermPlanDoc->GetTerminalPlacement()->m_vUndefined.size() > 0) {
		s.LoadString(IDS_TVNN_UNDERCONSTRUCTION);
		CTVNode* pUCNode = new CTVNode(s);
		pLayoutNode->AddChild(pUCNode);
		//add undefined to under construction
// 		for(i=0; i<static_cast<int>(m_pTermPlanDoc->GetTerminalPlacement()->m_vUndefined.size()); i++) {
// 			s.Format("%s-%d",m_pTermPlanDoc->GetTerminalPlacement()->m_vUndefined[i]->GetShape()->Name(),i);
// 			//			// TRACE("<s=:%s> <i=%d>\n", s, i);
// 			pNode = new CTVNode(s, IDR_CTX_UCPROCESSOR);
// 			pNode->m_dwData = (DWORD) m_pTermPlanDoc->GetTerminalPlacement()->m_vUndefined[i];
// 			pUCNode->AddChild(pNode);
// 		}

//		level1
		int nCount = m_pTermPlanDoc->GetTerminalPlacement()->m_vUndefined.getNodeCount();
		for (int i = 0; i < nCount; i++)
		{
			CNudefinePro2Node* pNodeLevel1 = NULL;
			pNodeLevel1 = m_pTermPlanDoc->GetTerminalPlacement()->m_vUndefined.getItem(i);
	
			CString strShapeName1 = pNodeLevel1->getNodeName();
			strShapeName1.TrimLeft();strShapeName1.TrimRight();
			if (strShapeName1.IsEmpty())
			{
				continue;
			}
			CTVNode* pShapeL1 = new CTVNode(strShapeName1);
			pUCNode->AddChild(pShapeL1);

			if (pNodeLevel1->getNodeCount() == 0)
			{
				pShapeL1->m_nIDResource = IDR_CTX_UCPROCESSOR;
				CProcessor2* pP2 = m_pTermPlanDoc->GetTerminalPlacement()->m_vUndefined.GetProc2(strShapeName1);
				ASSERT(pP2 != NULL);
				pShapeL1->m_dwData = (DWORD) pP2;
			}
			// level2
			for (int j = 0; j < pNodeLevel1->getNodeCount(); j++)
			{
				CNudefinePro2Node* pNodeLevel2 = NULL;
				pNodeLevel2 = pNodeLevel1->getItem(j);


				CString strShapeName2 = strShapeName1 + CString("-") + pNodeLevel2->getNodeName();
				strShapeName2.TrimLeft();strShapeName2.TrimRight();
				if (strShapeName2.IsEmpty())
				{
					continue;
				}

				CTVNode* pShapeL2 = new CTVNode(pNodeLevel2->getNodeName());
				pShapeL1->AddChild(pShapeL2);

				if(pNodeLevel2->getNodeCount() == 0) {
					pShapeL2->m_nIDResource = IDR_CTX_UCPROCESSOR;
					//get CProcessor2* associated with this id
					CProcessor2* pP2 = m_pTermPlanDoc->GetTerminalPlacement()->m_vUndefined.GetProc2(strShapeName2);
					if (pP2 == NULL)
					{
						pShapeL1->RemoveChild(pShapeL2);
						continue;
					}
					ASSERT(pP2 != NULL);
					pShapeL2->m_dwData = (DWORD) pP2;
				}
				//level3
				for (int n = 0; n < pNodeLevel2->getNodeCount(); n++)
				{
					CNudefinePro2Node* pNodeLevel3 = NULL;
					pNodeLevel3 = pNodeLevel2->getItem(n);
				
					CString strShapeName3 = strShapeName2 + CString("-") + pNodeLevel3->getNodeName();
					strShapeName3.TrimLeft();strShapeName3.TrimRight();
					if (strShapeName3.IsEmpty())
					{
						continue;
					}

					CTVNode* pShapeL3 = new CTVNode(pNodeLevel3->getNodeName());
					pShapeL2->AddChild(pShapeL3);
					
					if(pNodeLevel3->getNodeCount() == 0) {
						pShapeL3->m_nIDResource = IDR_CTX_UCPROCESSOR;
						//get CProcessor2* associated with this id
						CProcessor2* pP2 = m_pTermPlanDoc->GetTerminalPlacement()->m_vUndefined.GetProc2(strShapeName3);
						if (pP2 == NULL)
						{
							pShapeL2->RemoveChild(pShapeL3);
							continue;
						}
						ASSERT(pP2 != NULL);
						pShapeL3->m_dwData = (DWORD) pP2;
					}
              //level 4
					for (int m = 0; m < pNodeLevel3->getNodeCount(); m++)
					{		
						CNudefinePro2Node* pNodeLevel4 = NULL;
						pNodeLevel4 = pNodeLevel3->getItem(m);
						CString strShapeName4 = strShapeName3 + CString("-") + pNodeLevel4->getNodeName();
						strShapeName4.TrimLeft();strShapeName4.TrimRight();
						if (strShapeName4.IsEmpty())
						{
							continue;
						}

						CTVNode* pShapeL4 = new CTVNode( pNodeLevel4->getNodeName(),IDR_CTX_UCPROCESSOR);
						pShapeL3->AddChild(pShapeL4);


						CProcessor2* pP2 = m_pTermPlanDoc->GetTerminalPlacement()->m_vUndefined.GetProc2(strShapeName4);
						if (pP2 == NULL)
						{
							pShapeL3->RemoveChild(pShapeL4);
							continue;
						}
						ASSERT(pP2 != NULL);
						pShapeL4->m_dwData = (DWORD) pP2;;
					}
				}
			}
		}
	}

	//add placements node to layout
	s.LoadString(IDS_TVNN_PLACEMENTS);
	CTVNode* pPlacementsNode = new CTVNode(s, IDR_CTX_PLACEMENTS);
	pLayoutNode->AddChild(pPlacementsNode);
	//add all defined procs to placements (as a hierarchy)
	ProcessorID id, *pid = NULL;
	id.SetStrDict( terminal.inStrDict );
	CString sID;
	StringList strListL1;
	terminal.procList->getAllGroupNames (strListL1, -1);
	terminal.procList->getAllGroupNames (strListL1, BarrierProc);
	//level 1
	for(int i = 0; i < strListL1.getCount(); i++) {
		CString strTemp1 = strListL1.getString(i);
		strTemp1.TrimLeft(); strTemp1.TrimRight();
		if (strTemp1.IsEmpty())
			continue;

		CTVNode* pL1Node = new CTVNode(strTemp1, IDR_CTX_PROCESSORGROUP_WITH_CHANGETYPE);
		pPlacementsNode->AddChild(pL1Node);

		sID = strTemp1;
		id.setID((LPCSTR) sID);
		StringList strListL2;
		terminal.procList->getMemberNames( id, strListL2, -1 );
		terminal.procList->getMemberNames( id, strListL2, BarrierProc );
		if(strListL2.IsEmpty()) {
			pL1Node->m_nIDResource = IDR_CTX_PROCESSOR;
			//get CProcessor2* associated with this id
			CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_Terminal);
			ASSERT(pP2 != NULL);
			pL1Node->m_dwData = (DWORD) pP2;
		}

		// Level 2
		for( int j = 0; j < strListL2.getCount(); j++ ) {
			CString strTemp2 = strListL2.getString(j);
			strTemp2.TrimLeft();  strTemp2.TrimRight();
			if (strTemp2.IsEmpty())
				continue;

			//CTVNode* pL2Node = new CTVNode(strTemp1+"-"+strTemp2);
			CTVNode* pL2Node = new CTVNode(strTemp2, IDR_CTX_PROCESSORGROUP);
			pL1Node->AddChild(pL2Node);

			sID = strTemp1+"-"+strTemp2;
			id.setID((LPCSTR) sID);
			StringList strListL3;
			terminal.procList->getMemberNames( id, strListL3, -1 );
			terminal.procList->getMemberNames( id, strListL3, BarrierProc );
			if(strListL3.IsEmpty()) {
				pL2Node->m_nIDResource = IDR_CTX_PROCESSOR;
				//get CProcessor2* associated with this id
				CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_Terminal);
				ASSERT(pP2 != NULL);
				pL2Node->m_dwData = (DWORD) pP2;
			}

			// Level 3
			for( int k = 0; k < strListL3.getCount(); k++ ) {
				CString strTemp3 = strListL3.getString(k);
				strTemp3.TrimLeft(); strTemp3.TrimRight();
				if (strTemp3.IsEmpty())
					continue;

				//CTVNode* pL3Node = new CTVNode(strTemp1+"-"+strTemp2+"-"+strTemp3);
				CTVNode* pL3Node = new CTVNode(strTemp3, IDR_CTX_PROCESSORGROUP);
				pL2Node->AddChild(pL3Node);

				sID = strTemp1+"-"+strTemp2+"-"+strTemp3;
				id.setID((LPCSTR) sID);
				StringList strListL4;
				terminal.procList->getMemberNames( id, strListL4, -1 );
				terminal.procList->getMemberNames( id, strListL4, BarrierProc );
				if(strListL4.IsEmpty()) {
					pL3Node->m_nIDResource = IDR_CTX_PROCESSOR;
					//get CProcessor2* associated with this id
					CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_Terminal);
					ASSERT(pP2 != NULL);
					pL3Node->m_dwData = (DWORD) pP2;
				}

				// Level 4
				for( int l = 0; l < strListL4.getCount(); l++ ) {
					CString strTemp4 = strListL4.getString(l);
					strTemp4.TrimLeft(); strTemp4.TrimRight();
					if (strTemp4.IsEmpty())
						continue;

					//CTVNode* pL4Node = new CTVNode(strTemp1+"-"+strTemp2+"-"+strTemp3+"-"+strTemp4, IDR_CTX_PROCESSOR);
					CTVNode* pL4Node = new CTVNode(strTemp4, IDR_CTX_PROCESSOR);
					pL3Node->AddChild(pL4Node);
					//get CProcessor2* associated with this id
					sID = strTemp1+"-"+strTemp2+"-"+strTemp3+"-"+strTemp4;
					id.setID((LPCSTR) sID);
					CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_Terminal);
					ASSERT(pP2 != NULL);
					pL4Node->m_dwData = (DWORD) pP2;
				}
			}
		}
	}

	// Surface Areas
	s.LoadString(IDS_TVNN_SURFACEAREAS);
	pNode = new CTVNode(s, IDR_CTX_SURFACEAREAS);
	pLayoutNode->AddChild(pNode);
	m_pTermPlanDoc->m_msManager.SetSurfaceAreasNode(pNode, EnvMode_Terminal);
	AddStructureGroup(pNode);
	//CStructureList* pStructList = m_pTermPlanDoc->GetTerminal().m_pStructureList;
	//for (UINT i = 0; i < pStructList->getStructureNum(); i++)
	//{
	//	CString strName = pStructList->getStructureAt(i).GetNameStr();
	//	m_pTermPlanDoc->m_msManager.AddSurfaceAreaNode(strName,i, EnvMode_Terminal);
	//}
	//
	{        /**Wall shape*/
		s.LoadString(IDS_TVNN_WALLSHAPE);
		pNode = new CTVNode(s, IDR_CTX_WALLSHAPES);
		pLayoutNode->AddChild(pNode);
		AddWallShapeGroup(pNode);
		//WallShapeList * wallList= m_pTermPlanDoc->GetTerminal().m_pWallShapeList;
		//for (UINT i = 0; i < wallList->getShapeNum(); i++)
		//{
		//	CString strName = wallList->getShapeAt(i).GetNameStr();
		//	m_pTermPlanDoc->m_msManager.AddWallShapeNode(strName, i,EnvMode_Terminal);
		//}
		
	}	

	//Pipe definition
	s.LoadString(IDS_STRING_PIPE);
	pNode = new CTVNode(s,IDR_DEFINE_PIPE);
	pLayoutNode->AddChild(pNode);
	//add areas node to layout
	s.LoadString(IDS_TVNN_AREAS);
	pNode = new CTVNode(s, IDR_CTX_AREAS);
	pLayoutNode->AddChild(pNode);
	//add each area to the areas node
	for(int i=0; i<static_cast<int>(terminal.m_pAreas->m_vAreas.size()); i++) {
		CAreaNode* pAN = new CAreaNode(i);
		pAN->Name(terminal.m_pAreas->m_vAreas[i]->name);
		pNode->AddChild(pAN);
	}
	//add portals node to layout
	s.LoadString(IDS_TVNN_PORTALS);
	pNode = new CTVNode(s, IDR_CTX_PORTALS);
	pLayoutNode->AddChild(pNode);
	//add each portal to the portals node
	for(int i=0; i<static_cast<int>(m_pTermPlanDoc->GetTerminal().m_pPortals->m_vPortals.size()); i++) {
		CPortalNode* pPN = new CPortalNode(i);
		pPN->Name(m_pTermPlanDoc->GetTerminal().m_pPortals->m_vPortals[i]->name);
		pNode->AddChild(pPN);
	}

	//add railway layou
	s.LoadString(IDS_RAILWAY_LAYOUT);
	pNode = new CTVNode(s,IDR_RAILWAY_LAYOUT);
	pLayoutNode->AddChild(pNode);

	//add name list
	s.LoadString( IDS_TVNN_NAMELIST );
	pNode = new CTVNode(s, IDR_CTX_NAMELIST/*IDS_TVNN_NAMELIST*/);
	pLayoutNode->AddChild(pNode);


	//add extend data
	s.LoadString(IDS_TVNN_DATA);
	CTVNode* pDataNode = new CTVNode(s);
	m_tvRootNode.AddChild(pDataNode);
	//real time radar
	s.LoadString(IDS_TVNN_RADAR);
	CTVNode* pRadarNode = new CTVNode(s);
	pDataNode->AddChild(pRadarNode);
	//real time noise
	s.LoadString(IDS_TVNN_NOISE);
	CTVNode* pNoiseNode = new CTVNode(s);
	pDataNode->AddChild(pNoiseNode);
	//aodb
	s.LoadString(IDS_TVNN_AODB);
	CTVNode* pAodbNode = new CTVNode(s);
	pDataNode->AddChild(pAodbNode);
	//auto transfer
	s.LoadString(IDS_TVNN_AUTO);
	CTVNode* pAutoNode = new CTVNode(s);
	pAodbNode->AddChild(pAutoNode);
	//manual transfer
	s.LoadString(IDS_TVNN_MANUAL);
	CTVNode* pManualNode = new CTVNode(s);
	pAodbNode->AddChild(pManualNode);

	//add flights node to root
	s.LoadString(IDS_TVNN_FLIGHTS);
	CTVNode* pFlightsNode = new CTVNode(s);//, IDR_CTX_FLIGHTS);
	pFlightsNode->m_eState = CTVNode::expanded;
	m_tvRootNode.AddChild(pFlightsNode);
	//add schedule node to flights
	s.LoadString(IDS_TVNN_SCHEDULE);
	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	pFlightsNode->AddChild(pNode);
	// add flight type specification
	s.LoadString(IDS_TVNN_FLIGHTTYPESPECIFICATION);
	CTVNode* pFlightTypeSpecifictionNode = new CTVNode(s);
	pFlightTypeSpecifictionNode->m_eState = CTVNode::expanded;
	pFlightsNode->AddChild(pFlightTypeSpecifictionNode);
	s.LoadString(IDS_TVNN_FLIGHT_GROUPS);
	pNode = new CTVNode(s);
	pFlightTypeSpecifictionNode->AddChild(pNode);
	s.LoadString(IDS_TVNN_AIRPORTSECTORS);
	pNode = new CTVNode(s);
	pFlightTypeSpecifictionNode->AddChild(pNode);
	s.LoadString(IDS_TVNN_AIRLINEGROUPS);
	pNode = new CTVNode(s);
	pFlightTypeSpecifictionNode->AddChild(pNode);
	s.LoadString(IDS_TVNN_AIRCRAFTCATEGORIES);
	pNode = new CTVNode(s);
	pFlightTypeSpecifictionNode->AddChild(pNode);
	//	s.LoadString(IDS_TVNN_GROUPINGS);
	//	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	//	pFlightsNode->AddChild(pNode);
	s.LoadString(IDS_TVNN_DELAYS);
	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	pFlightsNode->AddChild(pNode);
	s.LoadString(IDS_TVNN_LOADFACTORS);
	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	pFlightsNode->AddChild(pNode);
	s.LoadString(IDS_TVNN_ACCAPACITIES);
	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	pFlightsNode->AddChild(pNode);
	s.LoadString(IDS_TVNN_BOARDINGCALLS);
	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	pFlightsNode->AddChild(pNode);

	s.LoadString(IDS_TVNN_GATE_ASSIGNMENT);
	CTVNode* pGateAssignmentNode = new CTVNode(s);
	pGateAssignmentNode->m_eState = CTVNode::expanded;
	pFlightsNode->AddChild(pGateAssignmentNode);
	{
		s.LoadString(IDS_TVNN_STAND_GATE_ASSIGNMENT);
		pNode = new CTVNode(s);//, IDR_CTX_NONE);
		pGateAssignmentNode->AddChild(pNode);
		s.LoadString(IDS_TVNN_ARRIVAL_GATE_ASSIGNMENT);
		pNode = new CTVNode(s);//, IDR_CTX_NONE);
		pGateAssignmentNode->AddChild(pNode);
		s.LoadString(IDS_TVNN_DEPARTURE_GATE_ASSIGNMENT);
		pNode = new CTVNode(s);//, IDR_CTX_NONE);
		pGateAssignmentNode->AddChild(pNode);        
	}

	s.LoadString(IDS_TVNN_HUBBINGDATA);
	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	pFlightsNode->AddChild(pNode);

	s.LoadString(IDS_TVNN_OPERATINGDOOR);
	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	pFlightsNode->AddChild(pNode);

	//add passengers node to root
	s.LoadString(IDS_TVNN_PASSENGERS);
	CTVNode* pPAXNode = new CTVNode(s);//, IDR_CTX_FLIGHTS);
	pPAXNode->m_eState = CTVNode::expanded;
	m_tvRootNode.AddChild(pPAXNode);
	s.LoadString(IDS_TVNN_PAXNAMES);
	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	pPAXNode->AddChild(pNode);
	s.LoadString(IDS_TVNN_PAXDISTRIBUTIONS);
	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	pPAXNode->AddChild(pNode);
	//s.LoadString(IDS_TVNN_PAXBAGCOUNT);
	//pNode = new CTVNode(s);//, IDR_CTX_NONE);
	//pPAXNode->AddChild(pNode);
	s.LoadString(IDS_TVNN_MOBILECOUNT);
	pNode = new CTVNode(s);
	pPAXNode->AddChild(pNode);
	//s.LoadString(IDS_TVNN_PAXCARTCOUNT);
	//pNode = new CTVNode(s);//, IDR_CTX_NONE);
	//pPAXNode->AddChild(pNode);
	s.LoadString(IDS_TVNN_PAXGROUPSIZE);
	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	pPAXNode->AddChild(pNode);
	s.LoadString(IDS_TVNN_PAXLLDIST);
	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	pPAXNode->AddChild(pNode);

	s.LoadString(IDS_TVNN_PAXDISTPROFILE);
	CTVNode* pChildNode = new CTVNode(s);
	pNode->AddChild(pChildNode);

	s.LoadString(IDS_TVNN_PAXBULK);
	pChildNode = new CTVNode(s);
	pNode->AddChild(pChildNode);

	pChildNode = new CTVNode(_T("Visitor STA Trigger"));
	pNode->AddChild(pChildNode);

	s.LoadString(IDS_TVNN_PAXISDIST);
	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	pPAXNode->AddChild(pNode);
	s.LoadString(IDS_TVNN_PAXSSDIST);
	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	pPAXNode->AddChild(pNode);
	s.LoadString(IDS_TVNN_PAXSPEED);
	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	pPAXNode->AddChild(pNode);
	//s.LoadString(IDS_TVNN_PAXVISCOUNT);
	//pNode = new CTVNode(s);//, IDR_CTX_NONE);
	//pPAXNode->AddChild(pNode);
	s.LoadString(IDS_TVNN_PAXVISTIME);
	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	pPAXNode->AddChild(pNode);
	s.LoadString(IDS_TVNN_EMERGBEH);
	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	pPAXNode->AddChild(pNode);
	s.LoadString(IDS_TVNN_INITRESPTIME);
	pChildNode = new CTVNode(s);
	pNode->AddChild(pChildNode);
	s.LoadString(IDS_TVNN_INITSPDIMP);
	pChildNode = new CTVNode(s);
	pNode->AddChild(pChildNode);
	s.LoadString(IDS_TVNN_PROPENTOPUSH);
	pChildNode = new CTVNode(s);
	pNode->AddChild(pChildNode);

	s.LoadString(IDS_TVNN_PROCESSORS);
	CTVNode* pProcNode = new CTVNode(s);//, IDR_CTX_FLIGHTS);
	pProcNode->m_eState = CTVNode::expanded;
	m_tvRootNode.AddChild(pProcNode);
	s.LoadString(IDS_TVNN_RETAIL_PRODUCTS);
	pNode = new CTVNode(s);
	pProcNode->AddChild(pNode);
	s.LoadString(IDS_TVNN_PROCSERVTIME);
	CTVNode* pServiceTimeNode = new CTVNode(s);//, IDR_CTX_NONE);
	pProcNode->AddChild(pServiceTimeNode);
    s.LoadString(IDS_TVNN_TERMINALSERVICETIME);
    pNode = new CTVNode(s);
    pServiceTimeNode->AddChild(pNode);
    s.LoadString(IDS_TVNN_ACENTRYPROCS);
    pNode = new CTVNode(s);
    pServiceTimeNode->AddChild(pNode);
    s.LoadString(IDS_TVNN_PROCPROCESS);
    pNode = new CTVNode(s);
	pProcNode->AddChild(pNode);
	s.LoadString(IDS_TVNN_PROCPAXFLOWS);
	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	pProcNode->AddChild(pNode);
	s.LoadString(IDS_TVNN_FLOWBYPAXTYPE);
	pChildNode = new CTVNode(s);
	pNode->AddChild(pChildNode);
	s.LoadString(IDS_TVNN_FLOWBYPROC);
	pChildNode = new CTVNode(s);
	pNode->AddChild(pChildNode);
	s.LoadString(IDS_TVNN_PROCROSTER);
	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	pProcNode->AddChild(pNode);
	s.LoadString(IDS_TVNN_PROCBAGDEVASSIGN);
	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	pProcNode->AddChild(pNode);
	s.LoadString(IDS_TVNN_PROCLINKAGES);
	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	pProcNode->AddChild(pNode);
	s.LoadString( IDS_ALLCAR_SCHEDULE );
	pNode = new CTVNode ( s );
	pProcNode->AddChild( pNode );
	s.LoadString(IDS_TVNN_MOVINGSIDEWALK);
	pNode = new CTVNode(s);
	pProcNode->AddChild(pNode);	
	s.LoadString( IDS_TVNN_RESOURCE );
	CTVNode* pResNode = new CTVNode(s);
	pProcNode->AddChild( pResNode );		
	s.LoadString( IDS_TVNN_RESOURCEPOOL );
	pNode = new CTVNode(s);
	pResNode->AddChild( pNode );
	s.LoadString( IDS_TVNN_PROCTORESPOOL );
	pNode = new CTVNode(s);
	pResNode->AddChild( pNode );
	s.LoadString(IDS_TVNN_PROCCONGAREA);
	pNode = new CTVNode(s);
	pProcNode->AddChild( pNode );

	// ---------------------- Loading Economic Menu Begin ---------------------- //

	CTVNode* pNode1 = NULL, * pNode2 = NULL;
	s.LoadString(IDS_TVNN_ECONOMIC);
	CTVNode* pEconomicNode = new CTVNode(s);//, IDR_CTX_FLIGHTS);
	pEconomicNode->m_eState = CTVNode::expanded;
	m_tvRootNode.AddChild(pEconomicNode);

	// - Real Costs - //
	s.LoadString(IDS_TVNN_REALCOST);
	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	pEconomicNode->AddChild(pNode);

	// -- Fixed Costs -- //
	s.LoadString(IDS_TVNN_FIXEDCOSTS);
	pNode1 = new CTVNode(s);
	pNode->AddChild(pNode1);
	s.LoadString(IDS_TVNN_BUILDINGDEP);
	pNode2 = new CTVNode(s);
	pNode1->AddChild(pNode2);
	s.LoadString(IDS_TVNN_INTERESTONCAPITAL);
	pNode2 = new CTVNode(s);
	pNode1->AddChild(pNode2);
	s.LoadString(IDS_TVNN_INSURANCE);
	pNode2 = new CTVNode(s);
	pNode1->AddChild(pNode2);
	s.LoadString(IDS_TVNN_CONTRACTS);
	pNode2 = new CTVNode(s);
	pNode1->AddChild(pNode2);
	s.LoadString(IDS_TVNN_PARKINGLOTSDEP);
	pNode2 = new CTVNode(s);
	pNode1->AddChild(pNode2);
	s.LoadString(IDS_TVNN_LANDSIDEFACILITIES);
	pNode2 = new CTVNode(s);
	pNode1->AddChild(pNode2);
	s.LoadString(IDS_TVNN_AIRSIZEFACILITIES);
	pNode2 = new CTVNode(s);
	pNode1->AddChild(pNode2);

	// -- Fixed and Variable Costs -- //
	s.LoadString(IDS_TVNN_FIXEDANDVARIABLECOSTS);
	pNode1 = new CTVNode(s);
	pNode->AddChild(pNode1);
	s.LoadString(IDS_TVNN_PROCESSOR);
	pNode2 = new CTVNode(s);
	pNode1->AddChild(pNode2);
	s.LoadString(IDS_TVNN_PASSENGER);
	pNode2 = new CTVNode(s);
	pNode1->AddChild(pNode2);
	s.LoadString(IDS_TVNN_AIRCRAFT);
	pNode2 = new CTVNode(s);
	pNode1->AddChild(pNode2);
	s.LoadString(IDS_TVNN_UTILITIES);
	pNode2 = new CTVNode(s);
	pNode1->AddChild(pNode2);
	s.LoadString(IDS_TVNN_LABOUR);
	pNode2 = new CTVNode(s);
	pNode1->AddChild(pNode2);
	s.LoadString(IDS_TVNN_MAINTENANCE);
	pNode2 = new CTVNode(s);
	pNode1->AddChild(pNode2);

	// - Revenue Factors - //
	s.LoadString(IDS_TVNN_REVENUE);
	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	pEconomicNode->AddChild(pNode);

	// -- Fixed Revenues -- //
	s.LoadString(IDS_TVNN_FIXEDREVENUES);
	pNode1 = new CTVNode(s);
	pNode->AddChild(pNode1);
	s.LoadString(IDS_TVNN_LEASES);
	pNode2 = new CTVNode(s);
	pNode1->AddChild(pNode2);
	s.LoadString(IDS_TVNN_ADVERTISING);
	pNode2 = new CTVNode(s);
	pNode1->AddChild(pNode2);
	s.LoadString(IDS_TVNN_REVENUESCONTRACTS);
	pNode2 = new CTVNode(s);
	pNode1->AddChild(pNode2);

	// -- Fixed and Variable Revenues -- //
	s.LoadString(IDS_TVNN_FIXEDANDVARIABLEREVENUES);
	pNode1 = new CTVNode(s);
	pNode->AddChild(pNode1);
	s.LoadString(IDS_TVNN_PASSENGERAIRPORTFEES);	
	pNode2 = new CTVNode(s);
	pNode1->AddChild(pNode2);
	s.LoadString(IDS_TVNN_LANDINGFEES);
	pNode2 = new CTVNode(s);
	pNode1->AddChild(pNode2);
	s.LoadString(IDS_TVNN_GATEUSAGE);
	pNode2 = new CTVNode(s);
	pNode1->AddChild(pNode2);
	s.LoadString(IDS_TVNN_PROCESSORUSAGE);
	pNode2 = new CTVNode(s);
	pNode1->AddChild(pNode2);
	s.LoadString(IDS_TVNN_RETAILPERCENTAGE);
	pNode2 = new CTVNode(s);
	pNode1->AddChild(pNode2);
	s.LoadString(IDS_TVNN_PAKINGFEES);
	pNode2 = new CTVNode(s);
	pNode1->AddChild(pNode2);
	s.LoadString(IDS_TVNN_LANDSIDETRANS);
	pNode2 = new CTVNode(s);
	pNode1->AddChild(pNode2);

	// - Capital Budgets Decisions - //
	s.LoadString(IDS_TVNN_CAPITALBDDECISIONS);
	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	pEconomicNode->AddChild(pNode);


	// - Waiting Cost Factors - //
	s.LoadString(IDS_TVNN_WAITINGCOST);
	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	pEconomicNode->AddChild(pNode);

	// - Annual Activity Deduction - //
	s.LoadString(IDS_TVNN_ANNUALACTIVITY);
	pNode = new CTVNode(s);//, IDR_CTX_NONE);
	pEconomicNode->AddChild(pNode);

	// ---------------------- Loading Economic Menu End ---------------------- //

	//add simulation to root
	s.LoadString(IDS_TVNN_SIMULATION);
	CTVNode* pSimNode = new CTVNode(s);
	pSimNode->m_eState = CTVNode::expanded;
	m_tvRootNode.AddChild(pSimNode);
	//add settings node to sim node
	s.LoadString(IDS_TVNN_SIMSETTINGS);
	pNode = new CTVNode(s);
	pSimNode->AddChild(pNode);
	//add run to sim node
	s.LoadString(IDS_TVNN_RUNSIM);
	pNode = new CTVNode(s);
	pSimNode->AddChild(pNode);

	//add analysis params to root
	s.LoadString(IDS_TVNN_ANALYSISPARAMS);
	CTVNode* pAPNode = new CTVNode(s);
	pAPNode->m_eState = CTVNode::expanded;
	m_tvRootNode.AddChild(pAPNode);
	//add mobile elements to analysis params
	s.LoadString(IDS_TVNN_MOBILEDISPLAY);
	pNode = new CTVNode(s);
	pAPNode->AddChild(pNode);
	//add mobile elements tags to analysis params
	s.LoadString(IDS_TVNN_MOBILETAGS);
	pNode = new CTVNode(s);
	pAPNode->AddChild(pNode);
	//add resource elements display
	s.LoadString(IDS_TVNN_RESOURCEDISPLAY);
	pNode = new CTVNode(s);
	pAPNode->AddChild(pNode);
	//add processor tags to analysis params
	s.LoadString(IDS_TVNN_PROCESSORTAGS);
	pNode = new CTVNode(s);
	pAPNode->AddChild(pNode);
	//add dynamic load display to analysis params
	s.LoadString(IDS_TVNN_DYNAMICLOADDISPLAY);
	pNode = new CTVNode(s);
	pAPNode->AddChild(pNode);
	//add reports to analysis params
	s.LoadString(IDS_TVNN_REPORTS);
	CTVNode* pRPNode = new CTVNode(s);
	pRPNode->m_eState = CTVNode::collapsed;
	pAPNode->AddChild(pRPNode);
	//////////////////////////////////////////////////////////////////////////
	// add report param
	s.LoadString( IDS_TVNN_REP_PASSENGER );
	CTVNode* pRP_Pax_Node = new CTVNode(s);
	pRP_Pax_Node->m_eState = CTVNode::collapsed;
	pRPNode->AddChild( pRP_Pax_Node );

	s.LoadString( IDS_TVNN_REP_PAX_LOG );
	pNode = new CTVNode(s);
	pRP_Pax_Node->AddChild( pNode );

	s.LoadString( IDS_TVNN_REP_PAX_DISTANCE );
	pNode = new CTVNode(s);
	pRP_Pax_Node->AddChild( pNode );

	s.LoadString( IDS_TVNN_REP_PAX_TIMEINQ );
	pNode = new CTVNode(s);
	pRP_Pax_Node->AddChild( pNode );

	s.LoadString( IDS_TVNN_REP_PAX_TIMEINT );
	pNode = new CTVNode(s);
	pRP_Pax_Node->AddChild( pNode );

	s.LoadString( IDS_TVNN_REP_PAX_TIMEINS );
	pNode = new CTVNode(s);
	pRP_Pax_Node->AddChild( pNode );

	s.LoadString( IDS_TVNN_REP_PAX_BREAKDOWN );
	pNode = new CTVNode(s);
	pRP_Pax_Node->AddChild( pNode );

	s.LoadString( IDS_TVNN_REP_FIREEVACUTION );
	pNode = new CTVNode(s);
	pRP_Pax_Node->AddChild( pNode );

	s.LoadString( IDS_TVNN_REP_CONVEYOR_WAIT_TIME );
	pNode = new CTVNode(s);
	pRP_Pax_Node->AddChild( pNode );

	s.LoadString( IDS_TVNN_REP_PROCESSOR );
	CTVNode* pRP_Proc_Node = new CTVNode(s);
	pRP_Proc_Node->m_eState = CTVNode::collapsed;
	pRPNode->AddChild( pRP_Proc_Node );

	s.LoadString( IDS_TVNN_REP_PROC_UTILISATION );
	pNode = new CTVNode(s);
	pRP_Proc_Node->AddChild( pNode );

	s.LoadString( IDS_TVNN_REP_PROC_THROUGHPUT );
	pNode = new CTVNode(s);
	pRP_Proc_Node->AddChild( pNode );

	s.LoadString( IDS_TVNN_REP_PROC_QLENGTH );
	pNode = new CTVNode(s);
	pRP_Proc_Node->AddChild( pNode );

	s.LoadString( IDS_TVNN_REP_PROC_QUEUEPARAM );
	pNode = new CTVNode(s);
	pRP_Proc_Node->AddChild( pNode );

	s.LoadString( IDS_TVNN_REP_CONVEYOR_WAIT_LENGTH );
	pNode = new CTVNode(s);
	pRP_Proc_Node->AddChild( pNode );


	s.LoadString( IDS_TVNN_REP_SPACE );
	CTVNode* pRP_Space_Node = new CTVNode(s);
	pRP_Space_Node->m_eState = CTVNode::collapsed;
	pRPNode->AddChild( pRP_Space_Node );

	s.LoadString( IDS_TVNN_REP_SPACE_OCCUPANCY );
	pNode = new CTVNode(s);
	pRP_Space_Node->AddChild( pNode );

	s.LoadString( IDS_TVNN_REP_SPACE_DENSITY );
	pNode = new CTVNode(s);
	pRP_Space_Node->AddChild( pNode );

	s.LoadString( IDS_TVNN_REP_SPACE_THROUGHPUT );
	pNode = new CTVNode(s);
	pRP_Space_Node->AddChild( pNode );

	s.LoadString( IDS_TVNN_REP_SPACE_COLLISIONS );
	pNode = new CTVNode(s);
	pRP_Space_Node->AddChild( pNode );

	s.LoadString( IDS_TVNN_REP_AIRCRAFT );
	pNode = new CTVNode(s);
	pRPNode->AddChild( pNode );

	s.LoadString( IDS_TVNN_REP_BAG );
	CTVNode* pRP_Bag_Node = new CTVNode(s);
	pRP_Bag_Node->m_eState = CTVNode::collapsed;
	pRPNode->AddChild( pRP_Bag_Node );

	s.LoadString( IDS_TVNN_REP_BAG_COUNT );
	pNode = new CTVNode(s);
	pRP_Bag_Node->AddChild( pNode );

	s.LoadString( IDS_TVNN_REP_BAG_WAITTIME );
	pNode = new CTVNode(s);
	pRP_Bag_Node->AddChild( pNode );

	s.LoadString( IDS_TVNN_REP_BAG_DISTRIBUTION );
	pNode = new CTVNode(s);
	pRP_Bag_Node->AddChild( pNode );

	s.LoadString( IDS_TVNN_REP_BAG_DELIVERYTIME );
	pNode = new CTVNode(s);
	pRP_Bag_Node->AddChild( pNode );

	s.LoadString( IDS_TVNN_CALLOUT_DISP_SPEC );
	pNode = new CTVNode(s);
	pAPNode->AddChild( pNode );
}
void CTerminalMSImpl::RebuildProcessor2Tree()
{
	ASSERT(m_pTermPlanDoc != NULL);
	Terminal& terminal = m_pTermPlanDoc->GetTerminal();

	CTVNode* pUCNode = m_pTermPlanDoc->m_msManager.FindNodeByName(IDS_TVNN_UNDERCONSTRUCTION);
	if (!pUCNode)
		return;

	ASSERT(pUCNode);
	pUCNode->DeleteAllChildren();

	int nCount = m_pTermPlanDoc->GetTerminalPlacement()->m_vUndefined.getNodeCount();
	for (int i = 0; i < nCount; i++)
	{
		CNudefinePro2Node* pNodeLevel1 = NULL;
		pNodeLevel1 = m_pTermPlanDoc->GetTerminalPlacement()->m_vUndefined.getItem(i);

		CString strShapeName1 = pNodeLevel1->getNodeName();
		strShapeName1.TrimLeft();strShapeName1.TrimRight();
		if (strShapeName1.IsEmpty())
		{
			continue;
		}
		CTVNode* pShapeL1 = new CTVNode(strShapeName1);
		pUCNode->AddChild(pShapeL1);

		if (pNodeLevel1->getNodeCount() == 0)
		{
			pShapeL1->m_nIDResource = IDR_CTX_UCPROCESSOR;
			CProcessor2* pP2 = m_pTermPlanDoc->GetTerminalPlacement()->m_vUndefined.GetProc2(strShapeName1);
			ASSERT(pP2 != NULL);
			pShapeL1->m_dwData = (DWORD) pP2;
		}
		// level2
		for (int j = 0; j < pNodeLevel1->getNodeCount(); j++)
		{
			CNudefinePro2Node* pNodeLevel2 = NULL;
			pNodeLevel2 = pNodeLevel1->getItem(j);


			CString strShapeName2 = strShapeName1 + CString("-") + pNodeLevel2->getNodeName();
			strShapeName2.TrimLeft();strShapeName2.TrimRight();
			if (strShapeName2.IsEmpty())
			{
				continue;
			}

			CTVNode* pShapeL2 = new CTVNode(pNodeLevel2->getNodeName());
			pShapeL1->AddChild(pShapeL2);

			if(pNodeLevel2->getNodeCount() == 0) {
				pShapeL2->m_nIDResource = IDR_CTX_UCPROCESSOR;
				//get CProcessor2* associated with this id
				CProcessor2* pP2 = m_pTermPlanDoc->GetTerminalPlacement()->m_vUndefined.GetProc2(strShapeName2);
				if (pP2 == NULL)
				{
					pShapeL1->RemoveChild(pShapeL2);
					continue;
				}
				ASSERT(pP2 != NULL);
				pShapeL2->m_dwData = (DWORD) pP2;
			}
			//level3
			for (int n = 0; n < pNodeLevel2->getNodeCount(); n++)
			{
				CNudefinePro2Node* pNodeLevel3 = NULL;
				pNodeLevel3 = pNodeLevel2->getItem(n);

				CString strShapeName3 = strShapeName2 + CString("-") + pNodeLevel3->getNodeName();
				strShapeName3.TrimLeft();strShapeName3.TrimRight();
				if (strShapeName3.IsEmpty())
				{
					continue;
				}

				CTVNode* pShapeL3 = new CTVNode(pNodeLevel3->getNodeName());
				pShapeL2->AddChild(pShapeL3);

				if(pNodeLevel3->getNodeCount() == 0) {
					pShapeL3->m_nIDResource = IDR_CTX_UCPROCESSOR;
					//get CProcessor2* associated with this id
					CProcessor2* pP2 = m_pTermPlanDoc->GetTerminalPlacement()->m_vUndefined.GetProc2(strShapeName3);
					if (pP2 == NULL)
					{
						pShapeL2->RemoveChild(pShapeL3);
						continue;
					}
					ASSERT(pP2 != NULL);
					pShapeL3->m_dwData = (DWORD) pP2;
				}
				//level 4
				for (int m = 0; m < pNodeLevel3->getNodeCount(); m++)
				{		
					CNudefinePro2Node* pNodeLevel4 = NULL;
					pNodeLevel4 = pNodeLevel3->getItem(m);
					CString strShapeName4 = strShapeName3 + CString("-") + pNodeLevel4->getNodeName();
					strShapeName4.TrimLeft();strShapeName4.TrimRight();
					if (strShapeName4.IsEmpty())
					{
						continue;
					}

					CTVNode* pShapeL4 = new CTVNode( pNodeLevel4->getNodeName(),IDR_CTX_UCPROCESSOR);
					pShapeL3->AddChild(pShapeL4);


					CProcessor2* pP2 = m_pTermPlanDoc->GetTerminalPlacement()->m_vUndefined.GetProc2(strShapeName4);
					if (pP2 == NULL)
					{
						pShapeL3->RemoveChild(pShapeL4);
						continue;
					}
					ASSERT(pP2 != NULL);
					pShapeL4->m_dwData = (DWORD) pP2;;
				}
			}
		}
	}
}
void CTerminalMSImpl::RebuildProcessorTree(const ProcessorID* _pIdToSelect)
{
	ASSERT(m_pTermPlanDoc != NULL);
	Terminal& terminal = m_pTermPlanDoc->GetTerminal();

	CTVNode* pPlacementsNode = m_pTermPlanDoc->m_msManager.FindNodeByName(IDS_TVNN_PLACEMENTS);
	if (!pPlacementsNode)
		return;

	ASSERT(pPlacementsNode);
	pPlacementsNode->DeleteAllChildren();

	ProcessorID id, *pid = NULL;
	id.SetStrDict(terminal.inStrDict);

	int i;
	CString sID;
	StringList strListL1;
	terminal.procList->getAllGroupNames (strListL1, -1);
	terminal.procList->getAllGroupNames (strListL1, BarrierProc);
	//level 1
	for(i = 0; i < strListL1.getCount(); i++) {
		CString strTemp1 = strListL1.getString(i);
		strTemp1.TrimLeft(); strTemp1.TrimRight();
		if (strTemp1.IsEmpty())
			continue;

		CTVNode* pL1Node = new CTVNode(strTemp1, IDR_CTX_PROCESSORGROUP_WITH_CHANGETYPE);
		pPlacementsNode->AddChild(pL1Node);

		sID = strTemp1;
		id.setID((LPCSTR) sID);

		if(_pIdToSelect != NULL && *_pIdToSelect == id) {
			//TODO: SelectNode
		}

		StringList strListL2;
		terminal.procList->getMemberNames( id, strListL2, -1 );
		terminal.procList->getMemberNames( id, strListL2, BarrierProc );
		if(strListL2.IsEmpty()) {
			pL1Node->m_nIDResource = IDR_CTX_PROCESSOR;
			//get CProcessor2* associated with this id
			CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_Terminal);
			ASSERT(pP2 != NULL);
			pL1Node->m_dwData = (DWORD) pP2;
		}

		// Level 2
		for( int j = 0; j < strListL2.getCount(); j++ ) {
			CString strTemp2 = strListL2.getString(j);
			strTemp2.TrimLeft();  strTemp2.TrimRight();
			if (strTemp2.IsEmpty())
				continue;

			//CTVNode* pL2Node = new CTVNode(strTemp1+"-"+strTemp2);
			CTVNode* pL2Node = new CTVNode(strTemp2, IDR_CTX_PROCESSORGROUP);
			pL1Node->AddChild(pL2Node);

			sID = strTemp1+"-"+strTemp2;
			id.setID((LPCSTR) sID);

			if(_pIdToSelect != NULL && *_pIdToSelect == id) {
				//TODO: SelectNode
			}

			StringList strListL3;
			terminal.procList->getMemberNames( id, strListL3, -1 );
			terminal.procList->getMemberNames( id, strListL3, BarrierProc );
			if(strListL3.IsEmpty()) {
				pL2Node->m_nIDResource = IDR_CTX_PROCESSOR;
				//get CProcessor2* associated with this id
				CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_Terminal);
				ASSERT(pP2 != NULL);
				pL2Node->m_dwData = (DWORD) pP2;
			}

			// Level 3
			for( int k = 0; k < strListL3.getCount(); k++ ) {
				CString strTemp3 = strListL3.getString(k);
				strTemp3.TrimLeft(); strTemp3.TrimRight();
				if (strTemp3.IsEmpty())
					continue;

				//CTVNode* pL3Node = new CTVNode(strTemp1+"-"+strTemp2+"-"+strTemp3);
				CTVNode* pL3Node = new CTVNode(strTemp3, IDR_CTX_PROCESSORGROUP);
				pL2Node->AddChild(pL3Node);

				sID = strTemp1+"-"+strTemp2+"-"+strTemp3;
				id.setID((LPCSTR) sID);

				if(_pIdToSelect != NULL && *_pIdToSelect == id) {
					//TODO: SelectNode
				}

				StringList strListL4;
				terminal.procList->getMemberNames( id, strListL4, -1 );
				terminal.procList->getMemberNames( id, strListL4, BarrierProc );
				if(strListL4.IsEmpty()) {
					pL3Node->m_nIDResource = IDR_CTX_PROCESSOR;
					//get CProcessor2* associated with this id
					CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_Terminal);
					ASSERT(pP2 != NULL);
					pL3Node->m_dwData = (DWORD) pP2;
				}

				// Level 4
				for( int l = 0; l < strListL4.getCount(); l++ ) {
					CString strTemp4 = strListL4.getString(l);
					strTemp4.TrimLeft(); strTemp4.TrimRight();
					if (strTemp4.IsEmpty())
						continue;

					//CTVNode* pL4Node = new CTVNode(strTemp1+"-"+strTemp2+"-"+strTemp3+"-"+strTemp4, IDR_CTX_PROCESSOR);
					CTVNode* pL4Node = new CTVNode(strTemp4, IDR_CTX_PROCESSOR);
					pL3Node->AddChild(pL4Node);
					//get CProcessor2* associated with this id
					sID = strTemp1+"-"+strTemp2+"-"+strTemp3+"-"+strTemp4;
					id.setID((LPCSTR) sID);
					CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_Terminal);
					ASSERT(pP2 != NULL);
					pL4Node->m_dwData = (DWORD) pP2;


					if(_pIdToSelect != NULL && *_pIdToSelect == id) {
						//TODO: SelectNode
					}
				}
			}
		}
	}
}


/************************************************************************
FUNCTION: add WallShape to node view
IN		: pGroupNode,the parent node of the node added(NULL DEFAULT)
OUT		:                                                                
/************************************************************************/
void CTerminalMSImpl::AddWallShapeGroup(CTVNode* pGroupNode /* = NULL */)
{
	ASSERT(m_pTermPlanDoc != NULL);
	if (pGroupNode == NULL)
	{
		CString strProcTypeName;
		if (strProcTypeName.LoadString(IDS_TVNN_WALLSHAPE))
			pGroupNode =  m_pTermPlanDoc->m_msManager.FindNodeByName(strProcTypeName);
	}
	ASSERT(pGroupNode != NULL);

	Terminal& terminal = m_pTermPlanDoc->GetTerminal();
	pGroupNode->DeleteAllChildren();
	ProcessorID id, *pid = NULL;
	id.SetStrDict(terminal.inStrDict);
	CString sID;
	StringList strListL1;
	m_pTermPlanDoc->GetWallShapeListByMode(EnvMode_Terminal).getAllGroupNames(strListL1);

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
		m_pTermPlanDoc->GetWallShapeListByMode(EnvMode_Terminal).getMemberNames(id, strListL2);
		//		m_pTermPlanDoc->GetCurWallShapeList(EnvMode_AirSide).getMemberNames(id, strListL2);
		if(strListL2.IsEmpty()) 
		{
			pL1Node->m_nIDResource = IDR_CTX_WALLSHAPE;
			//get CProcessor2* associated with this id
			WallShape * pWallShap = m_pTermPlanDoc->GetWallShapeListByMode(EnvMode_Terminal).getWallShape(id);
			ASSERT(pWallShap != NULL);
			pL1Node->m_dwData = (DWORD) pWallShap;
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
			CTVNode* pL2Node = new CTVNode(strTemp2);
			pL1Node->AddChild(pL2Node);
			sID = strTemp1+"-"+strTemp2;
			id.setID((LPCSTR) sID);
			StringList strListL3;
			//			terminal.GetProcessorList(EnvMode_AirSide)->getMemberNames( id, strListL3, -1 );
			m_pTermPlanDoc->GetWallShapeListByMode(EnvMode_Terminal).getMemberNames(id, strListL3);
			//			terminal.GetProcessorList(EnvMode_AirSide)->getMemberNames( id, strListL3, RunwayProcessor );
			if(strListL3.IsEmpty())
			{
				pL2Node->m_nIDResource = IDR_CTX_WALLSHAPE;
				//get CProcessor2* associated with this id
				//				CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_AirSide);
				WallShape * pWallShap = m_pTermPlanDoc->GetWallShapeListByMode(EnvMode_Terminal).getWallShape(id);
				ASSERT(pWallShap != NULL);
				pL2Node->m_dwData = (DWORD) pWallShap;
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
				CTVNode* pL3Node = new CTVNode(strTemp3);
				pL2Node->AddChild(pL3Node);
				sID = strTemp1+"-"+strTemp2+"-"+strTemp3;
				id.setID((LPCSTR) sID);
				StringList strListL4;
				//				terminal.GetProcessorList(EnvMode_AirSide)->getMemberNames( id, strListL4, -1 );
				m_pTermPlanDoc->GetWallShapeListByMode(EnvMode_Terminal).getMemberNames(id, strListL4);
				//				terminal.GetProcessorList(EnvMode_AirSide)->getMemberNames( id, strListL4, RunwayProcessor );

				if(strListL4.IsEmpty())
				{
					pL3Node->m_nIDResource = IDR_CTX_WALLSHAPE;
					//get CProcessor2* associated with this id
					//					CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_AirSide);
					WallShape * pWallShap = m_pTermPlanDoc->GetWallShapeListByMode(EnvMode_Terminal).getWallShape(id);
					ASSERT(pWallShap != NULL);
					pL3Node->m_dwData = (DWORD) pWallShap;
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
					CTVNode* pL4Node = new CTVNode(strTemp4, IDR_CTX_WALLSHAPE);
					pL3Node->AddChild(pL4Node);
					//get CProcessor2* associated with this id
					sID = strTemp1+"-"+strTemp2+"-"+strTemp3+"-"+strTemp4;
					id.setID((LPCSTR) sID);
					WallShape * pWallShap = m_pTermPlanDoc->GetWallShapeListByMode(EnvMode_Terminal).getWallShape(id);
					//					CProcessor2* pWallShap = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_AirSide);
					ASSERT(pWallShap != NULL);
					pL4Node->m_dwData = (DWORD) pWallShap;
				}
			}
		}
	}
	m_pTermPlanDoc->UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pGroupNode);
}
/************************************************************************
FUNCTION: add WallShape to node view
IN		: pGroupNode,the parent node of the node added(NULL DEFAULT)
OUT		:                                                                
/************************************************************************/
void CTerminalMSImpl::AddStructureGroup(CTVNode* pGroupNode /* = NULL */)
{
	ASSERT(m_pTermPlanDoc != NULL);
	if (pGroupNode == NULL)
	{
		CString strProcTypeName;
		if (strProcTypeName.LoadString(IDS_TVNN_SURFACEAREAS))
			pGroupNode =  m_pTermPlanDoc->m_msManager.FindNodeByName(strProcTypeName);
	}
	ASSERT(pGroupNode != NULL);

	Terminal& terminal = m_pTermPlanDoc->GetTerminal();
	pGroupNode->DeleteAllChildren();
	ProcessorID id, *pid = NULL;
	id.SetStrDict(terminal.inStrDict);
	CString sID;
	StringList strListL1;
	m_pTermPlanDoc->GetStructurelistByMode(EnvMode_Terminal).getAllGroupNames(strListL1);

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
		m_pTermPlanDoc->GetStructurelistByMode(EnvMode_Terminal).getMemberNames(id, strListL2);
		//		m_pTermPlanDoc->GetCurWallShapeList(EnvMode_AirSide).getMemberNames(id, strListL2);
		if(strListL2.IsEmpty()) 
		{
			pL1Node->m_nIDResource = IDR_CTX_STRUCTURE;
			//get CProcessor2* associated with this id
			CStructure * pStructure = m_pTermPlanDoc->GetStructurelistByMode(EnvMode_Terminal).getStructure(id);
			ASSERT(pStructure != NULL);
			pL1Node->m_dwData = (DWORD) pStructure;
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
			CTVNode* pL2Node = new CTVNode(strTemp2);
			pL1Node->AddChild(pL2Node);
			sID = strTemp1+"-"+strTemp2;
			id.setID((LPCSTR) sID);
			StringList strListL3;
			//			terminal.GetProcessorList(EnvMode_AirSide)->getMemberNames( id, strListL3, -1 );
			m_pTermPlanDoc->GetStructurelistByMode(EnvMode_Terminal).getMemberNames(id, strListL3);
			//			terminal.GetProcessorList(EnvMode_AirSide)->getMemberNames( id, strListL3, RunwayProcessor );
			if(strListL3.IsEmpty())
			{
				pL2Node->m_nIDResource = IDR_CTX_STRUCTURE;
				//get CProcessor2* associated with this id
				//				CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_AirSide);
				CStructure * pStructure = m_pTermPlanDoc->GetStructurelistByMode(EnvMode_Terminal).getStructure(id);
				ASSERT(pStructure != NULL);
				pL2Node->m_dwData = (DWORD) pStructure;
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
				CTVNode* pL3Node = new CTVNode(strTemp3);
				pL2Node->AddChild(pL3Node);
				sID = strTemp1+"-"+strTemp2+"-"+strTemp3;
				id.setID((LPCSTR) sID);
				StringList strListL4;
				//				terminal.GetProcessorList(EnvMode_AirSide)->getMemberNames( id, strListL4, -1 );
				m_pTermPlanDoc->GetStructurelistByMode(EnvMode_Terminal).getMemberNames(id, strListL4);
				//				terminal.GetProcessorList(EnvMode_AirSide)->getMemberNames( id, strListL4, RunwayProcessor );

				if(strListL4.IsEmpty())
				{
					pL3Node->m_nIDResource = IDR_CTX_STRUCTURE;
					//get CProcessor2* associated with this id
					//					CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_AirSide);
					CStructure * pStructure = m_pTermPlanDoc->GetStructurelistByMode(EnvMode_Terminal).getStructure(id);
					ASSERT(pStructure != NULL);
					pL3Node->m_dwData = (DWORD) pStructure;
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
					CTVNode* pL4Node = new CTVNode(strTemp4, IDR_CTX_STRUCTURE);
					pL3Node->AddChild(pL4Node);
					//get CProcessor2* associated with this id
					sID = strTemp1+"-"+strTemp2+"-"+strTemp3+"-"+strTemp4;
					id.setID((LPCSTR) sID);
					CStructure * pStructure = m_pTermPlanDoc->GetStructurelistByMode(EnvMode_Terminal).getStructure(id);
					//					CProcessor2* pWallShap = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_AirSide);
					ASSERT(pStructure != NULL);
					pL4Node->m_dwData = (DWORD) pStructure;
				}
			}
		}
	}
	m_pTermPlanDoc->UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pGroupNode);
}
