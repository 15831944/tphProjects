// ACCatPropertiesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "..\common\ACTypesManager.h"
#include "ACCatPropertiesDlg.h"
#include "common\WINMSG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "../Common/AirportDatabase.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgACCatProperties dialog


CDlgACCatProperties::CDlgACCatProperties(CACTypesManager* _pAcMan, CACCategory* pACC, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgACCatProperties::IDD, pParent), m_pAcMan( _pAcMan )
{
	ASSERT(pACC != NULL);
	//{{AFX_DATA_INIT(CDlgACCatProperties)
//	m_sDesc = _T("");
	m_sName = pACC->m_sName;
	//}}AFX_DATA_INIT
	m_pACC = pACC;
	int nSize = pACC->m_vACTypeFilters.size();
/*	for(int i=0; i<(nSize-1); i++) {
		m_sDesc = m_sDesc + pACC->m_vACTypeFilters[i].code + ",";
	}
	if(pACC->m_vACTypeFilters.size() > 0)
		m_sDesc = m_sDesc + pACC->m_vACTypeFilters[pACC->m_vACTypeFilters.size()-1].code;*/
}


void CDlgACCatProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgACCatProperties)
	DDX_Text(pDX, IDC_ACCATNAME, m_sName);
	DDV_MaxChars(pDX, m_sName, 512);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_LIST_CONDITION, m_lstCondition);
}


BEGIN_MESSAGE_MAP(CDlgACCatProperties, CDialog)
	//{{AFX_MSG_MAP(CDlgACCatProperties)
//	ON_BN_CLICKED(IDC_BUTTON_OR, OnButtonOr)
//	ON_BN_CLICKED(IDC_BUTTON_AND, OnButtonAnd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_COMMAND(ID_AIRCATFILTER_NEW, OnNewFilter)
	//ON_COMMAND(ID_AIRCATFILTER_CHANGE, OnEditFilter)
	ON_COMMAND(ID_AIRCATFILTER_DELETE, OnRemoveFilter)
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_INPLACE_EDIT, OnFieldCheck)
	ON_MESSAGE( WM_INPLACE_COMBO_KILLFOUCUS, OnFieldCheck)
	ON_BN_CLICKED(IDC_RADIO_SCRIPT, OnBnClickedScript)
	ON_BN_CLICKED(IDC_RADIO_NONSCRIPT, OnBnClickedNoScript)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgACCatProperties message handlers

BOOL CDlgACCatProperties::OnInitDialog() 
{
	CDialog::OnInitDialog();
	ASSERT(::IsWindow(m_lstCondition.m_hWnd));
	ASSERT(m_pAcMan != NULL);
	m_lstCondition.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_AIRCRAFTCAT_FILTER))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	CRect rect;
	GetDlgItem( IDC_STATIC_ACCATPROPERTY_TOOLBAR )->GetWindowRect( &rect );
	ScreenToClient(rect);
	m_wndToolBar.SetWindowPos(NULL,rect.left,rect.top,rect.Width(),26,NULL);
	
	InitListCtrl();

	int nIDCheckButton = m_pACC->m_bHasScript ? IDC_RADIO_SCRIPT : IDC_RADIO_NONSCRIPT;
	CheckRadioButton(IDC_RADIO_NONSCRIPT, IDC_RADIO_SCRIPT, nIDCheckButton );
	EnableListCtrl(m_pACC->m_bHasScript);
	
	return TRUE;
}

