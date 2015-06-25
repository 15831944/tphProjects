#include "StdAfx.h"
#include "resource.h"
#include "DlgTaxiwaySegmentSelect.h"
#include "InputAirside/Taxiway.h"
#include "InputAirside/InputAirside.h"
#include "InputAirside/ALTAirport.h"
#include "InputAirside/TimeRangeRunwayExitStrategyItem.h"

CDlgTaxiwaySegmentSelect::CDlgTaxiwaySegmentSelect(int nProjID,CWnd* pParent /*=NULL*/)
	 : CDialog(CDlgTaxiwaySegmentSelect::IDD, pParent)
	 ,m_nProjID(nProjID)
{
}


CDlgTaxiwaySegmentSelect::~CDlgTaxiwaySegmentSelect(void)
{
}

BEGIN_MESSAGE_MAP(CDlgTaxiwaySegmentSelect, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_TAXIWAY, OnCbnSelchangeComboData)
END_MESSAGE_MAP()

void CDlgTaxiwaySegmentSelect::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_TAXIWAY, m_wndComboTaxiway);
	DDX_Control(pDX, IDC_COMBO_START, m_wndComboStart);
	DDX_Control(pDX, IDC_COMBO_END, m_wndComboEnd);
}

BOOL CDlgTaxiwaySegmentSelect::OnInitDialog()
{
	CDialog::OnInitDialog();
	DisplayComboTaxiContent();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

void CDlgTaxiwaySegmentSelect::DisplayComboNodeContent( int nTaxiwayID )
{
	if (nTaxiwayID == -1)
		return;
	
	IntersectionNodeList taxiwayNodeList = GetTaxiwayNodeList(nTaxiwayID);

	int iStartSel = -1;
	int iEndSel = -1;
	for (size_t i = 0; i < taxiwayNodeList.size(); i++)
	{
		IntersectionNode node = taxiwayNodeList[i];
		//start
		int iStart = m_wndComboStart.AddString(node.GetName());
		m_wndComboStart.SetItemData(iStart,(DWORD)node.GetID());
		if (node.GetID() == m_nStartNodeID)
		{
			iStartSel = iStart;
		}

		//end
		int iEnd = m_wndComboEnd.AddString(node.GetName());
		m_wndComboEnd.SetItemData(iEnd,(DWORD)node.GetID());
		if (node.GetID() == m_nEndNodeID)
		{
			iEndSel = iEnd;
		}
	}

	m_wndComboStart.SetCurSel(iStartSel);
	m_wndComboEnd.SetCurSel(iEndSel);
}

IntersectionNodeList CDlgTaxiwaySegmentSelect::GetTaxiwayNodeList( int nTaxiwayID )
{
	Taxiway taxiway;
	taxiway.ReadObject(nTaxiwayID);
	IntersectionNodeList taxiwayNodeList = taxiway.GetIntersectNodes();
	return taxiwayNodeList;
}

void CDlgTaxiwaySegmentSelect::DisplayComboTaxiContent()
{
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	int iCurSel = -1;
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);
		airport.getName();
		ALTObjectList vTaxiways;
		ALTAirport::GetTaxiwayList(*iterAirportID,vTaxiways);

		for(int i = 0;i< (int)vTaxiways.size(); ++i)
		{
			ALTObject* pTaxiway = vTaxiways.at(i).get();
			int idx = m_wndComboTaxiway.AddString(pTaxiway->getName().GetIDString());
			m_wndComboTaxiway.SetItemData(idx,(DWORD)pTaxiway->getID());

			if (m_nTaxiwayID == pTaxiway->getID())
			{
				iCurSel = idx;
			}
		}
	}
	m_wndComboTaxiway.SetCurSel(iCurSel);

	DisplayComboNodeContent(m_nTaxiwayID);
}

void CDlgTaxiwaySegmentSelect::OnCbnSelchangeComboData()
{
	int nCurSel = m_wndComboTaxiway.GetCurSel();
	if (nCurSel == LB_ERR)
		return;
	
	int nTaxiwayID = (int)m_wndComboTaxiway.GetItemData(nCurSel);
	//display intersection node
	DisplayComboNodeContent(nTaxiwayID);
}

void CDlgTaxiwaySegmentSelect::OnOK()
{
	int iTaxiSel = m_wndComboTaxiway.GetCurSel();
	if (iTaxiSel == LB_ERR)
	{
		MessageBox(_T("Please select one taxiway"),_T("Warning"),MB_OK);
		return;
	}
	m_nTaxiwayID = (int)m_wndComboTaxiway.GetItemData(iTaxiSel);

	int iStartSel = m_wndComboStart.GetCurSel();
	if (iStartSel == LB_ERR)
	{
		MessageBox(_T("Please select start node"),_T("Warning"),MB_OK);
		return;
	}
	m_nStartNodeID = (int)m_wndComboStart.GetItemData(iStartSel);

	int iEndSel = m_wndComboEnd.GetCurSel();
	if (iEndSel == LB_ERR)
	{
		MessageBox(_T("Please select end node"),_T("Warning"),MB_OK);
		return;
	}
	m_nEndNodeID = (int)m_wndComboEnd.GetItemData(iEndSel);
	CDialog::OnOK();
}

void CDlgTaxiwaySegmentSelect::GetTaxiwaySegmentData( TaxiSegmentData& segData )
{
	segData.m_iTaxiwayID = m_nTaxiwayID;
	segData.m_iStartNode = m_nStartNodeID;
	segData.m_iEndNode = m_nEndNodeID;
}

void CDlgTaxiwaySegmentSelect::SetTaxiwaySegmentData( const TaxiSegmentData& segData )
{
	m_nTaxiwayID = segData.m_iTaxiwayID;
	m_nStartNodeID = segData.m_iStartNode;
	m_nEndNodeID = segData.m_iEndNode;
}
