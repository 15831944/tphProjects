// DlgRetailProductsStocking.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgRetailProductsStocking.h"
#include ".\dlgretailproductsstocking.h"
#include "MFCExControl\SmartEdit.h"
#include "common\WinMsg.h"
#include "ProcesserDialog.h"
#include ".\TermPlanDoc.h"
const char fileheader[] = "ARCTerm Product Stocking File";

// CDlgRetailProductsStocking dialog

IMPLEMENT_DYNAMIC(CDlgRetailProductsStocking, CXTResizeDialog)
CDlgRetailProductsStocking::CDlgRetailProductsStocking(CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgRetailProductsStocking::IDD, pParent)
{
	m_bResize=false;
	m_nSelIdx=-1;
	m_pParentView=pParent;
	m_ProductStockingList.ReadData(-1);
}

CDlgRetailProductsStocking::~CDlgRetailProductsStocking()
{
}

//void CDlgRetailProductsStocking::DoDataExchange(CDataExchange* pDX)
//{
//	CXTResizeDialog::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_LIST_PRODUCT, m_listboxProduct);
//	DDX_Control(pDX, IDC_LIST_STOCKINGDETAIL, m_listctrlDetail);
//}
BOOL CDlgRetailProductsStocking::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	SetResize(IDC_STATIC,SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_LIST_PRODUCT,SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_LIST_STOCKINGDETAIL,SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVEAS,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_LOAD,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	m_toolbarProduct.GetToolBarCtrl().EnableButton(ID_PRODUCT_ADD,TRUE);
	m_toolbarProduct.GetToolBarCtrl().EnableButton(ID_PRODUCT_DEL,FALSE);
	m_toolbarProduct.GetToolBarCtrl().EnableButton(ID_PRODUCT_EDIT,FALSE);

	m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_ADD,FALSE);
	m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_DEL,FALSE);
	m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_EDIT,FALSE);

	CRect rc;
	m_listboxProduct.GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.top-=28;
	rc.bottom=rc.top+22;
	rc.left+=4;
	m_toolbarProduct.MoveWindow(rc);
	m_toolbarProduct.ShowWindow(SW_SHOW);

	m_listctrlDetail.GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.top-=28;
	rc.left+=4;
	rc.bottom=rc.top+22;
	m_toolbarDetail.MoveWindow(rc);
	m_toolbarDetail.ShowWindow(SW_SHOW);
	m_bResize=true;

	InitStockingDetailList();
	SetProductNameList();

	return true;
}

BEGIN_MESSAGE_MAP(CDlgRetailProductsStocking, CXTResizeDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_PRODUCT_ADD,AddProduct)
	ON_COMMAND(ID_PRODUCT_EDIT,OnEditProduct)
	ON_COMMAND(ID_PRODUCT_DEL,OnDelProduct)
	ON_COMMAND(ID_STOCKING_ADD,AddStockingDetail)
	ON_COMMAND(ID_STOCKING_DEL,OnDelDetail)
	ON_LBN_DBLCLK(IDC_LIST_PRODUCT, OnDblclkListProductName)
	ON_MESSAGE(WM_END_EDIT,OnEndEdit)
	ON_LBN_SELCHANGE(IDC_LIST_PRODUCT, OnLbnSelchangeListProduct)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_STOCKINGDETAIL, OnNMDblclkListStockingdetail)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_STOCKINGDETAIL, OnLvnEndlabeleditListStockingdetail)
	ON_BN_CLICKED(IDC_BUTTON_SAVEAS, OnBnClickedButtonSaveas)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, OnBnClickedButtonLoad)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_STOCKINGDETAIL, OnLvnItemchangedListStockingdetail)
END_MESSAGE_MAP()


// CDlgRetailProductsStocking message handlers

int CDlgRetailProductsStocking::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	if (!m_toolbarProduct.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_toolbarProduct.LoadToolBar(IDR_ADDDELEDIT_PRODUCT))
	{
		return -1;
	}
	if (!m_toolbarDetail.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_toolbarDetail.LoadToolBar(IDR_ADDDELEDIT_PRODUCT_STOCKING))
	{
		return -1;
	}


	return 0;
}

