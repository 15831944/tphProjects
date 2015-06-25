#include "stdafx.h"

#include "PictureInfo.h"
#include "tinyxml/tinyxml.h"
#include "common/ARCStringConvert.h"

const CString CPictureFileInfo::ExportString = _T("PictureInfo");

const static CString sFileNameTag =  _T("FileName");
const static CString sPathTag = _T("PathName");
const static CString sOffsetTag = _T("Offset");
const static CString sScaleTag = _T("Scale");
const static CString sRotateTag = _T("Rotate");
const static CString sbVisibel = _T("Visible");

CPictureFileInfo::CPictureFileInfo() :dScale(0),dRotation(0),vOffset(0,0),bVisible(TRUE)
{

}

void CPictureFileInfo::Export( const CPictureFileInfo& in,TiXmlElement* pElmParent )
{
	TiXmlElement* pPicElm = (TiXmlElement*)pElmParent->InsertEndChild( TiXmlElement(CPictureFileInfo::ExportString) );
	if(pPicElm)
	{
		pPicElm->SetAttribute(sFileNameTag, in.sFileName);
		pPicElm->SetAttribute(sPathTag, in.sPathName);
		pPicElm->SetAttribute(sOffsetTag, ARCStringConvert::toString(in.vOffset) );
		pPicElm->SetDoubleAttribute(sScaleTag, in.dScale);
		pPicElm->SetDoubleAttribute(sRotateTag, in.dRotation);
		pPicElm->SetAttribute(sbVisibel, in.bVisible);
	}
}

void CPictureFileInfo::Import( const TiXmlElement* pElm, CPictureFileInfo& output )
{
	int iValue;
	double dValue;
	const char* strValue;

	if( strValue =  pElm->Attribute(sFileNameTag) )
	{
		output.sFileName = strValue;
	}
	if( strValue = pElm->Attribute(sPathTag) )
	{
		output.sPathName = strValue;
	}
	if( strValue = pElm->Attribute(sOffsetTag) )
	{
		output.vOffset = ARCStringConvert::parseVector2(strValue);
	}
	if( pElm->Attribute(sScaleTag, &dValue) )
	{
		output.dScale = dValue;
	}
	if(pElm->Attribute(sRotateTag,&dValue) )
	{
		output.dRotation = dValue;
	}
	if(pElm->Attribute(sbVisibel,&iValue))
	{
		output.bVisible = iValue;
	}
}