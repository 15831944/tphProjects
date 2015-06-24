// ACTypeModelDB.cpp : implementation file
//

#include "stdafx.h"

#include "TermPlan.h"
#include ".\dlgactypemodeldb.h"
#include ".\Common\ACTypesManager.h"
#include "DlgACTypeItemDefine.h"

#include <InputOnBoard/CInputOnboard.h>
#include <Common/AirportDatabase.h>
#include <Common/AircraftModel.h>
#include <Common/fileman.h>
#include <Common/SuperTypeRelationData.h>
#include <common/BitmapSaver.h>
#include <common/AircraftModelDatabase.h>
#include <Renderer/RenderEngine/RenderEngine.h>
#include "AircraftPropertySheet.h"
#include <Renderer\RenderEngine\3ds2mesh.h>


// CDlgACTypeModelDB dialog
IMPLEMENT_DYNAMIC(CDlgACTypeModelDB, CDialog)
CDlgACTypeModelDB::CDlgACTypeModelDB(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgACTypeModelDB::IDD, pParent)
	, m_pEditACType(NULL)
{
	ClearBmps(false);
	m_pEditModel = NULL;
}

CDlgACTypeModelDB::~CDlgACTypeModelDB()
{
	ClearBmps(true);
}

void CDlgACTypeModelDB::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MODEL, m_wndListCtrl);
	DDX_Control(pDX, IDC_MODEL_VIEW, m_wndPicture);
}


BEGIN_MESSAGE_MAP(CDlgACTypeModelDB, CDialog)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_MODEL_NEW,OnAddACTypeModel)
	ON_COMMAND(ID_MODEL_DELETE,OnRemoveACTypeModel)
	ON_COMMAND(ID_MODEL_EDIT,OnEditACTypeModel)
	ON_NOTIFY(LVN_ITEMCHANGED,IDC_LIST_MODEL,OnSelChangeACTypeModel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_ACTYPE,OnDisplayAircraftModel)
	ON_NOTIFY(LVN_ENDLABELEDIT,IDC_LIST_MODEL,OnSelListCtrlComboBox)
	ON_COMMAND(ID_EDIT_COMPONENT,OnEditACTypeModel)
	ON_NOTIFY(NM_RCLICK,IDC_LIST_MODEL, OnRClick)
END_MESSAGE_MAP()


// CDlgACTypeModelDB message handlers

BOOL CDlgACTypeModelDB::OnInitDialog()
{
	CDialog::OnInitDialog();

	OnInitToolbar();
	OnInitListCtrl();
	UpdateToolbarState();
	PopulateACModel();
	DisplayAllBmpInCtrl();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

int CDlgACTypeModelDB::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndACTypeModel.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS ) ||
		!m_wndACTypeModel.LoadToolBar(IDR_DATABASE_MODEL))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}

void CDlgACTypeModelDB::OnInitToolbar()
{
	CRect rect;
	m_wndListCtrl.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.bottom = rect.top - 6;
	rect.top = rect.bottom - 22;
	m_wndACTypeModel.MoveWindow(&rect,true);
	m_wndACTypeModel.ShowWindow(SW_SHOW);

	m_wndACTypeModel.GetToolBarCtrl().EnableButton(ID_MODEL_NEW,TRUE);
	m_wndACTypeModel.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,TRUE);
	m_wndACTypeModel.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,TRUE);
}

void CDlgACTypeModelDB::OnInitListCtrl()
{
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	char* columnLabel[] = {	"Index","Name", "Source", "Storage"};
	int DefaultColumnWidth[] = { 80,100, 200, 120};

	int nColFormat[] = 
	{	
		LVCFMT_NOEDIT,
		LVCFMT_DROPDOWN,
		LVCFMT_NOEDIT,
		LVCFMT_NOEDIT	
	};

	CStringList strList;
	CACTYPELIST* pvList = NULL;
	int nCount = -1;
	pvList = m_pAcMan->GetACTypeList();

	strList.AddTail(_T("New ACType..."));
	for(int i=0; i<static_cast<int>(pvList->size()); i++)
	{
		CACType* pACT = (*pvList)[i];
		strList.AddTail(pACT->GetIATACode());
	}

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	lvc.csList = &strList;

	for (int i = 0; i < 4; i++)
	{
		lvc.fmt = nColFormat[i];
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		m_wndListCtrl.InsertColumn(i, &lvc);
	}
}

