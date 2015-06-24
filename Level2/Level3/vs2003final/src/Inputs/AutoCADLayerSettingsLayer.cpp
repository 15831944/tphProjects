//////////////////////////////////////////////////////////////////////
// AutoCADLayerSettingsLayer.cpp: implementation for the 
// CAutoCADLayerSettingsLayer class.
//
// Private Members:
//   CString m_LayerName : The ARCport layer name to use/save
//	 bool m_ShowLayer : value to indicate if layer is shown or hidden.
//	 int m_DisplayColor : color to display the layer in
//	 int m_AutoCADFileLayer : value to link our settings to the autoCAD file
//								layer the settings belong to.
//
//
//	Written By: Cary Feldstein
//	December 6, 2005
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AutoCADLayerSettingsLayer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAutoCADLayerSettingsLayer::CAutoCADLayerSettingsLayer( CString LayerName, bool ShowLayer, int DisplayColor, int LayerNum )
{
	m_LayerName = LayerName;
	m_ShowLayer = ShowLayer;
	m_DisplayColor = DisplayColor;
	m_AutoCADFileLayer = LayerNum;
}

CAutoCADLayerSettingsLayer::~CAutoCADLayerSettingsLayer()
{
}

//////////////////////////////////////////////////////////////////////
// Function: setLayerName
// 
// sets the layer name
//
// Parameters: 
//		1) CString LayerName : The name to set the layer to.
//		
// Return Value: None
//
// Exceptions: None
//
//////////////////////////////////////////////////////////////////////
void CAutoCADLayerSettingsLayer::setLayerName (CString LayerName)
{
	m_LayerName = LayerName;
}
	
//////////////////////////////////////////////////////////////////////
// Function: hideLayer
// 
// makes the layer not visable 
//
// Parameters: None
//		
// Return Value: None
//
// Exceptions: None
//
//////////////////////////////////////////////////////////////////////
void CAutoCADLayerSettingsLayer::hideLayer()
{
	m_ShowLayer = FALSE;
}

//////////////////////////////////////////////////////////////////////
// Function: showLayer
// 
// makes the layer not visable 
//
// Parameters: None
//		
// Return Value: None
//
// Exceptions: None
//
//////////////////////////////////////////////////////////////////////
void CAutoCADLayerSettingsLayer::showLayer()
{
	m_ShowLayer = TRUE;
}

//////////////////////////////////////////////////////////////////////
// Function: getLayerName
// 
// returns the layer name
//
// Parameters: None
//		
// Return Value: CString
//
// Exceptions: None
//
//////////////////////////////////////////////////////////////////////
CString CAutoCADLayerSettingsLayer::getLayerName()
{
	return m_LayerName;
}

//////////////////////////////////////////////////////////////////////
// Function: setColor
// 
// sets the layers color value.
//
// Parameters: 
//		1) int Color : the color value to use
//		
// Return Value: None
//
// Exceptions: None
//
//////////////////////////////////////////////////////////////////////
void CAutoCADLayerSettingsLayer::setColor (int Color)
{
	m_DisplayColor = Color;
}

//////////////////////////////////////////////////////////////////////
// Function: getColor
// 
// returns the layer's current color value
//
// Parameters: None
//		
// Return Value: int
//
// Exceptions: None
//
//////////////////////////////////////////////////////////////////////
int CAutoCADLayerSettingsLayer::getColor()
{
	return m_DisplayColor;
}

//////////////////////////////////////////////////////////////////////
// Function: getDisplaySetting
// 
// returns the layer's visability setting
//
// Parameters: None
//		
// Return Value: bool
//
// Exceptions: None
//
//////////////////////////////////////////////////////////////////////
bool CAutoCADLayerSettingsLayer::getDisplaySetting()
{
	return m_ShowLayer;

}

//////////////////////////////////////////////////////////////////////
// Function: getAutoCADFileLayerNum
// 
// returns the layer's AutoCADFile index to match up with drawing file.
//
// Parameters: None
//		
// Return Value: int
//
// Exceptions: None
//
//////////////////////////////////////////////////////////////////////
int CAutoCADLayerSettingsLayer::getAutoCADFileLayerNum()
{
	return m_AutoCADFileLayer;
}

//////////////////////////////////////////////////////////////////////
// Function: setAutoCADFileLayerNum
// 
// sets the layer's AutoCADFile index to match up with drawing file.
//
// Parameters: int num : The layer number to set
//		
// Return Value: None
//
// Exceptions: None
//
//////////////////////////////////////////////////////////////////////
void CAutoCADLayerSettingsLayer::setAutoCADFileLayerNum(int num)
{
	m_AutoCADFileLayer = num;
}
