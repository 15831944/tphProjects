#include "stdafx.h"
#include "DatAnalyst.h"
#include "DialogArrowPointlist.h"

IMPLEMENT_DYNAMIC(CDialogArrowPointlist, CDialog)

CDialogArrowPointlist::CDialogArrowPointlist(CWnd* pParent)
: CDialog(CDialogArrowPointlist::IDD, pParent)
{
}

CDialogArrowPointlist::~CDialogArrowPointlist()
{
}

void CDialogArrowPointlist::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogArrowPointlist, CDialog)
END_MESSAGE_MAP()

