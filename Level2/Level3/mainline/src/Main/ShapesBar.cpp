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

	ON_COMMAND(ID_FOLDER_ADDFOLDER,OnAddFolder)
	ON_COMMAND(ID_FOLDER_RENAME,OnFolderRename)
	ON_COMMAND(ID_FOLDER_CHANGEPATH,OnChangeFolderPath)
	ON_COMMAND(ID_FOLDER_IMPORT,OnImport)
	ON_COMMAND(ID_FOLDER_EXPORT,OnExport)
	ON_COMMAND(ID_FOLDER_DELETEFOLDER,OnDeleteFolder)

	ON_COMMAND(ID_ITEM_ADDITM,OnAddItem)
	ON_COMMAND(ID_ITEM_RENAME,OnItemRename)
	ON_COMMAND(ID_ITEM_CHANGEPICTURE,OnChangeItemPicture)
	ON_COMMAND(ID_ITEM_CHANGEMODEL,OnChangeItemModel)
	ON_COMMAND(ID_ITEM_DELETEITEM,OnDeleteItem)

	ON_COMMAND(ID_OTHER_ADDFOLDER,OnAddFolder)
	ON_COMMAND(ID_OTHER_DELETEITEM,OnAddItem)
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
	CShapeFolder::folder_id = 0;
	CShapeItem::item_id = 0;

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
        // old folders can not be edit.
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
// 			if(FileINI.Compare(""))
// 				AfxMessageBox("INI file doesn\'t exist!", MB_OK);
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
		//AfxMessageBox("Error reading Shapes DB file: " + FileDir, MB_ICONEXCLAMATION | MB_OK);
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

void CShapesBar::OnAddFolder()
{
	CShapeFolder dlg;
    dlg.SetStyle(CShapeFolder::NEW);
	while(dlg.DoModal() == IDOK)
	{
        if(m_wndOutBarCtrl.IsFolderNameExist(dlg.folderName))
        {
            CString strErr;
            strErr.Format("A folder named '%s' already exists.", dlg.folderName);
            AfxMessageBox(strErr, MB_OK);
            continue;
        }

// 		//create a directory
// 		CString sNewFP = UserShapesPath + dlg.folderName;
// 		if(PathFileExists(sNewFP) == FALSE)
// 			CreateDirectory(sNewFP, NULL);

		//update the shape bar
		int index = m_wndOutBarCtrl.GetFolderCount();
		m_wndOutBarCtrl.AddFolder(dlg.folderName,index);
		m_wndOutBarCtrl.SetFolderPathText(index,dlg.folderPath);
		m_wndOutBarCtrl.iSelFolder = index;
		m_wndOutBarCtrl.Invalidate();
        break;
	}
}

void CShapesBar::OnFolderRename()
{
	CShapeFolder dlg(m_wndOutBarCtrl.GetFolderText(folder_index),m_wndOutBarCtrl.GetFolderPathText(folder_index),CShapeFolder::NAME);
	while(dlg.DoModal() == IDOK)
	{
        //check the name is available
        int nIndex = m_wndOutBarCtrl.GetIndexByFolderName(dlg.folderName);
        if(nIndex != -1 && nIndex != folder_index)
        {
            CString strErr;
            strErr.Format("A folder named '%s' already exists.", dlg.folderName);
            AfxMessageBox(strErr, MB_OK);
            continue;
        }

// 		//copy files to a new directory and delete the old directory
// 				CString sOldFP = UserShapesPath + m_wndOutBarCtrl.GetFolderText(folder_index);
// 				CString sNewFP = UserShapesPath + dlg.folderName;
// 		
// 				if(PathFileExists(sNewFP) == FALSE)
// 					CreateDirectory(sNewFP, NULL);
// 		
// 				if(PathFileExists(sOldFP) == TRUE)
// 				{
// 					myCopyDirectory(sOldFP,sNewFP);
// 					myDeleteDirectory(sOldFP);
// 				}
		
		//update the shape bar
		m_wndOutBarCtrl.SetFolderText(folder_index, dlg.folderName);
		m_wndOutBarCtrl.Invalidate();
        break;
	}
}

void CShapesBar::OnChangeFolderPath()
{
	CShapeFolder dlg(m_wndOutBarCtrl.GetFolderText(folder_index),m_wndOutBarCtrl.GetFolderPathText(folder_index),CShapeFolder::PATH);
	if(dlg.DoModal() == IDOK )
	{
		//update the shape bar
		m_wndOutBarCtrl.SetFolderPathText(folder_index, dlg.folderPath);
	}
}