void CDlgACTypeModelDB::OnPaint()
{	
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	HWND hwndT=::GetWindow(m_hWnd, GW_CHILD);
	CRect rectCW;
	CRgn rgnCW;
	rgnCW.CreateRectRgn(0,0,0,0);
	while (hwndT != NULL)
	{
		CWnd* pWnd=CWnd::FromHandle(hwndT)  ;
		if(pWnd->IsKindOf(RUNTIME_CLASS(CListCtrl))
			|| pWnd == GetDlgItem(IDC_MODEL_VIEW)
			|| pWnd == GetDlgItem(IDC_STATIC_SIDE)
			|| pWnd == GetDlgItem(IDC_STATIC_ISOMETRIC)
			|| pWnd == GetDlgItem(IDC_STATIC_FRONT))
		{
			if(!pWnd->IsWindowVisible())
			{
				hwndT=::GetWindow(hwndT,GW_HWNDNEXT);
				continue;
			}
			pWnd->GetWindowRect(rectCW);
			ScreenToClient(rectCW);
			CRgn rgnTemp;
			rgnTemp.CreateRectRgnIndirect(rectCW);
			rgnCW.CombineRgn(&rgnCW,&rgnTemp,RGN_OR);
		}
		hwndT=::GetWindow(hwndT,GW_HWNDNEXT);

	}
	CRect rect;
	GetClientRect(&rect);
	CRgn rgn;
	rgn.CreateRectRgnIndirect(rect);
	CRgn rgnDraw;
	rgnDraw.CreateRectRgn(0,0,0,0);
	rgnDraw.CombineRgn(&rgn,&rgnCW,RGN_DIFF);
	CBrush br(GetSysColor(COLOR_BTNFACE));
	dc.FillRgn(&rgnDraw,&br);
	CRect rectRight=rect;
	rectRight.left=rectRight.right-10;
	dc.FillRect(rectRight,&br);
	rectRight=rect;
	rectRight.top=rect.bottom-44;
	dc.FillRect(rectRight,&br);

	DisplayAllBmpInCtrl();
	CDialog::OnPaint();
}

void CDlgACTypeModelDB::OnRemoveACTypeModel()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel != LB_ERR)
	{
		CACType* pACT = (CACType*)m_wndListCtrl.GetItemData(nSel);
		CAircraftModel* pModel  = m_pModelList->getModelByName(pACT->GetIATACode());
		m_pModelList->DelModel(pModel);
		
		m_wndListCtrl.DeleteItemEx( nSel);
		UpdateToolbarState();
	}
}

void CDlgACTypeModelDB::OnAddACTypeModel()
{
	CACType* pItem = NULL;
	CDlgACTypeItemDefine dlg(m_pAcMan);
	dlg.SetNewComponent(true);
	if (dlg.DoModal() == IDOK)
	{
		CACType* pACT = NULL;
		CString strDBPath = _T("");

		CACTYPELIST* pvList = NULL;
		BOOL bExist = FALSE;
		pvList = m_pAcMan->GetACTypeList();
		for (int i = 0; i <(int)pvList->size(); i++)
		{
			CACType* pACType = (*pvList)[i];
			if (!pACType->GetIATACode().CompareNoCase(dlg.getACTypeName().GetIDString()))
			{
				if (m_pModelList->getModelByName(pACType->GetIATACode()))
				{
					MessageBox(_T("The ACType already exists!"));
					return;
				}
				else
				{
					bExist = TRUE;
					pItem = pACType;
					break;
				}
			}
		}

		if (!bExist)
		{
			pItem = new CACType();
			pItem->setIATACode(dlg.getACTypeName().GetIDString());
			pItem->m_sDescription = dlg.getCreator();
			m_pAcMan->GetACTypeList()->push_back(pItem);	
		}
		CAircraftModel* pNewModel = new CAircraftModel(m_pModelList);

		CWaitCursor wc;
		CTime t= CTime::GetCurrentTime();
		pNewModel->SetCreateTime(t);
		pNewModel->SetLastModifiedTime(t);		
		pNewModel->MakeSureFileOpen();
		if( C3DS2Mesh::ImportModel(dlg.m_sImportFile,*pNewModel,pNewModel->msTmpWorkDir,1.0f) )
		{
			pNewModel->mbExternalMesh = true;			
		}
		wc.Restore();
		pNewModel->m_sModelName = pItem->GetIATACode();		
		m_pModelList->AddModel(pNewModel);

		int nCount = m_wndListCtrl.GetItemCount();
		CString strIndex = _T("");
		strIndex.Format(_T("%d"),nCount + 1);
		m_wndListCtrl.InsertItem(nCount,strIndex);
		m_wndListCtrl.SetItemText(nCount,1,pItem->GetIATACode());
		m_wndListCtrl.SetItemText(nCount,2,pItem->m_sDescription);
		//m_wndListCtrl.SetItemText(nCount,3,CString(_T("INPUT\\AirportDB")));
		m_wndListCtrl.SetItemData(nCount,(DWORD_PTR)pItem);
	}
}

