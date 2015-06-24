// AffinityGroupSpec.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "AffinityGroupSpec.h"


// CAffinityGroupSpec dialog

IMPLEMENT_DYNAMIC(CAffinityGroupSpec, CDialog)
CAffinityGroupSpec::CAffinityGroupSpec(CWnd* pParent /*=NULL*/)
	: CDialog(CAffinityGroupSpec::IDD, pParent)
{
}

CAffinityGroupSpec::~CAffinityGroupSpec()
{
}

void CAffinityGroupSpec::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAffinityGroupSpec, CDialog)
END_MESSAGE_MAP()


// CAffinityGroupSpec message handlers
