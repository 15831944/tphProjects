#include "StdAfx.h"

#include "CADInfo.h"
#include "tinyxml/tinyxml.h"
#include "common/ARCStringConvert.h"

//
const static CString sbVisibleTag = _T("Visible");
const static CString sbMonochorTag = _T("Monochorom");
const static CString sMonoChorColorTag = _T("MonoChormColor");
//cad layer string tag
const static CString sCADLayers = _T("Layers");
const static CString sCADLayer = _T("Layer");
const static CString sCADLayerColor = _T("Color");
const static CString sCADLayerbOn = _T("bON");
const static CString sCADLayerName = _T("Name");
//cad inof
const static CString sCADInfo = _T("Info");
const static CString sCADFile = _T("File");
const static CString sCADPath = _T("Path");
const static CString sCADOffset = _T("Offset");
const static CString sCADSCale = _T("Scale");
const static CString sCADRotate = _T("Rotate");

TiXmlElement* CCADFileDisplayInfo::serializeTo( TiXmlElement* pCAdElm , int nVersion)const
{
	//TiXmlElement* pCAdElm = (TiXmlElement*)pElmParent->InsertEndChild( TiXmlElement(CCADFileDisplayInfo::ExportString) );
	if(pCAdElm)
	{
		pCAdElm->SetAttribute(sbVisibleTag, bVisible );
		pCAdElm->SetAttribute(sbMonochorTag, m_bIsMonochrome);
		pCAdElm->SetAttribute(sMonoChorColorTag, COLORREF(m_cMonochromeColor) );	
		
		//export info
		TiXmlElement* pCAdInfoElm = (TiXmlElement*)pCAdElm->InsertEndChild( TiXmlElement(sCADInfo) );
		if(pCAdInfoElm)
		{
			const CCADInfo& info = mCADInfo; 
			pCAdInfoElm->SetAttribute(sCADFile,info.sFileName);
			pCAdInfoElm->SetAttribute(sCADPath, info.sPathName);
			pCAdInfoElm->SetAttribute(sCADOffset, ARCStringConvert::toString(info.vOffset) );
			pCAdInfoElm->SetDoubleAttribute(sCADSCale, info.dScale);
			pCAdInfoElm->SetDoubleAttribute(sCADRotate, info.dRotation);
		}
		//export layers
		TiXmlElement* pCADLayersElm = (TiXmlElement* )pCAdElm->InsertEndChild( TiXmlElement(sCADLayers) );
		if(pCADLayersElm)
		{
			for(int i=0;i<GetLayerCount();i++)
			{
				const CCADLayerInfo& cadLayer = LayerAt(i);
				TiXmlElement* pLayerElm = (TiXmlElement*)pCADLayersElm->InsertEndChild(TiXmlElement(sCADLayer));
				if(pLayerElm)
				{
					pLayerElm->SetAttribute(sCADLayerColor, ARCStringConvert::toString((int)cadLayer.cColor) );
					pLayerElm->SetAttribute(sCADLayerbOn, cadLayer.bIsOn);
					pLayerElm->SetAttribute(sCADLayerName, cadLayer.sName);
				}
			}
		}

	}
	return pCAdElm;
}

