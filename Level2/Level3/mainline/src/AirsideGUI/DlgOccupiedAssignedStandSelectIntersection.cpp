// DlgOccupiedAssignedSelectIntersection.cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "Resource.h"
#include "DlgOccupiedAssignedStandSelectIntersection.h"
#include ".\dlgoccupiedassignedstandselectintersection.h"
#include "..\InputAirside\HoldPosition.h"
#include "..\InputAirside\Taxiway.h"
#include "InputAirside/AirportGroundNetwork.h"
// CDlgOccupiedSelectIntersection dialog

IMPLEMENT_DYNAMIC(CDlgOccupiedSelectIntersection, CDialog)
CDlgOccupiedSelectIntersection::CDlgOccupiedSelectIntersection(CAirportGroundNetwork* pAltNetwork, 
															   std::vector<Taxiway>&vectTaxiway,
															   AltObjectVectorMap* pTaxiwayVectorMap, 
															   int intersectionID,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOccupiedSelectIntersection::IDD, pParent)
	,m_pTaxiwayVectorMap(pTaxiwayVectorMap)
	,m_pAltNetwork(pAltNetwork)
	,m_vectTaxiway(vectTaxiway)
	,m_nIntersectionID(intersectionID)
{
}

CDlgOccupiedSelectIntersection::~CDlgOccupiedSelectIntersection()
{
	for (size_t i = 0 ; i < m_vectTaxiwayData.size(); i++)
		delete m_vectTaxiwayData.at(i);
	m_vectTaxiwayData.clear();
}

void CDlgOccupiedSelectIntersection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_INTERSECTIONTAXIWAY1, m_wndTaxiwayA);
	DDX_Control(pDX, IDC_COMBO_INTERSECTIONTAXIWAY2, m_wndTaxiwayB);
	//DDX_Control(pDX, IDC_COMBO_INTERSECTIONINDEX, m_ComboIntersectionIndex);
}


BEGIN_MESSAGE_MAP(CDlgOccupiedSelectIntersection, CDialog)
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_COMBO_INTERSECTIONTAXIWAY1, OnCbnSelchangeTaxiwayA)
	ON_CBN_SELCHANGE(IDC_COMBO_INTERSECTIONTAXIWAY2, OnCbnSelchangeTaxiwayB)
	ON_CBN_SELCHANGE(IDC_COMBO_INTERSECTIONINDEX, OnCbnSelchangeComboIntersectionindex)
END_MESSAGE_MAP()


// CDlgOccupiedSelectIntersection message handlers

