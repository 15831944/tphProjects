// ChangeVerticalProfileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ChangeVerticalProfileDlg.h"
#include "../common/UnitsManager.h"
#include ".\changeverticalprofiledlg.h"
#include "TermPlanDoc.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangeVerticalProfileDlg dialog


CChangeVerticalProfileDlg::CChangeVerticalProfileDlg(std::vector<double>&vXPos,std::vector<double>&vZPos,std::vector<double>&vHeights,const Path& _path, CStringArray* pStrArrFloorName,EnvironmentMode envMode,CWnd* pParent /*=NULL*/)
	: CDialog(CChangeVerticalProfileDlg::IDD, pParent)
{
	m_vXPos = vXPos;
	m_vZPos = vZPos;
	m_vHeights = vHeights;
	m_Path = _path;
	m_pStrArrFloorName=pStrArrFloorName;
	m_envMode = envMode;
	m_nMaxRange = vXPos.size();
	m_nStartSlope = 1;
	m_nEndSlope = 1;
}


void CChangeVerticalProfileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangeVerticalProfileDlg)
	DDX_Control(pDX, IDC_STATIC_ANGLE, m_staticAngle);
	DDX_Control(pDX, IDC_STATIC_LEN, m_staticLen);
	DDX_Control(pDX, IDC_EDIT_LENGTH, m_editLength);
	DDX_Control(pDX, IDC_EDIT_ANGLE, m_editAngle);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_STATIC_PTLINE, m_wndPtLineChart);

	DDX_Text(pDX, IDC_EDIT_STARTSLOPE, m_nStartSlope);
	DDV_MinMaxInt(pDX, m_nStartSlope, 1, m_nMaxRange);
	DDX_Text(pDX, IDC_EDIT_ENDSLOPE, m_nEndSlope);
	DDV_MinMaxInt(pDX, m_nEndSlope, 1, m_nMaxRange);
	DDX_Control(pDX, IDC_SPIN_STARTSLOPE, m_spinStartSlope);
	DDX_Control(pDX, IDC_SPIN_ENDSLOPE, m_spinEndSlope);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangeVerticalProfileDlg, CDialog)


	//{{AFX_MSG_MAP(CChangeVerticalProfileDlg)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_UPDATE_VALUE, UpdateLengthAngle )	
	ON_EN_CHANGE(IDC_EDIT_STARTSLOPE, OnChangeEditStartslope)
	ON_EN_CHANGE(IDC_EDIT_ENDSLOPE, OnChangeEditEndslope)
	ON_COMMAND(IDC_BUTTON_PLAY, OnPlay)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeVerticalProfileDlg message handlers

BOOL CChangeVerticalProfileDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_wndPtLineChart.SetData( m_vXPos, m_vZPos, m_vHeights, &m_Path,m_pStrArrFloorName, true ,m_envMode);
	
	// set length and angle
	m_spinStartSlope.SetRange32(1,m_nMaxRange);
	m_spinEndSlope.SetRange32(1,m_nMaxRange);



	CString _strLength, _strAngle, _strLengthLable;
	std::vector<double> vRealAltitude;
	if(m_envMode == EnvMode_OnBoard)
	{
		double _dLength = 0.0;
		double _dAngle  = 0.0;
		vRealAltitude = m_vHeights;
		//calculate two point's distance and angle

		double pi = 3.1415926535;

		double _dMaxAngle = 0.0;

		Point ptCur;
		int nPathCount = m_Path.getCount();
		if(nPathCount > 1)
			ptCur = m_Path.getPoint(0);
		for (int i = 1;  i < nPathCount;++i)
		{
			Point ptNext = m_Path.getPoint(i);

			double dCurLength = ptCur.distance(ptNext);
			_dLength += dCurLength;


			//max angle
			double dVerticalLength = 0.0;
			if(ptCur.getZ() >= ptNext.getZ())
				dVerticalLength = ptCur.getZ()-ptNext.getZ();
			else
				dVerticalLength =ptNext.getZ()- ptCur.getZ();

			if( dCurLength != 0.0 )
			{
				// get angle by asin
				long double _sin_radians = asin( dVerticalLength / dCurLength );
				_sin_radians = _sin_radians * 180.0 / pi;

				if( abs( _sin_radians) > abs( _dMaxAngle ) )
					_dMaxAngle = _sin_radians;
			}


			ptCur = ptNext;
		}

		_dAngle =  _dMaxAngle;
		
		_strLength.Format("%.2f", _dLength );
		_strAngle.Format("%.2f", _dAngle );
		_strLengthLable.Format("Length (m):" ); 
	}
	else
	{
		CTermPlanDoc* _pDoc = (CTermPlanDoc*) ((CMDIChildWnd *)((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveFrame())->GetActiveDocument();
		assert( _pDoc );

		_pDoc->GetCurModeFloorsRealAltitude(vRealAltitude);
		CString _strLength, _strAngle, _strLengthLable;
		_strLength.Format("%.2f", UNITSMANAGER->ConvertLength( m_Path.getFactLength(vRealAltitude)) );
		_strAngle.Format("%.2f", m_Path.getMaxAngle(vRealAltitude) );
		_strLengthLable.Format("Length (%s):", UNITSMANAGER->GetLengthUnitString( UNITSMANAGER->GetLengthUnits() ) ); 
	}



	m_editLength.SetWindowText( _strLength );
	m_editAngle.SetWindowText( _strAngle );
	//m_staticLen.SetWindowText( _strLengthLable );

	CRect rectWnd;
	GetClientRect(rectWnd);
	Resize(rectWnd.Width(),rectWnd.Height());
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChangeVerticalProfileDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	Resize(cx,cy);
	// TODO: Add your message handler code here
	
}


void CChangeVerticalProfileDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default
	lpMMI->ptMinTrackSize.x=764;
	lpMMI->ptMinTrackSize.y=320;
	CDialog::OnGetMinMaxInfo(lpMMI);
}

