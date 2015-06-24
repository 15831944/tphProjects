// FlightGateSelection.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "FlightGateSelection.h"
#include "FlightPriorityDlg.h"
#include "../Common/AirsideFlightType.h"
#include "DlgNewFlightType.h"
#include "../InputAirside/ALTAirport.h"
#include "InputAirside/InputAirside.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFlightGateSelectionDlg dialog


CFlightGateSelectionDlg::CFlightGateSelectionDlg(CWnd* pParent /*=NULL*/, int nProjID,int nAirportID, int nIndex)
	: CDialog(CFlightGateSelectionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFlightGateSelectionDlg
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	//m_flight.setFullID("DEFAULT");
	m_bChanged = false;
	m_nIndex = nIndex;	
	m_nAirportID = nAirportID;
	m_nProjID = nProjID;
}
 

void CFlightGateSelectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFlightGateSelectionDlg)
	DDX_Control(pDX, IDC_TREE_FLIGHTSECTOR, m_treeALTobject);
	DDX_Control(pDX, IDC_EDIT_FLIGHTSECTOR, m_edtFlightSector);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFlightGateSelectionDlg, CDialog)
	//{{AFX_MSG_MAP(CFlightGateSelectionDlg)
	ON_BN_CLICKED(IDC_BUTTON_FLIGHTSECTOR_SELECTION, OnButtonFlightsectorSelection)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFlightGateSelectionDlg message handlers

void CFlightGateSelectionDlg::OnButtonFlightsectorSelection() 
{
	//CFlightDialog flightDlg(NULL);
	//if(flightDlg.DoModal() != IDOK)
	//	return;	
	////char strFlight[200];
	//CString strFlight;
	//flightDlg.GetFlightSelection().screenPrint(strFlight);
	//m_flight = flightDlg.GetFlightSelection();

	DlgNewFlightType dlg(m_flight);
	if (dlg.DoModal() != IDOK)
		return;
	m_flight = dlg.GetFltType();
	CString strFlight = m_flight.getPrintString();
	m_edtFlightSector.SetWindowText(strFlight);
}

HTREEITEM CFlightGateSelectionDlg::FindObjNode(HTREEITEM hParentItem,const CString& strNodeText)
{
	HTREEITEM hRetItem = NULL;

	HTREEITEM hChildItem = m_treeALTobject.GetChildItem(hParentItem);

	//find
	while (hChildItem)
	{
		CString strItemText = m_treeALTobject.GetItemText(hChildItem);
		if(strItemText.CompareNoCase(strNodeText) == 0)
		{
			hRetItem = hChildItem;
			break;
		}
		hChildItem = m_treeALTobject.GetNextSiblingItem(hChildItem);
	}
	return hRetItem;
}

void CFlightGateSelectionDlg::AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject)
{
	ASSERT(hObjRoot);

	ALTObjectID objName;
	pObject->getObjName(objName);
	HTREEITEM hParentItem = hObjRoot;
	bool bObjNode = false;
	CString strNodeName = _T("");

	for (int nLevel =0; nLevel< OBJECT_STRING_LEVEL; ++nLevel)
	{
		if (nLevel != OBJECT_STRING_LEVEL -1 )
		{
			HTREEITEM hItem = FindObjNode(hParentItem,objName.m_val[nLevel].c_str());
			if (hItem != NULL)
			{
				hParentItem = hItem;
				continue;
			}
			if (objName.m_val[nLevel+1] != _T(""))
			{
				HTREEITEM hTreeItem = m_treeALTobject.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				//				m_treeALTobject.Expand(hParentItem, TVE_EXPAND);
				hParentItem = hTreeItem;

				continue;
			}
			else //objNode
			{
				HTREEITEM hTreeItem = m_treeALTobject.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				//				m_treeALTobject.Expand(hParentItem, TVE_EXPAND);
				break;
			}

		}
		else
		{
			HTREEITEM hTreeItem = m_treeALTobject.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
			//			m_treeALTobject.Expand(hParentItem, TVE_EXPAND);
			break;
		}
	}
}