void CDlgACCatProperties::InitListCtrl()
{
	static LPTSTR sColTitle[] =
	{
		_T("Logic"),_T("Variable"),_T("Condition"),_T("Parameter")
	};
	static const int nColWidths[] =
	{
		80, 160, 80, 150
	};
	static LPCTSTR sLogicOperator[] =
	{
		_T("="),_T("!="),_T(">"),_T(">="),_T("<"),_T("<=")
	};

	CStringList arConnectorComboBox;
	arConnectorComboBox.AddTail("Or");
	arConnectorComboBox.AddTail("And");
	
	CStringList arLogicComboBox;
	for(int n=0; n< sizeof(sLogicOperator)/sizeof(LPCSTR); n++)
		arLogicComboBox.AddTail( sLogicOperator[n]);

	int nColFormat[] = 
	{	
		LVCFMT_CENTER | LVCFMT_DROPDOWN, LVCFMT_CENTER | LVCFMT_DROPDOWN, LVCFMT_CENTER | LVCFMT_DROPDOWN , LVCFMT_CENTER | LVCFMT_EDIT 
	};

	CString sFieldName;
	sFieldName.LoadString(IDS_ATD_COLNAMES);
	int first = -1, next = -1;
	CStringList arVariableComboBox;
	while (TRUE)
	{
		next = sFieldName.Find(',', first+1);
		if(next != -1)
			arVariableComboBox.AddTail(sFieldName.Mid(first+1, next-first-1));
		else
		{
			arVariableComboBox.AddTail(sFieldName.Mid(first+1));
			break;
		}
		first = next;
	}

		LV_COLUMNEX	lvc;
	memset(&lvc, 0, sizeof(lvc));

	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;

	int nColCount = sizeof( nColWidths ) / sizeof( int );
	for(int i=0; i<nColCount; i++)
	{
		lvc.fmt = nColFormat[i];
		lvc.pszText = sColTitle[i];
		lvc.cx = nColWidths[i];

		switch( i )
		{
		case 0:
			lvc.csList = &arConnectorComboBox;
			break;
		case 1:
			lvc.csList = &arVariableComboBox;
			break;
		case 2:
			lvc.csList = &arLogicComboBox;
			break;
		}

		m_lstCondition.InsertColumn( i, &lvc );
	}

	if (m_pACC->m_bHasScript)
		FillListCtrl();
}

void CDlgACCatProperties::FillListCtrl()
{
/*

	ASSERT(m_pACC != NULL);
	m_lstCondition.DeleteAllItems();

	for (std::vector<CACTYPEFILTER>::const_iterator iter = m_pACC->m_vACTypeFilters.begin();
		 iter != m_pACC->m_vACTypeFilters.end();
		 iter++)
	{
		CString strFilter(iter->code);
		CString strLogical;
		if (strFilter.GetAt(0) == _T('|'))
		{
			strLogical = _T("Or");
		}
		else if (strFilter.GetAt(0) == _T('&'))
		{
			strLogical = _T("And");
		}
		CString strExpression = strFilter.Right(strFilter.GetLength() - 1);

		int iPos = m_lstCondition.GetItemCount();
		m_lstCondition.InsertItem(iPos, strLogical);
		m_lstCondition.SetItemText(iPos, 1, strExpression);
	}

*/
	m_lstCondition.DeleteAllItems();

	int nCount =0;
	CString sTemp("");
	int nPos1 = -1;
	int nPos2 = -1;
	for( std::vector<CACTYPEFILTER>::const_iterator iter = m_pACC->m_vACTypeFilters.begin(); 
		iter!=m_pACC->m_vACTypeFilters.end(); iter++)
	{
		nCount = m_lstCondition.GetItemCount();
		sTemp = iter->code;
		m_lstCondition.InsertItem( nCount, sTemp.GetAt(0)=='|'? "Or" : "And");
		nPos1 = sTemp.Find( _T('{'));
		m_lstCondition.SetItemText( nCount, 1, sTemp.Mid( 1,nPos1-1 ));//varable name
		nPos2 = sTemp.Find( _T('}'));
		m_lstCondition.SetItemText( nCount, 2, sTemp.Mid( nPos1+1, nPos2-nPos1-1));//relation operator
		m_lstCondition.SetItemText( nCount, 3, sTemp.Mid( nPos2+1));//value
	}

}

