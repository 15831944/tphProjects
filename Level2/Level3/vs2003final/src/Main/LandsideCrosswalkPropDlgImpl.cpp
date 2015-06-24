#include "stdafx.h"
#include "resource.h"
#include "LandsideCrosswalkPropDlgImpl.h"
#include "Landside/InputLandside.h"
#include "common/WinMsg.h"
#include "landside\LandsideLayoutObject.h"
#include "ARCLayoutObjectDlg.h"
#include <common/Path2008.h>
#include "TermplanDoc.h"
#include <commondata/QueryData.h>
#include "landside/LandsideQueryData.h"
#include "landside/LandsideCrosswalk.h"
#include "Landside/LandsideIntersectionNode.h"

//////////////////////////////////////////////////////////////////////////
LandsideCrosswalkPropDlgImpl::LandsideCrosswalkPropDlgImpl( LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg )
:ILayoutObjectPropDlgImpl(pObj,pDlg)
{

}

CString LandsideCrosswalkPropDlgImpl::getTitle()
{	
	return _T("Landside Crosswalk Specification");
}

void LandsideCrosswalkPropDlgImpl::LoadTree()
{
	LandsideCrosswalk* pCrosswalk = getCrosswalk();
	if(!pCrosswalk) return;

	CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
	ARCUnit_Length  curLengthUnit = GetCurLengthUnit();
	CString curLengthUnitString = CARCLengthUnit::GetLengthUnitString(curLengthUnit);


	m_treeProp.DeleteAllItems();
	m_hRClickItem = NULL;
	m_treeProp.SetRedraw(0);	

	//{
	//	CString sStretchRoot;
	//	HTREEITEM hStretchRoot = m_treeProp.InsertItem("Link Stretches");
	//	PropDlgTreeCtrlItem stretchItemRoot(m_treeProp,hStretchRoot);

	//}
	//linked stretch
	{
		CString sStretch;
		LandsideStretch* pStretch = pCrosswalk->getStretch();
		if (pStretch)
			sStretch = pStretch->getName().GetIDString();
		m_hStretch = m_treeProp.InsertItem("Link Stretch");
		TreeCtrlItemInDlg stretchItem(m_treeProp,m_hStretch);
		stretchItem.SetStringText("Link Stretch", sStretch);
	}

	//width
	{
		CString sWidth = "Width";
		m_hWidth = m_treeProp.InsertItem(sWidth);
		TreeCtrlItemInDlg widthItem(m_treeProp, m_hWidth);
		widthItem.SetLengthValueText(sWidth, pCrosswalk->getWidth(), curLengthUnit);
	}

	{
		CString sType;
		CrossType eCrossType=pCrosswalk->getCrossType();
		if (eCrossType==Cross_Intersection)
		{
			sType=_T("Intersection");
		}else if (eCrossType==Cross_Pelican)
		{			
			sType=_T("Pelican");
		}else if (eCrossType==Cross_Zebra)
		{
			sType=_T("Zebra");
		}
		m_hType = m_treeProp.InsertItem("Cross Type");
		TreeCtrlItemInDlg typeItem(m_treeProp,m_hType);
		typeItem.SetStringText("Type", sType);

		if(eCrossType==Cross_Intersection)
		{
			CString strLinkIntersection=_T("");
			m_hIntersection = m_treeProp.InsertItem("Link Intersection");
			LandsideIntersectionNode* pNode = pCrosswalk->getLinkIntersection();
			if (pNode)
			{
				strLinkIntersection=pCrosswalk->getLinkIntersection()->getName().GetIDString();
				int nLinkGroupID = pCrosswalk->getLinkGroup();
				CString strGroupName;
				if(nLinkGroupID >= 0)
				{
					const LaneLinkage& link = pNode->getLinkage(pCrosswalk->getLinkGroup());
					strGroupName = link.getGroupName();
				}

				m_hGroup = m_treeProp.InsertItem("Link Group");
				TreeCtrlItemInDlg linkGroupItem(m_treeProp,m_hGroup);
				linkGroupItem.SetStringText("Link Group", strGroupName);  
			}			
			TreeCtrlItemInDlg linkIntersectionItem(m_treeProp,m_hIntersection);
			linkIntersectionItem.SetStringText("Link Intersection", strLinkIntersection); 
		}

	}

	m_treeProp.SetRedraw(1);
}