void CCADFileDisplayInfo::serializeFrom( const TiXmlElement* pElm, int nVersion)
{
	mLayersInfo.clear();
	int iValue;
	double dValue;
	const char* strValue = NULL;

	if( pElm->Attribute(sbVisibleTag,&iValue) )
	{
		bVisible = iValue;
	}
	if(pElm->Attribute(sbMonochorTag,&iValue) )
	{
		m_bIsMonochrome = iValue;
	}
	if(pElm->Attribute(sMonoChorColorTag,&iValue))
	{
		m_cMonochromeColor = ARCColor3( (COLORREF)iValue);
	}
	//import info
	const TiXmlElement* pInfoElm = pElm->FirstChildElement(sCADInfo);
	if(pInfoElm)
	{
		CCADInfo& cadInfo = mCADInfo;
		if( strValue = pInfoElm->Attribute(sCADFile) )
		{
			cadInfo.sFileName = strValue;
		}
		if( strValue = pInfoElm->Attribute(sCADPath) )
		{
			cadInfo.sPathName = strValue;
		}
		if( strValue = pInfoElm->Attribute(sCADOffset) )
		{
			cadInfo.vOffset = ARCStringConvert::parseVector2(strValue);
		}
		if( pInfoElm->Attribute(sCADSCale,&dValue) )
		{
			cadInfo.dScale = dValue;
		}
		if( pInfoElm->Attribute(sCADRotate, &dValue) )
		{
			cadInfo.dRotation = dValue;
		}
	}
	//import layers
	const TiXmlElement* pLayersElm = pElm->FirstChildElement(sCADLayers);
	if(pLayersElm)
	{
		const TiXmlElement * pLayerElm  = pLayersElm->FirstChildElement(sCADLayer);
		while(pLayerElm) //import a layer
		{
			CCADLayerInfo layerinfo;
			if(pLayerElm->Attribute(sCADLayerColor,&iValue) )
			{
				layerinfo.cColor = ARCColor3((COLORREF)iValue);
			}
			if( strValue = pLayerElm->Attribute(sCADLayerName) )
			{
				layerinfo.sName = strValue;
			}
			if( pLayerElm->Attribute(sCADLayerbOn, &iValue) )
			{
				layerinfo.bIsOn = iValue;
			}
			AddLayerInfo(layerinfo);
			pLayerElm = pLayerElm->NextSiblingElement(sCADLayer);
		}
	}
}

CCADFileDisplayInfo::CCADFileDisplayInfo()
{
	m_bIsMonochrome = FALSE;
	m_cMonochromeColor = ARCColor3(ARCColor3::GREY_VISIBLEREGION);
	bVisible = TRUE;
}

int CCADFileDisplayInfo::GetLayerCount() const
{
	return (int)mLayersInfo.size();
}

int CCADFileDisplayInfo::GetLayerInfoIdx( CString sName ) const
{
	for(int i=0;i<GetLayerCount();i++)
	{
		if( LayerAt(i).sName.CompareNoCase(sName) == 0 )
			return i;
	}
	return -1;
}

bool CCADFileDisplayInfo::CompareList( const LayerInfoList& list1, const LayerInfoList& list2 )
{
	if(list1.size()!=list2.size())
	{
		return false;
	}
	for(int i=0;i<(int)list1.size();i++)
	{
		const CCADLayerInfo& info1 = list1.at(i);
		const CCADLayerInfo& info2 = list2.at(i);
		if(info1.bIsOn!=info2.bIsOn)
		{
			return false;
		}
		if( COLORREF(info1.cColor) != COLORREF(info2.cColor) )
		{
			return false;
		}
		if(info1.sName != info2.sName)
		{
			return false;
		}
	}
	return true;
}

void CCADFileDisplayInfo::SetMap( CString sMapFileName, CString sMapPathName )
{
	mLoadedLayers = CCADFileLayersManager::Instance().Load(sMapFileName,sMapPathName);	
	mCADInfo.sFileName = sMapFileName;
	mCADInfo.sPathName = sMapPathName;
	UpdateLayersInfo();
}

void CCADFileDisplayInfo::UpdateLayersInfo()
{
	mLayersInfo.clear();
	for(int i=0; i<mLoadedLayers->GetLayerCount(); i++) 
	{
		CCADLayer* pLayer = mLoadedLayers->GetLayer(i);
		CCADLayerInfo info;
		info.bIsOn  = pLayer->bIsOn;
		info.cColor = pLayer->cColor;
		info.sName =pLayer->sName;
		mLayersInfo.push_back(info);
	}
}
CCADInfo::CCADInfo() :dScale(0),dRotation(0),vOffset(0,0)
{
	sFileName = NO_MAP_STR;
	sPathName = _T("");
}

CCADLayerInfo::CCADLayerInfo()
{
	bIsOn = TRUE;
	cColor =  ARCColor3::White;
}