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

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

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
	folder_index = 0;
	item_index = 0;
	return 0;
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
		folder_index = index;
		item_index = 0;
        // old shape bars can not be edit.
        if(folder_index < 4)
            pSubMenu = Menu.GetSubMenu(3);
        else
            pSubMenu = Menu.GetSubMenu(0);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	}
	else if(ht == m_wndOutBarCtrl.htItem)
	{
		folder_index = m_wndOutBarCtrl.GetSelFolder();
		item_index = index;
		if(folder_index < 4)
            pSubMenu = Menu.GetSubMenu(3);
        else
            pSubMenu = Menu.GetSubMenu(1);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	}
	else
	{
		folder_index = m_wndOutBarCtrl.GetSelFolder();
		item_index = m_wndOutBarCtrl.GetItemCount();
		if(folder_index < 4)
            pSubMenu = Menu.GetSubMenu(3);
        else
            pSubMenu = Menu.GetSubMenu(2);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	}
	return 0;
}

BOOL CShapesBar::ImportShapeBarData(CString ImportFile)
{
	try
	{
		CString FileINI = ImportFile;
		CFile pFile;
		if(pFile.Open(FileINI, CFile::modeRead)==0)
		{
			return FALSE;
		}
		CShape::CShapeList* pSL = SHAPESMANAGER->GetShapeList();
		CArchive ar(&pFile,CArchive::load);
		char line[513];
		ar.ReadString(line,512);
		ar.ReadString(line,512);
		if(_stricmp(line,"Shapes Database") == 0)
		{
			ar.ReadString(line,512);
			while(*(ar.ReadString(line, 512)) != '\0')
			{
				
				CShape* pShape = new CShape();
				char* b = line;
				char* p = NULL;
				int c = 1;	
				while((p = strchr(b, ',')) != NULL)
				{
					*p = '\0';
					switch(c)
					{
					case 1: //name
						pShape->Name(b);
						break;
					case 2: //shape file
						pShape->ShapeFileName(b);
						break;
					default:
						break;
					}
					b = ++p;
					c++;
				}
				if(b!=NULL&&c==3) // the last column did not have a comma after it
				{
					pShape->ImageFileName(b);
				}
				pSL->push_back(pShape);

				HBITMAP hBitmap = (HBITMAP) ::LoadImage(NULL,pShape->ImageFileName() , IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
				CBitmap bm;
				bm.Attach(hBitmap);
				int nR = m_largeIL.Add(&bm,RGB(255,0,255));
				bm.DeleteObject();
				hBitmap = (HBITMAP) ::LoadImage(NULL,pShape->ImageFileName() , IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
				bm.Attach(hBitmap);
				m_smallIL.Add(&bm,RGB(255,0,255));
				bm.DeleteObject();
				m_wndOutBarCtrl.InsertItem(m_wndOutBarCtrl.GetSelFolder(), -1, pShape->Name(),nR, (DWORD)pShape);
			}
			m_wndOutBarCtrl.Invalidate();
 			ar.Close();
 			pFile.Close();
			return TRUE;
		}
		ar.Close();
		pFile.Close();
		return FALSE;
	}
	catch(CException* e)
	{
		e->Delete();
		return FALSE;
	}	
}

BOOL CShapesBar::ExportShapeBarData(CString ExportFile)
{
	CString FileINI = ExportFile;
	if(PathFileExists(FileINI)==TRUE)
		DeleteFile(FileINI);

	CString sProjectName;
	int n = UserProjectPath.GetLength() - UserProjectPath.ReverseFind('\\')-1;
	sProjectName = UserProjectPath.Right(n);
 	PROJECTINFO* pi = new PROJECTINFO();
 	PROJMANAGER->GetProjectInfo(sProjectName,pi);

	CString filename = ExportFile;
	n = filename.GetLength() - filename.ReverseFind('\\')-1;
	filename = filename.Right(n);

	CFile pFile(FileINI,CFile::modeWrite|CFile::modeCreate);
	CArchive ar(&pFile,CArchive::store);
	CString str;
 	str.Format(filename +",%d\r\n",pi->version);	ar.Write(str,str.GetLength());
	str = "Shapes Database\r\n";	ar.Write(str,str.GetLength());
	str = "Name,File,Image\r\n";	ar.Write(str,str.GetLength());
	CShape* pShape = NULL;
	for(int i=0;i<m_wndOutBarCtrl.GetItemCount();i++)
	{
		pShape = (CShape*)m_wndOutBarCtrl.GetItemData(i);
		str.Format("%s,%s,%s\r\n",pShape->Name(),pShape->ShapeFileName(),pShape->ImageFileName());
		ar.Write(str,str.GetLength());
	}
	m_wndOutBarCtrl.Invalidate();
	str = "\r\n";	ar.Write(str,str.GetLength());
	str = pi->modifytime.Format("%y/%m/%d,%H:%M:%S");	ar.Write(str,str.GetLength());
	ar.Close();
	pFile.Close();

	//CopyFile(FileINI,CString(ExportFilePath + m_wndOutBarCtrl.GetFolderText(folder_index) + ".INI"),TRUE);
	return TRUE;
}

BOOL CShapesBar::ExportShapeBarDataAndShapes(CString ExportFolder)
{
	if(PathFileExists(ExportFolder)==TRUE)
		myDeleteDirectory(ExportFolder);
	CreateDirectory(ExportFolder,NULL);

	CString sProjectName;
	int n = UserProjectPath.GetLength() - UserProjectPath.ReverseFind('\\')-1;
	sProjectName = UserProjectPath.Right(n);
	PROJECTINFO* pi = new PROJECTINFO();
	PROJMANAGER->GetProjectInfo(sProjectName,pi);

	CString filename = ExportFolder;
	n = filename.GetLength() - filename.ReverseFind('\\')-1;
	filename = filename.Right(n);

	CString FileINI = ExportFolder + "\\" + filename + ".INI";
	CFile pFile(FileINI,CFile::modeWrite|CFile::modeCreate);
	CArchive ar(&pFile,CArchive::store);
	CString str;
	str.Format(filename +",%d\r\n",pi->version);	ar.Write(str,str.GetLength());
	str = "Shapes Database\r\n";	ar.Write(str,str.GetLength());
	str = "Name,File,Image\r\n";	ar.Write(str,str.GetLength());
	CShape* pShape = NULL;
	for(int i=0;i<m_wndOutBarCtrl.GetItemCount();i++)
	{
		pShape = (CShape*)m_wndOutBarCtrl.GetItemData(i);

		CString shapefilename = ExportFolder + "\\" + pShape->Name() + ".bmp";
		CopyFile(pShape->ShapeFileName(),shapefilename,FALSE);

		CString imagefilename = ExportFolder + "\\" + pShape->Name() + ".dxf";
		CopyFile(pShape->ImageFileName(),imagefilename,FALSE);
	
		str.Format("%s,%s,%s\r\n",pShape->Name(),shapefilename,imagefilename);
		ar.Write(str,str.GetLength());
	}
	m_wndOutBarCtrl.Invalidate();
	str = "\r\n";	ar.Write(str,str.GetLength());
	str = pi->modifytime.Format("%y/%m/%d,%H:%M:%S");	ar.Write(str,str.GetLength());
	ar.Close();
	pFile.Close();

	return TRUE;
}

void CShapesBar::ImportUserShapeBars()
{
	try
	{
		CString FileINI = UserProjectPath + "\\INPUT\\UserShapesBar.INI";
		CFile pFile;
		if(pFile.Open(FileINI, CFile::modeRead)==0)
			return;
		CArchive ar(&pFile,CArchive::load);
		char line[513];
		ar.ReadString(line,512);
		ar.ReadString(line,512);
		if(_stricmp(line,"Shapes Database") == 0)
		{
			ar.ReadString(line,512);
			int nCount=4;
			int sel = m_wndOutBarCtrl.iSelFolder;
			while(ar.ReadString(line, 512))
			{
				CString str = line;
				if(!str.Compare(""))
					break;
				int n = str.Find(",");
				if(n != -1)
				{
					CString name = str.Left(n);
					m_wndOutBarCtrl.AddFolder(name,nCount);

					CString path = str.Right(str.GetLength()-n-1);
					m_wndOutBarCtrl.SetFolderPathText(nCount,path);

					CString filename = UserProjectPath + "\\INPUT\\UserBars\\" + name + ".INI";
					m_wndOutBarCtrl.iSelFolder = nCount;
					ImportShapeBarData(filename);

					nCount++;
				}
			}
			m_wndOutBarCtrl.iSelFolder = sel;
		}
	}
	catch(CException* e)
	{
		e->Delete();
		return;
	}
}

void CShapesBar::ExportUserShapeBars()
{
	CString FileINI = UserProjectPath + "\\INPUT\\UserShapesBar.INI";
	if(PathFileExists(FileINI)==TRUE)
		DeleteFile(FileINI);

	CString BarsDirectory = UserProjectPath + "\\INPUT\\UserBars\\";
	if(PathFileExists(BarsDirectory)==TRUE)
		myDeleteDirectory(BarsDirectory);
	CreateDirectory(BarsDirectory,NULL);

	int iFolderNum = m_wndOutBarCtrl.GetFolderCount();
	if(iFolderNum > 4)
	{	
		CString sProjectName;
		int n = UserProjectPath.GetLength() - UserProjectPath.ReverseFind('\\')-1;
		sProjectName = UserProjectPath.Right(n);

		PROJECTINFO* pi = new PROJECTINFO();
		PROJMANAGER->GetProjectInfo(sProjectName,pi);
		CFile pFile(FileINI,CFile::modeWrite|CFile::modeCreate);
		CArchive ar(&pFile,CArchive::store);
		CString str;
		str.Format("UserShapesBar.INI,%d\r\n",pi->version);	ar.Write(str,str.GetLength());
		str = "Shapes Database\r\n";	ar.Write(str,str.GetLength());
		str = "Name,File,Image\r\n";	ar.Write(str,str.GetLength());
		int sel = m_wndOutBarCtrl.iSelFolder;
		for (int i = 4 ; i < iFolderNum ; i++)
		{
			str = m_wndOutBarCtrl.GetFolderText(i);
			ar.Write(str,str.GetLength());

			m_wndOutBarCtrl.iSelFolder = i;
			ExportShapeBarData(BarsDirectory + str + ".INI");

			str = "," + m_wndOutBarCtrl.GetFolderPathText(i) + "\r\n";			
			ar.Write(str,str.GetLength());	
		}
		m_wndOutBarCtrl.iSelFolder = sel;
		str = "\r\n";	ar.Write(str,str.GetLength());
		str = pi->modifytime.Format("%y/%m/%d,%H:%M:%S");	ar.Write(str,str.GetLength());
		ar.Close();
		pFile.Close();	
	}
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

void CShapesBar::OnNewShapeBar()
{
	CShapeFolder dlg;
    
    CString strNewBar = _T("New Shape Bar");
    if(m_wndOutBarCtrl.IsFolderNameExist(strNewBar))
    {
        for(int i=1; ; i++)
        {
            strNewBar.Format("New Shape Bar%d", i);
            if(!m_wndOutBarCtrl.IsFolderNameExist(strNewBar))
                break;
        }
    }

    dlg.SetShapeBarName(strNewBar);
    while(dlg.DoModal() == IDOK)
    {
        if(m_wndOutBarCtrl.IsFolderNameExist(dlg.GetShapeBarName()))
        {
            CString strErr;
            strErr.Format("A shape bar named '%s' already exists.", dlg.GetShapeBarName());
            AfxMessageBox(strErr, MB_OK);
            continue;
        }

        //update the shape bar
        int index = m_wndOutBarCtrl.GetFolderCount();
        m_wndOutBarCtrl.AddFolder(dlg.GetShapeBarName(),index);
        m_wndOutBarCtrl.SetFolderPathText(index,dlg.GetShapeBarLocation());
        m_wndOutBarCtrl.iSelFolder = index;
        m_wndOutBarCtrl.Invalidate();
        break;
	}
}

void CShapesBar::OnEditShapeBar()
{
	CShapeFolder dlg(m_wndOutBarCtrl.GetFolderText(folder_index),m_wndOutBarCtrl.GetFolderPathText(folder_index));
    dlg.SetTitle("Edit Shape Bar");
	while(dlg.DoModal() == IDOK)
	{
        //check the name is available
        int nIndex = m_wndOutBarCtrl.GetFolderIndexByName(dlg.GetShapeBarName());
        if(nIndex != -1 && nIndex != folder_index)
        {
            CString strErr;
            strErr.Format("A shape bar named '%s' already exists.", dlg.GetShapeBarName());
            AfxMessageBox(strErr, MB_OK);
            continue;
        }
        //update the shape bar
        m_wndOutBarCtrl.SetFolderText(folder_index, dlg.GetShapeBarName());
        m_wndOutBarCtrl.SetFolderPathText(folder_index, dlg.GetShapeBarLocation());
        m_wndOutBarCtrl.Invalidate();
        break;
	}
}

void CShapesBar::OnImport()
{
	CString ImportFile;
	CFileDialog  dlgFile( TRUE,NULL,m_wndOutBarCtrl.GetFolderPathText(folder_index),
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"(*.zip)|*.zip|");
	if (dlgFile.DoModal() == IDOK)
	{
	 	ImportFile = dlgFile.GetPathName();
	}
	int sel = m_wndOutBarCtrl.iSelFolder;
	m_wndOutBarCtrl.iSelFolder = folder_index;
	ImportShapeBarData(ImportFile);
	m_wndOutBarCtrl.iSelFolder = sel;
}

void CShapesBar::OnExport()
{
    CString strExportPath = m_wndOutBarCtrl.GetFolderPathText(folder_index);
    strExportPath += m_wndOutBarCtrl.GetFolderText(folder_index);
    CFileDialog  dlgFile(FALSE, ".zip", strExportPath,
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "(*.zip)|*.zip|");
    if(dlgFile.DoModal() == IDOK)
    {
        strExportPath = dlgFile.GetPathName();
        int sel = m_wndOutBarCtrl.iSelFolder;
        m_wndOutBarCtrl.iSelFolder = folder_index;
        ExportShapeBarDataAndShapes(strExportPath);
        m_wndOutBarCtrl.iSelFolder = sel;
    }
}

void CShapesBar::OnDeleteShapeBar()
{

// 	//delete the directory
// 			CString sFP = UserShapesPath + m_wndOutBarCtrl.GetFolderText(folder_index);
// 			if(PathFileExists(sFP) == TRUE)
// 				myDeleteDirectory(sFP);

	//update the shape bar
    CString strWarn;
    strWarn.Format("Delete shape bar '%s'?", m_wndOutBarCtrl.GetFolderText(folder_index));
    if(MessageBox(strWarn, "Delete shape bar", MB_YESNO | MB_ICONWARNING) == IDYES)
    {
        m_wndOutBarCtrl.RemoveFolder(folder_index);
    }
}

void CShapesBar::OnNewShape()
{
    CString strNewShape = _T("New Shape");
    if(m_wndOutBarCtrl.IsItemNameExist(strNewShape))
    {
        for(int i=1; ; i++)
        {
            strNewShape.Format("New Shape%d", i);
            if(!m_wndOutBarCtrl.IsItemNameExist(strNewShape))
                break;
        }
    }

    CShapeItem dlg(m_wndOutBarCtrl.GetFolderPathText(folder_index));
    dlg.SetTitle("New Shape");
    dlg.SetShapeName(strNewShape);

    while(dlg.DoModal() == IDOK)
    {
        if(m_wndOutBarCtrl.IsItemNameExist(dlg.GetShapeName()))
        {
            CString strErr;
            strErr.Format("A shape named '%s' already exists.", dlg.GetShapeName());
            AfxMessageBox(strErr, MB_OK);
            continue;
        }

        //add item
        HBITMAP hBitmap = (HBITMAP) ::LoadImage(NULL,dlg.GetShapePicture(), IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
        CBitmap bm;
        bm.Attach(hBitmap);
        int nR = m_largeIL.Add(&bm,RGB(255,0,255));
        bm.DeleteObject();
        hBitmap = (HBITMAP) ::LoadImage(NULL,dlg.GetShapePicture() , IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
        bm.Attach(hBitmap);
        m_smallIL.Add(&bm,RGB(255,0,255));
        bm.DeleteObject();

        CShape* pShape = new CShape();
        pShape->Name(dlg.GetShapeName());
        pShape->ShapeFileName(dlg.GetShapeModel());
        pShape->ImageFileName(dlg.GetShapePicture());
        m_wndOutBarCtrl.InsertItem(folder_index, -1, dlg.GetShapeName(),nR,(DWORD)pShape);
        m_wndOutBarCtrl.Invalidate();

        //add new resource
        //CString StoragePath = UserShapesPath+m_wndOutBarCtrl.GetFolderText(folder_index);
        //if(PathFileExists(StoragePath) == FALSE)
        //	CreateDirectory(StoragePath, NULL);

        //CString sPictureName,sModelName;
        //int n = dlg.itemPicture.GetLength() - dlg.itemPicture.ReverseFind('\\')-1;
        //sPictureName = StoragePath + "\\" + dlg.itemPicture.Right(n);
        //n = dlg.itemModel.GetLength() - dlg.itemModel.ReverseFind('\\')-1;
        //sModelName = StoragePath  + "\\" + dlg.itemModel.Right(n);

        //CopyFile(dlg.itemPicture,sPictureName,TRUE);
        //CopyFile(dlg.itemModel,sModelName,TRUE);
        break;
	}

}

void CShapesBar::OnEditShape()
{
	CShape* pShape = (CShape*)m_wndOutBarCtrl.GetItemData(item_index);
	CShapeItem dlg(m_wndOutBarCtrl.GetFolderPathText(folder_index),pShape->Name(),
		pShape->ImageFileName(),pShape->ShapeFileName());
    dlg.SetTitle("Edit Shape");
    while(dlg.DoModal() == IDOK)
    {
        int nIndex = m_wndOutBarCtrl.GetItemIndexByName(dlg.GetShapeName());
        if(nIndex != -1 && nIndex != item_index)
        {
            CString strErr;
            strErr.Format("A shape named '%s' already exists.", dlg.GetShapeName());
            AfxMessageBox(strErr, MB_OK);
            continue;
        }
        m_wndOutBarCtrl.SetItemText(item_index,dlg.GetShapeName());
        m_wndOutBarCtrl.Invalidate();
        pShape->Name(dlg.GetShapeName());

        if(pShape->ImageFileName().Compare(dlg.GetShapePicture()) != 0)
        {
            HBITMAP hBitmap = (HBITMAP) ::LoadImage(NULL,dlg.GetShapePicture(), IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
            CBitmap bm;
            bm.Attach(hBitmap);
            int nR = m_largeIL.Add(&bm,RGB(255,0,255));
            bm.DeleteObject();
            hBitmap = (HBITMAP) ::LoadImage(NULL,dlg.GetShapePicture() , IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
            bm.Attach(hBitmap);
            m_smallIL.Add(&bm,RGB(255,0,255));
            bm.DeleteObject();

            m_wndOutBarCtrl.SetItemImage(item_index,nR);
            m_wndOutBarCtrl.Invalidate();

            pShape->ImageFileName(dlg.GetShapePicture());
        }

        if(pShape->ShapeFileName().Compare(dlg.GetShapeModel()) != 0)
        {
            pShape->ShapeFileName(dlg.GetShapeModel());
            pShape->SetObjListValid(FALSE);
        }
        break;
    }
}

void CShapesBar::OnDeleteShape()
{
    CString strWarn;
    strWarn.Format("Delete shape '%s'?", m_wndOutBarCtrl.GetItemText(item_index));
    if(MessageBox(strWarn, "Delete shape", MB_YESNO | MB_ICONWARNING) == IDYES)
    {
        m_wndOutBarCtrl.RemoveItem(item_index);
    }
}