void CDlgRetailProductsStocking::OnSize(UINT nType, int cx, int cy)
{
	CXTResizeDialog::OnSize(nType, cx, cy);

	if (m_bResize)
	{
		CRect rc;
		m_listboxProduct.GetWindowRect(&rc);
		ScreenToClient(&rc);
		rc.top-=28;
		rc.bottom=rc.top+22;
		rc.left+=4;
		m_toolbarProduct.MoveWindow(rc);
		m_toolbarProduct.ShowWindow(SW_SHOW);

		m_listctrlDetail.GetWindowRect(&rc);
		ScreenToClient(&rc);
		rc.top-=28;
		rc.bottom=rc.top+22;
		rc.left+=4;
		m_toolbarDetail.MoveWindow(rc);
		m_toolbarDetail.ShowWindow(SW_SHOW);
	}

	// TODO: Add your message handler code here
}
void CDlgRetailProductsStocking::OnDblclkListProductName()
{
	m_nSelIdx = m_listboxProduct.GetCurSel();
	if( m_nSelIdx == LB_ERR )
		return;

	EditItem( m_nSelIdx );	
}
LONG CDlgRetailProductsStocking::OnEndEdit( WPARAM p_wParam, LPARAM p_lParam )
{
	if( p_lParam != IDC_IPEDIT )
		return 0;

	if( m_nSelIdx == LB_ERR )
		return 0;

	CString csStr( (LPTSTR)p_wParam );
	if( csStr.IsEmpty() )
	{
		MessageBox( "Empty String", "Error", MB_OK|MB_ICONWARNING );
	}
	else
	{
		// check if dup
		for( int i=0; i<m_listboxProduct.GetCount(); i++ )
		{
			if (i!=m_nSelIdx)
			{
				CString csTemp;
				m_listboxProduct.GetText( i, csTemp );
				if( csTemp.CompareNoCase( csStr ) == 0 && csStr!="Please Edit")
				{
					MessageBox( "Duplicated Name", "Error", MB_OK|MB_ICONWARNING );
					break;
				}
			}
		}

		if( i == m_listboxProduct.GetCount() )
		{
			m_ProductStockingList.GetItem(m_nSelIdx)->SetProductName((LPTSTR)p_wParam );
		}
	}
	SetProductNameList();
 	m_listboxProduct.SetCurSel(m_nSelIdx);
 	SetStockingDetailList();
	return 0;
}
void CDlgRetailProductsStocking::InitStockingDetailList()
{
	DWORD dwStyle = m_listctrlDetail.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listctrlDetail.SetExtendedStyle(dwStyle);

	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.pszText = _T("Stocked at");
	lvc.cx = 150;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &strList;
	m_listctrlDetail.InsertColumn(0, &lvc);

	lvc.pszText = _T("Inventory");
	lvc.fmt = LVCFMT_NUMBER;
	lvc.cx = 150;
	m_listctrlDetail.InsertColumn(1, &lvc);
}
void CDlgRetailProductsStocking::AddProduct()
{
	ProductStockingSpecification *item=new ProductStockingSpecification;
	int nIndex=m_listboxProduct.AddString(item->GetProductName());
	m_listboxProduct.SetItemData(nIndex,(DWORD)item);
	m_ProductStockingList.AddItem(item);
	m_listboxProduct.SetCurSel(nIndex);
	m_nSelIdx=nIndex;
}
void CDlgRetailProductsStocking::AddStockingDetail()
{
	if (m_nSelIdx<0)
	{
		return;
	}
	ProductStockingItem *item=new ProductStockingItem;
	m_ProductStockingList.GetItem(m_nSelIdx)->AddItem(item);
	SetStockingDetailList();
	m_listctrlDetail.SetCurSel(m_ProductStockingList.GetItem(m_nSelIdx)->GetItemCount()-1);
}
void CDlgRetailProductsStocking::OnEditProduct()
{
	OnDblclkListProductName();
}
void CDlgRetailProductsStocking::OnDelProduct()
{
	if (m_nSelIdx<0)
	{
		return;
	}
	m_ProductStockingList.DeleteItem(m_nSelIdx);
	SetProductNameList();
	if (m_nSelIdx<m_ProductStockingList.GetItemCount())
	{
		m_listboxProduct.SetCurSel(m_nSelIdx);
	}else if(m_ProductStockingList.GetItemCount()>0)
	{
		m_nSelIdx--;
		m_listboxProduct.SetCurSel(m_nSelIdx);
		
	}else
	{
		m_nSelIdx=-1;

		m_toolbarProduct.GetToolBarCtrl().EnableButton(ID_PRODUCT_ADD,TRUE);
		m_toolbarProduct.GetToolBarCtrl().EnableButton(ID_PRODUCT_EDIT,FALSE);
		m_toolbarProduct.GetToolBarCtrl().EnableButton(ID_PRODUCT_DEL,FALSE);

		m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_ADD,FALSE);
		m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_DEL,FALSE);
		m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_EDIT,FALSE);
	}
	SetStockingDetailList();
}
void CDlgRetailProductsStocking::OnDelDetail()
{
	if (m_nSelIdx<0)
	{
		return;
	}
	int nIndex=m_listctrlDetail.GetCurSel();
	if (nIndex<0)
	{
		return;
	}
	m_ProductStockingList.GetItem(m_nSelIdx)->DeleteItem(nIndex);
	SetStockingDetailList();
	if (nIndex<m_listctrlDetail.GetItemCount())
	{
		m_listctrlDetail.SetCurSel(nIndex);
		int nIndex=m_listctrlDetail.GetCurSel();
		m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_ADD,TRUE);
		m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_DEL,TRUE);
		m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_EDIT,FALSE);
	}else if (m_listctrlDetail.GetItemCount()>0)
	{
		nIndex--;
		m_listctrlDetail.SetCurSel(nIndex);
		m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_ADD,TRUE);
		m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_DEL,TRUE);
		m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_EDIT,FALSE);
	}else
	{
		m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_ADD,TRUE);
		m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_DEL,FALSE);
		m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_EDIT,FALSE);
	}

}
void CDlgRetailProductsStocking::EditItem( int _nIdx )
{
	CRect rectItem;
	CRect rectLB;
	m_listboxProduct.GetWindowRect( &rectLB );
	ScreenToClient( &rectLB );
	m_listboxProduct.GetItemRect( _nIdx, &rectItem );

	rectItem.OffsetRect( rectLB.left+2, rectLB.top+1 );

	rectItem.right += 1;
	rectItem.bottom += 4;

	CString csItemText;
	m_listboxProduct.GetText( _nIdx, csItemText );
	CSmartEdit* pEdit = new CSmartEdit( csItemText );
	DWORD dwStyle = ES_LEFT | WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL;
	pEdit->Create( dwStyle, rectItem, this, IDC_IPEDIT );	
}
void CDlgRetailProductsStocking::SetProductNameList()
{
	int nCount=m_listboxProduct.GetCount();
	for(int i=0;i<nCount;i++)
		m_listboxProduct.DeleteString(0);

	for (int i=0;i<m_ProductStockingList.GetItemCount();i++)
	{
		m_listboxProduct.AddString(m_ProductStockingList.GetItem(i)->GetProductName());
	}
}
void CDlgRetailProductsStocking::SetStockingDetailList()
{
	m_listctrlDetail.DeleteAllItems();
	if (m_nSelIdx<0)
	{
		return;
	}
	ProductStockingSpecification *curProduct=m_ProductStockingList.GetItem(m_nSelIdx);
	if(curProduct==NULL)
		return;
    for (int i=0;i<curProduct->GetItemCount();i++)
    {
		ProductStockingItem *item=curProduct->GetItem(i);
		m_listctrlDetail.InsertItem(i,_T(""));
		CString strShopName=item->GetShopName();		
		if (strShopName==_T(""))
		{
			strShopName=_T("Please Edit");
		}
		m_listctrlDetail.SetItemText(i,0,strShopName);
		CString strInventory;
		strInventory.Format(_T("%.2f"),item->GetInventory());
		m_listctrlDetail.SetItemText(i,1,strInventory);
		m_listctrlDetail.SetItemData(i,(DWORD)item);
    }
}
void CDlgRetailProductsStocking::DoDataExchange(CDataExchange* pDX)
{
	// TODO: Add your specialized code here and/or call the base class

	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PRODUCT, m_listboxProduct);
	DDX_Control(pDX, IDC_LIST_STOCKINGDETAIL, m_listctrlDetail);
}