void CDlgACTypeModelDB::OnEditACTypeModel()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel == LB_ERR)
	{
		MessageBox(_T("Please select one aircraft model!"));
		return;
	}

	m_pEditACType = (CACType*)m_wndListCtrl.GetItemData(nSel);
	m_pEditModel = m_pModelList->getModelByName(m_pEditACType->GetIATACode());
	OnBnClickedOk();
}

void CDlgACTypeModelDB::UpdateToolbarState()
{
	if (m_wndListCtrl.GetCurSel() == LB_ERR)
	{
		m_wndACTypeModel.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,FALSE);
		m_wndACTypeModel.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,FALSE);
	}
	else
	{
		m_wndACTypeModel.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,TRUE);
		m_wndACTypeModel.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,TRUE);
	}
}

void CDlgACTypeModelDB::OnSelChangeACTypeModel(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;

	UpdateToolbarState();
	if((pnmv->uChanged & LVIF_STATE) != LVIF_STATE)
	{
		GetDlgItem(IDC_MODEL_VIEW)->Invalidate(FALSE);
		GetDlgItem(IDC_STATIC_SIDE)->Invalidate(FALSE);
		GetDlgItem(IDC_STATIC_FRONT)->Invalidate(FALSE);
		GetDlgItem(IDC_STATIC_ISOMETRIC)->Invalidate(FALSE);
		return;
	}

	if(!(pnmv->uOldState & LVIS_SELECTED) && !(pnmv->uNewState & LVIS_SELECTED))
	{
		GetDlgItem(IDC_MODEL_VIEW)->Invalidate(FALSE);
		GetDlgItem(IDC_STATIC_SIDE)->Invalidate(FALSE);
		GetDlgItem(IDC_STATIC_FRONT)->Invalidate(FALSE);
		GetDlgItem(IDC_STATIC_ISOMETRIC)->Invalidate(FALSE);
		return;
	}
		
	if(pnmv->uOldState & LVIS_SELECTED) 
	{
		if(!(pnmv->uNewState & LVIS_SELECTED)) 
		{
			DisplayBmpInCtrl(NULL,IDC_MODEL_VIEW);
			DisplayBmpInCtrl(NULL,IDC_STATIC_SIDE);
			DisplayBmpInCtrl(NULL,IDC_STATIC_FRONT);
			DisplayBmpInCtrl(NULL,IDC_STATIC_ISOMETRIC);

			m_wndACTypeModel.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,FALSE);
			m_wndACTypeModel.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,FALSE);
		}	
	}
	else if(pnmv->uNewState & LVIS_SELECTED)
	{
		int nSel = m_wndListCtrl.GetCurSel();
		if (nSel != LB_ERR)
		{
			CACType* pACT = NULL;
			pACT = (CACType*)m_wndListCtrl.GetItemData(nSel);
			if(pACT)
			{
				CAircraftModel* pModel = m_pModelList->getModelByName(pACT->GetIATACode());
				if(pModel)
				{
					LoadBmp(pModel);
				}

			}			
		}
		DisplayAllBmpInCtrl();
	}
	GetDlgItem(IDC_MODEL_VIEW)->Invalidate(FALSE);
	GetDlgItem(IDC_STATIC_SIDE)->Invalidate(FALSE);
	GetDlgItem(IDC_STATIC_FRONT)->Invalidate(FALSE);
	GetDlgItem(IDC_STATIC_ISOMETRIC)->Invalidate(FALSE);
}

