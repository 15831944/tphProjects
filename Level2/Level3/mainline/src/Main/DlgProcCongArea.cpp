// DlgProcCongArea.cpp : implementation file
#include "stdafx.h"
#include "TermPlan.h"
#include "DlgProcCongArea.h"

#include "DlgProcCongAreaParam.h"

#include "chart\vcplot.h"
#include "chart\vcaxis.h"
#include "chart\vcvaluescale.h"
#include "chart\vccategoryscale.h"
#include "chart\vclabels.h"
#include "chart\vclabel.h"
#include "chart\vcfont.h"
//#include "chart\vcrect.h"
//#include "chart\vccoor.h"
#include "chart\vcintersection.h"
//#include "chart\vcaxistitle.h"

#include "engine\CongestionAreaManager.h"

#include <algorithm>
#include ".\dlgproccongarea.h"
// CDlgProcCongArea dialog

IMPLEMENT_DYNAMIC(CDlgProcCongArea, CDialog)

CDlgProcCongArea::CDlgProcCongArea(Terminal* _pTerm,CTermPlanDoc* _pDoc,CWnd* pParent /*=NULL*/)
: CDialog(CDlgProcCongArea::IDD, pParent)
, m_dLambda(0.5)
, m_dTrigger(1.0)
{
	m_pAreas = _pTerm->m_pAreas;
	m_pPipeDataSet = _pTerm->m_pPipeDataSet;
	m_pDoc = _pDoc;

	m_hAllAreas = NULL;
	m_hAllPipes = NULL;

	m_pAreasIndex = NULL;
	m_pPipesIndex = NULL;
}

CDlgProcCongArea::~CDlgProcCongArea()
{

}

void CDlgProcCongArea::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LAMBDA, m_dLambda);
	DDX_Text(pDX, IDC_EDIT_TRIGGER, m_dTrigger);
	DDX_Control(pDX, IDC_MSCHART, m_msChart);
	DDX_Control(pDX, IDC_TREE_AREAS, m_treeTargetAreas);
	DDX_Control(pDX, IDC_TREE_PIPES, m_treeTargetPipes);
}

BEGIN_MESSAGE_MAP(CDlgProcCongArea, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADVANCED, OnBnClickedButtonAdvanced)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()
// CDlgProcCongArea message handlers

void CDlgProcCongArea::OnBnClickedButtonAdvanced()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CDlgProcCongAreaParam dlg;
	dlg.SetLambdaValue(m_dLambda);
	dlg.SetTriggerValue(m_dTrigger);

	if(dlg.DoModal() == IDOK)
	{
		m_dLambda = dlg.GetLambdaValue();
		m_dTrigger = dlg.GetTriggerValue();

		UpdateData(FALSE);
		UpdateChart();
		//SaveInputData();
	}
}

void CDlgProcCongArea::InitializeChart()
{
	m_msChart.SetChartType(16l);
	CVcPlot plot = m_msChart.GetPlot();
	plot.SetUniformAxis(FALSE);
	for(int nAxis=0; nAxis<2; nAxis++)
	{
		VARIANT Index;
		CVcAxis axis = plot.GetAxis(nAxis,Index); 
		for(int i=1; i<=axis.GetLabels().GetCount(); i++) 
		{
			CVcFont font = axis.GetLabels().GetItem(i).GetVtFont();
			font.SetSize(10.0f);
		}
	}

	m_msChart.Refresh();
}

