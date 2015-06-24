#include "stdafx.h"
#include "Resource.h"
#include "DlgAircraftAlias.h"
#include "../Common/AircraftAliasManager.h"
#include "../Common/AirportDatabase.h"
#include "Termplan.h"
#include ".\dlgaircraftalias.h"
#include "../Common/AirportDatabaseList.h"
#include "../src/Main/TermPlanDoc.h"
#include "../Common/ProjectManager.h"
#define DEFAULT_AC_NAME _T("defaultAC")
IMPLEMENT_DYNAMIC(CDlgAircraftAlias, CXTResizeDialog)
CDlgAircraftAlias::CDlgAircraftAlias(Terminal *pTerminalr, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgAircraftAlias::IDD, pParent)
	,m_pTerminal(pTerminalr)
{
	if(pTerminalr != NULL)
	   m_pACManager = ((CAirportDatabase*)(pTerminalr->m_pAirportDB))->getAcMan();
}

CDlgAircraftAlias::~CDlgAircraftAlias()
{
}

void CDlgAircraftAlias::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SHAPE, m_wndComboShapes);
	DDX_Control(pDX, IDC_LIST_ASSOC, m_wndListBoxAssoc);
	DDX_Control(pDX, IDC_LIST_UNASSOC, m_wndListBoxUnAssoc);
	DDX_Control(pDX, IDC_BUTTON_LOAD, m_Load);
	
}


BEGIN_MESSAGE_MAP(CDlgAircraftAlias, CXTResizeDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_SHAPE, OnSelChangeComboShape)
	ON_BN_CLICKED(IDC_MOVELEFT, OnBnClickedMoveLeft)
	ON_BN_CLICKED(IDC_MOVERIGHT, OnBnClickedMoveRight)
	ON_BN_CLICKED(IDSAVE, OnBnClickedSave)
	ON_BN_CLICKED(IDOK, OnBnClickedOK)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_COMMAND(ID_AIRPORT_SAVEAS, OnSaveAsTemplate)
	ON_COMMAND(ID_AIRPORT_LOAD, OnLoadFromTemplate)

	ON_BN_CLICKED(IDC_CHECK_BYSCHEDULE, OnBnClickedCheckByschedule)
END_MESSAGE_MAP()

// CDlgAircraftAlias message handlers

