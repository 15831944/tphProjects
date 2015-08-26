#include "stdafx.h"
#include "DatAnalyst.h"
#include "DialogMultiDatProcess.h"

IMPLEMENT_DYNAMIC(CDialogMultiDatProcess, CDialog)

CDialogMultiDatProcess::CDialogMultiDatProcess(CWnd* pParent /*=NULL*/)
: CDialog(CDialogMultiDatProcess::IDD, pParent)
{

}

CDialogMultiDatProcess::~CDialogMultiDatProcess()
{
}

void CDialogMultiDatProcess::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogMultiDatProcess, CDialog)
    ON_BN_CLICKED(IDC_BUTTON1, &CDialogMultiDatProcess::OnBnClickedButton1)
END_MESSAGE_MAP()

void CDialogMultiDatProcess::OnBnClickedButton1()
{
    MessageBox("this is multi dat processor");
}
