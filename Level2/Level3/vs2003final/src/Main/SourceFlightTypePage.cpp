#include "StdAfx.h"
#include ".\sourceflighttypepage.h"
#include "FlightDialog.h"

IMPLEMENT_DYNCREATE(CSourceFlightTypePage, CPollutantSourcePage)

CSourceFlightTypePage::CSourceFlightTypePage(CSourceEmissionManager *pSourceEmissionManager,FuelProperties *pFuelProperties,CAirPollutantList *pAirPollutantList,CWnd* pParentWnd)
:CPollutantSourcePage(IDS_POLLUTANTSOURCE_FLIGHTTYPE,pFuelProperties,pAirPollutantList,pParentWnd)
,m_pSourceFlightTypeComposite(pSourceEmissionManager->GetSourceFlightTypeComposite())
{
	if (m_pSourceFlightTypeComposite!=NULL)
	{
		m_pSourceFlightTypeComposite->ReadData();
	}
	m_pAirportDatabase=GetAirportDatabase();
}
CSourceFlightTypePage::CSourceFlightTypePage()
{

}

CSourceFlightTypePage::~CSourceFlightTypePage(void)
{
}

BEGIN_MESSAGE_MAP(CSourceFlightTypePage, CPollutantSourcePage)
END_MESSAGE_MAP()

