// DlgMobElementsDiagnosis.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgMobElementsDiagnosis.h"
#include "MFCExControl\SmartEdit.h"
#include "common\WinMsg.h"

#define DiagnosisFilePath ("\\Diagnosis\\")
#define MobElementDiagnosisConfigFile ("MobElementsDiagnosis.cfg")

//------------------------------------------------------------------------------------------------
//
#include "Common/termfile.h"
#include "Common/exeption.h"
#include "Common/fileman.h"
class DiagnosisMobElementsIDConfig
{
public:
	/**
	Config File Format:
	Mobile Elements Diagnosis Config
	recordCount,id,id,id
	3,77,1980,1982
	**/

	void loadConfig(std::string strFileName, std::vector<std::string>& mobElementIdList)
	{
		try {
			ArctermFile file;
			file.openFile (strFileName.c_str(), READ); 

			file.getLine();
			int nCount = 0;
			file.getInteger(nCount);
			for (int i = 0; i < nCount; i++)
			{
				char szBuffer[1024] = {0};	
				file.getField(szBuffer, SMALL);
				
				std::string str = szBuffer;
				if( str.empty() )
					continue;

				std::string strMobElementId(szBuffer);
				mobElementIdList.push_back( strMobElementId );
			}

			file.closeIn();
		}
		catch (FileOpenError *exception){
			delete exception;
			return;
		}
		

	}

	void saveConfig(std::string strFilePath,std::string strFileName, std::vector<std::string>& mobElementIdList)
	{
		try {
			strFilePath.append( strFileName);
			
			ArctermFile file;
			file.openFile (strFilePath.c_str(), WRITE, 1.0); 

			file.writeField ("Mobile Elements Diagnosis Config");
			file.writeLine();
			file.writeField ("recordCount,id,id,id,");
			file.writeLine();

			file.writeInt( (int)mobElementIdList.size());
			for (size_t i = 0; i < mobElementIdList.size(); i++)
			{
				if(mobElementIdList.at(i).empty())
					continue;
				file.writeField( mobElementIdList.at(i).c_str() );
			}
			file.writeLine();

			file.endFile();
		}
		catch (FileOpenError *exception){
			delete exception;
			return;
		}
		catch (FileWriteError *exception){
			delete exception;
			return;
		}
	}

};

//----------------------------------------------------------------------------
// CDlgMobElementsDiagnosis dialog

IMPLEMENT_DYNAMIC(CDlgMobElementsDiagnosis, CDialog)
CDlgMobElementsDiagnosis::CDlgMobElementsDiagnosis(CString strProjPath, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMobElementsDiagnosis::IDD, pParent)
	,m_strProjPath(strProjPath)
	,mDisableAll(0)
{
	std::string strDiagnosisFilePath = m_strProjPath;
	strDiagnosisFilePath.append(DiagnosisFilePath);
	FileManager::MakePath( strDiagnosisFilePath.c_str());

	m_nSelIdx = LB_ERR;

	m_ImageList.Create(IDB_BITMAP_ARCTOOLBAR, 14, 1, RGB(255,0,255));
	m_wndARCToolbar.SetImageList(&m_ImageList);
	m_wndARCToolbar.SetStyle(false, false);

	m_wndARCToolbar.AddButton(ID_ARCTOOLBAR_ADD, 0, "Add Id");
	m_wndARCToolbar.AddButton(ID_ARCTOOLBAR_EDIT, 1, "Edit Id");
	m_wndARCToolbar.AddButton(ID_ARCTOOLBAR_DELETE, 2, "Delete Id");

	m_wndARCToolbar.SetTitle(_T("MobElement Id"));

}

CDlgMobElementsDiagnosis::~CDlgMobElementsDiagnosis()
{
}

void CDlgMobElementsDiagnosis::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TOOLBAR, m_wndARCToolbar);	
	DDX_Control(pDX, IDC_LIST_MOBELEMENTSID, m_wndListBox);
	DDX_Check(pDX, IDC_CHECK_DISABLE, mDisableAll);
}


