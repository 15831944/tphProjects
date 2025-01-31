// ShapesBar.cpp: implementation of the CShapesBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "MainFrm.h"
#include "ShapesBar.h"
#include "ShapesManager.h"
#include "common\WinMsg.h"
#include "Common\ProjectManager.h"
#include "Ini.h"
#include "ShapeFolder.h"
#include "ShapeItem.h"
#include "Common\ZipInfo.h"
#include "Common\DirectoryOperation.h"
#include "Common\TERMFILE.H"
#include "ImportUserShapeBarDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

////////////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CShapesBar, CSizingControlBarG)
	//{{AFX_MSG_MAP(CShapesBar)
	ON_WM_CREATE()
	ON_MESSAGE(WM_SLB_SELCHANGED, OnSLBSelChanged)
	ON_MESSAGE(WM_SHAPEBAR_NOTIFY,OnPopMenu)
	ON_WM_CTLCOLOR()

	ON_COMMAND(ID_MENU_ADDFOLDER,OnNewShapeBar)
	ON_COMMAND(ID_MENU_EDITFOLDER,OnEditShapeBar)
	ON_COMMAND(ID_MENU_IMPORTFOLDER,OnImport)
	ON_COMMAND(ID_MENU_EXPORTFOLDER,OnExport)
	ON_COMMAND(ID_MENU_DELETEFOLDER,OnDeleteShapeBar)

	ON_COMMAND(ID_MENU_ADDITM,OnNewShape)
    ON_COMMAND(ID_MENU_EDITITEM,OnEditShape)
	ON_COMMAND(ID_MENU_DELETEITEM,OnDeleteShape)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShapesBar message handlers

int CShapesBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSizingControlBarG::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetSCBStyle(GetSCBStyle() | SCBS_SHOWEDGES | SCBS_SIZECHILD);

	SHAPESMANAGER->LoadData();
	CShape::CShapeList* pSL = SHAPESMANAGER->GetShapeList();
	ASSERT(pSL != NULL);
	CreateOutlookBar(pSL);

	m_brush.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));

    // Create an 8-point MS Sans Serif font for the list box.
    m_font.CreatePointFont (80, _T ("MS Sans Serif"));
	m_iSelFolder = 0;
	m_iSelItem = 0;
    char buf[256] = {0};
    GetTempPath(255, buf);
    m_strTempPath += CString(buf) + "shape_info";
    m_strShapeFileName = "export_shape_bar.txt";

	CString imageFileName = PROJMANAGER->GetAppPath()+"\\Databases\\Shapes\\Question.bmp";
	HBITMAP hBitmap = (HBITMAP) ::LoadImage(NULL,imageFileName , IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	CBitmap bm;
	bm.Attach(hBitmap);
	defBMP = m_largeIL.Add(&bm,RGB(255,0,255));
	bm.DeleteObject();
	hBitmap = (HBITMAP) ::LoadImage(NULL,imageFileName , IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	bm.Attach(hBitmap);
	m_smallIL.Add(&bm,RGB(255,0,255));
	bm.DeleteObject();
	return 0;
}

void CShapesBar::CreateOutlookBar(CShape::CShapeList* pSL)
{
    DWORD dwf =CGfxOutBarCtrl::fAnimation;

    m_wndOutBarCtrl.Create(WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), this,IDC_LIST_SHAPES, dwf);

    m_largeIL.Create(32, 32, ILC_COLOR24|ILC_MASK, 0, 4);
    m_smallIL.Create(16,16, ILC_COLOR24|ILC_MASK,0,4);

    m_wndOutBarCtrl.SetImageList(&m_largeIL, CGfxOutBarCtrl::fLargeIcon);
    m_wndOutBarCtrl.SetImageList(&m_smallIL, CGfxOutBarCtrl::fSmallIcon);

    m_wndOutBarCtrl.SetAnimationTickCount(6);

    CString strIniDir=PROJMANAGER->GetAppPath()+"\\Databases\\Shapes\\";
    CIniReader IniReader(strIniDir+"Shapes.INI");
    CStringList* pStrL = IniReader.getSectionData("Sections");

    POSITION position;
    CString strLeft,strRight,str;
    int nCount=0;
    for(position = pStrL->GetHeadPosition(); position != NULL; ) 
    {
        str=pStrL->GetNext(position);
        strLeft=IniReader.GetDataLeft(str);
        m_wndOutBarCtrl.AddFolder(strLeft, nCount);
        strRight=IniReader.GetDataRight(str);
        int nInt[128];
        int nIntCount=CIniReader::DispartStrToSomeInt(strRight,nInt);
        for(int i=0;i<nIntCount;i++)
        {
            CString s = pSL->at(nInt[i])->ImageFileName();
            HBITMAP hBitmap = (HBITMAP) ::LoadImage(NULL,pSL->at(nInt[i])->ImageFileName() , IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
            CBitmap bm;
            bm.Attach(hBitmap);
            int nR = m_largeIL.Add(&bm,RGB(255,0,255));
            bm.DeleteObject();
            hBitmap = (HBITMAP) ::LoadImage(NULL,pSL->at(nInt[i])->ImageFileName() , IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
            bm.Attach(hBitmap);
            m_smallIL.Add(&bm,RGB(255,0,255));
            bm.DeleteObject();
            m_wndOutBarCtrl.InsertItem(nCount, -1, pSL->at(nInt[i])->Name(),nR, (DWORD)pSL->at(nInt[i]));
        }
        nCount++;
    }	
    m_wndOutBarCtrl.SetSelFolder(0);
}

LRESULT CShapesBar::OnSLBSelChanged(WPARAM wParam, LPARAM lParam)
{
	// TRACE("CShapesBar::OnSLBSelChanged()\n");
	//ASSERT(GetParent()->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)));
	CWnd* pWnd = GetParent();
	while(pWnd != NULL && !pWnd->IsKindOf(RUNTIME_CLASS(CMainFrame))) {
		pWnd = pWnd->GetParent();
	}
	if(pWnd == NULL) {
		TRACE0("Error: Could not get MainFrame to send message!\n");
		return 0;
	}
	pWnd->SendMessage(WM_SLB_SELCHANGED, wParam, lParam);
	return 0;
}

HBRUSH CShapesBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CSizingControlBarG::OnCtlColor(pDC, pWnd, nCtlColor);
	return m_brush;
}

LRESULT CShapesBar::OnPopMenu(WPARAM wParam, LPARAM lParam)
{
	CMenu Menu,*pSubMenu;
	Menu.LoadMenu(IDR_MENU_USERSHAPES);

	CPoint point;
	GetCursorPos(&point);
 	ScreenToClient(&point);
	if (GetFocus() != this) SetFocus();
	int index;
	int ht = m_wndOutBarCtrl.HitTestEx(point, index);

	ClientToScreen(&point);
	if (ht == m_wndOutBarCtrl.htFolder)
	{
		m_iSelFolder = index;
		m_iSelItem = 0;
        // old shape bars can not be edit.
        if(m_iSelFolder < 4)
            pSubMenu = Menu.GetSubMenu(3);
        else
            pSubMenu = Menu.GetSubMenu(0);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	}
	else if(ht == m_wndOutBarCtrl.htItem)
	{
		m_iSelFolder = m_wndOutBarCtrl.GetSelFolder();
		m_iSelItem = index;
		if(m_iSelFolder < 4)
            pSubMenu = Menu.GetSubMenu(3);
        else
            pSubMenu = Menu.GetSubMenu(1);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	}
	else
	{
		m_iSelFolder = m_wndOutBarCtrl.GetSelFolder();
		m_iSelItem = m_wndOutBarCtrl.GetItemCount();
		if(m_iSelFolder < 4)
            pSubMenu = Menu.GetSubMenu(3);
        else
            pSubMenu = Menu.GetSubMenu(2);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	}
	return 0;
}

void CShapesBar::AddUserShapesToShapesBar()
{
    int nBarCount = m_pUserBarMan->GetUserBarCount();

    for(int i=0; i<nBarCount; i++)
    {
        CUserShapeBar* pUserBar = m_pUserBarMan->GetUserBarByIndex(i);
        int folderIndex = m_wndOutBarCtrl.AddFolder(pUserBar->GetBarName(), (DWORD)pUserBar);

        int nShapeCount = pUserBar->GetShapeCount();
        CShape::CShapeList* pShapeList = pUserBar->GetUserShapeList();
        for(int j=0; j<nShapeCount; j++)
        {
            HBITMAP hBitmap = (HBITMAP) ::LoadImage(NULL,pShapeList->at(j)->ImageFileName() , IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
            CBitmap bm;
            bm.Attach(hBitmap);
            int nR = m_largeIL.Add(&bm,RGB(255,0,255));
            bm.DeleteObject();
            hBitmap = (HBITMAP) ::LoadImage(NULL,pShapeList->at(j)->ImageFileName() , IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
            bm.Attach(hBitmap);
            m_smallIL.Add(&bm,RGB(255,0,255));
            bm.DeleteObject();
			if(!PathFileExists(pShapeList->at(j)->ImageFileName()))
			{
				pShapeList->at(j)->ImageFileName(((CTermPlanApp*) AfxGetApp())->GetShapeDataPath() + "\\Question.bmp");
				nR = defBMP;
			}
			if(!PathFileExists(pShapeList->at(j)->ShapeFileName()))
			{
				pShapeList->at(j)->ShapeFileName(((CTermPlanApp*) AfxGetApp())->GetShapeDataPath() + "\\FLATSQUARE24cm.dxf");
				nR = defBMP;
			}
			m_wndOutBarCtrl.InsertItem(folderIndex, -1, pShapeList->at(j)->Name(),nR, (DWORD)pShapeList->at(j));
			SHAPESMANAGER->AddShapeByName(pShapeList->at(j)->Name(),pShapeList->at(j));
        }
    }
}

void CShapesBar::OnImport()
{
    CImportUserShapeBarDlg dlg;
    dlg.SetShapeFileName(m_strShapeFileName);
    dlg.SetTempPath(m_strTempPath);
	dlg.SetBarLocation(m_strProjPath+"\\UserShapes");
    while(dlg.DoModal() == IDOK)
    {
		CUserShapeBar* pImportUserBar = dlg.GetUserBarMan().GetUserBarByIndex(0);
 		CString strBarName = pImportUserBar->GetBarName();      
 		CString strBarLocation = pImportUserBar->GetBarLocation();
		int nCount = pImportUserBar->GetShapeCount();

        // create unzip destination if not exist
        if(!PathFileExists(strBarLocation))
            CreateDirectory(strBarLocation, NULL);

		// change all shapes' ".bmp" and ".dxf" file name
		for(int i=0; i<nCount; i++)
		{
			CShape* pShape = pImportUserBar->GetShapeByIndex(i);
			CString strPic = pShape->ImageFileName();
			int nPos = strPic.ReverseFind('\\');
			strPic = strPic.Right(strPic.GetLength() - nPos - 1);
			pShape->ImageFileName(strBarLocation + "\\" + strPic);

			CString strDxf = pShape->ShapeFileName();
			nPos = strDxf.ReverseFind('\\');
			strDxf = strDxf.Right(strDxf.GetLength() - nPos - 1);
			pShape->ShapeFileName(strBarLocation + "\\" + strDxf);
		}

		CUserShapeBar* pNewBar = new CUserShapeBar;
		pNewBar->SetBarName(strBarName);
		pNewBar->SetBarLocation(strBarLocation);
		CShape::CShapeList* pUserShapeList = pNewBar->GetUserShapeList();
		for(int i = 0; i < nCount ; i++)
		{
			CShape *pShape = pImportUserBar->GetShapeByIndex(i);
			CString NewName = pShape->Name();
			CString strPic,strDxf;
			double dScale = pShape->GetScale();
			BOOL bNameAvailable = FALSE;
			strPic = pShape->ImageFileName();
			if (!PathFileExists(strPic))
				bNameAvailable = TRUE;

			strDxf = pShape->ShapeFileName();
			if (!PathFileExists(strDxf))
				bNameAvailable = TRUE;

			if(!m_pUserBarMan->IsUserShapeExist(NewName))
			{
				CShape *pNewShape = SHAPESMANAGER->GetShapeByName(NewName);
				pNewShape->ImageFileName(strPic);
				pNewShape->ShapeFileName(strDxf);
				pNewShape->SetScale(dScale);
				pUserShapeList->push_back(pNewShape);
			}
			else if(bNameAvailable == FALSE)
			{
				NewName = m_pUserBarMan->GetAnAvailableShapeName(NewName);
				CShape *pNewShape = SHAPESMANAGER->GetShapeByName(NewName);
				pNewShape->ImageFileName(strPic);
				pNewShape->ShapeFileName(strDxf);
				pNewShape->SetScale(dScale);
				pUserShapeList->push_back(pNewShape);
			}
			else
			{
				CShape *pNewShape = SHAPESMANAGER->GetShapeByName(NewName);
				pUserShapeList->push_back(pNewShape);
			}

		}

		// file list of temp path 
		std::vector<CString> vSrcDirFL;
		// file list of bar location(destination path)
		std::vector<CString> vDestDirFL;
		CImportUserShapeBarDlg::FindFiles(m_strTempPath, vSrcDirFL);
		CImportUserShapeBarDlg::FindFiles(strBarLocation, vDestDirFL);
		for(int i=0; i<(int)vSrcDirFL.size(); i++)
		{
			CString strFile = vSrcDirFL.at(i);
			if(strFile.CompareNoCase(m_strShapeFileName) == 0)
				continue;
			CString str1 = m_strTempPath + "\\" + strFile;
			CString str2 = strBarLocation + "\\" + strFile;
			CopyFile(str1, str2, FALSE); // copy files
		}

		int iFolder;
		nCount = pNewBar->GetShapeCount();
		if(!m_pUserBarMan->IsUserShapeBarExist(strBarName)&&SHAPESMANAGER->FindShapeSetIndexByName(strBarName)==-1)
		{
			iFolder = m_wndOutBarCtrl.AddFolder(strBarName, (DWORD)pNewBar);
			m_wndOutBarCtrl.iSelFolder = iFolder;
			m_pUserBarMan->AddUserBar(pNewBar);
			for (int i=0;i<nCount;i++)
			{
				CShape *pShape = pNewBar->GetShapeByIndex(i);
				SHAPESMANAGER->AddShapeByName(pShape->Name(),pShape);
				HBITMAP hBitmap = (HBITMAP) ::LoadImage(NULL,pShape->ImageFileName() , IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
				CBitmap bm;
				bm.Attach(hBitmap);
				int nR = m_largeIL.Add(&bm,RGB(255,0,255));
				bm.DeleteObject();
				hBitmap = (HBITMAP) ::LoadImage(NULL,pShape->ImageFileName() , IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
				bm.Attach(hBitmap);
				m_smallIL.Add(&bm,RGB(255,0,255));
				bm.DeleteObject();
				if(!PathFileExists(pShape->ImageFileName()))
				{
					pShape->ImageFileName(((CTermPlanApp*) AfxGetApp())->GetShapeDataPath() + "\\Question.bmp");
					nR = defBMP;
				}
				if(!PathFileExists(pShape->ShapeFileName()))
				{
					pShape->ShapeFileName(((CTermPlanApp*) AfxGetApp())->GetShapeDataPath() + "\\FLATSQUARE24cm.dxf");
					nR = defBMP;
				}
				m_wndOutBarCtrl.InsertItem(m_wndOutBarCtrl.GetSelFolder(), -1, pShape->Name(),nR, (DWORD)pShape);
			}
			m_wndOutBarCtrl.SetSelFolder(iFolder);
		}
		else
		{
			iFolder = m_wndOutBarCtrl.GetFolderIndex(strBarName);
			m_wndOutBarCtrl.iSelFolder = iFolder;
			CUserShapeBar* pUserShapeBar = m_pUserBarMan->FindUserBarByName(strBarName);
			pUserShapeBar->SetBarLocation(pNewBar->GetBarLocation());
			for (int i=0;i<nCount;i++)
			{
				CShape *pShape = pNewBar->GetShapeByIndex(i);
				SHAPESMANAGER->AddShapeByName(pShape->Name(),pShape);
				if(pUserShapeBar->IsShapeExist(pShape->Name()))
				{
					HBITMAP hBitmap = (HBITMAP) ::LoadImage(NULL,pShape->ImageFileName(), IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
					CBitmap bm;
					bm.Attach(hBitmap);
					int nR = m_largeIL.Add(&bm,RGB(255,0,255));
					bm.DeleteObject();
					hBitmap = (HBITMAP) ::LoadImage(NULL,pShape->ImageFileName() , IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
					bm.Attach(hBitmap);
					m_smallIL.Add(&bm,RGB(255,0,255));
					bm.DeleteObject();

					m_wndOutBarCtrl.SetItemImage(pUserShapeBar->GetShapeIndex(pShape->Name()),nR);
					m_wndOutBarCtrl.Invalidate();
				}
				else
				{
					pUserShapeBar->AddShape(pShape);
					HBITMAP hBitmap = (HBITMAP) ::LoadImage(NULL,pShape->ImageFileName() , IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
					CBitmap bm;
					bm.Attach(hBitmap);
					int nR = m_largeIL.Add(&bm,RGB(255,0,255));
					bm.DeleteObject();
					hBitmap = (HBITMAP) ::LoadImage(NULL,pShape->ImageFileName() , IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
					bm.Attach(hBitmap);
					m_smallIL.Add(&bm,RGB(255,0,255));
					bm.DeleteObject();
					if(!PathFileExists(pShape->ImageFileName()))
					{
						pShape->ImageFileName(((CTermPlanApp*) AfxGetApp())->GetShapeDataPath() + "\\Question.bmp");
						nR = defBMP;
					}
					if(!PathFileExists(pShape->ShapeFileName()))
					{
						pShape->ShapeFileName(((CTermPlanApp*) AfxGetApp())->GetShapeDataPath() + "\\FLATSQUARE24cm.dxf");
						nR = defBMP;
					}
					m_wndOutBarCtrl.InsertItem(m_wndOutBarCtrl.GetSelFolder(), -1, pShape->Name(),nR, (DWORD)pShape);
				}
			}
			m_wndOutBarCtrl.SetSelFolder(iFolder);
		}
		m_pUserBarMan->saveDataSet(m_strProjPath, false);
		break;
	}
}

void CShapesBar::OnExport()
{
    CUserShapeBar* pUserBar = (CUserShapeBar*)m_wndOutBarCtrl.GetFolderData(m_iSelFolder);
    CString strZipDest = pUserBar->GetBarName();
    CFileDialog  dlgFile(FALSE, ".zip", strZipDest, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "(*.zip)|*.zip|");
    if(dlgFile.DoModal() == IDOK)
    {
        strZipDest = dlgFile.GetPathName();
    }

    CString tempBarInformationFile = m_strTempPath + "\\" + m_strShapeFileName;
 	if(!PathFileExists(m_strTempPath))
 		CreateDirectory(m_strTempPath, NULL);
    CUserShapeBarManager tempUserBarMan;
    tempUserBarMan.AddUserBar(pUserBar);
    tempUserBarMan.saveDataSetToOtherFile(tempBarInformationFile);
    std::vector<CString> vZipList;
    vZipList.push_back(tempBarInformationFile);
    int nCount = pUserBar->GetShapeCount();
    for(int i=0; i<nCount; i++)
    {
        CShape* pShape = pUserBar->GetShapeByIndex(i);
        int nFileCount = (int)vZipList.size();
        for(int j=0; j<nFileCount; j++)
        {

        }
        vZipList.push_back(pShape->ImageFileName());
        vZipList.push_back(pShape->ShapeFileName());
    }
    sort(vZipList.begin(), vZipList.end());
    vZipList.erase(unique(vZipList.begin(), vZipList.end()), vZipList.end());
    ZipFiles(strZipDest, vZipList);

    myDeleteDirectory(m_strTempPath);
}

BOOL CShapesBar::ZipFiles(const CString& strFilePath, const std::vector<CString>& vZipFiles)
{
    int iFileCount = vZipFiles.size();
    if (iFileCount == 0)
    {
        return TRUE;
    }

    char **pFiles = (char **) new  int [iFileCount];
    for (int i=0; i<iFileCount; i++)
    {
        pFiles[i] = new char[MAX_PATH+1];
        strcpy( pFiles[i], vZipFiles[i] );
    }

    CZipInfo InfoZip;
    if (!InfoZip.AddFiles(strFilePath, pFiles, iFileCount))
    {
        return FALSE;
    }
    return TRUE;
}

void CShapesBar::OnNewShapeBar()
{
    CShapeFolder dlg;

    CString strNewBar = _T("New Shape Bar");
    if(m_pUserBarMan->IsUserShapeBarExist(strNewBar))
    {
        for(int i=1; ; i++)
        {
            strNewBar.Format("New Shape Bar%d", i);
            if(!m_pUserBarMan->IsUserShapeBarExist(strNewBar))
                break;
        }
    }

    dlg.SetShapeBarName(strNewBar);
    dlg.SetTitle(_T("New Shape Bar"));
    while(dlg.DoModal() == IDOK)
    {
        if(m_pUserBarMan->IsUserShapeBarExist(dlg.GetShapeBarName()))
        {
            CString strErr;
            strErr.Format("A shape bar named '%s' already exists.", dlg.GetShapeBarName());
            AfxMessageBox(strErr, MB_OK);
            continue;
        }

        //update the shape bar
        CUserShapeBar* pUserBar = new CUserShapeBar();
        pUserBar->SetBarName(dlg.GetShapeBarName());
        pUserBar->SetBarLocation(dlg.GetShapeBarLocation());
        m_pUserBarMan->AddUserBar(pUserBar);

        // update GUI
        int index = m_wndOutBarCtrl.AddFolder(dlg.GetShapeBarName(), (DWORD)pUserBar);
        m_wndOutBarCtrl.iSelFolder = index;
        m_wndOutBarCtrl.Invalidate();

        // if the directory for this shape bar doesn't exist, create one.
        if(PathFileExists(dlg.GetShapeBarLocation())==FALSE)
            CreateDirectory(dlg.GetShapeBarLocation(), NULL);
        m_pUserBarMan->saveDataSet(m_strProjPath, false);
        break;
    }
}

void CShapesBar::OnEditShapeBar()
{
    CUserShapeBar* pUserBar = (CUserShapeBar*)m_wndOutBarCtrl.GetFolderData(m_iSelFolder);
    ASSERT(pUserBar);
    CShapeFolder dlg(pUserBar->GetBarName(), pUserBar->GetBarLocation());
    dlg.SetTitle("Edit Shape Bar");
    while(dlg.DoModal() == IDOK)
    {
        //check the name is available
        CUserShapeBar* pTempUserBar = m_pUserBarMan->FindUserBarByName(dlg.GetShapeBarName());
        if(pTempUserBar != NULL && pTempUserBar != pUserBar)
        {
            CString strErr;
            strErr.Format("A shape bar named '%s' already exists.", dlg.GetShapeBarName());
            AfxMessageBox(strErr, MB_OK);
            continue;
        }

        //update the shape bar data
        pUserBar->SetBarName(dlg.GetShapeBarName());
        pUserBar->SetBarLocation(dlg.GetShapeBarLocation());

        // update GUI
        m_wndOutBarCtrl.SetFolderText(m_wndOutBarCtrl.iSelFolder, dlg.GetShapeBarName());
        m_wndOutBarCtrl.Invalidate();
        m_pUserBarMan->saveDataSet(m_strProjPath, false);
        break;
    }
}

void CShapesBar::OnDeleteShapeBar()
{
    CString strWarn;
    strWarn.Format("Delete shape bar '%s'?", m_wndOutBarCtrl.GetFolderText(m_iSelFolder));
    if(MessageBox(strWarn, "Delete shape bar", MB_YESNO | MB_ICONWARNING) == IDYES)
    {
        CUserShapeBar* pUserBar = (CUserShapeBar*)m_wndOutBarCtrl.GetFolderData(m_iSelFolder);
        ASSERT(m_pUserBarMan->FindUserBarByName(pUserBar->GetBarName()) != NULL);

        // delete shape bar
        m_pUserBarMan->DeleteUserShapeBar(pUserBar);

		int nShapeCount = m_wndOutBarCtrl.GetItemCount(); 
		for(int i = nShapeCount-1; i>=0; i--)
			SHAPESMANAGER->RemoveShapeByName(m_wndOutBarCtrl.GetItemText(i));
        // update GUI
        m_wndOutBarCtrl.RemoveFolder(m_iSelFolder);

        m_pUserBarMan->saveDataSet(m_strProjPath, false);

		CMainFrame *pMain=(CMainFrame *)AfxGetMainWnd();
		CView* pView = pMain->MDIGetActive()->GetActiveView();
		pView->Invalidate();
		pView->UpdateWindow();
    }
}

void CShapesBar::OnNewShape()
{
    CString strNewShape = _T("New Shape");
    if(m_pUserBarMan->IsUserShapeExist(strNewShape))
    {
        for(int i=1; ; i++)
        {
            strNewShape.Format("New Shape%d", i);
            if(!m_pUserBarMan->IsUserShapeExist(strNewShape))
                break;
        }
    }

    CUserShapeBar* pUserBar = (CUserShapeBar*)m_wndOutBarCtrl.GetFolderData(m_iSelFolder);
    CShapeItem dlg(pUserBar->GetBarLocation());
    dlg.SetTitle("New Shape");
    dlg.SetShapeName(strNewShape);

    while(dlg.DoModal() == IDOK)
    {
        if(m_pUserBarMan->IsUserShapeExist(dlg.GetShapeName()))
        {
            CString strErr;
            strErr.Format("A shape named '%s' already exists.", dlg.GetShapeName());
            AfxMessageBox(strErr, MB_OK);
            continue;
        }

        // update data
        CUserShapeBar* pUserBar = (CUserShapeBar*)m_wndOutBarCtrl.GetFolderData(m_iSelFolder);
        CShape* pShape = SHAPESMANAGER->GetShapeByName(dlg.GetShapeName());
        pShape->Name(dlg.GetShapeName());
        pShape->ShapeFileName(dlg.GetShapeModel());
        pShape->ImageFileName(dlg.GetShapePicture());
		pShape->SetScale(UnitToScale(dlg.GetShapeUnit()));
        pUserBar->AddShape(pShape);

        // update GUI
        HBITMAP hBitmap = (HBITMAP) ::LoadImage(NULL,dlg.GetShapePicture(), IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
        CBitmap bm;
        bm.Attach(hBitmap);
        int nR = m_largeIL.Add(&bm,RGB(255,0,255));
        bm.DeleteObject();
        hBitmap = (HBITMAP) ::LoadImage(NULL,dlg.GetShapePicture() , IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
        bm.Attach(hBitmap);
        m_smallIL.Add(&bm,RGB(255,0,255));
        bm.DeleteObject();
		if(!PathFileExists(pShape->ImageFileName()))
		{
			pShape->ImageFileName(((CTermPlanApp*) AfxGetApp())->GetShapeDataPath() + "\\Question.bmp");
			nR = defBMP;
		}
		if(!PathFileExists(pShape->ShapeFileName()))
		{
			pShape->ShapeFileName(((CTermPlanApp*) AfxGetApp())->GetShapeDataPath() + "\\FLATSQUARE24cm.dxf");
			nR = defBMP;
		}
		m_wndOutBarCtrl.InsertItem(m_iSelFolder, -1, pShape->Name(),nR, (DWORD)pShape);
		SHAPESMANAGER->AddShapeByName(pShape->Name(),pShape);
        m_wndOutBarCtrl.Invalidate();
        m_pUserBarMan->saveDataSet(m_strProjPath, false);
        break;
	}

}

void CShapesBar::OnEditShape()
{
    CUserShapeBar* pUserBar = (CUserShapeBar*)m_wndOutBarCtrl.GetFolderData(m_iSelFolder);
    CShape* pShape = (CShape*)m_wndOutBarCtrl.GetItemData(m_iSelItem);
    CShapeItem dlg(pUserBar->GetBarLocation(),pShape->Name(),
        pShape->ImageFileName(),pShape->ShapeFileName(),ScaleToUnit(pShape->GetScale()));
    dlg.SetTitle("Edit Shape");
    while(dlg.DoModal() == IDOK)
    {
        CShape* pTempShape = m_pUserBarMan->FindShapeByName(dlg.GetShapeName());
        if(pTempShape != NULL && pTempShape != pShape)
        {
            CString strErr;
            strErr.Format("A shape named '%s' already exists.", dlg.GetShapeName());
            AfxMessageBox(strErr, MB_OK);
            continue;
        }

        pShape->Name(dlg.GetShapeName());
        m_wndOutBarCtrl.SetItemText(m_iSelItem,dlg.GetShapeName());
        m_wndOutBarCtrl.Invalidate();

        if(pShape->ImageFileName().Compare(dlg.GetShapePicture()) != 0)
        {
            pShape->ImageFileName(dlg.GetShapePicture());

            HBITMAP hBitmap = (HBITMAP) ::LoadImage(NULL,dlg.GetShapePicture(), IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
            CBitmap bm;
            bm.Attach(hBitmap);
            int nR = m_largeIL.Add(&bm,RGB(255,0,255));
            bm.DeleteObject();
            hBitmap = (HBITMAP) ::LoadImage(NULL,dlg.GetShapePicture() , IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
            bm.Attach(hBitmap);
            m_smallIL.Add(&bm,RGB(255,0,255));
            bm.DeleteObject();

            m_wndOutBarCtrl.SetItemImage(m_iSelItem,nR);
            m_wndOutBarCtrl.Invalidate();
        }

        if(pShape->ShapeFileName().Compare(dlg.GetShapeModel()) != 0)
        {
            pShape->ShapeFileName(dlg.GetShapeModel());
            pShape->SetObjListValid(FALSE);
        }

		if(ScaleToUnit(pShape->GetScale()).Compare(dlg.GetShapeUnit()) != 0)
		{
			pShape->SetScale(UnitToScale(dlg.GetShapeUnit()));
			//pShape->SetObjListValid(FALSE);
		}
		
        m_pUserBarMan->saveDataSet(m_strProjPath, false);
        break;
    }
}

void CShapesBar::OnDeleteShape()
{
    CString strWarn;
    strWarn.Format("Delete shape '%s'?", m_wndOutBarCtrl.GetItemText(m_iSelItem));
    if(MessageBox(strWarn, "Delete shape", MB_YESNO | MB_ICONWARNING) == IDYES)
    {
        CUserShapeBar* pUserBar = (CUserShapeBar*)m_wndOutBarCtrl.GetFolderData(m_iSelFolder);
        CShape* pShape = (CShape*)m_wndOutBarCtrl.GetItemData(m_iSelItem);
		m_wndOutBarCtrl.RemoveItem(m_iSelItem);
		SHAPESMANAGER->RemoveShapeByName(pShape->Name());

        pUserBar->DeleteShape(pShape);

        m_pUserBarMan->saveDataSet(m_strProjPath, false);

 		 CMainFrame *pMain=(CMainFrame *)AfxGetMainWnd();
 		 CView* pView = pMain->MDIGetActive()->GetActiveView();
		 pView->Invalidate();
 		 pView->UpdateWindow();
    }
}