void CDlgACTypeModelDB::PopulateACModel()
{
	CACTYPELIST* pvList;
	pvList = m_pAcMan->GetACTypeList();
	m_wndListCtrl.DeleteAllItems();

	int nItem = 0;
	for(int i=0; i<static_cast<int>(pvList->size()); i++)
	{
		CACType* pACT = (*pvList)[i];

		if (m_pModelList->getModelByName(pACT->GetIATACode()))
		{
			CString strIndex = _T("");
			strIndex.Format(_T("%d"),nItem + 1);
			m_wndListCtrl.InsertItem(nItem,strIndex);
			m_wndListCtrl.SetItemText(nItem, 1,pACT->GetIATACode());
			m_wndListCtrl.SetItemText(nItem, 2, pACT->m_sDescription);
			//m_wndListCtrl.SetItemText(nItem, 3, CString(_T("INPUT\\AirportDB")));
			m_wndListCtrl.SetItemData(nItem,(DWORD_PTR)pACT);
			nItem++;
		}
	}
}

void CDlgACTypeModelDB::OnBnClickedOk()
{
	CWaitCursor wc;
	m_pAcMan->saveDatabase(m_pAirportDB);
	m_pModelList->SaveData();
	CDialog::OnOK();
}

void CDlgACTypeModelDB::OnDisplayAircraftModel()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel != LB_ERR)
	{
		CACType* pACT = (CACType*)m_wndListCtrl.GetItemData(nSel);
		CAircraftPropertySheet dlg(_T("Aircraft property"),this);
		dlg.setACType(pACT);
		dlg.DoModal();
	}
	else
	{
		::AfxMessageBox(_T("You donot select aircraft model item"),MB_OK);
	}
}
CACType* CDlgACTypeModelDB::GetAcTypeByName(CString strName)
{
	CACTYPELIST* pvList = m_pAcMan->GetACTypeList();
	for (size_t i = 0; i <pvList->size(); i++)
	{
		CACType* pACType = (*pvList)[i];
		if (0 == pACType->GetIATACode().CompareNoCase(strName))
		{
			return pACType;
		}
	}
	return NULL;
}

void CDlgACTypeModelDB::OnSelListCtrlComboBox(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	
	CACType* pPreACT = (CACType*)m_wndListCtrl.GetItemData(pDispInfo->item.iItem);

	CString strSel = pDispInfo->item.pszText;
	if(strSel == _T("New ACType..."))
	{
		CDlgACTypeItemDefine dlg(m_pAcMan);
		if (dlg.DoModal() == IDOK)
		{
			CACType* pCurACT = GetAcTypeByName(dlg.getACTypeName().GetIDString());
			if (pCurACT)
			{
				if (m_pModelList->getModelByName(pCurACT->GetIATACode()))
				{
					MessageBox(_T("The ACType already exists!"));
					m_wndListCtrl.SetItemText(pDispInfo->item.iItem,1,pPreACT->GetIATACode());
					return;
				}
			}
			else
			{
				pCurACT = new CACType();
				pCurACT->setIATACode(dlg.getACTypeName().GetIDString());
				pCurACT->m_sDescription = dlg.getCreator();
				m_pAcMan->GetACTypeList()->push_back(pCurACT);
			}

			if (pPreACT) // clear former actype and related data
			{
				pPreACT->RemoveFiles(m_pAcMan->getDBPath());
				pPreACT->m_sFileName = _T("");

				CString strAircraftModel;
				strAircraftModel.Format("%s\\%s.xml", m_pAcMan->getDBPath(), pPreACT->GetIATACode());
				if (PathFileExists(strAircraftModel))
				{
					FileManager::DeleteFile(strAircraftModel);
				}
				m_pModelList->RemoveItem(pPreACT->GetIATACode());
			}

			
			m_pModelList->AddItem(pPreACT->GetIATACode());
			CString strIndex = _T("");
			strIndex.Format(_T("%d"),pDispInfo->item.iItem+ 1);
			m_wndListCtrl.SetItemText(pDispInfo->item.iItem,0,strIndex);
			m_wndListCtrl.SetItemText(pDispInfo->item.iItem,1,pCurACT->GetIATACode());
			m_wndListCtrl.SetItemText(pDispInfo->item.iItem,2,pCurACT->m_sDescription);
			m_wndListCtrl.SetItemText(pDispInfo->item.iItem,3,pCurACT->m_sFileName);
			m_wndListCtrl.SetItemData(pDispInfo->item.iItem,(DWORD_PTR)pCurACT);
		}
		else
		{
			m_wndListCtrl.SetItemText(pDispInfo->item.iItem,1,pPreACT->GetIATACode());
		}
	}
	else
	{
		CACType* pCurACT = m_pAcMan->getACTypeItem(pDispInfo->item.pszText);
		if (pCurACT == pPreACT)
		{
			return;
		}
		if (pCurACT && !m_pModelList->getModelByName(pCurACT->GetIATACode()))
		{
			m_pModelList->RemoveItem(pPreACT->GetIATACode());

			CAircraftModel* pNewModel  = new CAircraftModel(m_pModelList);
			pNewModel->m_sModelName = pCurACT->GetIATACode();			
			m_pModelList->AddModel(pNewModel);
			m_wndListCtrl.SetItemText(pDispInfo->item.iItem,1,pCurACT->GetIATACode());
			m_wndListCtrl.SetItemText(pDispInfo->item.iItem,2,pCurACT->m_sDescription);
			m_wndListCtrl.SetItemText(pDispInfo->item.iItem,3,pCurACT->m_sFileName);
			m_wndListCtrl.SetItemData(pDispInfo->item.iItem,(DWORD_PTR)pCurACT);
		}
	}
	DisplayAllBmpInCtrl();
}

