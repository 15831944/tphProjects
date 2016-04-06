#include "stdafx.h"
#include "demo_clientapi.h"
#include "demo_clientapiDlg.h"
#include "afxdialogex.h"
#include "..\\..\\public\\package.h"
#include "..\\..\\public\\clientapi.h"
#include "ApiRsp.h"
#include "requestid.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CClientApi *pApi = NULL;
#define SEVICENO 9999
class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();
    enum { IDD = IDD_ABOUTBOX };
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

Cdemo_clientapiDlg::Cdemo_clientapiDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(Cdemo_clientapiDlg::IDD, pParent)
    , m_Username(_T(""))
    , m_Password(_T(""))
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cdemo_clientapiDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
    DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
    DDX_Control(pDX, IDC_EDIT_USER, m_EditUser);
    DDX_Control(pDX, IDC_EDIT_PASSWORD, m_EditPassword);
    DDX_Text(pDX, IDC_EDIT_USER, m_Username);
    DDX_Text(pDX, IDC_EDIT_PASSWORD, m_Password);
    DDX_Control(pDX, IDC_LIST2, m_ListQry);
    DDX_Control(pDX, IDC_STATIC_QRY, m_QryName);
    DDX_Control(pDX, IDC_BUTTON_LOGIN, m_LoginButton);
    DDX_Control(pDX, IDC_BUTTON1, m_SendFile);
    DDX_Control(pDX, IDC_BUTTON_QRYMATCH, m_QryMatch);
    DDX_Control(pDX, IDC_BUTTON_QRYPOSI, m_QryPosi);
    DDX_Control(pDX, IDC_BUTTON_QRYFUND, m_QryFund);
    DDX_Control(pDX, IDC_BUTTON_CANCEL, m_ButtonCancel);
    DDX_Control(pDX, IDC_BUTTON_SETTLEMENT, m_Settlement);

    DDX_Control(pDX, IDOK, m_Exit);
    DDX_Control(pDX, IDC_EDIT_PASSWORD2, m_EditDate);
}

BEGIN_MESSAGE_MAP(Cdemo_clientapiDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON1, &Cdemo_clientapiDlg::OnBnClickedButton1)
    ON_MESSAGE(WM_ADDINFO,OnShowMsg)
    ON_BN_CLICKED(IDC_BUTTON2, &Cdemo_clientapiDlg::OnBnClickedButton2)
    ON_WM_CLOSE()
    //ON_BN_CLICKED(IDC_BUTTON3, &Cdemo_clientapiDlg::OnBnClickedButton3)
    ON_BN_CLICKED(IDC_BUTTON_LOGIN, &Cdemo_clientapiDlg::OnBnClickedButtonLogin)
    ON_BN_CLICKED(IDC_BUTTON_QRYMATCH, &Cdemo_clientapiDlg::OnBnClickedButtonQrymatch)
    ON_BN_CLICKED(IDC_BUTTON_QRYPOSI, &Cdemo_clientapiDlg::OnBnClickedButtonQryposi)
    ON_BN_CLICKED(IDC_BUTTON_QRYFUND, &Cdemo_clientapiDlg::OnBnClickedButtonQryfund)
    ON_BN_CLICKED(IDC_BUTTON_CANCEL, &Cdemo_clientapiDlg::OnBnClickedButtonCancel)
    ON_BN_CLICKED(IDC_BUTTON_SETTLEMENT, &Cdemo_clientapiDlg::OnBnClickedButtonSettlement)
    ON_BN_CLICKED(IDOK, &Cdemo_clientapiDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDC_BUTTON4, &Cdemo_clientapiDlg::OnBnClickedButton4)
END_MESSAGE_MAP()

