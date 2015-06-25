#include "stdafx.h"
#include "resource.h"
#include "DialogWPImport.h"
#include "..\Common\latlong.h"
#include "..\InputAirside\InputAirside.h"
#include "../InputAirside/ALTObject.h"


IMPLEMENT_DYNAMIC(CDialogWPImport,CXTResizeDialog /*CXTResizeDialog*/)
//----------------------------------------------------------------------------------------------
CDialogWPImport::CDialogWPImport(int nProjectID, CWnd* pParent /*=NULL*/)
: CXTResizeDialog(CDialogWPImport::IDD, pParent)
,m_nProjectId(nProjectID)

{
	m_vWaypoints.clear();
}

CDialogWPImport::~CDialogWPImport()
{	 
}
void CDialogWPImport::UpdateUIState()
{
	int nSel=m_list.GetSelectedCount(); 
	if(nSel)
	{
		m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_IMPORT_DEL, TRUE);
	}
}
int CDialogWPImport::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndFltToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndFltToolbar.LoadToolBar(IDR_TOOLBAR_IPORT))
	{
		return -1;
	}
	return 0;
}
BOOL CDialogWPImport::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	std::vector<int> vAirports;
	InputAirside::GetAirportList(m_nProjectId,vAirports);
	if(vAirports.size()==0)
	{
		MessageBox("No airport in this project, at least one airport needed!");
		return TRUE;
	}

	CRect rectFltToolbar;
	GetDlgItem(IDC_STATIC_IMPORT)->GetWindowRect(&rectFltToolbar);
	ScreenToClient(&rectFltToolbar);
	m_wndFltToolbar.MoveWindow(&rectFltToolbar, true);

	m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_IMPORT_BROWSE, TRUE);	
	m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_IMPORT_DEL, FALSE);	

	m_Airport.ReadAirport(vAirports.at(0));
	CString strLat = "Latitude(" + m_Airport.getLatitude() +")";
	CString strLong = "Longitude(" + m_Airport.getLongtitude() + ")";

	DWORD dwStyle = m_list.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_list.SetExtendedStyle(dwStyle);
	m_list.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 120);
	m_list.InsertColumn(1, strLat, LVCFMT_LEFT, 140);
	m_list.InsertColumn(2, strLong, LVCFMT_LEFT, 140);
	m_list.InsertColumn(3, _T("Lower Limit(m)"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(4, _T("High Limit(m)"), LVCFMT_LEFT, 100);
	UpdateUIState();
	
	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
	SetResize(m_wndFltToolbar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_FM,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_LIST,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_INFO,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

void CDialogWPImport::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_list);
}

BEGIN_MESSAGE_MAP(CDialogWPImport, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_IMPORT_BROWSE,OnNewImport)
	ON_COMMAND(ID_IMPORT_DEL, OnDelRecord)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnLvnItemchangedList)
	ON_BN_CLICKED(IDC_SAVE,OnSave)
END_MESSAGE_MAP()