void CDlgACCatProperties::OnOK() 
{
	UpdateData(TRUE);
	if (m_sName.IsEmpty())
	{
		AfxMessageBox("Please enter a aircraft category name", MB_ICONEXCLAMATION|MB_OK);
		return;
	}

    m_sName.Replace("\'"," \'\'");
	m_pACC->m_sName = m_sName;
	m_pACC->m_vACTypeFilters.clear();
	CACTYPEFILTER actf;
/*
		while(TRUE) {
			int nIdx = m_sDesc.Find(',');
			if(nIdx != -1) { // found a comma
				actf.code = m_sDesc.Left(nIdx);
				actf.code.TrimLeft();
				m_pAcMan->AddACCategory(m_sName, actf);
				m_sDesc = m_sDesc.Right(m_sDesc.GetLength()-nIdx-1);
			}
			else if(!m_sDesc.IsEmpty()){ // no comma, but string not empty
				actf.code = m_sDesc;
				actf.code.TrimLeft();
				m_pAcMan->AddACCategory(m_sName, actf);
				break;
			}
			else { // no comma, string empty
				break;
			}
		}
*/
	CString sFilter("");
	CString sTemp("");
	int j=0;
	int nColumns = m_lstCondition.GetHeaderCtrl()->GetItemCount();
	for(int i=0;i<m_lstCondition.GetItemCount(); i++)
	{
		for(j=0; j< nColumns; j++)
		{
			sTemp = m_lstCondition.GetItemText(i, j);
			sTemp.TrimRight();
			sTemp.TrimLeft();
			if(sTemp.IsEmpty())break;
		}
		if( j==nColumns )
		{
			sFilter = m_lstCondition.GetItemText( i, 0) == _T("Or") ? _T("|") : _T("&");
			sFilter += m_lstCondition.GetItemText( i, 1)//variable
				+ "{"+m_lstCondition.GetItemText(i, 2)+"}"//relation operator
				+ m_lstCondition.GetItemText(i, 3);//value

			actf.code = sFilter;
			//m_pAcMan->AddACCategory( m_sName, actf);
			m_pACC->m_vACTypeFilters.push_back( actf );
		}
	}
	m_pACC->ClacACTypeList();
	
		
	CDialog::OnOK();
}

/*
void CDlgACCatProperties::OnButtonOr() 
{

		UpdateData();
		CString strField;
		CString strOperator;
		m_cmbACField.GetWindowText(strField);
		m_cmbOperator.GetWindowText(strOperator);
	
		if (strField.IsEmpty() || strField.IsEmpty())
			return;
	
		while (TRUE)
		{
			int nIdx = m_strDesc.Find(',');
			if (nIdx != -1) // found a comma
			{ 
				CString strCondition(m_strDesc.Left(nIdx));
				strCondition.TrimLeft();
				strCondition = _T('|') + strField + strOperator + strCondition;
				CACTYPEFILTER actypeFilter;
				actypeFilter.code = strCondition;
				m_pACC->m_vACTypeFilters.push_back(actypeFilter);
				m_strDesc = m_strDesc.Right(m_strDesc.GetLength()-nIdx-1);
			}
			else if (!m_strDesc.IsEmpty()) // no comma, but string not empty
			{ 
				m_strDesc.TrimLeft();
				CString strCondition = _T('|') + strField + strOperator + m_strDesc;
				CACTYPEFILTER actypeFilter;
				actypeFilter.code = strCondition;
				m_pACC->m_vACTypeFilters.push_back(actypeFilter);
				break;
			}
			else // no comma, string empty
			{ 
				break;
			}
		}
		UpdateData(FALSE);
		FillListCtrl();
	
}

void CDlgACCatProperties::OnButtonAnd() 
{
	UpdateData();
	CString strField;
	CString strOperator;
	m_cmbACField.GetWindowText(strField);
	m_cmbOperator.GetWindowText(strOperator);

	if (strField.IsEmpty() || strField.IsEmpty())
		return;

	while (TRUE)
	{
		int nIdx = m_strDesc.Find(',');
		if (nIdx != -1) // found a comma
		{ 
			CString strCondition(m_strDesc.Left(nIdx));
			strCondition.TrimLeft();
			strCondition = _T('&') + strField + strOperator + strCondition;
			CACTYPEFILTER actypeFilter;
			actypeFilter.code = strCondition;
			m_pACC->m_vACTypeFilters.push_back(actypeFilter);
			m_strDesc = m_strDesc.Right(m_strDesc.GetLength()-nIdx-1);
		}
		else if (!m_strDesc.IsEmpty()) // no comma, but string not empty
		{ 
			m_strDesc.TrimLeft();
			CString strCondition = _T('&') + strField + strOperator + m_strDesc;
			CACTYPEFILTER actypeFilter;
			actypeFilter.code = strCondition;
			m_pACC->m_vACTypeFilters.push_back(actypeFilter);
			break;
		}
		else // no comma, string empty
		{ 
			break;
		}
	}
	UpdateData(FALSE);
	FillListCtrl();
}
*/

