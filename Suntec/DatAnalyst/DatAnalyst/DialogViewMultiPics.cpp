#include "stdafx.h"
#include "DatAnalyst.h"
#include "DialogViewMultiPics.h"

IMPLEMENT_DYNAMIC(DialogViewMultiPics, CDialog)

DialogViewMultiPics::DialogViewMultiPics(CWnd* pParent)
: CDialog(DialogViewMultiPics::IDD, pParent)
{
}

DialogViewMultiPics::~DialogViewMultiPics()
{
}

void DialogViewMultiPics::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DialogViewMultiPics, CDialog)
END_MESSAGE_MAP()

