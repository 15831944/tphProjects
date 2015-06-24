#include "StdAfx.h"
#include ".\SourceVehiclePage.h"

IMPLEMENT_DYNCREATE(CSourceVehiclePage , CPollutantSourcePage)

CSourceVehiclePage ::CSourceVehiclePage (CSourceEmissionManager *pSourceEmissionManager,FuelProperties *pFuelProperties,CAirPollutantList *pAirPollutantList,CWnd* pParentWnd,UINT nIDCaption)
:CPollutantSourcePage(nIDCaption,pFuelProperties,pAirPollutantList,pParentWnd)
{
	m_nPrjID=GetProjectID();

	switch(nIDCaption)
	{
	case IDS_POLLUTANTSOURCE_VEHICLE_GSE:
		{
			m_VehicleType=Source_Vehicle_GSE;
			m_pVehicleComposite=pSourceEmissionManager->GetAirsideGSEVehicleComposite();
			break;
		}
	case IDS_POLLUTANTSOURCE_VEHICLE_AME:
		{
			m_VehicleType=Source_Vehicle_AME;
			m_pVehicleComposite=pSourceEmissionManager->GetAirsideAMEVehicleComposite();
			break;
		}
	case IDS_POLLUTANTSOURCE_VEHICLE_LANDSIDE:
		{
			m_VehicleType=Source_Vehicle_LANDSIDE;
			m_pVehicleComposite=pSourceEmissionManager->GetLandsideVehicleComposite();
			break;
		}
	}
	if (m_pVehicleComposite!=NULL)
	{
		m_pVehicleComposite->ReadData();
	}
}
CSourceVehiclePage::CSourceVehiclePage()
{

}

CSourceVehiclePage::~CSourceVehiclePage (void)
{
}
BEGIN_MESSAGE_MAP(CSourceVehiclePage, CPollutantSourcePage)
 	ON_WM_CREATE()
END_MESSAGE_MAP()
 