BOOL Cdemo_clientapiDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    m_ListQry.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
    bitmap.LoadBitmap(IDB_BITMAP2);
    m_LoginButton.SetBitmap((HBITMAP)bitmap.GetSafeHandle());

    bitmap_SendFile.LoadBitmap(IDB_BITMAP_SENDFILE);
    m_SendFile.SetBitmap((HBITMAP)bitmap_SendFile.GetSafeHandle());

    bitmap_QryMatch.LoadBitmap(IDB_BITMAP_QRYMATCH);
    m_QryMatch.SetBitmap((HBITMAP)bitmap_QryMatch.GetSafeHandle());

    bitmap_QryPosi.LoadBitmap(IDB_BITMAP_QRYPOSI);
    m_QryPosi.SetBitmap((HBITMAP)bitmap_QryPosi.GetSafeHandle());

    bitmap_QryFund.LoadBitmap(IDB_BITMAP_QRYFUND);
    m_QryFund.SetBitmap((HBITMAP)bitmap_QryFund.GetSafeHandle());

    bitmap_Cancel.LoadBitmap(IDB_BITMAP_CANCEL);
    m_ButtonCancel.SetBitmap((HBITMAP)bitmap_Cancel.GetSafeHandle());

    bitmap_Exit.LoadBitmap(IDB_BITMAP_EXIT);
    m_Exit.SetBitmap((HBITMAP)bitmap_Exit.GetSafeHandle());

    bitmap_Settlement.LoadBitmap(IDB_BITMAP_SETTLEMENT);
    m_Settlement.SetBitmap((HBITMAP)bitmap_Settlement.GetSafeHandle());

    bitmap_List.LoadBitmap(IDB_BITMAP_LIST);
    m_ImageList.Create(16,19,ILC_COLOR24,5,1);
    m_ImageList.Add(&bitmap_List,RGB(0,0,0));
    m_ListCtrl.SetImageList(&m_ImageList,LVSIL_SMALL);

    bitmap_ListQry.LoadBitmap(IDB_BITMAP_QRY);
    m_ImageList_Qry.Create(16,19,ILC_COLOR24,5,1);
    m_ImageList_Qry.Add(&bitmap_ListQry,RGB(0,0,0));
    m_ListQry.SetImageList(&m_ImageList_Qry,LVSIL_SMALL);

    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);
    m_ListCtrl.InsertColumn(0,"信息",DT_LEFT,300);
    pApi = CClientApi::CreateClientApi();
    m_pRsp = new CApiRsp;
    m_EditUser.SetWindowText("test");
    m_EditPassword.SetWindowText("88888888");
    ERROR_INFO errInfo;
    pApi->Init(errInfo);
    InsertInfo("初始化......");
    pApi->RegisterSpi(m_pRsp);
    bool bRet = pApi->Connect("172.31.194.40",2008,errInfo);
    if(bRet == true)
        InsertInfo("连接到总线172.31.194.40:2008成功!");
    else
        InsertInfo("连接到总线172.31.194.40:2008失败!");

    m_Progress.SetRange(0,100);
    //bRet = pApi->AddService(111,errInfo);
    return TRUE;
}

void Cdemo_clientapiDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

void Cdemo_clientapiDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this);
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

HCURSOR Cdemo_clientapiDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


int g_nEventhandle = 100;
void Cdemo_clientapiDlg::OnBnClickedButton1()
{
    CFileFind finder;
    BOOL bWorking = finder.FindFile("./输入/*.*");
    ERROR_INFO errInfo;
    while(bWorking)
    {
        bWorking = finder.FindNextFile();
        if(finder.IsDots()) continue;
        CString name = finder.GetFileName();
        CString TransFile = "./输入/";
        TransFile += name;
        CString strPrompt = "发送文件";
        strPrompt += TransFile;
        InsertInfo(strPrompt.GetBuffer(0));
        g_nEventhandle = pApi->SendFileToService(TransFile.GetBuffer(0),SEVICENO,errInfo);
        if(g_nEventhandle < 0)
        {
            InsertInfo(errInfo.strErrMsg);
        }
    }

    /*InsertInfo("./输入/输入成交.dbf");

    g_nEventhandle = pApi->SendFileToService("./输入/输入成交.dbf",111,errInfo);
    if(g_nEventhandle < 0)
    {
    InsertInfo(errInfo.strErrMsg);
    }*/

    /*g_nEventhandle = pApi->SendFileToService("./输入/输入成交.dbf",111,errInfo);
    if(g_nEventhandle < 0)
    {
    InsertInfo(errInfo.strErrMsg);
    }
    */
    // 
}