void CDlgRetailProductsStocking::OnLbnSelchangeListProduct()
{
	// TODO: Add your control notification handler code here
	m_nSelIdx=m_listboxProduct.GetCurSel();
	if (m_nSelIdx<0)
	{
		m_toolbarProduct.GetToolBarCtrl().EnableButton(ID_PRODUCT_ADD,TRUE);
		m_toolbarProduct.GetToolBarCtrl().EnableButton(ID_PRODUCT_EDIT,FALSE);
		m_toolbarProduct.GetToolBarCtrl().EnableButton(ID_PRODUCT_DEL,FALSE);

		m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_ADD,FALSE);
		m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_DEL,FALSE);
		m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_EDIT,FALSE);

		return;
	}
	m_toolbarProduct.GetToolBarCtrl().EnableButton(ID_PRODUCT_ADD,TRUE);
	m_toolbarProduct.GetToolBarCtrl().EnableButton(ID_PRODUCT_EDIT,TRUE);
	m_toolbarProduct.GetToolBarCtrl().EnableButton(ID_PRODUCT_DEL,TRUE);

	m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_ADD,TRUE);
	m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_DEL,FALSE);
	m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_EDIT,FALSE);
	SetStockingDetailList();
}

void CDlgRetailProductsStocking::OnNMDblclkListStockingdetail(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if (m_pParentView==NULL)
	{
		return;
	}
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	int iItem = pnmv->iItem;
	int iColumn = pnmv->iSubItem;

	if(iColumn ==0 && iItem < m_listctrlDetail.GetItemCount() && iItem >=0 )
	{
		CTermPlanDoc *pDoc=(CTermPlanDoc *)((CView *)m_pParentView)->GetDocument();
		CProcesserDialog dlg((InputTerminal*)&(pDoc->GetTerminal()));
		dlg.SetType(RetailProc);
		if (dlg.DoModal()==IDOK)
		{
			ProductStockingItem *item=(ProductStockingItem *)m_listctrlDetail.GetItemData(iItem);
			if (item==NULL)
			{
				return;
			}
			ProcessorID selRetailID;
			dlg.GetProcessorID(selRetailID);
			CString strShopName=selRetailID.GetIDString();
			for (int i=0;i<m_listctrlDetail.GetItemCount();i++)
			{
				if(m_listctrlDetail.GetItemText(i,0)==strShopName)
				{
					MessageBox(_T("Retail Already Exist."));
					SetStockingDetailList();
					return;
				}

			}
			item->SetShopName(strShopName);
		}
	}
	SetStockingDetailList();
}

