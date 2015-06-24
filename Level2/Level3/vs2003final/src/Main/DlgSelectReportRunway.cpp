// DlgSelectReportRunway.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgSelectReportRunway.h"
#include ".\dlgselectreportrunway.h"
#include "../InputAirside/ALTObject.h"
#include "../InputAirside/Runway.h"

// CDlgSelectReportRunway dialog

IMPLEMENT_DYNAMIC(CDlgSelectReportRunway, CDialog)
CDlgSelectReportRunway::CDlgSelectReportRunway(int nProjID,CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_REPORTSELECTRUNWAYMARK, pParent)
{
	m_nProjID = nProjID;
}

CDlgSelectReportRunway::~CDlgSelectReportRunway()
{
	
}

void CDlgSelectReportRunway::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTRUNWAY, m_lbRunway);
}


BEGIN_MESSAGE_MAP(CDlgSelectReportRunway, CDialog)
END_MESSAGE_MAP()


// CDlgSelectReportRunway message handlers

BOOL CDlgSelectReportRunway::OnInitDialog()
{
	CDialog::OnInitDialog();

	//add all
	CAirsideReportRunwayMark allRunwayMark;
	allRunwayMark.SetRunway(-1,RUNWAYMARK_FIRST,_T("ALL"));
	m_lbRunway.AddString(_T("All runway"));
	m_vAllRunwayMark.push_back(allRunwayMark);

	std::vector< ALTObject> vRunwayObjct;		
	try
	{
		CString strSQL = _T("");

		CString strSelect = _T("");
		strSelect.Format(_T("SELECT ID, APTID, NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOCKED\
							FROM         ALTOBJECT\
							WHERE     (TYPE = %d) AND (APTID IN\
							(SELECT     ID\
							FROM          ALTAIRPORT\
							WHERE      (PROJID = %d)))"),ALT_RUNWAY,m_nProjID);


		long nRecordCount = -1;
		CADORecordset adoRecordset;
		CADODatabase::ExecuteSQLStatement(strSelect,nRecordCount,adoRecordset);
		while(!adoRecordset.IsEOF())
		{
			ALTObject object;
			int nObjID = -1;
			adoRecordset.GetFieldValue(_T("ID"),nObjID);
			object.ReadObject(adoRecordset);
			object.setID(nObjID);

			vRunwayObjct.push_back(object);

			adoRecordset.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}

	//add runway node
	for (int nRunway = 0; nRunway < static_cast<int>(vRunwayObjct.size()); ++ nRunway)
	{
		int nObjID = vRunwayObjct.at(nRunway).getID();
		Runway* pRunway = (Runway *)ALTObject::ReadObjectByID(nObjID);
		if(pRunway != NULL)
		{
			CAirsideReportRunwayMark runwayMark1;
			CString strMark1 = CString(pRunway->GetMarking1().c_str());
			runwayMark1.SetRunway(pRunway->getID(),RUNWAYMARK_FIRST,strMark1);


			CAirsideReportRunwayMark runwayMark2;
			CString strMark2 = CString(pRunway->GetMarking2().c_str());
			runwayMark2.SetRunway(pRunway->getID(),RUNWAYMARK_SECOND,strMark2);

			

			m_lbRunway.AddString(strMark1);
			m_vAllRunwayMark.push_back(runwayMark1);
			m_lbRunway.AddString(strMark2);
			m_vAllRunwayMark.push_back(runwayMark2);
		}
	}
	
	//default
	m_lbRunway.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectReportRunway::OnOK()
{
	m_vRunwayMarkSel.clear();
	// TODO: Add your specialized code here and/or call the base class
	int nSelCount = m_lbRunway.GetSelCount();
	if (nSelCount <= 0)
	{
		MessageBox(_T("Please select at least one item."));
		return;
	}

	CArray<int,int>   aryListBoxSel;   
	aryListBoxSel.SetSize(nSelCount);   
	m_lbRunway.GetSelItems(nSelCount, aryListBoxSel.GetData());

	for (int nSel = 0; nSel < nSelCount; ++nSel)
	{
		int nItem = aryListBoxSel[nSel];
		if(nItem < static_cast<int>(m_vAllRunwayMark.size()))
		{
			if(m_vAllRunwayMark[nItem].m_nRunwayID == -1)//all selected
			{
				m_vRunwayMarkSel.clear();
				m_vRunwayMarkSel.push_back(m_vAllRunwayMark[nItem]);
				break;
			}
			m_vRunwayMarkSel.push_back(m_vAllRunwayMark[nItem]);
		}
	}
	CDialog::OnOK();
}

void CDlgSelectReportRunway::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnCancel();
}