BOOL CSourceVehiclePage::OnApply()
{
	m_pVehicleComposite->SaveData();
	return true;
}
BOOL CSourceVehiclePage::OnInitDialog()
{
	CPollutantSourcePage::OnInitDialog();
	m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,FALSE);

	return true;
}
void CSourceVehiclePage::ReloadTree()
{
	if (m_pVehicleComposite==NULL)
	{
		return;
	}
	m_tree.DeleteAllItems();

	switch (m_VehicleType)
	{
	case Source_Vehicle_GSE:
		{
			ReloadGSETree();
			break;
		}
	case Source_Vehicle_AME:
		{
			ReloadAMETree();
			break;
		}
	case Source_Vehicle_LANDSIDE:
		{
			ReloadLandsideTree();
			break;
		}
	}
}
void CSourceVehiclePage::ReloadGSETree()
{
	CVehicleSpecifications vehicleSpecifications;
	vehicleSpecifications.ReadData(m_nPrjID);
	size_t nVehicleCount = vehicleSpecifications.GetElementCount();

	for (size_t i = 0; i < nVehicleCount; i++)
	{
		CVehicleSpecificationItem* pItem = vehicleSpecifications.GetItem(i);

		CVehicleProperty *pVehicleProperty=m_pVehicleComposite->GetItemByVehicleID(pItem->GetID());
		if (pVehicleProperty==NULL)
		{
			pVehicleProperty=new CVehicleProperty();
			pVehicleProperty->SetVehicleID(pItem->GetID());
			m_pVehicleComposite->AddItem(pVehicleProperty);
		}
		AddGSEVehicleItem(pVehicleProperty,pItem->getName());
	}
}
void CSourceVehiclePage::ReloadAMETree()
{

}
void CSourceVehiclePage::ReloadLandsideTree()
{
	InputLandside *pLandSide=GetInputLanside();
	ASSERT(FALSE);
	/*size_t nVehicleCount = pLandSide->getOperationVehicleList().GetElementCount();
	CString strValue;
	for (size_t i = 0; i < nVehicleCount; i++)
	{
		CVehicleItemDetail* pItem = pLandSide->getOperationVehicleList().GetItem(i);
		CVehicleProperty *pVehicleProperty=m_pVehicleComposite->GetItemByVehicleID(pItem->GetID());
		if (pVehicleProperty==NULL)
		{
			pVehicleProperty=new CVehicleProperty();
			pVehicleProperty->SetVehicleID(pItem->GetID());
			m_pVehicleComposite->AddItem(pVehicleProperty);
		}
		AddGSEVehicleItem(pVehicleProperty,pItem->getName());
	}*/
}
HTREEITEM CSourceVehiclePage::AddGSEVehicleItem(CVehicleProperty *pVehicleProperty,CString strVehicleName)
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net=NET_SHOW_DIALOGBOX;
	
	HTREEITEM hFltTypeItem=m_tree.InsertItem(_T("Vehicle Type:")+strVehicleName,cni,FALSE);
	m_tree.SetItemData(hFltTypeItem,(DWORD)pVehicleProperty);

	m_tree.SelectItem(hFltTypeItem);

	//add Fuel Item
	bool bFuelAvailable=false;
	CString strFuel,strAcceleration,strSteadySpeed,strDeceleration,strIdle;
	int nFuelID=pVehicleProperty->GetFueProID();
	FuelProperty *pFuelPro=m_pFuelProperties->GetFuelByID(nFuelID);
	if (pFuelPro==NULL)
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


	cni.net=NET_NORMAL;
	HTREEITEM hPollutantItem=m_tree.InsertItem("Pollutant",cni,FALSE,FALSE,hFltTypeItem);

	if (bFuelAvailable)
	{
		AddPollutantChildrenItem(pVehicleProperty,hPollutantItem,pFuelPro);
		m_tree.Expand(hPollutantItem,TVE_EXPAND);
	}

	m_tree.Expand(hFltTypeItem,TVE_EXPAND);
	return hFltTypeItem;
}
void CSourceVehiclePage::AddPollutantChildrenItem(CVehicleProperty *pVehicleProperty,HTREEITEM hPollutantItem,FuelProperty *pFuelPro)
{
	int nFuelID=pVehicleProperty->GetFueProID();
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
				CVehiclePollutantSetting *curSetting;
				curSetting=pVehicleProperty->GetSettingByPollutantID(nPollutantID);
				if (curSetting==NULL)
				{
					curSetting=new CVehiclePollutantSetting();
					curSetting->m_nPollutantID=nPollutantID;
					pVehicleProperty->AddItem(curSetting);
				}
				AddPollutantSettingItem(strSymbol,curSetting,hPollutantItem);
			}
		}
	}
}
HTREEITEM CSourceVehiclePage::AddPollutantSettingItem(CString strPollutantSymbol,CVehiclePollutantSetting *pSetting,HTREEITEM parentItem)
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net=NET_NORMAL;

	HTREEITEM hSettingItem=m_tree.InsertItem(strPollutantSymbol,cni,FALSE,FALSE,parentItem);

	CString strAcceleration,strSteadySpeed,strDeceleration,strIdle;

	cni.net=NET_EDIT_FLOAT;
	strAcceleration.Format(_T("Acceleration(gm/min):%.2f"),pSetting->m_dAcc);
	strSteadySpeed.Format(_T("Steady Speed(gm/min):%.2f"),pSetting->m_dSteadySpeed);
	strDeceleration.Format(_T("Deceleration(gm/min):%.2f"),pSetting->m_dDec);
	strIdle.Format(_T("Idle(gm/min):%.2f"),pSetting->m_dIdle);

	HTREEITEM hItem;
	cni.fMinValue=(float)pSetting->m_dAcc;
	hItem=m_tree.InsertItem(strAcceleration,cni,FALSE,FALSE,hSettingItem);
	m_tree.SetItemData(hItem,Type_Acceleration);
	cni.fMinValue=(float)pSetting->m_dSteadySpeed;
	hItem=m_tree.InsertItem(strSteadySpeed,cni,FALSE,FALSE,hSettingItem);
	m_tree.SetItemData(hItem,Type_SteadySpeed);
	cni.fMinValue=(float)pSetting->m_dDec;
	hItem=m_tree.InsertItem(strDeceleration,cni,FALSE,FALSE,hSettingItem);
	m_tree.SetItemData(hItem,Type_Deceleration);
	cni.fMinValue=(float)pSetting->m_dIdle;
	hItem=m_tree.InsertItem(strIdle,cni,FALSE,FALSE,hSettingItem);
	m_tree.SetItemData(hItem,Type_Idle);

	m_tree.SetItemData(hSettingItem,(DWORD)pSetting);
	m_tree.Expand(hSettingItem,TVE_EXPAND);
	return hSettingItem;
}