void CDlgProcCongArea::UpdateChart()
{
	if(m_dLambda <= 0.00001)
	{
		m_dLambda = 0.5 ;
	}
	if(m_dTrigger <= 0.00001)
	{
		m_dTrigger = 0.0;
	}
	UpdateData(FALSE);

	m_msChart.EnableWindow(TRUE);
	m_msChart.ShowWindow(SW_SHOW);
	int nCount = 101;
	double* x = new double[nCount];
	double* y = new double[nCount];
	double dMin = 0;
	double dMax = 1.0 / m_dLambda + m_dTrigger;
	double dStep = (dMax-dMin)*(1.0/(nCount-1));
	for(int i=0; i<nCount; i++)
	{
		x[i] = dMin + i*dStep;
		if(x[i] < m_dTrigger)
			y[i] = 0;
		else
			y[i] = m_dLambda*100.0*(x[i] - m_dTrigger);
//			y[i] = m_dLambda*exp(m_dLambda*(x[i]-m_dTrigger)) - m_dLambda;
	}

// 	TRACE("points: \n   ");
// 	for(int i=0; i<nCount; i++) 
// 	{
// 		TRACE("(%.1f, %.1f) ", x[i],y[i]);
// 	}
// 	TRACE("\n");

	COleSafeArray saData;
	DWORD numElements[] = {nCount, 2};
	saData.Create(VT_R8, 2, numElements);
	long index[2];
	for(index[0]=0; index[0]<static_cast<long>(numElements[0]); index[0]++) 
	{
		for(index[1]=0; index[1]<static_cast<long>(numElements[1]); index[1]++) 
		{
			if(!index[1])
			{
				saData.PutElement(index, &(x[index[0]]));
			}
			else 
			{
				saData.PutElement(index, &(y[index[0]]));
				double temp;
				saData.GetElement(index, &temp);
				// TRACE("y=%f\n", temp);
			}
		}
	}

	m_msChart.SetChartData(saData.Detach());

	CVcPlot plot = m_msChart.GetPlot();
	VARIANT Index;
	CVcAxis axisX = plot.GetAxis(0,Index); 
	axisX.GetValueScale().SetAuto(FALSE);
	axisX.GetValueScale().SetMaximum(x[nCount-1]);
	axisX.GetValueScale().SetMinimum(x[0]);
	axisX.GetValueScale().SetMajorDivision(4);
	axisX.GetCategoryScale().SetAuto(FALSE);
	CVcAxis axisY = plot.GetAxis(1,Index);
	axisY.GetIntersection().SetAuto(FALSE);
	axisY.GetIntersection().SetPoint(x[0]);

	/*	CVcAxisTitle xaxistitle(axisX.GetAxisTitle());
	xaxistitle.SetVisible(true);
	xaxistitle.SetText("pax / squ m");
	// Label Y Axis
	CVcAxisTitle yaxistitle(axisY.GetAxisTitle());
	yaxistitle.SetVisible(true);
	yaxistitle.SetText("m / s");
	*/


	m_msChart.Refresh();

	delete [] x;
	delete [] y;

	m_msChart.BringWindowToTop();

	Invalidate(FALSE);
}

void CDlgProcCongArea::InitTreeAreas()
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net=NET_NORMAL;
	cni.nt=NT_CHECKBOX;

	m_treeTargetAreas.DeleteAllItems();
	if(m_pAreasIndex->size() <= 0)
        m_hAllAreas = m_treeTargetAreas.InsertItem("All Areas", cni, FALSE, FALSE);
	else
		m_hAllAreas = m_treeTargetAreas.InsertItem("All Areas", cni, TRUE, FALSE);
	int nAreaCount = m_pAreas->m_vAreas.size();
	for(int i=0; i<nAreaCount; i++) 
	{
		BOOL bCheck = FALSE;
		std::vector<int>::iterator iterFind	= std::find( m_pAreasIndex->begin(),m_pAreasIndex->end(), i );
		if( iterFind != m_pAreasIndex->end() )
			bCheck = TRUE;

		HTREEITEM h = m_treeTargetAreas.InsertItem(m_pAreas->m_vAreas[i]->name, cni, bCheck, FALSE, m_hAllAreas);
		m_treeTargetAreas.SetItemData(h, i);
	}

	m_treeTargetAreas.Expand(m_hAllAreas, TVE_EXPAND);
}

void CDlgProcCongArea::InitTreePipes()
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net=NET_NORMAL;
	cni.nt=NT_CHECKBOX;

	m_treeTargetPipes.DeleteAllItems();
	if(m_pPipesIndex->size() <= 0)
		m_hAllPipes = m_treeTargetPipes.InsertItem("All Pipes", cni, FALSE, FALSE);
	else
		m_hAllPipes = m_treeTargetPipes.InsertItem("All Pipes", cni, TRUE, FALSE);
	int nPipeCount	= m_pPipeDataSet->GetPipeCount();
	for(int i=0; i<nPipeCount; i++ )
	{
		BOOL bCheck = FALSE;
		std::vector<int>::iterator iterFind	= std::find( m_pPipesIndex->begin(),m_pPipesIndex->end(), i );
		if( iterFind != m_pPipesIndex->end() )
			bCheck = TRUE;

		HTREEITEM h = m_treeTargetPipes.InsertItem(m_pPipeDataSet->GetPipeAt(i)->GetPipeName(), cni, bCheck, FALSE, m_hAllPipes);
		m_treeTargetPipes.SetItemData(h, i);
	}

	m_treeTargetPipes.Expand(m_hAllPipes, TVE_EXPAND);
}