void CDlgACTypeModelDB::DisplayBmpInCtrl(HBITMAP hBmp,UINT nID)
{
	if (hBmp != NULL)
	{
		CBitmap bmpObj;
		bmpObj.Attach(hBmp);
		CWnd *pWnd = GetDlgItem(nID);
		CDC *pDC = pWnd->GetDC();

		CDC DCCompatible;
		DCCompatible.CreateCompatibleDC(pDC);
		CWnd::DefWindowProc(WM_PAINT, (WPARAM)DCCompatible.m_hDC , 0);

		CRect rect;
		pWnd->GetClientRect(rect);
		CBitmap *pOldBmp = DCCompatible.SelectObject(&bmpObj);
		BITMAP bmpInfo;
		bmpObj.GetObject(sizeof(bmpInfo),&bmpInfo);

		pDC->SetStretchBltMode(STRETCH_HALFTONE);
		pDC->StretchBlt(0, 0, rect.Width(), rect.Height(), &DCCompatible, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, SRCCOPY );
		DCCompatible.SelectObject(pOldBmp);
		bmpObj.Detach();
		return;

	}
	// bkcolor
	CRect rcClient;
	CWnd *pWnd = GetDlgItem(nID);
	CDC *dc = pWnd->GetDC();
	pWnd->GetClientRect(&rcClient);

	CBrush pOrgBrush(RGB(255,255,255));
	dc->FillRect(rcClient,&pOrgBrush);
}