void CDlgRetailProductsStocking::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	m_ProductStockingList.SaveData(-1);
	CXTResizeDialog::OnOK();
}

void CDlgRetailProductsStocking::OnLvnEndlabeleditListStockingdetail(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	ProductStockingItem *item=(ProductStockingItem *)m_listctrlDetail.GetItemData(pDispInfo->item.iItem);
	if (item==NULL)
	{
		return;
	}
	item->SetInventory(atof(pDispInfo->item.pszText));
	SetStockingDetailList();
}

void CDlgRetailProductsStocking::OnBnClickedButtonSaveas()
{
	// TODO: Add your control notification handler code here
	CFileDialog filedlg( FALSE,"rps", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Product Stocking File(*.rps)|*.rps;*.fpr|All type (*.*)|*.*|", NULL );
	if(filedlg.DoModal()!=IDOK)
		return;

	CString csFileName=filedlg.GetPathName();

	// file title, header
	ArctermFile file;
	file.openFile(csFileName,WRITE);
	file.writeField("ARCTerm Product Stocking File");
	file.writeLine();

	CString strData;
	for (int i=0;i<m_ProductStockingList.GetItemCount();i++)
	{
		ProductStockingSpecification *item=m_ProductStockingList.GetItem(i);
		strData=_T("Product Name:")+item->GetProductName();
		file.writeField(strData);
		file.writeLine();
		for (int j=0;j<item->GetItemCount();j++)
		{			
			strData=_T("Stock at:")+item->GetItem(j)->GetShopName();
			file.writeField(strData);
			file.writeLine();
			strData.Format("Inventory:%.2f",item->GetItem(j)->GetInventory());
			file.writeField(strData);
			file.writeLine();
		}		
	}
	file.endFile();
}

void CDlgRetailProductsStocking::OnBnClickedButtonLoad()
{
	// TODO: Add your control notification handler code here
	CFileDialog filedlg( TRUE,"rps", NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_SHAREAWARE , \
		"FlightGroup File (*.rps)|*.rps;*.FRP|All type (*.*)|*.*|", NULL );
	if(filedlg.DoModal()!=IDOK)
		return;
	CString csFileName=filedlg.GetPathName();

	if( ReadFile( csFileName ) )
	{
		// clear
		m_ProductStockingList.DeleteData();
		ReadStockingList();
		SetProductNameList();
	}
}

bool CDlgRetailProductsStocking::ReadFile(const CString &strFileName)
{
	m_vFileData.RemoveAll();

	CStdioFile file;
	if(!file.Open( strFileName, CFile::modeRead ) )
	{
		CString strMsg;
		strMsg.Format("Can not open file:\r\n%s", strFileName );
		MessageBox( strMsg, "ERROR", MB_OK|MB_ICONERROR );
		return false;
	}

	// read all line to file
	CString strLineText;
	while( file.ReadString( strLineText )  )
	{
		m_vFileData.Add( strLineText );
	}

	// if is arcterm file, remove the first three line and the last two line of the file
	if( m_vFileData.GetAt(1) == CString(fileheader) )	// is arcterm file with header
	{
		//get file version
		CString strFileNameAndVersion = m_vFileData.GetAt(0);
		int nPos = strFileNameAndVersion.ReverseFind(',');
		CString strFileVersion = strFileNameAndVersion.Right(strFileNameAndVersion.GetLength() - nPos - 1);
		//m_dFileVersion = atof(strFileVersion);

				// remove first three line
		m_vFileData.RemoveAt( 0,2 );
		// remove last two line
		m_vFileData.RemoveAt( m_vFileData.GetSize()-2,2 );
	}

	file.Close();
	return true;
}
void CDlgRetailProductsStocking::ReadStockingList()
{
	CString strData;
	int	flightNum = m_vFileData.GetSize();
	CStringArray strArray;	

	if( flightNum == 0)
	{
		return ;
	}

	strData = m_vFileData.GetAt(0);
	if(strData.IsEmpty())
		return ;

// 	int nColNum = GetColNumFromStr(strEditText,",",strArray);
// 
// 	AddColumn(nColNum);
    ProductStockingSpecification *pProduct=NULL;
	ProductStockingItem *pItem=NULL;
	CString strLeft,strRight;
	for (int i = 0; i < flightNum; i++)
	{	
		strData = m_vFileData.GetAt(i);
		int nPos=strData.Find(_T(":"),0);
		if (nPos!=-1)
		{
			strLeft=strData.Left(nPos);
			strRight=_T("");
			strRight=strData.Right(strData.GetLength()-nPos-1);
			if (strLeft==_T("Product Name"))
			{				
				pProduct=new ProductStockingSpecification;
				pProduct->SetProductName(strRight);	
				m_ProductStockingList.AddItem(pProduct);
			}else if (strLeft==_T("Stock at"))
			{
                pItem=new ProductStockingItem;
				pItem->SetShopName(strRight);
				if (pProduct!=NULL)
				{
					pProduct->AddItem(pItem);
				}
			}else if (strLeft==_T("Inventory"))
			{
                if (pItem!=NULL)
                {
					pItem->SetInventory(atof(strRight));
                }
			}
		}
		
	}

	return ;
}
void CDlgRetailProductsStocking::OnLvnItemchangedListStockingdetail(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	m_nSelIdx=m_listboxProduct.GetCurSel();
	if (m_nSelIdx<0)
	{
		m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_ADD,FALSE);
		m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_DEL,FALSE);
		m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_EDIT,FALSE);
	}
	int nIndex=m_listctrlDetail.GetCurSel();
	if (m_listctrlDetail.GetCurSel()<0)
	{
		m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_ADD,TRUE);
		m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_DEL,FALSE);
		m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_EDIT,FALSE);	
	}else
	{
		m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_ADD,TRUE);
		m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_DEL,TRUE);
		m_toolbarDetail.GetToolBarCtrl().EnableButton(ID_STOCKING_EDIT,FALSE);	
	}

}