void CShapesBar::OnImport()
{
	CString ImportFile;
	CFileDialog  dlgFile(TRUE,NULL,m_wndOutBarCtrl.GetFolderPathText(folder_index),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"(*.INI)|*.INI");
	if (dlgFile.DoModal() == IDOK)
	{
	 	ImportFile = dlgFile.GetPathName();
	}
	int sel = m_wndOutBarCtrl.iSelFolder;
	m_wndOutBarCtrl.iSelFolder = folder_index;
	ImportShapeBarData(ImportFile);
	m_wndOutBarCtrl.iSelFolder = sel;
	//ImportUserShapeBars();
// 	CString StoragePath = UserShapesPath+m_wndOutBarCtrl.GetFolderText(folder_index);
// 	if(PathFileExists(StoragePath) == FALSE)
// 		CreateDirectory(StoragePath, NULL);
// 
// 	CString  ImportFile;
// 	CFileDialog  dlgFile(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"(*.zip)|*.zip");
// 	if (dlgFile.DoModal() == IDOK)
// 	{
// 		ImportFile = dlgFile.GetPathName();
// 	}


// 	if(PathFileExists(ImportFile) == TRUE)
// 	{
// 		BeginWaitCursor();
// 		CZipInfo InfoZip;
// 		if (!InfoZip.ExtractFiles(ImportFile,CString(StoragePath + "\\")))
// 		{
// 			EndWaitCursor();
// 			AfxMessageBox("Unzip file failed! file: " + ImportFile, MB_OK);
// 		}
 				
// 		EndWaitCursor();
// 	}
}

void CShapesBar::OnExport()
{
//  	CString ExportFile = m_wndOutBarCtrl.GetFolderPathText(folder_index) + "\\" + m_wndOutBarCtrl.GetFolderText(folder_index)+".zip";
//  	if(PathFileExists(ExportFile) == TRUE)
//  		DeleteFile(ExportFile);
	
	char szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath));   
	BROWSEINFO bi;   
	bi.hwndOwner = m_hWnd;   
	bi.pidlRoot = NULL;   
	bi.pszDisplayName = szPath;   
	bi.lpszTitle = "Select a folder path:";   
	bi.ulFlags = 0;   
	bi.lpfn = NULL;   
	bi.lParam = 0;   
	bi.iImage = 0;   
	LPITEMIDLIST lp = SHBrowseForFolder(&bi);   

	CString ExportFile;
	if(lp && SHGetPathFromIDList(lp, szPath))   
	{
		ExportFile = szPath;
		int n = ExportFile.ReverseFind('\\');
		if (n == ExportFile.GetLength()-1)
		{
			ExportFile = ExportFile.Left(n);
		}
		ExportFile = ExportFile + "\\" + m_wndOutBarCtrl.GetFolderText(folder_index) + ".INI";
	}
	int sel = m_wndOutBarCtrl.iSelFolder;
	m_wndOutBarCtrl.iSelFolder = folder_index;
	ExportShapeBarData(ExportFile);
	m_wndOutBarCtrl.iSelFolder = sel;
	//ExportUserShapeBars();
// 	CString StoragePath = UserShapesPath+m_wndOutBarCtrl.GetFolderText(folder_index);
// 	if(PathFileExists(StoragePath) == TRUE)
// 	{
// 		BeginWaitCursor();
// 		ExportShapeBarData();
// 		CZipInfo InfoZip;
// 		if (!InfoZip.AddDirectory(ExportFile,CString(StoragePath+"\\") ))
// 		{
// 			EndWaitCursor();
// 			AfxMessageBox("Zip file failed! file: " + ExportFile, MB_OK);
// 		}
// 		EndWaitCursor();
// 	}
// 	else
// 		AfxMessageBox("file: " + StoragePath + " doesn\'t exist", MB_OK);
}

void CShapesBar::OnDeleteFolder()
{

// 	//delete the directory
// 			CString sFP = UserShapesPath + m_wndOutBarCtrl.GetFolderText(folder_index);
// 			if(PathFileExists(sFP) == TRUE)
// 				myDeleteDirectory(sFP);

	//update the shape bar
	m_wndOutBarCtrl.RemoveFolder(folder_index);
}

