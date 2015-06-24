#include "StdAfx.h"
#include "resource.h"
#include ".\dlgshapetranformprop.h"
#include <InputOnBoard\AircraftElmentShapeDisplay.h>

IMPLEMENT_DYNAMIC(CDlgShapeTranformProp, CXTResizeDialog)
CDlgShapeTranformProp::CDlgShapeTranformProp(CAircraftElmentShapeDisplay* pDisplay,int nProjeID,CWnd* pParent /*=NULL*/)
: CXTResizeDialog(IDD_DIALOG_3D_OBJECT_PROP/*CDlgShapeTranformProp::IDD*/, pParent)
{
	m_nProjectID= nProjeID;
	mpDisplay = pDisplay;
}

CDlgShapeTranformProp::~CDlgShapeTranformProp()
{
}

void CDlgShapeTranformProp::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgShapeTranformProp, CXTResizeDialog)
	ON_COMMAND(ID_APPLY, OnApply)
END_MESSAGE_MAP()


// CDlgShapeTranformProp message handlers

void CDlgShapeTranformProp::CommitChanges()
{

	ARCVector3 vPos ;
	ARCVector3 vScale;
	double dRot;

	vPos = ARCVector3(
		RetrieveContent(IDC_EDIT_POS_X,true),
		RetrieveContent(IDC_EDIT_POS_Y,true),
		RetrieveContent(IDC_EDIT_POS_Z,true)
		);

	vScale = ARCVector3(
		RetrieveContent(IDC_EDIT_SCALE_X),
		RetrieveContent(IDC_EDIT_SCALE_Y),
		RetrieveContent(IDC_EDIT_SCALE_Z)
		);

	dRot = RetrieveContent(IDC_EDIT_ROT_Z);

	if(mpDisplay)
	{
		mpDisplay->SetShapePos(vPos);
		mpDisplay->SetShapeScale(vScale);
		mpDisplay->SetShapeRotate(dRot);
	}

	
}



void CDlgShapeTranformProp::SetContent( int nControlID, double dValue, bool bLengtUnit /*= false*/ )
{
	double dPrintVal = bLengtUnit?ConvertLength(dValue):dValue;
	CString strText;
	strText.Format(_T("%f"), dPrintVal);
	
	CWnd* pControl = GetDlgItem(nControlID);
	if (pControl)
	{
		pControl->EnableWindow(TRUE);
		pControl->SetWindowText(strText);
	}
	else
		ASSERT(FALSE);
}
void CDlgShapeTranformProp::ClearContent( int nControlID )
{
	CWnd* pControl = GetDlgItem(nControlID);
	if (pControl)
	{
		pControl->EnableWindow(FALSE);
		pControl->SetWindowText(_T(""));
	}
	else
		ASSERT(FALSE);
}

double CDlgShapeTranformProp::RetrieveContent( int nControlID, bool LengUnit)
{
	CWnd* pControl = GetDlgItem(nControlID);
	if (pControl)
	{
		CString strText;
		pControl->GetWindowText(strText);		
		double dval =  atof(strText);
		if(LengUnit)
		{
			return UnConvertLength(dval);
		}
		else
			return dval;
	}
	else
		ASSERT(FALSE);
	return 0.0;
}


void CDlgShapeTranformProp::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	OnApply();
	CDialog::OnOK();
}

void CDlgShapeTranformProp::OnApply()
{
	// TODO: Add your specialized code here and/or call the base class
	CommitChanges();	
}

BOOL CDlgShapeTranformProp::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();	

	//initialize unit manager
	//initialize unit manager
	CUnitPiece::InitializeUnitPiece(m_nProjectID,UM_ID_1,this);
	CRect rectItem;
	GetDlgItem(IDOK)->GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int yPos = rectItem.top;
	GetDlgItem(IDC_STATIC_FRAME)->GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int xPos = rectItem.left;
	CUnitPiece::MoveUnitButton(xPos,yPos);
	SetResize(XIAOHUABUTTON_ID, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	CUnitPiece::SetUnitInUse(ARCUnit_Length_InUse);
	//

	GetDlgItem(IDC_EDIT_ROT_X)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_ROT_Y)->ShowWindow(SW_HIDE);

	//
	/*SetResize(IDC_STATIC_FRAME, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_X, SZ_TOP_CENTER, SZ_TOP_CENTER);
	SetResize(IDC_STATIC_Y, SZ_TOP_CENTER, SZ_TOP_CENTER);
	SetResize(IDC_STATIC_Z, SZ_TOP_CENTER, SZ_TOP_CENTER);
	SetResize(IDC_STATIC_POS, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_SCALE, SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);
	SetResize(IDC_STATIC_ROT, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);

	SetResize(IDC_EDIT_SCALE_X, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);
	SetResize(IDC_EDIT_SCALE_Y, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);
	SetResize(IDC_EDIT_SCALE_Z, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);*/

	//SetResize(IDC_EDIT_ROT_X, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);
	//SetResize(IDC_EDIT_ROT_Y, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);
	//SetResize(IDC_EDIT_ROT_Z, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);

	//SetResize(IDC_EDIT_POS_X, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);
	//SetResize(IDC_EDIT_POS_Y, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);
	//SetResize(IDC_EDIT_POS_Z, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);

	//SetResize(ID_APPLY, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	//SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	//SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	LoadContent(mpDisplay);

	return TRUE;
}

void CDlgShapeTranformProp::LoadContent( CAircraftElmentShapeDisplay* pDsp)
{
	ARCVector3 pos  = pDsp->GetShapePos();
	ARCVector3 scale = pDsp->GetShapeScale();
	double dRot =pDsp->GetShapeRotate();

	SetContent(IDC_EDIT_POS_X, pos.x,true);
	SetContent(IDC_EDIT_POS_Y, pos.y,true);
	SetContent(IDC_EDIT_POS_Z, pos.z,true);

	SetContent(IDC_EDIT_ROT_Z, dRot);
	

	SetContent(IDC_EDIT_SCALE_X, scale.x);
	SetContent(IDC_EDIT_SCALE_Y, scale.y);
	SetContent(IDC_EDIT_SCALE_Z, scale.z);
}

bool CDlgShapeTranformProp::RefreshGUI( void )
{
	LoadContent(mpDisplay);
	return true;
}