BOOL CDlgOccupiedSelectIntersection::OnInitDialog()
{
	CDialog::OnInitDialog();

	ASSERT( m_pAltNetwork != NULL );

	if(m_nIntersectionID!=-1)
	{
		IntersectionNode intersectNode;
		intersectNode.ReadData(m_nIntersectionID);

		//CString strIntersectionTaxiway1Name,strIntersectionTaxiway2Name;
		int taxiwayAID,taxiwayBID;
		taxiwayAID=taxiwayBID=-1;		
		int nFlag = 0;
		AltObjectVectorMapIter iter = m_pTaxiwayVectorMap->begin();
		for(; iter != m_pTaxiwayVectorMap->end(); iter++)
		{
			AltObjectVector& vec = iter->second;
			for(AltObjectVectorIter it = vec.begin();it != vec.end(); it++)
			{
				if( intersectNode.HasObject(it->second) )
				{
					nFlag++;
					if (nFlag==1)
					{
						//strIntersectionTaxiway1Name = it->first;
						taxiwayAID=it->second;
					}else
					{
						//strIntersectionTaxiway2Name = it->first;
						taxiwayBID=it->second;
					}

					if(nFlag == 2)
						break;
				}				
			}
			if(nFlag == 2)
				break;
		}

		
		size_t nCount = m_vectTaxiway.size();
		for(size_t i =0; i < nCount; i++)
		{
			Taxiway& aTaxiway = m_vectTaxiway.at(i);
			
			int nIndex = m_wndTaxiwayA.AddString(aTaxiway.GetMarking().c_str());

			m_wndTaxiwayA.SetItemData( nIndex, (DWORD_PTR)&aTaxiway);
		}

		int taxiwayAIndex,taxiwayBIndex;
		Taxiway* pTmpTaxiwayA;
		for (taxiwayAIndex=0;taxiwayAIndex<m_wndTaxiwayA.GetCount();taxiwayAIndex++)
		{
			pTmpTaxiwayA = (Taxiway*)m_wndTaxiwayA.GetItemData( taxiwayAIndex );
			if (pTmpTaxiwayA->getID()==taxiwayAID)
			{
				break;
			}
		}
		if(taxiwayAIndex> 0)
		{
			m_wndTaxiwayA.SetCurSel( taxiwayAIndex );
		}else
		{
			m_wndTaxiwayA.SetCurSel( 0 );
		}

		{
			int nIndex = m_wndTaxiwayA.GetCurSel();
			if(nIndex == LB_ERR)
				return true;

			Taxiway* pTaxiwayA = (Taxiway*)m_wndTaxiwayA.GetItemData( nIndex );
			ASSERT( pTaxiwayA != NULL );

			m_wndTaxiwayB.ResetContent();

			std::set<ALTObject*> vTaxiways;
			std::set<ALTObject *> vRunways;
			m_pAltNetwork->GetIntersectedTaxiwayRunway((ALTObject*)pTaxiwayA,
				vTaxiways, vRunways);

			
			for (std::set<ALTObject*>::iterator iterTaxiway = vTaxiways.begin(); 
				iterTaxiway != vTaxiways.end(); ++iterTaxiway)
			{
				ALTObject* pObj = (*iterTaxiway);
				if (pObj->GetType() != ALT_TAXIWAY)
					continue;
				Taxiway* pTaxiway = (Taxiway*)pObj;

				IntersectionNodeList twINodeList;
				m_pAltNetwork->GetTaxiwayTaxiwayIntersectNodeList(pTaxiwayA,
					(Taxiway*)(*iterTaxiway) , twINodeList);

				if (twINodeList.empty())
					continue;

				if(twINodeList.size() == 1)
				{
					TaxiwayBData* pNewData = new TaxiwayBData;
					pNewData->bJustOneIntersection = true;
					pNewData->pTaxiway = pTaxiway;
					pNewData->nNodeIndex = twINodeList[0].GetIndex();
					
					m_vectTaxiwayData.push_back( pNewData );
					int nIndex = m_wndTaxiwayB.AddString( pTaxiway->GetMarking().c_str());

					m_wndTaxiwayB.SetItemData( nIndex, (DWORD_PTR)pNewData);
					continue;

				}
				else
				{
					for (int i=0; i<(int)twINodeList.size(); i++)
					{
						TaxiwayBData* pNewData = new TaxiwayBData;
						pNewData->bJustOneIntersection = false;
						pNewData->pTaxiway = pTaxiway;
						pNewData->nNodeIndex = twINodeList[i].GetIndex();
						CString strObjMarkTemp = pTaxiway->GetMarking().c_str();
						CString strObjMark;
						strObjMark.Format("%s(%s&%d)",strObjMarkTemp,
							pTaxiwayA->GetMarking().c_str(),pNewData->nNodeIndex);
						int nIndex = m_wndTaxiwayB.AddString( strObjMark);
// 						if (pTaxiway->getID()==taxiwayBID && intersectNode.GetIndex()==pNewData->nNodeIndex)
// 						{
// 							taxiwayBIndex=nIndex;
// 						}
						m_wndTaxiwayB.SetItemData( nIndex, (DWORD_PTR)pNewData);
					}
				}

			}

			TaxiwayBData* pTmpData;
			for (taxiwayBIndex=0;taxiwayBIndex<m_wndTaxiwayB.GetCount();taxiwayBIndex++)
			{
				pTmpData = (TaxiwayBData*)m_wndTaxiwayB.GetItemData( taxiwayBIndex );
				if (pTmpData->pTaxiway->getID()==taxiwayBID && intersectNode.GetIndex()==pTmpData->nNodeIndex)
				{
					break;
				}
			}


			if(taxiwayBIndex>0)
			{
				m_wndTaxiwayB.SetCurSel( taxiwayBIndex);
			}else
			{
				m_wndTaxiwayB.SetCurSel( 0);
			}			
		}

	}else
	{
		size_t nCount = m_vectTaxiway.size();
		for(size_t i =0; i < nCount; i++)
		{
			Taxiway& aTaxiway = m_vectTaxiway.at(i);
			int nIndex = m_wndTaxiwayA.AddString(aTaxiway.GetMarking().c_str());
			m_wndTaxiwayA.SetItemData( nIndex, (DWORD_PTR)&aTaxiway);
		}


		if(nCount > 0)
		{
			m_wndTaxiwayA.SetCurSel( 0 );
		}

	}

	

	updateUIState();

//	AltObjectVectorMapIter iter = m_pTaxiwayVectorMap->begin();
//	for(; iter != m_pTaxiwayVectorMap->end(); iter++)
//	{
////		CString strAirportName = iter->first;
//
//		AltObjectVector& vec = iter->second;
//		for(AltObjectVectorIter it = vec.begin();it != vec.end(); it++)
//		{
//			CString strIntersectionTaxiway1Name = it->first;
//			int nIntersectionTaxiway1ID = it->second;
//			int nIndex = m_wndTaxiwayA.AddString(strIntersectionTaxiway1Name);
//			m_wndTaxiwayA.SetItemData(nIndex,nIntersectionTaxiway1ID);
//		}
//	}
	return TRUE;
}
void CDlgOccupiedSelectIntersection::OnSize(UINT nType, int cx, int cy)
{

}
void CDlgOccupiedSelectIntersection::updateUIState()
{
	BOOL bEnable = (m_wndTaxiwayA.GetCurSel()!=LB_ERR && m_wndTaxiwayB.GetCurSel() != LB_ERR);

	GetDlgItem(IDOK)->EnableWindow(bEnable);
}