void CFlightGateSelectionDlg::ReloadDatabase()
{
	HTREEITEM hStandFamily = m_treeALTobject.InsertItem(_T("Stand Family:"));
	if( m_nIndex >= 0 )
	{
		m_flight = ((CFlightPriorityDlg *)GetParent())->m_vectFlightGate[m_nIndex].flight;
		char strText[200];
		CString strText1 = m_flight.getPrintString();
		//m_flight.screenPrint(strText1);
		m_edtFlightSector.SetWindowText(strText1);

		((CFlightPriorityDlg *)GetParent())->m_vectFlightGate[m_nIndex].procID.printID( strText );
		m_treeALTobject.m_strSelectedID = strText;
	}
	
	//m_treeProc.LoadData(GetInputTerminal(), NULL, GateProc);

//	m_treeALTobject.LoadData(m_nAirportID,ALT_STAND);
	{
		std::vector<int> vAirportIds;
		InputAirside::GetAirportList(m_nProjID, vAirportIds);
		for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
		{
			ALTAirport airport;
			airport.ReadAirport(*iterAirportID);
			airport.getName();
			//HTREEITEM hAirport = m_treeALTobject.InsertItem(airport.getName(),hStandFamily);

			//m_treeALTobject.SetItemData(hAirport, *iterAirportID);
			ALTObjectList vStands;
			ALTAirport::GetStandList(*iterAirportID,vStands);

			for(int i = 0;i< (int)vStands.size(); ++i)
			{
				Stand * pStand =(Stand*) vStands.at(i).get();
				AddObjectToTree(hStandFamily,pStand);
			}

		}
		m_treeALTobject.Expand(hStandFamily, TVE_EXPAND);
	}
}

extern CTermPlanApp theApp;
InputTerminal* CFlightGateSelectionDlg::GetInputTerminal()
{
//	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)(m_pParentWnd->GetParent()))->GetDocument();
	POSITION ps = theApp.m_pDocManager->GetFirstDocTemplatePosition();
	POSITION ps2 = theApp.m_pDocManager->GetNextDocTemplate(ps)->GetFirstDocPosition();
	CTermPlanDoc *pDoc = (CTermPlanDoc*)(theApp.m_pDocManager->GetNextDocTemplate(ps)->GetNextDoc(ps2));
    return (InputTerminal*)&pDoc->GetTerminal();
}

BOOL CFlightGateSelectionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	ReloadDatabase();	

	//set the flight/sector string.
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFlightGateSelectionDlg::OnOK() 
{

	if(m_nIndex == -1)
	{
		FlightGate flightGate;
		
		//get gate processor id;
		HTREEITEM hSelItem = m_treeALTobject.GetSelectedItem();
		if(hSelItem == NULL)
		{
			CDialog::OnOK();
			return;
		}
		CString strSelItem = m_treeALTobject.GetTreeItemFullString(hSelItem);
	/*	ProcessorID *procID = new ProcessorID;
		procID->SetStrDict(GetInputTerminal()->inStrDict);*/
		ALTObjectID objID(strSelItem.GetBuffer(strSelItem.GetLength() + 1));
		flightGate.procID = objID;
		
		//get flight constraint;
		flightGate.flight = m_flight;
		
		((CFlightPriorityDlg *)GetParent())->m_vectFlightGate.push_back(flightGate);
		
		//delete procID;
		
		m_bChanged = true;
		CDialog::OnOK();
	}
	else
	{
		FlightGate flightGate;
		
		//get gate processor id;
		HTREEITEM hSelItem = m_treeALTobject.GetSelectedItem();
		if(hSelItem == NULL)
		{
			CDialog::OnOK();
			return;
		}
		CString strSelItem = m_treeALTobject.GetTreeItemFullString(hSelItem);
		//ProcessorID *procID = new ProcessorID;
		//procID->SetStrDict(GetInputTerminal()->inStrDict);
		//procID->setID();
		
		flightGate.procID = strSelItem.GetBuffer(strSelItem.GetLength() + 1);
		
		//get flight constraint;
		flightGate.flight = m_flight;
		
		((CFlightPriorityDlg *)GetParent())->m_vectFlightGate[m_nIndex] = flightGate;
		
		//delete procID;
		
		m_bChanged = true;
		CDialog::OnOK();
	}

}