void CDlgProcCongArea::InitInputData()
{
	CCongestionAreaManager& dataset = m_pDoc->getARCport()->getCongestionAreaManager();
	const CongestionParam& param = dataset.GetCongParam();
	m_dLambda = param.dLambda;
	m_dTrigger = param.dTrigger;
	m_pAreasIndex = dataset.GetAreasVect();
	m_pPipesIndex = dataset.GetPipesVect();
	UpdateData(FALSE);
}

void CDlgProcCongArea::SaveInputData()
{
	UpdateData(TRUE);

	GetTreeData();

	CongestionParam param;
	param.dLambda = m_dLambda;
	param.dTrigger = m_dTrigger;

	CString strPath = m_pDoc->m_ProjInfo.path + "\\INPUT";
	CCongestionAreaManager& dataset = m_pDoc->getARCport()->getCongestionAreaManager();
	dataset.SetCongParam(param);
	dataset.saveDataSet(strPath, FALSE);
}

BOOL CDlgProcCongArea::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	InitInputData();
	InitializeChart();
	InitTreeAreas();
	InitTreePipes();
	UpdateChart();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgProcCongArea::GetTreeData()
{
	m_pAreasIndex->clear();
	m_pPipesIndex->clear();

	BOOL bCheck = FALSE;
	if(m_treeTargetAreas.IsCheckItem(m_hAllAreas) == 1)
		bCheck = TRUE;
	else
		bCheck = FALSE;
	if(bCheck)
	{
		if(BOOL bHas = m_treeTargetAreas.ItemHasChildren(m_hAllAreas))
		{
			HTREEITEM hNextItem;
			HTREEITEM hChildItem = m_treeTargetAreas.GetChildItem(m_hAllAreas);

			while (hChildItem != NULL)
			{
				if(m_treeTargetAreas.IsCheckItem(hChildItem) == 1)
				{
					int nData = (int)m_treeTargetAreas.GetItemData(hChildItem);
					m_pAreasIndex->push_back(nData);
				}
				hNextItem = m_treeTargetAreas.GetNextItem(hChildItem, TVGN_NEXT);
				hChildItem = hNextItem;
			}
		}
	}

	if(m_treeTargetPipes.IsCheckItem(m_hAllPipes) == 1)
		bCheck = TRUE;
	else
		bCheck = FALSE;

	if(bCheck)
	{
		if(BOOL bHas = m_treeTargetPipes.ItemHasChildren(m_hAllPipes))
		{
			HTREEITEM hNextItem;
			HTREEITEM hChildItem = m_treeTargetPipes.GetChildItem(m_hAllPipes);

			while (hChildItem != NULL)
			{
				if(m_treeTargetPipes.IsCheckItem(hChildItem) == 1)
				{
					int nData = (int)m_treeTargetPipes.GetItemData(hChildItem);
					m_pPipesIndex->push_back(nData);
				}
				hNextItem = m_treeTargetPipes.GetNextItem(hChildItem, TVGN_NEXT);
				hChildItem = hNextItem;
			}
		}
	}
}

LRESULT CDlgProcCongArea::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message == UM_CEW_STATUS_CHANGE)
	{
/*		HTREEITEM hItem = (HTREEITEM)wParam;
		if(hItem == m_hAllAreas)
		{
			BOOL bCheck = FALSE;
			if(m_treeTargetAreas.IsCheckItem(hItem) == 1)
				bCheck = TRUE;

			if(BOOL bHas = m_treeTargetAreas.ItemHasChildren(hItem))
			{
				HTREEITEM hNextItem;
				HTREEITEM hChildItem = m_treeTargetAreas.GetChildItem(hItem);

				while (hChildItem != NULL)
				{
					hNextItem = m_treeTargetAreas.GetNextItem(hChildItem, TVGN_NEXT);
					m_treeTargetAreas.SetCheckStatus(hChildItem, bCheck);
					hChildItem = hNextItem;
				}
			}
		}
		else if(hItem == m_hAllPipes)
		{
			BOOL bCheck = FALSE;
			if(m_treeTargetPipes.IsCheckItem(hItem) == 1)
				bCheck = TRUE;

			if(BOOL bHas = m_treeTargetPipes.ItemHasChildren(hItem))
			{
				HTREEITEM hNextItem;
				HTREEITEM hChildItem = m_treeTargetPipes.GetChildItem(hItem);

				while (hChildItem != NULL)
				{
					hNextItem = m_treeTargetPipes.GetNextItem(hChildItem, TVGN_NEXT);
					m_treeTargetPipes.SetCheckStatus(hChildItem, bCheck);
					hChildItem = hNextItem;
				}
			}
		}
*/

	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}
void CDlgProcCongArea::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	SaveInputData();
	OnOK();
}