BEGIN_MESSAGE_MAP(CDlgMobElementsDiagnosis, CDialog)
	ON_BN_CLICKED(ID_ARCTOOLBAR_ADD, &CDlgMobElementsDiagnosis::OnAddItem)
	ON_BN_CLICKED(ID_ARCTOOLBAR_EDIT, &CDlgMobElementsDiagnosis::OnEditItem)
	ON_BN_CLICKED(ID_ARCTOOLBAR_DELETE, &CDlgMobElementsDiagnosis::OnDeleteItem)


	ON_LBN_DBLCLK(IDC_LIST_MOBELEMENTSID, OnDblclkListBox)
	ON_MESSAGE(WM_END_EDIT,OnEndEdit)
	ON_LBN_SELCHANGE(IDC_LIST_MOBELEMENTSID, OnSelchangeListBox)
	ON_EN_KILLFOCUS(IDC_EDIT_NAME, OnEnKillfocusEditMobElementID)

	ON_BN_CLICKED(IDC_BTN_OPEN, OnBnClickedOpen)
END_MESSAGE_MAP()


// CDlgMobElementsDiagnosis message handlers

BOOL CDlgMobElementsDiagnosis::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_wndARCToolbar.EnableButton(ID_ARCTOOLBAR_EDIT, FALSE);
	LoadMobileElementsID();

	( (CButton*)GetDlgItem(IDC_RADIO_PAXS))->SetCheck( TRUE );

	//<TODO::>
	( (CButton*)GetDlgItem(IDC_RADIO_FLIGHTS))->EnableWindow( FALSE );
	( (CButton*)GetDlgItem(IDC_RADIO_AVEHICLES))->EnableWindow( FALSE );
	( (CButton*)GetDlgItem(IDC_RADIO_LVEHICLES))->EnableWindow( FALSE );
	( (CButton*)GetDlgItem(IDC_CHECK_DISABLE))->EnableWindow( FALSE );


	return TRUE;
}

void CDlgMobElementsDiagnosis::UpdateUIState()
{

}

void CDlgMobElementsDiagnosis::LoadMobileElementsID()
{

	std::string strDiagnosisFile;
	strDiagnosisFile = m_strProjPath;
	strDiagnosisFile.append(DiagnosisFilePath);
	strDiagnosisFile.append(MobElementDiagnosisConfigFile);
	
	DiagnosisMobElementsIDConfig _cfg;
	_cfg.loadConfig(strDiagnosisFile, mMobElementIdList);

	if(mMobElementIdList.size() < 1)
		return;

	for (size_t i = 0 ; i < mMobElementIdList.size(); i++)
	{
		m_wndListBox.AddString( LPCTSTR(mMobElementIdList.at(i).c_str()) );
	}
}

void CDlgMobElementsDiagnosis::OnEnKillfocusEditMobElementID()
{

	CString levelName ;
	m_wndEditBox.GetWindowText(levelName ) ;

}

void CDlgMobElementsDiagnosis::OnSelchangeListBox()
{
	int nCurSel = m_wndListBox.GetCurSel();
	nCurSel == LB_ERR ? m_wndARCToolbar.EnableButton(ID_ARCTOOLBAR_DELETE, FALSE) :
	m_wndARCToolbar.EnableButton(ID_ARCTOOLBAR_DELETE, TRUE);
}

void CDlgMobElementsDiagnosis::OnDblclkListBox()
{
	m_nSelIdx = m_wndListBox.GetCurSel();
	if( m_nSelIdx == LB_ERR )
		return;

	EditItem( m_nSelIdx );
}

void CDlgMobElementsDiagnosis::EditItem( int _nIdx )
{
	CRect rectItem;
	CRect rectLB;
	m_wndListBox.GetWindowRect( &rectLB );
	ScreenToClient( &rectLB );
	m_wndListBox.GetItemRect( _nIdx, &rectItem );

	rectItem.OffsetRect( rectLB.left+2, rectLB.top+1 );

	rectItem.right += 1;
	rectItem.bottom += 4;

	CString csItemText;
	m_wndListBox.GetText( _nIdx, csItemText );
	CSmartEdit* pEdit = new CSmartEdit( csItemText );
	DWORD dwStyle = ES_LEFT | WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|ES_NUMBER;
	pEdit->Create( dwStyle, rectItem, this, IDC_IPEDIT );	

	m_wndARCToolbar.EnableButton(ID_ARCTOOLBAR_ADD, FALSE);
	m_wndARCToolbar.EnableButton(ID_ARCTOOLBAR_DELETE, FALSE);
}