void CDlgAircraftAlias::OnSize(UINT nType, int cx, int cy)
{
	
}
BOOL CDlgAircraftAlias::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	SetResize(IDC_STATIC_GROUP, SZ_TOP_LEFT , SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_TITLELEFT, SZ_TOP_LEFT , SZ_TOP_CENTER);
	SetResize(IDC_STATIC_TITLERIGHT, SZ_TOP_CENTER , SZ_TOP_RIGHT);
	
	SetResize(IDC_COMBO_SHAPE, SZ_TOP_LEFT , SZ_TOP_CENTER);
	SetResize(IDC_LIST_ASSOC, SZ_TOP_LEFT , SZ_BOTTOM_CENTER);
	SetResize(IDC_LIST_UNASSOC, SZ_TOP_CENTER , SZ_BOTTOM_RIGHT);

	

	SetResize(IDC_MOVELEFT, SZ_MIDDLE_CENTER , SZ_MIDDLE_CENTER);
	SetResize(IDC_MOVERIGHT, SZ_MIDDLE_CENTER , SZ_MIDDLE_CENTER);
	SetResize(IDSAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

    SetResize(IDC_CHECK_BYSCHEDULE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	//	Init Cool button
	m_Load.SetOperation(0);
	m_Load.SetWindowText(_T("Database Template"));
	m_Load.SetOpenButton(FALSE);
	m_Load.SetType(CCoolBtn::TY_AIRCRAFTALIAS);

	// TODO:  Add extra initialization hereSetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	 ACALIASMANAGER->SetCACTypeManager(m_pACManager);
	/* CProjectManager::GetInstance()->getapp
	  CProjectManager::GetInstance()*/
    
	if(!ACALIASMANAGER->readData(m_pTerminal->m_pAirportDB))
		return false;

	LoadShapesName();
	
	InitShapesComboBox();

	InitUnAssociatedListBox();
    initCheckBoxByScheduleData();
	//ResetAssociatedListBox();

	OnSelChangeComboShape();
   

	return TRUE;
}
void CDlgAircraftAlias::OnLoadFromTemplate()
{
	m_Load.SetOperation(0);
	m_Load.SetWindowText(_T("Load from Template"));
	LoadFromTemplateDatabase();
}

void CDlgAircraftAlias::OnClickMultiBtn()
{

	int type = m_Load.GetOperation() ;
	if (type == 0)
		OnLoadFromTemplate();
	if(type == 1)
		OnSaveAsTemplate();
}
void CDlgAircraftAlias::LoadShapesName()
{
	m_vectShape.clear();

	CString strFlightShapeFile;

	strFlightShapeFile.Format("%s\\Shapes.data", ((CTermPlanApp*) AfxGetApp())->GetShapeDataPath() + _T("\\aircraft"));
	
	CFile * pFile = NULL;
	try{
		pFile = new CFile(strFlightShapeFile, CFile::modeRead | CFile::shareDenyNone);
		CArchive ar(pFile, CArchive::load);
		const static int strbufsize = 512;
		char line[strbufsize];
		//skip a line;
		ar.ReadString(line,strbufsize);
		//read head
		ar.ReadString(line,strbufsize);
		if(stricmp(line,"Shapes Database")==0){
			//skip the column name;
			ar.ReadString(line,strbufsize);
			//read the values
			//
			while(*(ar.ReadString(line, 512)) != '\0')
			{				
				CString name, filename;
				char* b = line;
				char* p = NULL;
				int c = 1;
				while((p = strchr(b, ',')) != NULL)
				{
					*p = '\0';
					switch(c)
					{
					case 1: //name
						name = _T(b);
						m_vectShape.push_back(name);
						break;					
					default:
						break;
					}
					b = ++p;
					c++;
				}
			}

		}
	}

	catch(CException * e){
		AfxMessageBox("Error reading Shapes Database file", MB_ICONEXCLAMATION | MB_OK);
		e->Delete();
		if(pFile != NULL)
			delete pFile;
		return;
	}
}

void CDlgAircraftAlias::OnSaveAsTemplate()
{
	m_Load.SetOperation(1);
	m_Load.SetWindowText(_T("Save as Template"));

	CAirportDatabase* DefaultDB = AIRPORTDBLIST->getAirportDBByName(m_pTerminal->m_pAirportDB->getName());
	if(DefaultDB == NULL)
	{
		CString strError(_T(""));
		strError.Format(_T("%s AirportDB is not Exist in AirportDB template."),m_pTerminal->m_pAirportDB->getName());
		MessageBox(strError,"Warning",MB_OK);
		return ;
	}

	m_pTerminal->m_pAirportDB->saveAsTemplateDatabase(DefaultDB);
}
void CDlgAircraftAlias::LoadFromTemplateDatabase()
{
	CAirportDatabase* projectDB = m_pTerminal->m_pAirportDB ;
	CAirportDatabase* DefaultDB = AIRPORTDBLIST->getAirportDBByName(projectDB->getName());
	if(DefaultDB == NULL)
	{
		CString msg ;
		msg.Format("%s AirportDB is not Exist in AirportDB template.",projectDB->getName()) ;
		MessageBox(msg,"Warning",MB_OK) ;
		return ;
	}
	//m_pFlightMan->ReadDataFromDB(DefaultDB->GetID(),DATABASESOURCE_TYPE_ACCESS_GLOBALDB) ;
	//m_pFlightMan->loadDatabase(DefaultDB);
	ACALIASMANAGER->loadDatabase(DefaultDB);
	CString msg ;
	msg.Format("Load %s from %s AirportDB template DataBase successfully.",DefaultDB->getName(),DefaultDB->getName()) ;
	MessageBox(msg,NULL,MB_OK) ;
	m_wndComboShapes.SetCurSel(0);
	OnSelChangeComboShape();
	
	InitUnAssociatedListBox();

}
void CDlgAircraftAlias::InitShapesComboBox()
{
	//check for new item
	/*for(int i=0; i<(int)m_vectShape.size(); i++)
	{
		if(ACALIASMANAGER->isAssociated(m_vectShape[i]))
			continue;

		AircraftAliasItem* pNewItem = new AircraftAliasItem;
		pNewItem->setSharpName(m_vectShape[i]);
		ACALIASMANAGER->addItem(pNewItem);
	}


	for (int i=0; i<ACALIASMANAGER->getCount(); i++)
	{
		AircraftAliasItem* pItem = ACALIASMANAGER->getItem(i);
		int nIdx = m_wndComboShapes.AddString(pItem->getSharpName());
		m_wndComboShapes.SetItemData(nIdx, DWORD_PTR(pItem));
	}*/
	for (int i=0;i<(int)m_vectShape.size();i++)

	{
		m_wndComboShapes.AddString(m_vectShape[i]);
		//m_wndComboShapes.SetItemData(i,(DWORD_PTR)(m_vectShape[i]));
	}
	m_wndComboShapes.SetCurSel(0);
}

void CDlgAircraftAlias::ResetAssociatedListBox(CString strShapeName)
{
	m_wndListBoxAssoc.ResetContent();
	/*for (int i=0; i<pItem->getCount(); i++)
	{
		CString strAssoc = pItem->getItem(i);
		m_wndListBoxAssoc.AddString(strAssoc);
	}*/
	bool b=false;
	for (int i=0;i<ACALIASMANAGER->getCount();i++)
	{
		
		if (ACALIASMANAGER->isAssociatedShapeName(strShapeName,i))
		{
			CString strAssoc=ACALIASMANAGER->getItem(i);
			m_wndListBoxAssoc.AddString(strAssoc);
			b=true;

		}
		
		
	}
	if (!b)
	{
		m_wndListBoxAssoc.AddString(DEFAULT_AC_NAME);
	}
}

void CDlgAircraftAlias::InitUnAssociatedListBox()
{
	m_vectShape.clear();
	m_wndListBoxUnAssoc.ResetContent();
	CACTYPELIST* pACList = m_pACManager->GetACTypeList();

	for (int i=0; i<(int)pACList->size(); i++)
	{
		CACType* pACType = (*pACList)[i];
		if( !ACALIASMANAGER->isAssociated(pACType->GetIATACode()) )
			m_vectUnAssocAC.push_back( pACType->GetIATACode() );
	}

	setUnAssociatedListBoxwithOutFilter();
}


void CDlgAircraftAlias::OnSelChangeComboShape()
{
	int nSel = m_wndComboShapes.GetCurSel();
	if(nSel == -1)
		return;
	CString StrShapeName;
	m_wndComboShapes.GetLBText(nSel,StrShapeName);
	//m_wndComboShapes.GetDlgItemText(nSel,StrShapeName);

	ResetAssociatedListBox(StrShapeName);
}


void CDlgAircraftAlias::OnBnClickedMoveLeft()
{
	int nSel = m_wndComboShapes.GetCurSel();
	if(nSel == -1)
		return;
	//AircraftAliasItem* pItem = 
	//	(AircraftAliasItem*)m_wndComboShapes.GetItemData(nSel);
	CString StrShapeName;
	m_wndComboShapes.GetLBText(nSel,StrShapeName);

	int nSelCount = m_wndListBoxUnAssoc.GetSelCount();
	CArray<int,int> aryListBoxSel;
	aryListBoxSel.SetSize(nSelCount);
	m_wndListBoxUnAssoc.GetSelItems(nSelCount, aryListBoxSel.GetData());

	std::vector<CString> vDelString;
	for(int i=0; i<aryListBoxSel.GetSize(); i++)
	{
		/*int nSelIndex = aryListBoxSel.GetAt(i);
		CString strACName = _T("");
		m_wndListBoxUnAssoc.GetText(nSelIndex - i, strACName);
		
		vDelString.push_back(strACName);
		m_wndListBoxUnAssoc.DeleteString(nSelIndex - i);

		m_wndListBoxAssoc.AddString(strACName);
		pItem->addAssocItem(strACName);	*/
		int nSelIndex = aryListBoxSel.GetAt(i);
		CString strACName = _T("");
		m_wndListBoxUnAssoc.GetText(nSelIndex - i, strACName);

		vDelString.push_back(strACName);
		m_wndListBoxUnAssoc.DeleteString(nSelIndex - i);

		m_wndListBoxAssoc.AddString(strACName);
		ACALIASMANAGER->addItem(strACName,StrShapeName);

	}

	GetDlgItem(IDSAVE)->EnableWindow(TRUE);
}

void CDlgAircraftAlias::OnBnClickedMoveRight()
{
	
	int nSel = m_wndComboShapes.GetCurSel();
	if(nSel == -1)
		return;
	/*AircraftAliasItem* pItem = 
		(AircraftAliasItem*)m_wndComboShapes.GetItemData(nSel);*/
	
	CString  pItem ;
   m_wndComboShapes.GetLBText(nSel,pItem);

	int nSelCount = m_wndListBoxAssoc.GetSelCount();
	CArray<int,int> aryListBoxSel;
	aryListBoxSel.SetSize(nSelCount);
	m_wndListBoxAssoc.GetSelItems(nSelCount, aryListBoxSel.GetData());

	std::vector<CString> vDelString;
	for(int i=0; i<aryListBoxSel.GetSize(); i++)
	{
		int nSelIndex = aryListBoxSel.GetAt(i);
		CString strACName = _T("");
		m_wndListBoxAssoc.GetText(nSelIndex - i, strACName);
		
		vDelString.push_back(strACName);
		m_wndListBoxAssoc.DeleteString(nSelIndex - i);
		ACALIASMANAGER->deleteItem(strACName);
		
		//pItem->deleteAssocItem(strACName);

		if( strACName.CompareNoCase(_T("defaultAC"))  != 0)
			m_wndListBoxUnAssoc.AddString(strACName);
	}
	
	GetDlgItem(IDSAVE)->EnableWindow(TRUE);
}

void CDlgAircraftAlias::OnBnClickedSave()
{
	ACALIASMANAGER->saveData(m_pTerminal->m_pAirportDB);
	GetDlgItem(IDSAVE)->EnableWindow(FALSE);
}

void CDlgAircraftAlias::OnBnClickedOK()
{
	ACALIASMANAGER->saveData(m_pTerminal->m_pAirportDB);
	OnOK();
}

void CDlgAircraftAlias::OnBnClickedCancel()
{
	ACALIASMANAGER->readData(m_pTerminal->m_pAirportDB);
	OnCancel();
}

void CDlgAircraftAlias::OnBnClickedCheckByschedule()
{
	// TODO: Add your control notification handler code here
	CButton  * checkbox = (CButton*) GetDlgItem(IDC_CHECK_BYSCHEDULE);
	int state = checkbox->GetCheck();
	if(state == BST_UNCHECKED)
         setUnAssociatedListBoxwithOutFilter();
	if(state == BST_CHECKED)	  
        setUnAssociatedListBoxwithFilter();
}
//
//init the Vector<Cstring> CheckedList ,It contain the Actype which in the schedule !
//
void CDlgAircraftAlias::initCheckBoxByScheduleData()
{

	if(m_pTerminal == NULL && m_pACManager == NULL )
		return ;
	CACTYPELIST* pACList = m_pACManager->GetACTypeList();
	char AcType[1024] = {0};
	FlightSchedule* schedule = m_pTerminal->flightSchedule;
	Flight *pscheduleFlight = NULL ;
	int count = schedule->getCount();
	for (int i = 0; i < count; i++)
	{
		pscheduleFlight = schedule->getItem(i);
		if(pscheduleFlight != NULL )
		{	 
			pscheduleFlight->getACType(AcType);
			CString  CACtype(AcType);
			CACTYPELIST::iterator iter = NULL;
			for (iter = (*pACList).begin();iter != (*pACList).end();++iter)
			{

				if (strcmp((*iter)->GetIATACode(),CACtype) == 0 && find(CheckedList.begin(),CheckedList.end(),CACtype) == CheckedList.end())
				{
					CheckedList.push_back(CACtype);
					break ;
				}
			}
		}

	}
}
//
//set UnAssociatedListBox when checkbox was checked !
//
void CDlgAircraftAlias::setUnAssociatedListBoxwithFilter()
{
	m_vectShape.clear();
	m_wndListBoxUnAssoc.ResetContent();
	ITERATOR_FLIGHT iter = NULL ;
	for (iter = CheckedList.begin(); iter != CheckedList.end(); ++iter)
	{

		if( !ACALIASMANAGER->isAssociated(*iter) )
			m_wndListBoxUnAssoc.AddString((*iter).GetString());
	}

}
//
//set UnAssociatedListBox when checkbox was not checked !
//
void CDlgAircraftAlias::setUnAssociatedListBoxwithOutFilter()
{
	for (int i=0; i<(int)m_vectUnAssocAC.size(); i++)
	{
		m_wndListBoxUnAssoc.AddString(m_vectUnAssocAC[i]);
	}
}