void CDlgACTypeModelDB::DisplayAllBmpInCtrl()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel != LB_ERR)
	{
		CACType* pACType = (CACType*)m_wndListCtrl.GetItemData(nSel);
		CAircraftModel* pModel = m_pModelList->getModelByName(pACType->GetIATACode());
		if (pACType && pModel)
		{
			HBITMAP* hBmp = m_hBmp;
		/*	if (!pModel->mFile.FileExists())
				return;

			CString zipFile = pModel->mFile.m_sFilePath;
			ResourceFileArchive::AddZipFile(zipFile);
		
			DataBuffer::SharedPtr dbufferptr = ResourceFileArchive::ReadFileAsBuffer(STR_THUMBFILE_X,zipFile);
			hBmp[0] = BitmapSaver::FromBuffer(dbufferptr->lpData,dbufferptr->nsize);	
			
			dbufferptr = ResourceFileArchive::ReadFileAsBuffer(STR_THUMBFILE_Y,zipFile);
			hBmp[1] = BitmapSaver::FromBuffer(dbufferptr->lpData,dbufferptr->nsize);	

			dbufferptr = ResourceFileArchive::ReadFileAsBuffer(STR_THUMBFILE_Z,zipFile);
			hBmp[2] = BitmapSaver::FromBuffer(dbufferptr->lpData,dbufferptr->nsize);	

			dbufferptr = ResourceFileArchive::ReadFileAsBuffer(STR_THUMBFILE_P,zipFile);
			hBmp[3] = BitmapSaver::FromBuffer(dbufferptr->lpData,dbufferptr->nsize);	*/

			

			DisplayBmpInCtrl(hBmp[0],IDC_MODEL_VIEW);
			DisplayBmpInCtrl(hBmp[1],IDC_STATIC_SIDE);
			DisplayBmpInCtrl(hBmp[2],IDC_STATIC_FRONT);
			DisplayBmpInCtrl(hBmp[3],IDC_STATIC_ISOMETRIC);

			GetDlgItem(IDC_MODEL_VIEW)->Invalidate(FALSE);
			GetDlgItem(IDC_STATIC_SIDE)->Invalidate(FALSE);
			GetDlgItem(IDC_STATIC_FRONT)->Invalidate(FALSE);
			GetDlgItem(IDC_STATIC_ISOMETRIC)->Invalidate(FALSE);
			return;
		}
	}

	DisplayBmpInCtrl(NULL,IDC_MODEL_VIEW);
	DisplayBmpInCtrl(NULL,IDC_STATIC_SIDE);
	DisplayBmpInCtrl(NULL,IDC_STATIC_FRONT);
	DisplayBmpInCtrl(NULL,IDC_STATIC_ISOMETRIC);

	GetDlgItem(IDC_MODEL_VIEW)->Invalidate(FALSE);
	GetDlgItem(IDC_STATIC_SIDE)->Invalidate(FALSE);
	GetDlgItem(IDC_STATIC_FRONT)->Invalidate(FALSE);
	GetDlgItem(IDC_STATIC_ISOMETRIC)->Invalidate(FALSE);
}

void CDlgACTypeModelDB::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType,cx,cy);
	
	if (m_wndListCtrl.m_hWnd != NULL)
	{
		CRect rect;
		CRect rc;
		GetDlgItem(IDC_STATIC_COMPONENTS)->SetWindowPos(NULL,10,11,cx-2*10,16,NULL);
		GetDlgItem(IDC_STATIC_COMPONENTS)->GetWindowRect(&rect);
		ScreenToClient(rect);

		m_wndACTypeModel.SetWindowPos(NULL,rect.left+1,rect.bottom,cx-22,26,NULL);
		m_wndACTypeModel.GetWindowRect(&rect);
		ScreenToClient(rect);

		m_wndListCtrl.SetWindowPos(NULL,rect.left+1,rect.bottom + 2,cx-2*rect.left-4,cy/2-rect.bottom - 20,NULL);
		m_wndListCtrl.GetWindowRect(&rc);
		ScreenToClient(&rc);

		GetDlgItem(IDC_STATIC_GROUP)->SetWindowPos(NULL,rc.left+1,rc.bottom + 10,cx - 2*rect.left-4,cy- rc.bottom -55,NULL);
		GetDlgItem(IDC_STATIC_GROUP)->GetWindowRect(&rc);
		ScreenToClient(rc);

		GetDlgItem(IDC_STATIC_Z)->SetWindowPos(NULL,rc.left+2,rc.top + 20,rc.Width()/4 -2,16,NULL);
		GetDlgItem(IDC_STATIC_Y)->SetWindowPos(NULL,rc.left+rc.Width()/4,rc.top+20,rc.Width()/4,16,NULL);
		GetDlgItem(IDC_STATIC_X)->SetWindowPos(NULL,rc.left+rc.Width()/2,rc.top+20,rc.Width()/4,16,NULL);
		GetDlgItem(IDC_STATIC_ISO)->SetWindowPos(NULL,rc.right-rc.Width()/4,rc.top+20,rc.Width()/4,16,NULL);

		CRect rcView;
		GetDlgItem(IDC_MODEL_VIEW)->SetWindowPos(NULL,rc.left,rc.top+38,rc.Width()/4,rc.Height()-38,NULL);
		GetDlgItem(IDC_MODEL_VIEW)->GetWindowRect(&rcView);
		ScreenToClient(&rcView);
		GetDlgItem(IDC_STATIC_SIDE)->SetWindowPos(NULL,rcView.right,rc.top+38,rc.Width()/4,rc.Height()-38,NULL);
		GetDlgItem(IDC_STATIC_SIDE)->GetWindowRect(&rcView);
		ScreenToClient(&rcView);
		GetDlgItem(IDC_STATIC_FRONT)->SetWindowPos(NULL,rcView.right,rc.top+38,rc.Width()/4,rc.Height()-38,NULL);
		GetDlgItem(IDC_STATIC_FRONT)->GetWindowRect(&rcView);
		ScreenToClient(&rcView);
		GetDlgItem(IDC_STATIC_ISOMETRIC)->SetWindowPos(NULL,rcView.right,rc.top+38,rc.right-rcView.right,rc.Height()-38,NULL);

		CRect rcBtn;
		GetDlgItem(IDCANCEL)->GetWindowRect(&rcBtn);
		GetDlgItem(IDCANCEL)->SetWindowPos(NULL,rc.right - rcBtn.Width() - 2,rc.bottom + 8,rcBtn.Width(),rcBtn.Height(),NULL);
		GetDlgItem(IDCANCEL)->GetWindowRect(&rcBtn);
		ScreenToClient(rcBtn);
		GetDlgItem(IDOK)->SetWindowPos(NULL,rcBtn.left - rcBtn.Width() - 16,rc.bottom + 8,rcBtn.Width(),rcBtn.Height(), NULL);
		GetDlgItem(IDOK)->GetWindowRect(&rcBtn);
		ScreenToClient(rcBtn);
		CRect rcActype;
		GetDlgItem(IDC_ACTYPE)->GetWindowRect(&rcActype);
		GetDlgItem(IDC_ACTYPE)->SetWindowPos(NULL,rcBtn.left - rcActype.Width() - 16,rc.bottom+8,rcActype.Width(),rcActype.Height(),NULL);
	}
}

