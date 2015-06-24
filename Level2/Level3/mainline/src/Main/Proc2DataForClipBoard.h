// Proc2DataForClipBoard.h: interface for the CProc2DataForClipBoard class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROC2DATAFORCLIPBOARD_H__379E3274_A076_424C_9176_41C34114ED35__INCLUDED_)
#define AFX_PROC2DATAFORCLIPBOARD_H__379E3274_A076_424C_9176_41C34114ED35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common\ARCVector.h"
#include "common\Path.h"
#include "Processor2.h"

typedef struct PROCESSOR2DATAFORCLIPBOARD
{
	//Processor2 Data
	ARCVector3 m_vLocation;					// x-y location (offset applied to shape)
	ARCVector3 m_vScale;					// x-y scale applied to shape
	double m_dRotation;						// rotation applied to shape
	//Shape Data
	CString m_strShapeName;
	//Processor Data
	CString ProcName;
	ProcessorClassList ProcType;
	Path m_location;
	Path In_Constr;
	Path Out_Constr;
	//Queue Data
	Path m_qCorners;
	BOOL bHasProcessor;
	//Baggage Device
	BOOL bIsBaggageDevice;
	Path m_pathBagMovingOn;
	Path m_pathBarrierSL;//ServiceLocation
    CProcessor2::CProcDispProperties m_dispProperties ;

}PROCESSOR2_DATA_FOR_CLIPBOARD;

enum PDFC_ELEMENT
{
	PDFCE_PROC2_LOCATION=1,
	PDFCE_PROC2_SCALE=2,
	PDFCE_PROC2_ROTATION=3,
	PDFCE_SHAPE_NAME=4,
	PDFCE_PROC2_HASPROC=5,
	PDFCE_PROC_NAME=6,
	PDFCE_PROC_TYPE=7,
	PDFCE_PROC_LOCATION=8,
	PDFCE_PROC_INCONSTR=9,
	PDFCE_PROC_OUTCONSTR=10,
	PDFCE_QUEUE_CORNERS=11,
	PDFCE_ISBAGAGE=12,
	PDFCE_PROC_PROCDISPPROPERTIES =13,
	PDFCE_BAGGAGE_PATHMOVINGON=14,
	PDFCE_BAGGAGE_BARRIER_LOCATION=15,
	
};

class CPDFCFormat
{
public:
	CString m_strPDFCFormat;
	std::vector<PROCESSOR2_DATA_FOR_CLIPBOARD*> m_vpPDFC;
	std::vector<int> m_vProcPos;
	
	CPDFCFormat(CString strSrc);
	CPDFCFormat();
	virtual ~CPDFCFormat();
public:
	BOOL IsBaggageProc(const CString &strSrc);
	BOOL GetOriginFromStr(ARCVector3& arcVector);
	int GetStrBetween2Char(const CString& strSrc,CString strFinded,CString& strResult);
	BOOL GetPointCountFromStr(const CString& strSrc,int& nCount);
	BOOL GetPathFromStr(const CString& strSrc,Path& path,PDFC_ELEMENT pdfcElement,double fFLoorHeight,double fOffsetX,double fOffsetY);
	BOOL GetStrFromStr(const CString &strSrc, CString &strDest);
	BOOL GetIntFromStr(const CString& strSrc,int& fDResult);
	BOOL GetStrAfterColon(const CString& strSrc,CString& strResult);
	BOOL GetDoubleFromStr(const CString& strSrc,double& fDResult);
	BOOL Parse(double fFLoorHeight,ARCVector3& arcVector);
	BOOL Get3DPointFromStr(const CString& strSrc,DistanceUnit* pPoint );
	BOOL GetARCVector3FromStr(const CString &strSrc,ARCVector3& arcVector3);
	BOOL HasProcFromStr(const CString& strSrc);
	BOOL GetProcElement(const CString& strSrc,PDFC_ELEMENT pdfcElement,CString& strResult);
	void Clear(BOOL bProcPos=TRUE);
	BOOL GetPDFCCount(int& nCount,BOOL bCheckVer=TRUE);
	BOOL GetProcStrFromIndex(CString& pStrDest,int nIndex);
	BOOL IsContainHeadStr(const CString& strSrc);
	int FindCharFromStr(const CString& strSrc,CString strFinded,std::vector<int>* pVPos=NULL,BOOL bFindOneof=FALSE);
	BOOL GetPDFCCountFromStr(const CString &strSrc,int& nCount, BOOL bCheckVer=TRUE);
	BOOL IsHeadStr(const CString& strSrc);
	CString GetStringHead();
	BOOL GetProcDispPropertiesStr(const CString &strSrc,CProcessor2::CProcDispProperties& arcVector3);
	
};

class CProc2DataForClipBoard  
{
public:
	CProc2DataForClipBoard();
	CProc2DataForClipBoard(PROCESSOR2DATAFORCLIPBOARD* pPDFC,int nCount,BOOL bAppend=FALSE);
	virtual ~CProc2DataForClipBoard();
public:	
	static CString m_strHead;
	CProc2DataForClipBoard(CPROCESSOR2LIST& proc2List,BOOL bAppend=FALSE);
	CString GetStringFromIndex(int nIndex);
	void Clear();
	CString ToString(const ARCVector3& arcVector,BOOL bContainHead=TRUE);


	std::vector<PROCESSOR2_DATA_FOR_CLIPBOARD*> m_vpPDFC;
};

#endif // !defined(AFX_PROC2DATAFORCLIPBOARD_H__379E3274_A076_424C_9176_41C34114ED35__INCLUDED_)