LRESULT CChangeVerticalProfileDlg::UpdateLengthAngle( WPARAM wParam, LPARAM lParam )
{

	CString _strLength, _strAngle; 
	if(m_envMode == EnvMode_OnBoard)
	{
		double _dLength = wParam ;
		double _dAngle= lParam;
		_strLength.Format("%.2f", _dLength  );
		_strAngle.Format( "%.2f", _dAngle );
	}
	else
	{
		double _dLength = wParam / SCALE_FACTOR;
		double _dAngle= lParam / SCALE_FACTOR;

		_strLength.Format("%.2f", UNITSMANAGER->ConvertLength( _dLength ) );
		_strAngle.Format( "%.2f", _dAngle );

	}
	m_editLength.SetWindowText( _strLength );
	m_editAngle.SetWindowText( _strAngle );
	return TRUE;
}

void CChangeVerticalProfileDlg::Resize(int cx,int cy)
{
	if(::IsWindow(m_wndPtLineChart.m_hWnd))
	{
		CRect rectWnd;
		m_btnCancel.GetWindowRect(rectWnd);
		m_btnCancel.SetWindowPos(NULL,cx-10-rectWnd.Width(),cy-10-rectWnd.Height(),0,0,SWP_NOSIZE);
		m_btnCancel.GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);
		
		m_btnOK.SetWindowPos(NULL,rectWnd.left-10-rectWnd.Width(),rectWnd.top,0,0,SWP_NOSIZE);
		
		m_wndPtLineChart.MoveWindow(10,10,cx-20,cy-30-rectWnd.Height());
		m_wndPtLineChart.GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);
		
		m_staticLen.SetWindowPos(NULL,rectWnd.left,rectWnd.bottom+8,0,0,SWP_NOSIZE);
		m_staticLen.GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);
		
		m_editLength.SetWindowPos(NULL,rectWnd.right+8,rectWnd.top-2,0,0,SWP_NOSIZE);
		m_editLength.GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);
		
		m_staticAngle.SetWindowPos(NULL,rectWnd.right+4,rectWnd.top+2,0,0,SWP_NOSIZE);
		m_staticAngle.GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);
		
		m_editAngle.SetWindowPos(NULL,rectWnd.right,rectWnd.top-2,0,0,SWP_NOSIZE);
		m_editAngle.GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);
				
		GetDlgItem(IDC_STATIC_CONSTRANT_SLOPE)->SetWindowPos(NULL,rectWnd.right+4,rectWnd.top+2,0,0,SWP_NOSIZE);
		GetDlgItem(IDC_STATIC_CONSTRANT_SLOPE)->GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);

		GetDlgItem(IDC_EDIT_STARTSLOPE)->SetWindowPos(NULL,rectWnd.right+4,rectWnd.top-2,0,0,SWP_NOSIZE);
		GetDlgItem(IDC_EDIT_STARTSLOPE)->GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);

		GetDlgItem(IDC_SPIN_STARTSLOPE)->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
		GetDlgItem(IDC_SPIN_STARTSLOPE)->GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);
		
		GetDlgItem(IDC_EDIT_ENDSLOPE)->SetWindowPos(NULL,rectWnd.right+4,rectWnd.top,0,0,SWP_NOSIZE);
		GetDlgItem(IDC_EDIT_ENDSLOPE)->GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);

		GetDlgItem(IDC_SPIN_ENDSLOPE)->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
		GetDlgItem(IDC_SPIN_ENDSLOPE)->GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);

		GetDlgItem(IDC_BUTTON_PLAY)->SetWindowPos(NULL,rectWnd.right+4,rectWnd.top,0,0,SWP_NOSIZE);
		GetDlgItem(IDC_BUTTON_PLAY)->GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);
		Invalidate();
		
	}
}