BOOL CDlgACTypeModelDB::OnEraseBkgnd(CDC*  pDC)     
{   
	return TRUE;   
} 


void CDlgACTypeModelDB::OnRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPoint cliPt;
	GetCursorPos(&cliPt);
	CPoint point = cliPt;
	m_wndListCtrl.ScreenToClient(&cliPt);
	int index;
	if((index = m_wndListCtrl.HitTestEx(cliPt, NULL)) != -1)
	{
		CMenu menu;
		menu.LoadMenu(IDR_MENU_COMPONENT);
		menu.GetSubMenu(0)->ModifyMenu(ID_EDIT_COMPONENT, MF_BYCOMMAND | MF_STRING, ID_EDIT_COMPONENT, _T("Edit aircraft model"));
		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON,point.x,point.y,this);
	}
}


void CDlgACTypeModelDB::ClearBmps( bool bDelete )
{
	if(bDelete)
	{
		for(int i=0;i<ACTYPEBMP_COUNT;i++)
			::DeleteObject(m_hBmp[i]);
	}
	for(int i=0;i<ACTYPEBMP_COUNT;i++)
		m_hBmp[i] = NULL;
}

void CDlgACTypeModelDB::InitOpAirpotDB( CAirportDatabase* pAirportDB )
{
	ASSERT(pAirportDB);
	m_pAirportDB = pAirportDB;

	m_pAcMan = m_pAirportDB->getAcMan();
	m_pModelList = m_pAirportDB->getModeList();
}

void CDlgACTypeModelDB::LoadBmp( CModel* pModel )
{
	ClearBmps(true);
	HBITMAP* hBmp = m_hBmp;
	if(!pModel->MakeSureFileOpen())
	{
		return;
	}


	CString sBmpFile = pModel->msTmpWorkDir + STR_THUMBFILE_X;			
	hBmp[0] = BitmapSaver::FromFile(sBmpFile);	
	sBmpFile = pModel->msTmpWorkDir + STR_THUMBFILE_Y;
	hBmp[1] = BitmapSaver::FromFile(sBmpFile);	

	sBmpFile = pModel->msTmpWorkDir + STR_THUMBFILE_Z;
	hBmp[2] = BitmapSaver::FromFile(sBmpFile);	

	sBmpFile = pModel->msTmpWorkDir + STR_THUMBFILE_P;
	hBmp[3] = BitmapSaver::FromFile(sBmpFile);
}