void CSourceFlightTypePage::OnNewFlightType()
{
	FlightConstraint fltType;												
	CFlightDialog flightDlg(m_pParentWnd);
	if (flightDlg.DoModal()==IDOK)
	{
		fltType=flightDlg.GetFlightSelection();
		if (m_pSourceFlightTypeComposite->FlightExist(fltType))
		{
			MessageBox("Flight already exists.");
			return;
		}
		CSourceFlightTypeProperty *newSourceFltTypePro=new CSourceFlightTypeProperty(m_pAirportDatabase);
		newSourceFltTypePro->SetFlightType(fltType);		
		m_pSourceFlightTypeComposite->AddItem(newSourceFltTypePro);
		AddFltTypeItem(newSourceFltTypePro);
		SetModified();
	}
}
void CSourceFlightTypePage::OnDelFlightType()
{
	HTREEITEM hItem=m_tree.GetSelectedItem();
	if (hItem!=NULL)
	{
		CSourceFlightTypeProperty *selFlightType=(CSourceFlightTypeProperty *)m_tree.GetItemData(hItem);
		if (selFlightType!=NULL)
		{
			m_pSourceFlightTypeComposite->DeleteItem(selFlightType);

			HTREEITEM hSelItem=m_tree.GetNextItem(hItem,TVGN_NEXT);
			if (hSelItem==NULL)
			{
				hSelItem=m_tree.GetPrevSiblingItem(hItem);
			}	
			m_tree.DeleteItem(hItem);
			if (hSelItem!=NULL)
			{
				m_tree.SelectItem(hSelItem);
			}else
			{
				m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,FALSE);
			}
		}
	}
}
void CSourceFlightTypePage::ReloadTree()
{
	if (m_pAirportDatabase==NULL||m_pSourceFlightTypeComposite==NULL)
	{
		return;
	}
	m_tree.DeleteAllItems();

	for (int i=0;i<m_pSourceFlightTypeComposite->GetCount();i++)
	{
		CSourceFlightTypeProperty* curSourceFlightType=m_pSourceFlightTypeComposite->GetItem(i);
		AddFltTypeItem(curSourceFlightType);
	}
}
void CSourceFlightTypePage::OnButtonAdd()
{
	OnNewFlightType();
}
void CSourceFlightTypePage::OnButtonDel()
{
	OnDelFlightType();
}
HTREEITEM CSourceFlightTypePage::AddFltTypeItem(CSourceFlightTypeProperty *pSourceFltTypePro)
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net=NET_SHOW_DIALOGBOX;
	
	CString strFltType;
	pSourceFltTypePro->GetFlightType().screenPrint(strFltType);
	strFltType=_T("Flight Type:")+strFltType;
	HTREEITEM hFltTypeItem=m_tree.InsertItem(strFltType,cni,FALSE);
	m_tree.SetItemData(hFltTypeItem,(DWORD)pSourceFltTypePro);
	
	m_tree.SelectItem(hFltTypeItem);
	
	//add Fuel Item
	bool bFuelAvailable=false;
	CString strFuel,strArrApuTime,strDepApuTime;
	int nFuelID=pSourceFltTypePro->GetFueProID();
	FuelProperty *pFuelPro=m_pFuelProperties->GetFuelByID(nFuelID);
    if (pFuelPro==NULL /*|| pFuelPro->getName()==_T("")*/)
    {
		cni.net=NET_COMBOBOX;
		strFuel=_T("Fuel: Please Choose");

    }else
	{
		cni.net=NET_COMBOBOX;
		strFuel=_T("Fuel: ")+pFuelPro->getName();
		bFuelAvailable=true;
	}
	m_tree.InsertItem(strFuel,cni,FALSE,FALSE,hFltTypeItem);

	
	cni.net = NET_EDIT_FLOAT;
	cni.fMaxValue=100;
	strArrApuTime.Format(_T("Arrival APU time(%% of time,land to park):%.2f"),pSourceFltTypePro->GetArrAPUTime());
	cni.fMinValue=(float)pSourceFltTypePro->GetArrAPUTime();	
	HTREEITEM hArrApuItem=m_tree.InsertItem(strArrApuTime,cni,FALSE,FALSE,hFltTypeItem);
	m_tree.SetItemData(hArrApuItem,ArrAPUTime);

	strDepApuTime.Format(_T("Departure APU time(%% of time,takeoff-off chocks):%.2f"),pSourceFltTypePro->GetDepAPUTime());
	cni.fMinValue=(float)pSourceFltTypePro->GetDepAPUTime();
	HTREEITEM hDepApuItem=m_tree.InsertItem(strDepApuTime,cni,FALSE,FALSE,hFltTypeItem);
	m_tree.SetItemData(hDepApuItem,DepAPUTime);

	cni.net=NET_NORMAL;
	HTREEITEM hPollutantItem=m_tree.InsertItem("Pollutant",cni,FALSE,FALSE,hFltTypeItem);

	if (bFuelAvailable)
	{
		AddPollutantChildrenItem(pSourceFltTypePro,hPollutantItem,pFuelPro);
		m_tree.Expand(hPollutantItem,TVE_EXPAND);
	}

	m_tree.Expand(hFltTypeItem,TVE_EXPAND);
	return hFltTypeItem;
}
void CSourceFlightTypePage::AddPollutantChildrenItem(CSourceFlightTypeProperty *pSourceFltTypePro,HTREEITEM hPollutantItem,FuelProperty *pFuelPro)
{
	int nFuelID=pSourceFltTypePro->GetFueProID();
	if (pFuelPro==NULL)
	{
		pFuelPro=m_pFuelProperties->GetFuelByID(nFuelID);
	}

	for (int i=0;i<pFuelPro->getItemCount();i++)
	{	
		//add the pollutant of the choosing fuel

		PropItem *curPollutant=pFuelPro->getItem(i);
		if (curPollutant!=NULL && curPollutant->getValue()==1)
		{
			int nPollutantID=curPollutant->getType();
			CString strSymbol=m_pAirPollutantList->getSymbolByID(nPollutantID);
			if (strSymbol!=_T(""))
			{
				CFlightTypePollutantSetting *curSetting;
				curSetting=pSourceFltTypePro->GetSettingByPollutantID(nPollutantID);
				if (curSetting==NULL)
				{
					curSetting=new CFlightTypePollutantSetting();
					curSetting->m_dPollutantID=nPollutantID;
					pSourceFltTypePro->AddItem(curSetting);
				}
				AddPollutantSettingItem(strSymbol,curSetting,hPollutantItem);
			}
		}
	}
		
}
HTREEITEM CSourceFlightTypePage::AddPollutantSettingItem(CString strPollutantSymbol,CFlightTypePollutantSetting *pSetting,HTREEITEM parentItem)
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net=NET_NORMAL;

	HTREEITEM hSettingItem=m_tree.InsertItem(strPollutantSymbol,cni,FALSE,FALSE,parentItem);

	CString strGroundACC,strGroundTaxi,strGroundDec,strApuGenerate,strTakeoff,strLanding;

	cni.net=NET_EDIT_FLOAT;
	strGroundACC.Format(_T("Ground acceleration(gm/min):%.2f"),pSetting->m_dGroundAcc);
	strGroundTaxi.Format(_T("Ground taxi(gm/min):%.2f"),pSetting->m_dGroundTaxi);
	strGroundDec.Format(_T("Ground deceleration(gm/min):%.2f"),pSetting->m_dGroundDec);
	strApuGenerate.Format(_T("APU Generated(gm/min):%.2f"),pSetting->m_dApuGenerate);
	strTakeoff.Format(_T("Takeoff(gm):%.2f"),pSetting->m_dTakeoff);
	strLanding.Format(_T("Landing(gm):%.2f"),pSetting->m_dLanding);

	HTREEITEM hItem;
	cni.fMinValue=(float)pSetting->m_dGroundAcc;
	hItem=m_tree.InsertItem(strGroundACC,cni,FALSE,FALSE,hSettingItem);
 	m_tree.SetItemData(hItem,GroundAcc);
	cni.fMinValue=(float)pSetting->m_dGroundTaxi;
	hItem=m_tree.InsertItem(strGroundTaxi,cni,FALSE,FALSE,hSettingItem);
	m_tree.SetItemData(hItem,GroundTaxi);
	cni.fMinValue=(float)pSetting->m_dGroundDec;
	hItem=m_tree.InsertItem(strGroundDec,cni,FALSE,FALSE,hSettingItem);
	m_tree.SetItemData(hItem,GroundDec);
	cni.fMinValue=(float)pSetting->m_dApuGenerate;
	hItem=m_tree.InsertItem(strApuGenerate,cni,FALSE,FALSE,hSettingItem);
	m_tree.SetItemData(hItem,APUGenerated);
	cni.fMinValue=(float)pSetting->m_dTakeoff;
	hItem=m_tree.InsertItem(strTakeoff,cni,FALSE,FALSE,hSettingItem);
	m_tree.SetItemData(hItem,TakeOff);
	cni.fMinValue=(float)pSetting->m_dLanding;
	hItem=m_tree.InsertItem(strLanding,cni,FALSE,FALSE,hSettingItem);
	m_tree.SetItemData(hItem,Landing);

	m_tree.SetItemData(hSettingItem,(DWORD)pSetting);
	m_tree.Expand(hSettingItem,TVE_EXPAND);
	return hSettingItem;
}
BOOL CSourceFlightTypePage::OnApply()
{
	if(m_pSourceFlightTypeComposite!=NULL)
		m_pSourceFlightTypeComposite->SaveData();	
	return true;
}
void CSourceFlightTypePage::OnTvnSelchangedTreePollutantsource(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hItem=pNMTreeView->itemNew.hItem;
	if (hItem==NULL)
	{
		return;
	}
	if (m_tree.GetParentItem(hItem)==NULL)
	{
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,TRUE);
	}else
	{
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,FALSE);
	}

	*pResult = 0;
}
CAirportDatabase *CSourceFlightTypePage::GetAirportDatabase()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->GetTerminal().m_pAirportDB;
}
LRESULT CSourceFlightTypePage::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{	
	case UM_CEW_EDITSPIN_END:
	case UM_CEW_EDIT_END:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			HTREEITEM hParentItem=m_tree.GetParentItem(hItem);
			EditItemType itemType=(EditItemType)m_tree.GetItemData(hItem);

			CString strItemText,strValue = *((CString*)lParam);
			double dValue;
			if( strValue == "" )
			{
				dValue=0;
			}else
			{
				dValue= atof( strValue );
			}			

			switch(itemType)
			{
			case ArrAPUTime:
				{
					CSourceFlightTypeProperty *curSourceFlight=(CSourceFlightTypeProperty *)m_tree.GetItemData(hParentItem);

					if (dValue<0)
					{						
						dValue=0;
					}else if (dValue>100)
					{
						dValue=100;
					}
					
					strItemText.Format(_T("Arrival APU time(%% of time,land to park):%.2f"),dValue);
					m_tree.SetItemText(hItem,strItemText);

					curSourceFlight->SetArrAPUTime(dValue);

					break;
				}
			case DepAPUTime:
				{
					CSourceFlightTypeProperty *curSourceFlight=(CSourceFlightTypeProperty *)m_tree.GetItemData(hParentItem);

					if (dValue<0)
					{						
						dValue=0;
					}else if (dValue>100)
					{
						dValue=100;
					}

					strItemText.Format(_T("Departure APU time(%% of time,takeoff-off chocks):%.2f"),dValue);
					m_tree.SetItemText(hItem,strItemText);

					curSourceFlight->SetDepAPUTime(dValue);

					break;
				}
			case GroundAcc:
				{
					CFlightTypePollutantSetting *pSetting=(CFlightTypePollutantSetting *)m_tree.GetItemData(hParentItem);

					if (dValue<0 )
					{
						dValue=0;
					}

					strItemText.Format(_T("Ground acceleration(gm/min):%.2f"),dValue);
					m_tree.SetItemText(hItem,strItemText);

					pSetting->m_dGroundAcc=dValue;

					break;
				}
			case GroundTaxi:
				{
					CFlightTypePollutantSetting *pSetting=(CFlightTypePollutantSetting *)m_tree.GetItemData(hParentItem);

					if (dValue<0 )
					{
						dValue=0;
					}
					strItemText.Format(_T("Ground taxi(gm/min):%.2f"),dValue);
					m_tree.SetItemText(hItem,strItemText);

					pSetting->m_dGroundTaxi=dValue;

					break;
				}
			case GroundDec:
				{
					CFlightTypePollutantSetting *pSetting=(CFlightTypePollutantSetting *)m_tree.GetItemData(hParentItem);

					if (dValue<0 )
					{
						dValue=0;
					}
					strItemText.Format(_T("Ground deceleration(gm/min):%.2f"),dValue);
					m_tree.SetItemText(hItem,strItemText);

					pSetting->m_dGroundDec=dValue;

					break;
				}
			case APUGenerated:
				{
					CFlightTypePollutantSetting *pSetting=(CFlightTypePollutantSetting *)m_tree.GetItemData(hParentItem);

					if (dValue<0 )
					{
						dValue=0;
					}
					strItemText.Format(_T("APU Generated(gm/min):%.2f"),dValue);
					m_tree.SetItemText(hItem,strItemText);

					pSetting->m_dApuGenerate=dValue;

					break;
				}
			case TakeOff:
				{
					CFlightTypePollutantSetting *pSetting=(CFlightTypePollutantSetting *)m_tree.GetItemData(hParentItem);

					if (dValue<0 )
					{
						dValue=0;
					}
					strItemText.Format(_T("Takeoff(gm):%.2f"),dValue);
					m_tree.SetItemText(hItem,strItemText);

					pSetting->m_dTakeoff=dValue;

					break;
				}
			case Landing:
				{
					CFlightTypePollutantSetting *pSetting=(CFlightTypePollutantSetting *)m_tree.GetItemData(hParentItem);

					if (dValue<0 )
					{
						dValue=0;
					}
					strItemText.Format(_T("Landing(gm):%.2f"),dValue);
					m_tree.SetItemText(hItem,strItemText);

					pSetting->m_dLanding=dValue;

					break;
				}
			default:
				break;
			}

			COOLTREE_NODE_INFO *cni=m_tree.GetItemNodeInfo(hItem);
			cni->fMinValue=(float)dValue;

			SetModified();

			break;
		}
	case UM_CEW_COMBOBOX_BEGIN:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			CWnd* pWnd=m_tree.GetEditWnd(hItem);
			CRect rectWnd;
			m_tree.GetItemRect(hItem,rectWnd,TRUE);
			pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
			CComboBox* pCB=(CComboBox*)pWnd;
			pCB->ResetContent();
			pCB->SetDroppedWidth(100);
			FuelProperty *curFule;
			for (int i=0;i<m_pFuelProperties->GetItemCount();i++)
			{
				curFule=(FuelProperty *)m_pFuelProperties->GetItem(i);
				int nIndex = pCB->AddString(curFule->getName());
				pCB->SetItemData(nIndex,curFule->GetID());
			}
			break;
		}
	case UM_CEW_COMBOBOX_SELCHANGE:
		{
			HTREEITEM hFirstItem=m_tree.GetFirstVisibleItem();

			CWnd* pWnd = m_tree.GetEditWnd((HTREEITEM)wParam);
			CComboBox* pCB = (CComboBox*)pWnd;
			HTREEITEM hFuelItem=(HTREEITEM)wParam;
			HTREEITEM hFltTypeItem=m_tree.GetParentItem(hFuelItem);

			CString strText = *(CString*)lParam;

			int nSel = pCB->GetCurSel();
			int nFuelID = pCB->GetItemData(nSel);

			FuelProperty *pFuelPro=m_pFuelProperties->GetFuelByID(nFuelID);
			CString strFuel;
			if (pFuelPro==NULL)
			{
				strFuel=_T("Fuel: Please Choose");

			}else
			{
				strFuel=_T("Fuel: ")+pFuelPro->getName();
			}
			m_tree.SetItemText(hFuelItem,strFuel);

			CSourceFlightTypeProperty *curSourceFlight=(CSourceFlightTypeProperty *)m_tree.GetItemData(hFltTypeItem);
			curSourceFlight->SetFuelProID(nFuelID);
			curSourceFlight->DeleteAllPollutantItem();

			HTREEITEM hPollutantItem=hFuelItem;
			for (int i=0;i<3;i++)
			{
				hPollutantItem=m_tree.GetNextItem(hPollutantItem,TVGN_NEXT);
			}
			m_tree.DeleteItem(hPollutantItem);
			COOLTREE_NODE_INFO cni;
			CCoolTree::InitNodeInfo(this,cni);
			cni.net=NET_NORMAL;
			hPollutantItem=m_tree.InsertItem("Pollutant",cni,FALSE,FALSE,hFltTypeItem);

			AddPollutantChildrenItem(curSourceFlight,hPollutantItem);
			m_tree.Expand(hPollutantItem,TVE_EXPAND);

			m_tree.SelectItem(hFuelItem);
			m_tree.EnsureVisible(hFirstItem);

			SetModified();
			break;
		}
	case UM_CEW_SHOW_DIALOGBOX_BEGIN:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			CFlightDialog flightDlg(m_pParentWnd);
			if (flightDlg.DoModal()==IDOK)
			{
				FlightConstraint fltType=flightDlg.GetFlightSelection();
				if (m_pSourceFlightTypeComposite->FlightExist(fltType))
				{
					MessageBox("Flight already exists.");
					break;
				}
				CSourceFlightTypeProperty *curSourceFltTypePro=(CSourceFlightTypeProperty *)m_tree.GetItemData(hItem);
				curSourceFltTypePro->SetFlightType(fltType);	
				CString strFltType;
				fltType.screenPrint(strFltType);
				strFltType=_T("Flight Type:")+strFltType;
				m_tree.SetItemText(hItem,strFltType);
			}
			break;
		}
	default:
		break;
	}
	return CPollutantSourcePage::DefWindowProc(message, wParam, lParam);
}