//add by Mark Chen  04/01/2004
void CChangeVerticalProfileDlg::OnOK() 
{
	// TODO: Add extra validation here
	if(m_envMode == EnvMode_OnBoard)//onboard
	{
		if(OnOkOnBoard())
		{
			return CDialog::OnOK();
			 	
		}
		else
			return;
	}

	std::vector<double>vZPos;
	vZPos = m_wndPtLineChart.m_vPointYPos;

	if((int)vZPos.size() > 0 && (int)m_vZPos.size() > 0)
	{
		if(m_vZPos[0] > vZPos[0] + 0.00001 
			|| m_vZPos[0] < vZPos[0] - 0.00001)
		{
			MessageBox("The first point can not be changed!", "Warning");
			return;		
		}
	}

	int i = 0;
	for( i=0; i<static_cast<int>(vZPos.size()); ++i )
	{
		vZPos[ i ] = vZPos[ i ] * SCALE_FACTOR;	
	
		// TRACE( "%f\n", vZPos[i] );


		int nZMod = int(vZPos[ i ]) % 100;
		int nNum  = int(vZPos[ i ]) / 100;
		if(( nZMod > 10 && nZMod < 90 )&&( vZPos.size()== i))		//out  10% //modify matt,if the EndPoint then continue;
		{
			MessageBox("End Point not on floor, you can Ctrl+Mouse!", "Invalidate");
			return;
		}
		else if( nZMod <= 10 && nZMod > 0)
		{
			if( MessageBox("End Point not on floor. Would you like me to Fix please check OK, else check Cancel!", "Invalidate", MB_OKCANCEL) == IDOK)
			{
				m_wndPtLineChart.m_vPointYPos[i] = nNum * 1.00;
			}
			else
				return;
		}
		else if( nZMod >= 90 && nZMod < 100)
		{
			if( MessageBox("End Point not on floor. Would you like me to Fix please check OK, else check Cancel!", "Invalidate", MB_OKCANCEL) == IDOK )
			{
				m_wndPtLineChart.m_vPointYPos[i] = (nNum + 1) * 1.00;
			}
			else
				return;

		}

		
	}

	CDialog::OnOK();
}
bool CChangeVerticalProfileDlg::OnOkOnBoard()
{
	std::vector<double>vZPos;
	vZPos = m_wndPtLineChart.m_vPointYPos;

	if((int)vZPos.size() > 0 && (int)m_vZPos.size() > 0)
	{
		if(m_vZPos[0] > vZPos[0] + 0.00001 
			|| m_vZPos[0] < vZPos[0] - 0.00001)
		{
			MessageBox("The first point can not be changed!", "Warning");
			return false;		
		}
	}

	//vZPos.clear();
	//vZPos.push_back(*m_wndPtLineChart.m_vPointYPos.begin());
	//vZPos.push_back(*m_wndPtLineChart.m_vPointYPos.rbegin());

	//int i = 0;
	//for( i=0; i<static_cast<int>(vZPos.size()); ++i )
	{
		int i=static_cast<int>(vZPos.size())-1;//vZPos.size()-1;
		bool bAvailbale = false;
		int nHeightCount = static_cast<int>(m_vHeights.size());
		for (int nHeight = 0; nHeight < nHeightCount; ++nHeight)
		{
			if(abs(m_vHeights[nHeight] - vZPos[i]) < 0.00001)//miscalculate less than 0.1
			{
				m_wndPtLineChart.m_vPointYPos[i] = m_vHeights[nHeight];
				bAvailbale = true;
				break;
			}
		}	
		
		if(bAvailbale == false)
		{
			MessageBox("End Point not on floor, you can Ctrl+Mouse!", "Invalidate");
			return false;
		}
	}
	
	return true;
}

void CChangeVerticalProfileDlg::OnStnClickedStaticLen2()
{
	// TODO: Add your control notification handler code here
}
void CChangeVerticalProfileDlg::OnChangeEditStartslope()
{
	GetDlgItem(IDC_EDIT_STARTSLOPE)->UpdateData(TRUE);
	int nStartSlope = GetDlgItemInt(IDC_EDIT_STARTSLOPE);	
	if (nStartSlope < 1)
		return;
	m_nStartSlope = nStartSlope;
}

void CChangeVerticalProfileDlg::OnChangeEditEndslope()
{	
	GetDlgItem(IDC_EDIT_ENDSLOPE)->UpdateData(TRUE);
	int nEndSlope = GetDlgItemInt(IDC_EDIT_ENDSLOPE);	
	if (nEndSlope < 1)
		return;
	m_nEndSlope = nEndSlope;	
}

void CChangeVerticalProfileDlg::OnPlay()
{
	m_wndPtLineChart.UpdataChart(m_nStartSlope,m_nEndSlope);
}