CString CDlgOccupiedSelectIntersection::GetIntersectionName()
{
	return m_strIntName;


}

void CDlgOccupiedSelectIntersection::OnCbnSelchangeTaxiwayA()
{
	int nIndex = m_wndTaxiwayA.GetCurSel();
	if(nIndex == LB_ERR)
		return;

	Taxiway* pTaxiwayA = (Taxiway*)m_wndTaxiwayA.GetItemData( nIndex );
	ASSERT( pTaxiwayA != NULL );

	m_wndTaxiwayB.ResetContent();

	std::set<ALTObject*> vTaxiways;
	std::set<ALTObject *> vRunways;
	m_pAltNetwork->GetIntersectedTaxiwayRunway((ALTObject*)pTaxiwayA,
		vTaxiways, vRunways);

	for (std::set<ALTObject*>::iterator iterTaxiway = vTaxiways.begin(); 
		iterTaxiway != vTaxiways.end(); ++iterTaxiway)
	{
		ALTObject* pObj = (*iterTaxiway);
		if (pObj->GetType() != ALT_TAXIWAY)
			continue;
		Taxiway* pTaxiway = (Taxiway*)pObj;

		IntersectionNodeList twINodeList;
		m_pAltNetwork->GetTaxiwayTaxiwayIntersectNodeList(pTaxiwayA,
			(Taxiway*)(*iterTaxiway) , twINodeList);

		if (twINodeList.empty())
			continue;

		if(twINodeList.size() == 1)
		{
			TaxiwayBData* pNewData = new TaxiwayBData;
			pNewData->bJustOneIntersection = true;
			pNewData->pTaxiway = pTaxiway;
			pNewData->nNodeIndex = twINodeList[0].GetIndex();
			m_vectTaxiwayData.push_back( pNewData );
			int nIndex = m_wndTaxiwayB.AddString( pTaxiway->GetMarking().c_str());
			m_wndTaxiwayB.SetItemData( nIndex, (DWORD_PTR)pNewData);
			continue;

		}
		else
		{
			for (int i=0; i<(int)twINodeList.size(); i++)
			{
				TaxiwayBData* pNewData = new TaxiwayBData;
				pNewData->bJustOneIntersection = false;
				pNewData->pTaxiway = pTaxiway;
				pNewData->nNodeIndex = twINodeList[i].GetIndex();
				CString strObjMarkTemp = pTaxiway->GetMarking().c_str();
				CString strObjMark;
				strObjMark.Format("%s(%s&%d)",strObjMarkTemp,
					pTaxiwayA->GetMarking().c_str(),pNewData->nNodeIndex);
				int nIndex = m_wndTaxiwayB.AddString( strObjMark);
				m_wndTaxiwayB.SetItemData( nIndex, (DWORD_PTR)pNewData);
			}
		}

	}

	m_wndTaxiwayB.SetCurSel( 0);

	updateUIState();

	



	//m_nIntersectionTaxiway1ID = m_wndTaxiwayA.GetItemData(nIndex);
 //   m_wndTaxiwayA.GetLBText(nIndex,m_strIntersectionTaxiway1Name);
	//m_wndTaxiwayB.ResetContent();
	//m_ComboIntersectionIndex.ResetContent();
	//GetTaxiway2AndIndexByTaxiway1ID(m_nIntersectionTaxiway1ID);
}