LRESULT Cdemo_clientapiDlg::OnShowMsg(WPARAM wpara,LPARAM lpara)
{
    int n = wpara;
    m_Progress.SetPos(n);
    return 0;
}


void Cdemo_clientapiDlg::OnBnClickedButton2()
{//取消文件发送
    ERROR_INFO ErrMsg;
    PACKAGEHEADER head;

    memset(&head,0,sizeof(PACKAGEHEADER));
    head.nEventHandle = g_nEventhandle;

    CPackage *pPackage = pApi->CreatePackage();
    int iRec = pPackage->AppendBlankRecord(ErrMsg);
    pPackage->SetPackageHeader(&head);
    pPackage->SetFieldValue(iRec,"field","取消文件发送",ErrMsg);
    pApi->SendRequestToService(SEVICENO,CANCELEVENT,pPackage,ErrMsg);
    pApi->ReleasePackage(pPackage);
}


void Cdemo_clientapiDlg::OnClose()
{
    CDialogEx::OnClose();
    if(m_pRsp != NULL) delete m_pRsp;
}


void Cdemo_clientapiDlg::OnBnClickedButton3()
{
    ERROR_INFO ErrMsg;
    CPackage *pPackage = pApi->CreatePackage();
    int iRec = pPackage->AppendBlankRecord(ErrMsg);
    pPackage->SetFieldValue(iRec,"field","查询请求",ErrMsg);
    pApi->SendRequestToService(SEVICENO,100,pPackage,ErrMsg);
    pApi->ReleasePackage(pPackage);
}


void Cdemo_clientapiDlg::OnBnClickedButton6()
{
}


void Cdemo_clientapiDlg::OnBnClickedButtonLogin()
{
    InsertInfo("登陆请求!");
    ERROR_INFO ErrMsg;
    m_EditUser.GetWindowText(m_Username);
    m_EditPassword.GetWindowText(m_Password);

    bool bRet = pApi->Login(m_Username.GetBuffer(0),m_Password.GetBuffer(0),ErrMsg);
    if(bRet == false)
    {
        CString s;
        s.Format("登陆错误:%s!",ErrMsg.strErrMsg);
    }
}


void Cdemo_clientapiDlg::OnBnClickedButtonQrymatch()
{
    InsertInfo("发送查询成交请求!");
    ERROR_INFO ErrMsg;
    CPackage *pPackage = pApi->CreatePackage();
    int iRec = pPackage->AppendBlankRecord(ErrMsg);
    pPackage->SetFieldValue(iRec,"field","查询成交",ErrMsg);
    int nRet = pApi->SendRequestToService(SEVICENO,QRY_MATCH,pPackage,ErrMsg);
    if(nRet < 0)
    {
        InsertInfo(ErrMsg.strErrMsg);
    }
    pApi->ReleasePackage(pPackage);
}


void Cdemo_clientapiDlg::OnBnClickedButtonQryposi()
{
    InsertInfo("发送查询持仓请求!");
    ERROR_INFO ErrMsg;
    CPackage *pPackage = pApi->CreatePackage();
    int iRec = pPackage->AppendBlankRecord(ErrMsg);
    pPackage->SetFieldValue(iRec,"field","查询持仓",ErrMsg);
    int nRet = pApi->SendRequestToService(SEVICENO,QRY_POSI,pPackage,ErrMsg);
    if(nRet < 0)
    {
        InsertInfo(ErrMsg.strErrMsg);
    }
    pApi->ReleasePackage(pPackage);
}


void Cdemo_clientapiDlg::OnBnClickedButtonQryfund()
{
    InsertInfo("发送查询资金请求!");
    ERROR_INFO ErrMsg;
    CPackage *pPackage = pApi->CreatePackage();
    int iRec = pPackage->AppendBlankRecord(ErrMsg);
    pPackage->SetFieldValue(iRec,"field","查询资金",ErrMsg);
    int nRet = pApi->SendRequestToService(SEVICENO,QRY_FUND,pPackage,ErrMsg);
    if(nRet < 0)
    {
        InsertInfo(ErrMsg.strErrMsg);
    }
    pApi->ReleasePackage(pPackage);

}
void Cdemo_clientapiDlg::InsertInfo(char *pInfo)
{
    CTime t = CTime::GetCurrentTime();
    CString s;
    s.Format("%02d:%02d:%02d  %s",t.GetHour(),t.GetMinute(),t.GetSecond(),pInfo);
    m_ListCtrl.InsertItem(0,s.GetBuffer(0));
}