FallbackReason LandsideCrosswalkPropDlgImpl::GetFallBackReason()
{
	if(m_hRClickItem == m_hStretch)
		return PICK_STRETCH_POS;

	return PICK_MANYPOINTS;
}

void LandsideCrosswalkPropDlgImpl::OnContextMenu(CMenu& menuPopup, CMenu *&pMenu)
{
	if(m_hRClickItem == m_hStretch)
	{		
		pMenu = menuPopup.GetSubMenu(73);	
	}

}

void LandsideCrosswalkPropDlgImpl::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	m_hRClickItem = hTreeItem;
	if (hTreeItem == m_hWidth)
	{
		OnEditWdith(hTreeItem);
	}else if (hTreeItem == m_hType)
	{
		OnSelectType(hTreeItem);
	}else if (hTreeItem == m_hIntersection)
	{
		OnSelectIntersection(hTreeItem);
	}else if (hTreeItem == m_hGroup)
	{
		OnSelectGroup(hTreeItem);
	}
}

void LandsideCrosswalkPropDlgImpl::DoFallBackFinished(WPARAM wParam, LPARAM lPara)
{
	LandsideCrosswalk* pCrosswalk = getCrosswalk();

	if(m_hRClickItem == m_hStretch && pCrosswalk)
	{
		QueryData& qdata = *((QueryData*)wParam);
		LandsideStretch*pStretch = NULL;
		double dPos = 0;
		if( qdata.GetData(KeyAltObjectPointer,(int&)pStretch) 
			&&	qdata.GetData(KeyPosInStretch,dPos)  
			&& pStretch )
		{
			pCrosswalk->setStretch(pStretch);
			pCrosswalk->setPosOnStretch(dPos);
			LoadTree();
			Update3D();
		}

	}
}

LandsideCrosswalk* LandsideCrosswalkPropDlgImpl::getCrosswalk()
{
	if (m_pObject->GetType() == ALT_LCROSSWALK)
	{
		return (LandsideCrosswalk*)m_pObject;
	}
	return NULL;
}

