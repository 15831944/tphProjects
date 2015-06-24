#include "StdAfx.h"
#include ".\treectrlitemindlg.h"
#include <common/Path2008.h>
#include <common/CARCUnit.h>

void PropDlgTreeItemDataInstance::Clear()
{
	for(size_t i=0;i<m_vDatalist.size();i++)
	{
		delete m_vDatalist[i];
	}
	m_vDatalist.clear();
}

void PropDlgTreeItemDataInstance::Add( IPropDlgTreeItemData* pdata )
{
	if( std::find(m_vDatalist.begin(),m_vDatalist.end(),pdata)!=m_vDatalist.end() )
		return;
	m_vDatalist.push_back(pdata);	
}
//////////////////////////////////////////////////////////////////////////
TreeCtrlItemInDlg::TreeCtrlItemInDlg( CAirsideObjectTreeCtrl& ctrl, HTREEITEM hitem ) :m_ctrl(ctrl)
{
	m_hItem = hitem;

	ConstructInit();
}

TreeCtrlItemInDlg TreeCtrlItemInDlg::AddChild( const CString& strNode,HTREEITEM HAfter /*= TVI_LAST*/ )
{
	HTREEITEM hitem = m_ctrl.InsertItem(strNode,m_hItem,HAfter);
	return TreeCtrlItemInDlg(m_ctrl,hitem);
}

void TreeCtrlItemInDlg::Expand()
{
	m_ctrl.Expand(m_hItem,TVE_EXPAND);
}


TreeCtrlItemInDlg TreeCtrlItemInDlg::GetParent() const
{
	HTREEITEM hParent = NULL;
	if(m_hItem)
	{
		hParent = m_ctrl.GetParentItem(m_hItem);
	}
	return TreeCtrlItemInDlg(m_ctrl,hParent);
}

void TreeCtrlItemInDlg::AddTokenItem( CString strToken, bool bClear,COLORREF color/*=RGB(0,0,255)*/ )
{
	AirsideObjectTreeCtrlItemDataEx* pDataEx = GetInitData();

	ASSERT(pDataEx);


	ItemStringSectionColor isscStringColor;	
	isscStringColor.strSection = strToken;
	isscStringColor.colorSection = color;
	if(bClear)
		pDataEx->vrItemStringSectionColorShow.clear();

	pDataEx->vrItemStringSectionColorShow.push_back(isscStringColor);
}

AirsideObjectTreeCtrlItemDataEx* TreeCtrlItemInDlg::GetData()const
{
	if(m_hItem)
		return m_ctrl.GetItemDataEx(m_hItem);
	return NULL;
}

AirsideObjectTreeCtrlItemDataEx* TreeCtrlItemInDlg::InitData()
{
	AirsideObjectTreeCtrlItemDataEx dext;
	dext.lSize = sizeof(dext);
	dext.dwptrItemData = 0;
	m_ctrl.SetItemDataEx(m_hItem,dext);	
	return  GetData();
}

void TreeCtrlItemInDlg::AddDrivePipePath(const CPath2008& path, ARCUnit_Length curUnit, double dLevel )
{
	int iPathCount = path.getCount();
	for(int i = 0;i < iPathCount; i++)
	{
		const CPoint2008& pt = path[i];
		double dUnitX = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curUnit,pt.getX());
		double dUnitY = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curUnit,pt.getY());
		double dUnitZ = 0.0;
		dUnitZ = dLevel + pt.getZ() + 1;
		CString csPoint;	
		csPoint.Format("x=%.2f; y =%.2f ; z =%.2f", dUnitX ,dUnitY, dUnitZ);

		TreeCtrlItemInDlg childItem = AddChild(csPoint);
		CString strTmp;

		strTmp.Format("%.2f", dUnitX);			
		childItem.AddTokenItem(strTmp);
		strTmp.Format("%.2f",dUnitY);
		childItem.AddTokenItem(strTmp);

	}
}

void TreeCtrlItemInDlg::AddChildPath( const CPath2008& path, ARCUnit_Length curUnit, int nLevel )
{
	int iPathCount = path.getCount();
	for(int i = 0;i < iPathCount; i++)
	{
		const CPoint2008& pt = path[i];
		double dUnitX = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curUnit,pt.getX());
		double dUnitY = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curUnit,pt.getY());
		double dUnitZ = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curUnit,pt.getZ());
		int offset=(int)(dUnitZ>0?dUnitZ+0.5:dUnitZ-0.5);
		CString csPoint;	
		csPoint.Format("x=%.2f; y =%.2f ; Level %d", dUnitX ,dUnitY, nLevel+1+offset );

		TreeCtrlItemInDlg childItem = AddChild(csPoint);
		CString strTmp;

		strTmp.Format("%.2f", dUnitX);			
		childItem.AddTokenItem(strTmp);
		strTmp.Format("%.2f",dUnitY);
		childItem.AddTokenItem(strTmp);

	}
}