LONG CDlgMobElementsDiagnosis::OnEndEdit( WPARAM p_wParam, LPARAM p_lParam )
{
	if( p_lParam != IDC_IPEDIT )
		return 0;

	if( m_nSelIdx == LB_ERR )
		return 0;

	CString csStr( (LPTSTR)p_wParam );
	if( csStr.IsEmpty() )
	{
		//MessageBox( "Empty string", "Error", MB_OK|MB_ICONWARNING );
		m_wndListBox.DeleteString( m_nSelIdx );
		m_wndListBox.SetCurSel( -1 );
		OnSelchangeListBox();
	}
	else
	{
		// check if dup
		int i=0;
		for(; i<m_nSelIdx; i++ )
		{
			CString csTemp;
			m_wndListBox.GetText( i, csTemp );
			if( csTemp.CompareNoCase( csStr ) == 0 )
			{
				MessageBox( "Duplicated id", "Error", MB_OK|MB_ICONWARNING );
				m_wndListBox.DeleteString( m_nSelIdx );
				m_wndListBox.SetCurSel( -1 );
				OnSelchangeListBox();
				break;
			}
		}

		if( i == m_nSelIdx )
		{

			m_wndListBox.InsertString( m_nSelIdx, (LPTSTR)p_wParam );
			m_wndListBox.DeleteString( m_nSelIdx + 1 );

			m_wndListBox.SetCurSel( m_nSelIdx );
			OnSelchangeListBox();
		}
	}
	m_wndARCToolbar.EnableButton(ID_ARCTOOLBAR_ADD, TRUE);
	m_wndARCToolbar.EnableButton(ID_ARCTOOLBAR_DELETE, TRUE);
	return 0;
}

void CDlgMobElementsDiagnosis::OnAddItem()
{
	int nCount = m_wndListBox.GetCount();
	m_nSelIdx = m_wndListBox.GetCount();
	m_wndListBox.InsertString( m_nSelIdx, "" );
	m_wndListBox.SetCurSel( m_nSelIdx );
	OnSelchangeListBox();

	EditItem( m_nSelIdx );	

}

void CDlgMobElementsDiagnosis::OnEditItem()
{

}

void CDlgMobElementsDiagnosis::OnDeleteItem()
{
	int nCurSel = m_wndListBox.GetCurSel();
	if( nCurSel == LB_ERR )
		return;

	m_wndListBox.DeleteString( nCurSel );
	m_wndListBox.SetCurSel( -1 );
	OnSelchangeListBox();
}


void CDlgMobElementsDiagnosis::OnOK()
{

	mMobElementIdList.clear();

	for( int i = 0 ; i < m_wndListBox.GetCount(); i++)
	{
		CString strMobIDString;
		m_wndListBox.GetText( i, strMobIDString );

		long nValue = atoi(strMobIDString);
		if(nValue < 0 || nValue > INT_MAX)
			continue;

		std::string strTemp(strMobIDString);
		mMobElementIdList.push_back( strTemp );
	}

	// save to file
	std::string strDiagnosisFilePath = m_strProjPath;
	strDiagnosisFilePath.append(DiagnosisFilePath);

	DiagnosisMobElementsIDConfig _cfg;
	_cfg.saveConfig(strDiagnosisFilePath, MobElementDiagnosisConfigFile, mMobElementIdList);

	CDialog::OnOK();
}

void CDlgMobElementsDiagnosis::OnBnClickedOpen()
{
	std::string strDiagnosisFilePath;
	strDiagnosisFilePath = m_strProjPath;
	strDiagnosisFilePath.append(DiagnosisFilePath);

	ShellExecute(NULL, "open", strDiagnosisFilePath.c_str(), NULL, NULL, SW_SHOW);
}

