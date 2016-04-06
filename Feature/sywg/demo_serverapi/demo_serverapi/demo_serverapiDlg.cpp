#include "stdafx.h"
#include "demo_serverapi.h"
#include "demo_serverapiDlg.h"
#include "afxdialogex.h"
#include "..\\..\public\\Package.h"
#include "..\\..\public\\serverapi.h"
#include "ApiRsp.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CServerApi *pApi = NULL;
CTime g_time;
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

Cdemo_serverapiDlg::Cdemo_serverapiDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(Cdemo_serverapiDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cdemo_serverapiDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
    DDX_Control(pDX, IDC_BTN_STOP, m_Button);
    DDX_Control(pDX, IDC_LIST_STATUS, m_ListCtrl);
}

BEGIN_MESSAGE_MAP(Cdemo_serverapiDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BTN_SENDFILE, &Cdemo_serverapiDlg::OnBnClickedSendFile)
    ON_BN_CLICKED(IDC_BTN_STOP, &Cdemo_serverapiDlg::OnBnClickedStop)
    ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL Cdemo_serverapiDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    m_pRsp = NULL;
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
    pApi = CServerApi::CreateServerApi();
    m_pRsp = new CApiRsp;
    m_ListCtrl.InsertColumn(0,"信息",DT_LEFT,300);

    ERROR_INFO errInfo;
    pApi->Init(errInfo);
    pApi->RegisterSpi(m_pRsp);
    bool bRet = pApi->Connect("172.31.194.40", 2008, errInfo);
    if(bRet)
    {
        InsertInfo("连接到总线172.31.194.40: 2008成功！");
    }
    else 
    {
        InsertInfo("连接到总线172.31.194.40: 2008失败！");
    }

    bRet = pApi->AddService(9999, errInfo);
    InsertInfo("注册服务号9999到总线成功！");
    m_Progress.SetRange(0,100);

    return TRUE;
}

void Cdemo_serverapiDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void Cdemo_serverapiDlg::OnPaint()
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

HCURSOR Cdemo_serverapiDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


int nCancelHandle;
void Cdemo_serverapiDlg::OnBnClickedSendFile()
{
    ERROR_INFO errInfo;
    m_Progress.SetPos(0);
    m_Button.SetWindowText("发送文件serverapi1.pp");
    nCancelHandle = pApi->SendFileToService("serverapi1.pp", 111, errInfo);
    //pApi->SendFileToService("hello.txt",111,errInfo);
    /*CPackage *pPackage = pApi->CreatePackage();
    PACKAGEHEADER head;
    memset(&head,0,sizeof(PACKAGEHEADER));
    head.nFirstFlag = 1;
    head.nFuncType = 2;
    head.nNextFlag = 3;
    head.nPackageIndex = 4;
    head.nRetCode = 5;
    head.nTotalRecordNum = 6;
    pPackage->SetPackageHeader(&head);
    int iRec = pPackage->AppendBlankRecord(errInfo);
    string s ;
    vector <string> strvector;
    strvector.push_back("sample1");
    strvector.push_back("sample2");
    vector <int> intvec;
    vector <double>doublevec;
    intvec.push_back(1);
    intvec.push_back(2);
    doublevec.push_back(0.3);
    doublevec.push_back(0.4);
    pPackage->SetFieldValue(iRec,"name","zhangshan",errInfo);
    pPackage->SetFieldValue(iRec,"age",25,errInfo);
    pPackage->SetFieldValue(iRec,"double",0.2,errInfo);
    pPackage->SetFieldValue(iRec,"email","email@163.com",errInfo);
    pPackage->SetFieldValue(iRec,"strvectortest",strvector,errInfo);
    pPackage->SetFieldValue(iRec,"intvectortest",intvec,errInfo);
    pPackage->SetFieldValue(iRec,"doublevectortest",doublevec,errInfo);
    pApi->RegisterSpi(NULL);
    pApi->SendPackageToClient(pPackage,"abc",errInfo);

    pPackage->GetSerializeString(s,errInfo);

    pPackage->ParseFromString(s,errInfo);
    int nRec = pPackage->GetRecordCount();
    string a;
    int b;
    double c;
    strvector.clear();
    intvec.clear();
    for(int i = 0;i<nRec ;i++)
    {
    pPackage->GetFieldValue(i,"name",a,errInfo);
    pPackage->GetFieldValue(i,"age",b,errInfo);
    pPackage->GetFieldValue(i,"double",c,errInfo);

    pPackage->GetFieldValue(i,"email",a,errInfo);
    pPackage->GetFieldValue(i,"strvectortest",strvector,errInfo);
    pPackage->GetFieldValue(i,"intvectortest",intvec,errInfo);
    pPackage->GetFieldValue(i,"doublevectortest",doublevec,errInfo);
    }
    pApi->ReleasePackage(pPackage);
    pApi->SendFileToClient("serverapi1.pdb","abc",errInfo);*/
}



void Cdemo_serverapiDlg::OnBnClickedStop()
{
    PACKAGEHEADER head;
    memset(&head,0,sizeof(PACKAGEHEADER));
    head.nFuncType = REQTYPE;
    head.nFuncNo = CANCELEVENT;
    //head.nNextFlag = 0;
    head.nPackageIndex = 0;
    //head.nRetCode = 0;
    head.nTotalRecordNum = 1;
    head.nTotalPackageNum = 1;
    head.nEventHandle = nCancelHandle;
    ERROR_INFO ErrMsg;
    CPackage *pPackage = pApi->CreatePackage();
    pPackage->SetPackageHeader(&head);
    int iRec = pPackage->AppendBlankRecord(ErrMsg);
    pPackage->SetFieldValue(iRec,"field","取消事务",ErrMsg);
    PACKAGEHEADER *p = pPackage->GetPackageHeader();
    pApi->SendRequestToService(111,CANCELEVENT,pPackage,ErrMsg);
    pApi->ReleasePackage(pPackage);
}


void Cdemo_serverapiDlg::OnClose()
{
    CDialogEx::OnClose();
    if(m_pRsp != NULL) delete m_pRsp;
}

void Cdemo_serverapiDlg::InsertInfo(char *pInfo)
{
    CTime t = CTime::GetCurrentTime();
    CString s;
    s.Format("%02d:%02d:%02d  %s",t.GetHour(),t.GetMinute(),t.GetSecond(),pInfo);
    m_ListCtrl.InsertItem(0,s.GetBuffer(0));
}
