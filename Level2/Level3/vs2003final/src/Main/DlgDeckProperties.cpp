#include "StdAfx.h"
#include ".\dlgdeckproperties.h"
#include "TermPlanDoc.h"
#include "../InputOnBoard/Deck.h"
#include "3DGridOptionsDlg.h"
#include "DlgLayerOptions.h"
//BEGIN_MESSAGE_MAP(CProperiesDlg, CDialog)
//	//{{AFX_MSG_MAP(CFloorPropertiesDlg)
//	ON_WM_CREATE()
//	
//	
//	ON_BN_CLICKED(IDC_LAYEROPTIONS, OnLayerOptions)
//	
//	//}}AFX_MSG_MAP
//END_MESSAGE_MAP()
CDlgDeckProperties::CDlgDeckProperties(CDeck* _deck,CTermPlanDoc* _Doc):m_deck(_deck),CProperiesDlg(_deck,_Doc)
{
   m_sFloorName = _deck->GetName() ;
   m_deck->LoadDeckCAD(m_pDoc->GetTerminal().m_pAirportDB);
  
}
CDlgDeckProperties::~CDlgDeckProperties()
{

}
BOOL CDlgDeckProperties::OnInitDialog()
{
	BOOL res = CProperiesDlg::OnInitDialog() ;
	GetDlgItem(IDC_STATIC_GROUP1)->SetWindowText(_T("Deck")) ;
	return res ;
}
void CDlgDeckProperties::OnGridOptions()
{
	int nLevelID = m_deck->GetID();
	C3DGridOptionsDlg dlg(*(m_deck->GetGrid()),m_pDoc);
	if(dlg.DoModal() == IDOK) 
	{
		dlg.UpdateGridData(m_deck->GetGrid());

	}
}
void CDlgDeckProperties::OnOK()
{
	if (SaveData())
	{
		try
		{
			
			CADODatabase::BeginTransaction() ;
			m_deck->SaveGridInfo();
			m_deck->SaveCADInfo();
			m_deck->SaveDeckProperies() ;
			
			CADODatabase::CommitTransaction() ;
			m_deck->SaveLayerProperties(m_pDoc->GetTerminal().m_pAirportDB);
		}
		catch (CADOException e)
		{
			CADODatabase::RollBackTransation() ;
		}

		CDialog::OnOK();
	}
}

void CDlgDeckProperties::OnCancel()
{
    m_deck->ReadCADInfo() ;
	m_deck->ReadDeckProperies() ;
	m_deck->ReadGridInfo() ;
	CDialog::OnCancel();
}