void Cdemo_clientapiDlg::ClearList()
{
    m_ListQry.DeleteAllItems();
    while(m_ListQry.GetHeaderCtrl()->GetItemCount()>0)
        m_ListQry.DeleteColumn(0);
}
void Cdemo_clientapiDlg::SetListHeader(vector<FIELDINFO> * pFieldInfo)
{
    ClearList();
    int nIndex = 0;
    vector <FIELDINFO>::iterator iter;
    for(iter = pFieldInfo->begin(); iter != pFieldInfo->end();iter++)
    {
        FIELDINFO *pItem = &(*iter);
        int nWidth = 100;
        if(pFieldInfo->size() == 3 && nIndex == 1)nWidth = 200; 
        m_ListQry.InsertColumn(nIndex,pItem->FieldDefine.FieldName,DT_LEFT,nWidth);
        nIndex++;
    }
}
void Cdemo_clientapiDlg::SetQryTitle(char *title)
{
    m_QryName.SetWindowText(title);
}

void Cdemo_clientapiDlg::AddFieldValue(int nCol,char *strvalue)
{
    int nItem;
    if(nCol == 0)
    {
        nItem =m_ListQry.InsertItem(m_ListQry.GetItemCount(),strvalue);
    }
    else 
    {
        m_ListQry.SetItemText(m_ListQry.GetItemCount()-1,nCol,strvalue);
    }
}


void Cdemo_clientapiDlg::OnBnClickedButtonCancel()
{
    ERROR_INFO ErrMsg;
    PACKAGEHEADER head;

    memset(&head,0,sizeof(PACKAGEHEADER));
    head.nEventHandle = g_nEventhandle;

    CPackage *pPackage = pApi->CreatePackage();
    int iRec = pPackage->AppendBlankRecord(ErrMsg);
    pPackage->SetPackageHeader(&head);
    pPackage->SetFieldValue(iRec,"field","取消文件发送",ErrMsg);
    pApi->SendRequestToService(SEVICENO,CANCELEVENT,pPackage,ErrMsg);
    pApi->ReleasePackage(pPackage);

}

//结算请求
void Cdemo_clientapiDlg::OnBnClickedButtonSettlement()
{
    CString strDate ;
    m_EditDate.GetWindowTextA(strDate);
    InsertInfo("发送结算请求!");
    ERROR_INFO ErrMsg;
    CPackage *pPackage = pApi->CreatePackage();
    int iRec = pPackage->AppendBlankRecord(ErrMsg);
    pPackage->SetFieldValue(iRec,"field","结算请求",ErrMsg);
    pPackage->SetFieldValue(iRec,"cleardate",strDate.GetBuffer(0),ErrMsg);

    int nRet = pApi->SendRequestToService(SEVICENO,START_SETTLEMENT,pPackage,ErrMsg);
    if(nRet < 0)
    {
        InsertInfo(ErrMsg.strErrMsg);
    }
    pApi->ReleasePackage(pPackage);
}


void Cdemo_clientapiDlg::OnBnClickedOk()
{
    pApi->Release();
    CDialogEx::OnOK();
}


void Cdemo_clientapiDlg::OnBnClickedButton4()
{
    ERROR_INFO ErrMsg;
    CPackage *pPackage = pApi->CreatePackage();
    int iRec = pPackage->AppendBlankRecord(ErrMsg);
    pPackage->SetFieldValue(iRec,"field","aa",ErrMsg);

    int nRet = pApi->SendRequestToService(SEVICENO,6,pPackage,ErrMsg);
    if(nRet < 0)
    {
        InsertInfo(ErrMsg.strErrMsg);
    }
    pApi->ReleasePackage(pPackage);
}