void CDlgACCatProperties::OnButtonDel() 
{
	if(AfxMessageBox(_T("Are you sure?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		POSITION pos = m_lstCondition.GetFirstSelectedItemPosition();
		while (pos)
		{
			int sel= m_lstCondition.GetNextSelectedItem(pos);
			if(sel != -1)
			{
				m_pACC->m_vACTypeFilters.erase(m_pACC->m_vACTypeFilters.begin() + sel);
				FillListCtrl();
			}
		}
	}
	
}


void CDlgACCatProperties::OnNewFilter()
{
	m_lstCondition.EditNew();
	
}

void CDlgACCatProperties::OnEditFilter()
{
}

void CDlgACCatProperties::OnRemoveFilter()
{
	int nCurItem = m_lstCondition.GetCurSel();
	if( nCurItem != -1)
		m_lstCondition.DeleteItemEx( nCurItem );
}

LRESULT CDlgACCatProperties::OnFieldCheck( WPARAM wParam, LPARAM lParam)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)lParam;
	LV_ITEM* plvItem = &pDispInfo->item;
	
	int nCurSubItem = plvItem->iSubItem;
	int nCurItem = plvItem->iItem;
	CString sTemp(plvItem->pszText);
	sTemp.TrimRight();
	sTemp.TrimLeft();

	m_lstCondition.SetItemText(nCurItem, nCurSubItem, sTemp);
	return TRUE;
/*
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)lParam;
	LV_ITEM* plvItem = &pDispInfo->item;
	
	int nCurSubItem = plvItem->iSubItem;
	int nCurItem = plvItem->iItem;
	int nColumns = m_lstCondition.GetHeaderCtrl()->GetItemCount();
	
	int nTargetCol = -1;
	//two steps
	// 1. current col 2 last col
	for(int i=nCurSubItem; i<nColumns; i++)
	{
		if( m_lstCondition.GetItemText( nCurItem, i).IsEmpty() )
		{
			nTargetCol = i;
			break;
		}
	}
	// 2. first col 2 current col
	if(nTargetCol == -1)
	{
		for( i=0; i<nCurSubItem; i++)
		{
			if( m_lstCondition.GetItemText( nCurItem, i).IsEmpty())
			{
				nTargetCol = i;
				break;
			}
		}
	}

	if( nTargetCol != -1)
	{
		CRect rect(0,0,0,0);
		CPoint point(0,0);
		CHeaderCtrl* pHeader = m_lstCondition.GetHeaderCtrl();
		pHeader->GetItemRect(  nTargetCol, &rect );
		point.x = rect.left+1;
		m_lstCondition.GetItemRect( nCurItem, &rect, LVIR_LABEL);

		point.y = rect.top +1;

		m_lstCondition.SendMessage( WM_LBUTTONDOWN, 0, MAKELPARAM(point.x, point.y));
	}
*/

}

void CDlgACCatProperties::EnableListCtrl(bool bEnable)
{
	m_wndToolBar.EnableWindow(bEnable);
	m_lstCondition.EnableWindow(bEnable);
}

void CDlgACCatProperties::OnBnClickedScript()
{
	m_pACC->m_bHasScript = true;
	EnableListCtrl();
}

void CDlgACCatProperties::OnBnClickedNoScript()
{
	m_pACC->m_bHasScript = false;
	EnableListCtrl(false);
}