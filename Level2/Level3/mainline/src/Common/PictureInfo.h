#pragma once
#include <common/ARCVector.h>
class TiXmlElement;
class CPictureFileInfo
{
public:
	CString sFileName;
	CString sPathName;
	ARCVector2 vOffset;
	double dScale;
	double dRotation;
	
	BOOL bVisible;

	CPictureFileInfo();



public:
	static void Export(const CPictureFileInfo& in,TiXmlElement* pElmParent );
	static void Import(const TiXmlElement* pElm, CPictureFileInfo& info );
	const static CString ExportString;

};