void CDialogWPImport::OnNewImport()
{

	CFileDialog FileDlg(TRUE,NULL, NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT|OFN_EXPLORER,	_T("CSV Files (*.CSV )|*.CSV;*text|"), NULL, 0, FALSE );
	if(!(IDOK==FileDlg.DoModal()))
		return;
	
	CString FilePath=_T("");
	FilePath=FileDlg.GetPathName();		

	CStdioFile  file;
	file.Open(FilePath,CFile::modeRead);
	CString strLine = _T("");
	CString temp = _T("");
	int idx = -1;
	for(int i=0;file.ReadString(strLine);i++)
	{
		int pos = -1; 

		pos = strLine.Find(_T(",")); 
		temp = strLine.Left(pos);  

		ALTObjectID objName(temp.GetString());	

		if (i ==0 && objName.IsBlank())		// title ignore
			continue;

		if(objName.IsBlank() || !ALTObject::CheckObjNameIsValid(objName, m_Airport.getID()))
		{	
			CString strError;
			strError.Format("The waypoint name in line %d is invalid, cannot be imported, continue or not?",i+1);
			if (AfxMessageBox(strError, MB_YESNO|MB_ICONERROR) == IDNO)
				return;

			continue;
		}
		
		CString space = strLine.Mid(pos+1,strLine.GetLength()); 
		space.TrimLeft(); 		
		pos = space.Find(_T(","));
		temp = space.Left(pos); 
		if(temp.Find("N") == -1 && temp.Find("S") == -1)
		{
			CString strError;
			strError.Format("Invalid latitude data in line %d,cannot be imported, continue or not?",i+1);
			if (AfxMessageBox(strError, MB_YESNO|MB_ICONERROR) == IDNO)
				return;

			continue;
		}

		CLatitude Latitude;
		Latitude.SetValue(temp);


		space = space.Mid(pos+1,space.GetLength()); 
		space.TrimLeft();
		pos = space.Find(_T(",")); 
		temp = space.Left(pos); 

		if(temp.Find("E") == -1 && temp.Find("W") == -1)
		{
			CString strError;
			strError.Format("Invalid longitude data in line %d,cannot be imported, continue or not?",i+1);
			if (AfxMessageBox(strError, MB_YESNO|MB_ICONERROR) == IDNO)
				return;

			continue;
		}
		CLongitude Longtitude;
		Longtitude.SetValue(temp);

		space = space.Mid(pos+1,space.GetLength()); 
		space.TrimLeft(); 
		pos = space.Find(_T(",")); 
		temp = space.Left(pos); 
		DOUBLE Lowerlimit = atof(temp);

		space = space.Mid(pos+1,space.GetLength());  
		space.TrimLeft();
		temp=space;
		DOUBLE Highlimit = atof(temp);

		CPoint2008 Way=m_Airport.GetLatLongPos(Latitude,Longtitude);
		Way.setZ(0);

		AirWayPoint Waypoint;
		Waypoint.setObjName(objName);
		Waypoint.SetServicePoint(Way);
		Waypoint.SetLowLimit(Lowerlimit*100);
		Waypoint.SetHighLimit(Highlimit*100);
		m_vWaypoints.push_back(Waypoint);
		idx++;

		CString strValue;
		m_list.InsertItem(idx,objName.GetIDString());

		Latitude.GetValue(strValue);
		m_list.SetItemText(idx, 1, strValue);

		Longtitude.GetValue(strValue);
		m_list.SetItemText(idx, 2, strValue);

		strValue.Format("%.2f", Lowerlimit);
		m_list.SetItemText(idx, 3, strValue);

		strValue.Format("%.2f", Highlimit);
		m_list.SetItemText(idx, 4, strValue);

	}
	file.Close();

	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
	UpdateUIState();	
}
void CDialogWPImport::OnDelRecord()
{ 
	int nCount = m_list.GetSelectedCount();
	for(int i=0;i < nCount;i++)
	{	
		POSITION pos = m_list.GetFirstSelectedItemPosition();   
		int	n = m_list.GetNextSelectedItem(pos);   
		m_list.DeleteItem(n); 
		m_vWaypoints.erase(m_vWaypoints.begin() + n);
	}
	UpdateUIState();
}

void CDialogWPImport::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	UpdateUIState();
	*pResult = 0;
}
void CDialogWPImport::OnOK()
{	 	
	OnSave();
	CDialog::OnOK();
}
void CDialogWPImport::OnSave()
{
	if (m_vWaypoints.empty())
		return;

	int nAirportID =  m_Airport.getID();	
	int nCount = m_vWaypoints.size();
	try
	{
		CADODatabase::BeginTransaction();

		for (int i =0; i < nCount; i++)
		{
			m_vWaypoints[i].SaveObject(nAirportID);
		}

		CADODatabase::CommitTransaction();
	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();

		MessageBox(_T("Save data error!"));
		return;
	}

	m_vWaypoints.clear();
	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
}