void CShapesBar::OnAddItem()
{
	CShapeItem dlg(m_wndOutBarCtrl.GetFolderPathText(folder_index));
	if(dlg.DoModal() == IDOK )
	{
		//check the name is available
		for(int i = 0;i<m_wndOutBarCtrl.GetItemCount();i++)
		{
			CString ExistFN = m_wndOutBarCtrl.GetItemText(i);
			if (!ExistFN.Compare(dlg.itemName))
			{
				AfxMessageBox(dlg.itemName + " exists", MB_OK);
				return;
			}
		}
		dlg.item_id++;

		//add item
		HBITMAP hBitmap = (HBITMAP) ::LoadImage(NULL,dlg.itemPicture, IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		CBitmap bm;
		bm.Attach(hBitmap);
		int nR = m_largeIL.Add(&bm,RGB(255,0,255));
		bm.DeleteObject();
		hBitmap = (HBITMAP) ::LoadImage(NULL,dlg.itemPicture , IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		bm.Attach(hBitmap);
		m_smallIL.Add(&bm,RGB(255,0,255));
		bm.DeleteObject();

		CShape* pShape = new CShape();
		pShape->Name(dlg.itemName);
		pShape->ShapeFileName(dlg.itemModel);
		pShape->ImageFileName(dlg.itemPicture);
		m_wndOutBarCtrl.InsertItem(folder_index, -1, dlg.itemName,nR,(DWORD)pShape);
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
	}

}

void CShapesBar::OnItemRename()
{
	CShape* pShape = (CShape*)m_wndOutBarCtrl.GetItemData(item_index);
	CShapeItem dlg(m_wndOutBarCtrl.GetFolderPathText(folder_index),pShape->Name(),
		pShape->ImageFileName(),pShape->ShapeFileName(),CShapeItem::NAME);
	if(dlg.DoModal() == IDOK )
	{
		//check the name is available
		for(int i = 0;i<m_wndOutBarCtrl.GetItemCount();i++)
		{
			CString ExistFN = m_wndOutBarCtrl.GetItemText(i);
			if (!ExistFN.Compare(dlg.itemName))
			{
				AfxMessageBox(dlg.itemName + " exists", MB_OK);
				return;
			}
		}

		m_wndOutBarCtrl.SetItemText(item_index,dlg.itemName);
		m_wndOutBarCtrl.Invalidate();

		pShape->Name(dlg.itemName);
	}
}

void CShapesBar::OnChangeItemPicture()
{
	CShape* pShape = (CShape*)m_wndOutBarCtrl.GetItemData(item_index);
	CShapeItem dlg(m_wndOutBarCtrl.GetFolderPathText(folder_index),pShape->Name(),
		pShape->ImageFileName(),pShape->ShapeFileName(),CShapeItem::PICTURE);
	if(dlg.DoModal() == IDOK )
	{
		//change picture
		HBITMAP hBitmap = (HBITMAP) ::LoadImage(NULL,dlg.itemPicture, IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		CBitmap bm;
		bm.Attach(hBitmap);
		int nR = m_largeIL.Add(&bm,RGB(255,0,255));
		bm.DeleteObject();
		hBitmap = (HBITMAP) ::LoadImage(NULL,dlg.itemPicture , IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		bm.Attach(hBitmap);
		m_smallIL.Add(&bm,RGB(255,0,255));
		bm.DeleteObject();

		m_wndOutBarCtrl.SetItemImage(item_index,nR);
		m_wndOutBarCtrl.Invalidate();

		//remove old resource and new resource
// 		CString StoragePath = UserShapesPath+m_wndOutBarCtrl.GetFolderText(folder_index);
// 		if(PathFileExists(StoragePath) == FALSE)
// 			CreateDirectory(StoragePath, NULL);
// 
// 		CString sPictureName;
// 		int n = dlg.itemPicture.GetLength() - dlg.itemPicture.ReverseFind('\\')-1;
// 		sPictureName = StoragePath + "\\" + dlg.itemPicture.Right(n);
// 		CopyFile(dlg.itemPicture,sPictureName,TRUE);

		pShape->ImageFileName(dlg.itemPicture);
	}
}

void CShapesBar::OnChangeItemModel()
{
	CShape* pShape = (CShape*)m_wndOutBarCtrl.GetItemData(item_index);
	CShapeItem dlg(m_wndOutBarCtrl.GetFolderPathText(folder_index),pShape->Name(),
		pShape->ImageFileName(),pShape->ShapeFileName(),CShapeItem::MODEL);
	if(dlg.DoModal() == IDOK )
	{
		//remove old resource and new resource
// 		CString StoragePath = UserShapesPath+m_wndOutBarCtrl.GetFolderText(folder_index);
// 		if(PathFileExists(StoragePath) == FALSE)
// 			CreateDirectory(StoragePath, NULL);
// 
// 		CString sModelName;
// 		int n = dlg.itemModel.GetLength() - dlg.itemModel.ReverseFind('\\')-1;
// 		sModelName = StoragePath + "\\" + dlg.itemModel.Right(n);
// 		CopyFile(dlg.itemModel,sModelName,TRUE);

		pShape->ShapeFileName(dlg.itemModel);
		pShape->SetObjListValid(FALSE);		
	}
}

void CShapesBar::OnDeleteItem()
{
	m_wndOutBarCtrl.RemoveItem(item_index);
}