void TreeCtrlItemInDlg::AddChildPath( const CPath2008& path, ARCUnit_Length curUnit)
{
	int iPathCount = path.getCount();
	for(int i = 0;i < iPathCount; i++)
	{
		const CPoint2008& pt = path[i];
		double dUnitX = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curUnit,pt.getX());
		double dUnitY = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curUnit,pt.getY());
		CString csPoint;	
		csPoint.Format("x=%.2f; y =%.2f", dUnitX ,dUnitY);

		TreeCtrlItemInDlg childItem = AddChild(csPoint);
		CString strTmp;

		strTmp.Format("%.2f", dUnitX);			
		childItem.AddTokenItem(strTmp);
		strTmp.Format("%.2f",dUnitY);
		childItem.AddTokenItem(strTmp);
	}
}

void TreeCtrlItemInDlg::SetLengthValueText( CString strPrfix, double dValue,ARCUnit_Length curUnit )
{
	//double dUnitValue = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curUnit,dValue);
	CString strUnit = CARCLengthUnit::GetLengthUnitString(curUnit);
	CString strLength = GetValueText(dValue,curUnit);
	//strLength.Format("%.2f",dUnitValue);
	CString str;
	str.Format("%s(%s): %s ",strPrfix,strUnit,strLength);


	m_ctrl.SetItemText(m_hItem,str);
	AddTokenItem(strLength,true);
}

void TreeCtrlItemInDlg::SetDegreeValueText( CString strPrfix, double dValue )
{
	CString strLength;
	strLength.Format("%d",(int)dValue);
	CString str;
	str.Format("%s: %s (degree)",strPrfix,strLength);

	m_ctrl.SetItemText(m_hItem,str);
	AddTokenItem(strLength,true);
}

void TreeCtrlItemInDlg::SetStringText( CString strPrix, CString str )
{
	CString strText;
	strText.Format("%s: %s",strPrix,str);
	m_ctrl.SetItemText(m_hItem,strText);

	AddTokenItem(str,true);
}

void TreeCtrlItemInDlg::SetIntValueText( CString strPrefix,int iValue )
{
	CString strIValue;
	strIValue.Format("%d",iValue);
	SetStringText(strPrefix,strIValue);
}

AirsideObjectTreeCtrlItemDataEx* TreeCtrlItemInDlg::GetInitData()
{
	if(!GetData())
	{
		InitData();
	}
	return GetData();
}

CString TreeCtrlItemInDlg::GetValueText( double dValue,ARCUnit_Length curUnit )
{
	double dUnitValue = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curUnit,dValue);
	//CString strUnit = CARCLengthUnit::GetLengthUnitString(curUnit);
	CString strLength;
	strLength.Format("%.2f",dUnitValue);
	return strLength;
}

CString TreeCtrlItemInDlg::GetValueText( double dValue,ARCUnit_Velocity curUnit )
{
	double dUnitValue = CARCVelocityUnit::ConvertVelocity(DEFAULT_DATABASE_VELOCITY_UNIT,curUnit,dValue);
	//CString strUnit = CARCLengthUnit::GetLengthUnitString(curUnit);
	CString strLength;
	strLength.Format("%.2f",dUnitValue);
	return strLength;
}

TreeCtrlItemInDlg TreeCtrlItemInDlg::GetRootItem() const
{
	return GetRootItem(m_ctrl);
}

TreeCtrlItemInDlg TreeCtrlItemInDlg::GetRootItem( CAirsideObjectTreeCtrl& ctrl )
{
	return TreeCtrlItemInDlg(ctrl,ctrl.GetRootItem());
}

namespace
{
	void DelItemChild(CTreeCtrl& treeCtrl,   HTREEITEM   hItem);
	void DelItem(CTreeCtrl& treeCtrl,   HTREEITEM   hItem) 
	{ 
		DelItemChild(treeCtrl,hItem);
		treeCtrl.DeleteItem(hItem);
	}
	void DelItemChild(CTreeCtrl& treeCtrl,   HTREEITEM   hItem)
	{	
		HTREEITEM   hChild   =   treeCtrl.GetChildItem(hItem); 
		while(hChild   !=   NULL)
		{ 		
			HTREEITEM nextChild =  treeCtrl.GetNextSiblingItem(hChild); 
			DelItem(treeCtrl,hChild);		
			hChild = nextChild;
		}	
	}
};
void TreeCtrlItemInDlg::Destroy()
{
	DelItem(m_ctrl,m_hItem);
	m_hItem = NULL;
}

void TreeCtrlItemInDlg::DeleteAllChild()
{
	DelItemChild(m_ctrl,m_hItem);
}

BOOL TreeCtrlItemInDlg::SetSelect()
{
	return m_ctrl.SelectItem(m_hItem);
}


void TreeCtrlItemInDlg::SetString( CString str,bool bToken /*= false*/ )
{
	m_ctrl.SetItemText(m_hItem,str);
	if(bToken){
		AddTokenItem(str);
	}
}

void TreeCtrlItemInDlg::SetSpeedValueText( CString strPrfix, double dValue,ARCUnit_Velocity curUnit )
{
	//double dUnitValue = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curUnit,dValue);
	CString strUnit = CARCVelocityUnit::GetVelocityUnitString(curUnit);
	CString strLength = GetValueText(dValue,curUnit);
	//strLength.Format("%.2f",dUnitValue);
	CString str;
	str.Format("%s(%s): %s ",strPrfix,strUnit,strLength);


	m_ctrl.SetItemText(m_hItem,str);
	AddTokenItem(strLength,true);
}

//
//void TreeCtrlItemInDlg::RemoveFromParent()
//{
//
//}