void LandsideCrosswalkPropDlgImpl::OnEditWdith(HTREEITEM hItem)
{
	LandsideCrosswalk* pCrosswalk = (LandsideCrosswalk*)m_pObject;

	double dUnitNum = ConvertLength(pCrosswalk->getWidth());	
	double dMaxLen = 10000;
	if(pCrosswalk->getStretch())
	{
		dMaxLen = ConvertLength(pCrosswalk->getStretch()->getControlPath().getBuildPath().GetTotalLength());
	}

	GetTreeCtrl().SetDisplayType( 2 );
	GetTreeCtrl().SetSpinRange(0,static_cast<int>(dMaxLen) );
	GetTreeCtrl().SetDisplayNum(static_cast<int>(dUnitNum+0.5));	
	GetTreeCtrl().SpinEditLabel( hItem );
}
void LandsideCrosswalkPropDlgImpl::OnSelectType(HTREEITEM hItem)
{
	std::vector<CString> vStrType;
	vStrType.push_back(_T("Intersection"));
	vStrType.push_back(_T("Pelican"));
	vStrType.push_back(_T("Zebra"));
	GetTreeCtrl().SetComboString(hItem,vStrType);
	GetTreeCtrl().m_comboBox.SetCurSel((int)(getCrosswalk()->getCrossType()));
}
void LandsideCrosswalkPropDlgImpl::OnSelectIntersection(HTREEITEM hItem)
{
	m_vIntersection.clear();
	std::vector<CString> vStrIntersection;

	LandsideCrosswalk* pCrosswalk = getCrosswalk();
	if (pCrosswalk==NULL || pCrosswalk->getCrossType()!=Cross_Intersection)
	{
		return;
	}

	LandsideFacilityLayoutObjectList &objList=m_pParentDlg->getInputLandside()->getObjectList();	
	for (int i=0;i<objList.getCount();i++)
	{
		LandsideFacilityLayoutObject * pObj = objList.getObject(i);
		if(pObj->GetType() == ALT_LINTERSECTION)
		{
			LandsideIntersectionNode *pIntersection=(LandsideIntersectionNode *)pObj;
			for (int k=0;k<pIntersection->getLinageCount();k++)
			{
				if(pIntersection->getLinkage(k).mLaneEntry.m_pStretch==pCrosswalk->getStretch()
				 ||pIntersection->getLinkage(k).mLaneExit.m_pStretch==pCrosswalk->getStretch())
				{
                    vStrIntersection.push_back(pIntersection->getName().GetIDString());
					m_vIntersection.push_back(pIntersection);
					break;
				}
			}
		}
	}
	GetTreeCtrl().SetComboString(hItem,vStrIntersection);
}
void LandsideCrosswalkPropDlgImpl::OnSelectGroup(HTREEITEM hItem)
{
	std::vector<CString> vStrGroup;

	LandsideCrosswalk* pCrosswalk = getCrosswalk();
	if (pCrosswalk==NULL || pCrosswalk->getCrossType()!=Cross_Intersection || pCrosswalk->getLinkIntersection()==NULL)
	{
		return;
	}
	LandsideIntersectionNode *pIntersection=pCrosswalk->getLinkIntersection();
	if (pIntersection==NULL)
	{
		return;
	}
	
	//for (int i=0;i<pIntersection->GetGroupCount();i++)
	//{
	//	CString strGroup;
	//	int nGroup=pIntersection->GetGroup(i);
	//	strGroup.Format(_T("%d"),nGroup);
	//	vStrGroup.push_back(strGroup);		
	//}

	pIntersection->GetGroupNameList(vStrGroup);
	GetTreeCtrl().SetComboString(hItem,vStrGroup);
}
BOOL LandsideCrosswalkPropDlgImpl::OnDefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!m_hRClickItem)
		return TRUE;

	LandsideCrosswalk* pCrosswalk = getCrosswalk();

	if( message == WM_INPLACE_SPIN )
	{
		if(m_hWidth == m_hRClickItem)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			double dValue = UnConvertLength(pst->iPercent);
			pCrosswalk->setWidth(dValue);
		}

		LoadTree();	
		Update3D();
		
	}else if (message == WM_INPLACE_COMBO2)
	{	
		if(m_hType == m_hRClickItem)
		{
			int selitem = GetTreeCtrl().m_comboBox.GetCurSel();	
			if (selitem!=-1)
			{
				if (pCrosswalk->getCrossType()!=Cross_Intersection && (CrossType)selitem==Cross_Intersection)
				{
					pCrosswalk->setLinkIntersection(NULL);
					pCrosswalk->setLinkGroup(-1);
				}/*
				else if (pCrosswalk->getCrossType()==Cross_Intersection && (CrossType)selitem!=Cross_Intersection)
								{
									if(pCrosswalk->getLinkIntersection()!=NULL && pCrosswalk->getLinkGroup()>=0)
									{
										pCrosswalk->getLinkIntersection()->RemoveCrossFromGroup(pCrosswalk->getLinkGroup(),pCrosswalk);
									}
								}
				*/
				
				pCrosswalk->setCrossType((CrossType)selitem);
			}
			
			LoadTree();
			Update3D();
		}else if (m_hIntersection == m_hRClickItem)
		{
			int selitem = GetTreeCtrl().m_comboBox.GetCurSel();	
			if (selitem>=0&&selitem<(int)m_vIntersection.size())
			{
				pCrosswalk->setLinkIntersection(m_vIntersection.at(selitem));
				LoadTree();
			}
		}else if (m_hGroup == m_hRClickItem)
		{
			int selitem = GetTreeCtrl().m_comboBox.GetCurSel();	
			LandsideIntersectionNode* pNode = pCrosswalk->getLinkIntersection();
			std::vector<CString> strNameList;
			if(pNode)
			{
				pNode->GetGroupNameList(strNameList);
				int nCount = (int)strNameList.size();
				if (selitem>=0&&selitem<nCount)
				{
					CString strGroupName = strNameList[selitem];
					int nGroupID = pNode->GetGroupID(strGroupName);
					pCrosswalk->setLinkGroup(nGroupID);
				}
			}
				
			LoadTree();
			
		}
	}
	return TRUE;
}

bool LandsideCrosswalkPropDlgImpl::OnOK( CString& errorMsg )
{
	if(LandsideCrosswalk* pCrosswalk = getCrosswalk())
	{
		if( !pCrosswalk->getStretch() )
		{
			errorMsg = _T("define crosswalk on a stretch");
			return false;
		}
		if (pCrosswalk->getCrossType()==Cross_Intersection)
		{
			if (pCrosswalk->getLinkIntersection()==NULL ||pCrosswalk->getLinkGroup()<0)
			{
				errorMsg = _T("Must choose link intersection and group!");
				return false;
			}
		}
		return __super::OnOK(errorMsg);
	}	
	return false;
}