void CDlgOccupiedSelectIntersection::OnCbnSelchangeTaxiwayB()
{
	updateUIState();

	// TODO: Add your control notification handler code here
	//int nIndex = m_wndTaxiwayB.GetCurSel();
	//m_nIntersectionTaxiway2ID = m_wndTaxiwayB.GetItemData(nIndex);
	//m_wndTaxiwayB.GetLBText(nIndex,m_strIntersectionTaxiway2Name);
	//m_ComboIntersectionIndex.ResetContent();
	//GetIntersectionIndexAndIDByTaxiway2ID(m_nIntersectionTaxiway2ID);
}

void CDlgOccupiedSelectIntersection::OnCbnSelchangeComboIntersectionindex()
{
	// TODO: Add your control notification handler code here
//	int nIndex = m_ComboIntersectionIndex.GetCurSel();
////	m_nIntersectionIndex = m_ComboIntersectionIndex.GetItemData(nIndex);
//	IntersectionNode* pItem = (IntersectionNode*)m_ComboIntersectionIndex.GetItemData(nIndex);
//	m_nIntersectionIndex = pItem->GetIndex() + 1;
//	m_nIntersectionID = pItem ->GetID();
}
void CDlgOccupiedSelectIntersection::GetTaxiway2AndIndexByTaxiway1ID(int nTaxiway1ID)
{
	////std::vector<IntersectionNode> vNodeListAll;

	////int nCurNodeCount;
	////if (nTaxiway1ID == -1)	// have no taxiway
	////{
	////	m_vNodeList.clear();
	////	return;
	////}

	////Taxiway taxiway;
	////taxiway.ReadObject(nTaxiway1ID);
	////vNodeListAll.clear();
	////IntersectionNode::ReadIntersecNodeList(nTaxiway1ID, vNodeListAll);
	////m_vNodeList.clear();
	////std::vector<IntersectionNode>::iterator iter = vNodeListAll.begin();
	////for (; iter != vNodeListAll.end(); ++iter)
	////{
	////	/*ALTObject * pObj  = (*iter).GetOtherObject(nTaxiway1ID);
	////	if(pObj && pObj->GetType() == ALT_TAXIWAY)*/
	////	if( iter->HasObject(nTaxiway1ID) && iter->GetTaxiwayIntersectItemList().size()>1 )
	////	{
	////		m_vNodeList.push_back((*iter));
	////	}
	////}

	////nCurNodeCount = m_vNodeList.size();
	////if (nCurNodeCount <= 0)
	////	return;

	////m_strListTaxiway2.RemoveAll();
	////iter = m_vNodeList.begin();
	////for (; iter != m_vNodeList.end(); ++iter)
	////{
	////	/*CString strObjName;
	////	ALTObject * pObj  = (*iter).GetOtherObject(nTaxiway1ID);
	////	if(pObj && pObj->GetType() == ALT_TAXIWAY)
	////	{
	////		strObjName = ((Taxiway*)(pObj))->GetMarking().c_str();
	////	}		*/
	////	std::vector<TaxiwayIntersectItem*> vTaxiwayItems = iter->GetTaxiwayIntersectItemList();
	////	for(int itaxiIdx=0;itaxiIdx<(int)vTaxiwayItems.size();itaxiIdx++)
	////	{
	////		TaxiwayIntersectItem* taxiItem  = vTaxiwayItems[itaxiIdx];
	////		Taxiway * pOtherTaxiway = taxiItem->GetTaxiway();
	////		if(pOtherTaxiway && pOtherTaxiway->getID()!= nTaxiway1ID)
	////		{
	////			CString strObjName = pOtherTaxiway->GetMarking().c_str();
	////			if(!IsInStrListTaxiway2(strObjName))
	////			{
	////				m_strListTaxiway2.InsertAfter(m_strListTaxiway2.GetTailPosition(), strObjName);
	////				int nIndex = m_wndTaxiwayB.AddString(strObjName);
	////				m_wndTaxiwayB.SetItemData(nIndex,pOtherTaxiway->getID());					
	////			}
	////		}
	////	}
	////	
	////	
	////}
}
void CDlgOccupiedSelectIntersection::GetIntersectionIndexAndIDByTaxiway2ID(int nTaxiway2ID)
{	
	//std::vector<IntersectionNode>::iterator iter = m_vNodeList.begin();
	//for (; iter != m_vNodeList.end(); ++iter)
	//{
	//	IntersectionNode& node = *iter;
	//	if( node.HasObject(nTaxiway2ID) )
	//	{
	//		CString strIndex;
	//		strIndex.Format("%d",((*iter).GetIndex())+1);
	//		int nIndex = m_ComboIntersectionIndex.AddString(strIndex);
	//	//	m_ComboIntersectionIndex.SetItemData(nIndex,((*iter).m_idx)+1);
	//		m_ComboIntersectionIndex.SetItemData(nIndex,(DWORD_PTR)(&(*iter)));
	//	}		
	//}
}
bool CDlgOccupiedSelectIntersection::IsInStrListTaxiway2(CString str)
{
	//if((m_strListTaxiway2.Find(str)) == NULL)
	//	return false;
	//else
		return true;
}
void CDlgOccupiedSelectIntersection::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	if(m_wndTaxiwayA.GetCurSel() == -1 || m_wndTaxiwayB.GetCurSel() == -1 /*|| m_ComboIntersectionIndex.GetCurSel() == -1*/)
	{
		AfxMessageBox("Please select IntersectionTaxiway1 and IntersectionTaxiway2 and IntersectionIndex.");
		return;
	}

	int nIndex = m_wndTaxiwayB.GetCurSel();
	if(nIndex == LB_ERR)
		return;
	TaxiwayBData* taxiData = (TaxiwayBData*)m_wndTaxiwayB.GetItemData( nIndex );
	ASSERT( taxiData != NULL );
	Taxiway* pTaxiwayB = taxiData->pTaxiway;

	std::vector<IntersectionNode> vNodeList;
	IntersectionNode::ReadIntersecNodeList(pTaxiwayB->getID(), vNodeList);

	std::vector<IntersectionNode>::iterator iter = vNodeList.begin();
	Taxiway* pTaxiwayA = (Taxiway*)m_wndTaxiwayA.GetItemData( m_wndTaxiwayA.GetCurSel());
	for (; iter != vNodeList.end(); ++iter)
	{
		IntersectionNode& node = *iter;
		if( node.HasObject(pTaxiwayA->getID()) && (*iter).GetIndex() ==taxiData->nNodeIndex )
		{
			m_nIntersectionID = node.GetID();
			break;
		}		
	}

	
	if(taxiData->bJustOneIntersection)
		m_strIntName.Format("%s(%s)",
		pTaxiwayB->GetMarking().c_str(), pTaxiwayA->GetMarking().c_str());
	else
		m_strIntName.Format("%s(%s&%d)",
		pTaxiwayB->GetMarking().c_str(), pTaxiwayA->GetMarking().c_str(),
		taxiData->nNodeIndex);





	CDialog::OnOK();
}