int CSourceVehiclePage::GetProjectID()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	return pDoc->GetProjectID();
}
InputLandside *CSourceVehiclePage::GetInputLanside()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	return pDoc->getARCport()->m_pInputLandside;
}
LRESULT CSourceVehiclePage::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{	
	case UM_CEW_EDITSPIN_END:
	case UM_CEW_EDIT_END:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			HTREEITEM hParentItem=m_tree.GetParentItem(hItem);
			EditItemType itemType=(EditItemType)m_tree.GetItemData(hItem);

			CVehiclePollutantSetting *pSetting=(CVehiclePollutantSetting *)m_tree.GetItemData(hParentItem);

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
			case Type_Acceleration:
				{

					if (dValue<0 )
					{
						dValue=0;
					}

					strItemText.Format(_T("Acceleration(gm/min):%.2f"),dValue);
					m_tree.SetItemText(hItem,strItemText);

					pSetting->m_dAcc=dValue;

					break;
				}
			case Type_SteadySpeed:
				{

					if (dValue<0 )
					{
						dValue=0;
					}
					strItemText.Format(_T("Steady Speed(gm/min):%.2f"),dValue);
					m_tree.SetItemText(hItem,strItemText);

					pSetting->m_dSteadySpeed=dValue;

					break;
				}
			case Type_Deceleration:
				{

					if (dValue<0 )
					{
						dValue=0;
					}
					strItemText.Format(_T("Deceleration(gm/min):%.2f"),dValue);
					m_tree.SetItemText(hItem,strItemText);

					pSetting->m_dDec=dValue;

					break;
				}
			case Type_Idle:
				{

					if (dValue<0 )
					{
						dValue=0;
					}
					strItemText.Format(_T("Idle(gm/min):%.2f"),dValue);
					m_tree.SetItemText(hItem,strItemText);

					pSetting->m_dIdle=dValue;

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
			HTREEITEM hVehicleItem=m_tree.GetParentItem(hFuelItem);

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

			CVehicleProperty *curVehicleProperty=(CVehicleProperty *)m_tree.GetItemData(hVehicleItem);
			curVehicleProperty->SetFuelProID(nFuelID);
			curVehicleProperty->DeleteAllPollutantItem();

			HTREEITEM hPollutantItem=m_tree.GetNextItem(hFuelItem,TVGN_NEXT);

			m_tree.DeleteItem(hPollutantItem);

			COOLTREE_NODE_INFO cni;
			CCoolTree::InitNodeInfo(this,cni);
			cni.net=NET_NORMAL;
			hPollutantItem=m_tree.InsertItem("Pollutant",cni,FALSE,FALSE,hVehicleItem);

			AddPollutantChildrenItem(curVehicleProperty,hPollutantItem);
 			m_tree.SelectItem(hFuelItem);

			m_tree.Expand(hPollutantItem,TVE_EXPAND);
			m_tree.EnsureVisible(hFirstItem);

			SetModified();
			break;
		}
	case UM_CEW_SHOW_DIALOGBOX_BEGIN:
		{
			break;
		}
	default:
		break;
	}
	return CPollutantSourcePage::DefWindowProc(message, wParam, lParam);
}
