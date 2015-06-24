// Proc2DataForClipBoard.cpp: implementation of the CProc2DataForClipBoard class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "Proc2DataForClipBoard.h"
#include <CommonData/Shape.h>
#include "engine\baggage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CString CProc2DataForClipBoard::m_strHead=
	"[ARCport Proc Clipboard Data(Version:1)]\n";

CProc2DataForClipBoard::CProc2DataForClipBoard()
{
}

CProc2DataForClipBoard::~CProc2DataForClipBoard()
{
	Clear();
}

CString CProc2DataForClipBoard::ToString(const ARCVector3& arcVector,BOOL bContainHead)
{
	CString strResult;
	if(bContainHead)
		strResult+=CProc2DataForClipBoard::m_strHead;
	CString strTemp;
	strTemp.Format("[0]Origin:%f,%f,%f[0]\n",arcVector[0],arcVector[1],arcVector[2]);
	strResult+=strTemp;

	int nCount=m_vpPDFC.size();
	for(int i=0;i<nCount;i++)
	{
		strResult+=GetStringFromIndex(i);
	}
	return strResult;
}

CProc2DataForClipBoard::CProc2DataForClipBoard(
	CPROCESSOR2LIST& proc2List, BOOL bAppend)
{
	if(bAppend==FALSE)
		Clear();
	int nCount=proc2List.size();
	for(int i=0;i<nCount;i++)
	{
		PROCESSOR2DATAFORCLIPBOARD* pNewPDFC=new PROCESSOR2DATAFORCLIPBOARD;
		m_vpPDFC.push_back(pNewPDFC);
		
		pNewPDFC->m_vLocation=proc2List[i]->GetLocation();
		pNewPDFC->m_vScale=proc2List[i]->GetScale();
		proc2List[i]->GetRotation(pNewPDFC->m_dRotation);
		pNewPDFC->bHasProcessor=FALSE;
		if(proc2List[i]->HasShape())
			pNewPDFC->m_strShapeName=proc2List[i]->GetShape()->Name();
		pNewPDFC->m_dispProperties = proc2List[i]->m_dispProperties;
		Processor* pProc=proc2List[i]->GetProcessor();
		if(pProc)
		{
			pNewPDFC->bHasProcessor=TRUE;
			char bufID[256];
			pProc->getID()->printID(bufID);
			pNewPDFC->ProcName=bufID;
			pNewPDFC->ProcType=(ProcessorClassList)(pProc->getProcessorType());
			
			Path* pPath;
			pPath=pProc->serviceLocationPath();	
			if(pPath)
				pNewPDFC->m_location=(*pPath);
			
			pPath=pProc->outConstraint();	
			if(pPath)
				pNewPDFC->Out_Constr=(*pPath);
			
			pPath=pProc->inConstraint();	
			if(pPath)
				pNewPDFC->In_Constr=(*pPath);
			
			pPath=pProc->queuePath();
			if(pPath)
				pNewPDFC->m_qCorners=(*pPath);

			pNewPDFC->bIsBaggageDevice=FALSE;
			if(pProc->getProcessorType()==BaggageProc)
			{
				pNewPDFC->bIsBaggageDevice=TRUE;
				BaggageProcessor* pBP=(BaggageProcessor*)pProc;
				pPath=pBP->GetPathBagMovingOn();
				if(pPath)
					pNewPDFC->m_pathBagMovingOn=*pPath;
				Barrier* pBarrier=pBP->GetBarrier();
				pPath=pBarrier->serviceLocationPath();
				if(pPath)
					pNewPDFC->m_pathBarrierSL=*pPath;
			}
		}
	}
	
	
}

CProc2DataForClipBoard::CProc2DataForClipBoard(
	PROCESSOR2DATAFORCLIPBOARD* pPDFC,int nCount,BOOL bAppend)
{
	if(bAppend==FALSE)
		Clear();
	
	for(int i=0;i<nCount;i++)
	{
		PROCESSOR2DATAFORCLIPBOARD* pNewPDFC=new PROCESSOR2DATAFORCLIPBOARD;
		*pNewPDFC=pPDFC[i];
		m_vpPDFC.push_back(pNewPDFC);
	}
	
}

void CProc2DataForClipBoard::Clear()
{
	int nCount=m_vpPDFC.size();
	for(int i=0;i<nCount;i++)
	{
		delete m_vpPDFC[i];
	}
	m_vpPDFC.clear();
}

CString CProc2DataForClipBoard::GetStringFromIndex(int nIndex)
{
	CString strTemp,strResult;
	//Proc2 Data
	strTemp.Format("\n[1]Proc2_Location:%f,%f,%f[1]\n",m_vpPDFC[nIndex]->m_vLocation[0],
		m_vpPDFC[nIndex]->m_vLocation[1],
		m_vpPDFC[nIndex]->m_vLocation[2]);
	strResult+=strTemp;
	
	strTemp.Format("[2]Proc2_Scale:%f,%f,%f[2]\n",m_vpPDFC[nIndex]->m_vScale[0],
		m_vpPDFC[nIndex]->m_vScale[1],
		m_vpPDFC[nIndex]->m_vScale[2]);
	strResult+=strTemp;
	
	strTemp.Format("[3]Proc2_Rotation:%f[3]\n",m_vpPDFC[nIndex]->m_dRotation);
	strResult+=strTemp;
	//Shape Data
	strTemp.Format("[4]Shape_Name:%s[4]\n",m_vpPDFC[nIndex]->m_strShapeName);
	strResult+=strTemp;
	
	strTemp.Format("[5]Proc2_HasProc:%d[5]\n",m_vpPDFC[nIndex]->bHasProcessor?1:0);
	strResult+=strTemp;
	
	if(m_vpPDFC[nIndex]->bHasProcessor==FALSE)	
		return strResult;
	//Proc Data
	strTemp.Format("[6]Proc_Name:%s[6]\n",m_vpPDFC[nIndex]->ProcName);
	strResult+=strTemp;
	
	strTemp.Format("[7]Proc_Type:%d[7]\n",m_vpPDFC[nIndex]->ProcType);
	strResult+=strTemp;
	
	int nCount;
	Point* pPtList;
	
	nCount=m_vpPDFC[nIndex]->m_location.getCount();
	pPtList=m_vpPDFC[nIndex]->m_location.getPointList();
	strTemp.Format("[8]Proc_Location(%d):",nCount);
	strResult+=strTemp;
	for(int i=0;i<nCount;i++)
	{
		strTemp.Format("[8-%d]Point:%f,%f,%f[8-%d]",i,
			pPtList[i].getX(),pPtList[i].getY(),
			pPtList[i].getZ(),i);
		strResult+=strTemp;
	}
	strResult+="[8]\n";
	
	nCount=m_vpPDFC[nIndex]->In_Constr.getCount();
	pPtList=m_vpPDFC[nIndex]->In_Constr.getPointList();
	strTemp.Format("[9]Proc_InConstr(%d):",nCount);
	strResult+=strTemp;
	for(int i=0;i<nCount;i++)
	{
		strTemp.Format("[9-%d]Point:%f,%f,%f[9-%d]",i,
			pPtList[i].getX(),pPtList[i].getY(),
			pPtList[i].getZ(),i);
		strResult+=strTemp;
	}	
	strResult+="[9]\n";
	
	nCount=m_vpPDFC[nIndex]->Out_Constr.getCount();
	pPtList=m_vpPDFC[nIndex]->Out_Constr.getPointList();
	strTemp.Format("[10]Proc_OutConstr(%d):",nCount);
	strResult+=strTemp;
	for(int i=0;i<nCount;i++)
	{
		strTemp.Format("[10-%d]Point:%f,%f,%f[10-%d]",i,
			pPtList[i].getX(),pPtList[i].getY(),
			pPtList[i].getZ(),i);
		strResult+=strTemp;
	}
	strResult+="[10]\n";
	
	nCount=m_vpPDFC[nIndex]->m_qCorners.getCount();
	pPtList=m_vpPDFC[nIndex]->m_qCorners.getPointList();
	strTemp.Format("[11]Queue_Corners(%d):",nCount);
	strResult+=strTemp;
	for(int i=0;i<nCount;i++)
	{
		strTemp.Format("[11-%d]Point:%f,%f,%f[11-%d]",i,
			pPtList[i].getX(),pPtList[i].getY(),
			pPtList[i].getZ(),i);
		strResult+=strTemp;
	}	
	strResult+="[11]\n";
	
	strTemp.Format("[12]Proc_IsBaggage:%d[12]\n",m_vpPDFC[nIndex]->bIsBaggageDevice? 1:0);
	strResult+=strTemp;

	strResult = strResult + "[13]ProcDispProperties:";
	for(int i = 0;i < PDP_ARRAY_SIZE; i++)
	{
		strTemp.Format("%d",m_vpPDFC[nIndex]->m_dispProperties.bDisplay[i]);
		strResult = strResult + strTemp + ",";
		ARCColor3 tmpColor = m_vpPDFC[nIndex]->m_dispProperties.color[i];
		strTemp.Format("%d,%d,%d",tmpColor[0],tmpColor[1],tmpColor[2]);		
		strResult =strResult + strTemp +"@";		
	}
	strResult+="[13]\n";	
	
	if(m_vpPDFC[nIndex]->bIsBaggageDevice==FALSE)	
		return strResult;

	nCount=m_vpPDFC[nIndex]->m_pathBagMovingOn.getCount();
	pPtList=m_vpPDFC[nIndex]->m_pathBagMovingOn.getPointList();
	strTemp.Format("[14]Proc_BaggageBMO(%d):",nCount);
	strResult+=strTemp;
	for(int i=0;i<nCount;i++)
	{
		strTemp.Format("[14-%d]Point:%f,%f,%f[14-%d]",i,
			pPtList[i].getX(),pPtList[i].getY(),
			pPtList[i].getZ(),i);
		strResult+=strTemp;
	}	
	strResult+="[14]\n";	

	nCount=m_vpPDFC[nIndex]->m_pathBarrierSL.getCount();
	pPtList=m_vpPDFC[nIndex]->m_pathBarrierSL.getPointList();
	strTemp.Format("[15]Proc_BaggageBSL(%d):",nCount);
	strResult+=strTemp;
	for(int i=0;i<nCount;i++)
	{
		strTemp.Format("[15-%d]Point:%f,%f,%f[15-%d]",i,
			pPtList[i].getX(),pPtList[i].getY(),
			pPtList[i].getZ(),i);
		strResult+=strTemp;
	}	
	strResult+="[15]\n";	
	
	
	return strResult;
}

//---------------------------------CPDFCFormat------------------------------------------------------
CPDFCFormat::CPDFCFormat()
{
}

CPDFCFormat::~CPDFCFormat()
{
}

CPDFCFormat::CPDFCFormat(CString strSrc)
{
	m_strPDFCFormat=strSrc;
}

BOOL CPDFCFormat::GetProcStrFromIndex(CString& pStrDest, int nIndex)
{
	if(nIndex<0||nIndex>=static_cast<int>(m_vProcPos.size()))
		return FALSE;
	pStrDest=m_strPDFCFormat.Mid(m_vProcPos[nIndex],
		m_vProcPos[nIndex+1]-m_vProcPos[nIndex]);

	return TRUE;
}

BOOL CPDFCFormat::IsContainHeadStr(const CString &strSrc)
{
	if(strSrc.Find(CProc2DataForClipBoard::m_strHead)==-1)
		return FALSE;
	else
		return TRUE;
	
}

int CPDFCFormat::FindCharFromStr(
	const CString &strSrc, CString strFinded,std::vector<int>* pVPos,BOOL bFindOneof)
{
	int nResult=strSrc.Find(strFinded);
	if(bFindOneof)
		return nResult;
	
	if(pVPos)
		pVPos->clear();
	int nCount=0;
	while(nResult!=-1)
	{
		nCount++;
		if(pVPos)
			pVPos->push_back(nResult);
		nResult=strSrc.Find(strFinded,nResult+strFinded.GetLength());
	}
	return nCount;
}

BOOL CPDFCFormat::GetPDFCCountFromStr(
	const CString &strSrc,int& nCount, BOOL bCheckVer)
{
	if(bCheckVer)
	{
		if(IsContainHeadStr(strSrc)==FALSE)
			return FALSE;
	}
	nCount=FindCharFromStr(strSrc,CString("\n\n"),&m_vProcPos);
	m_vProcPos.insert(m_vProcPos.end(),strSrc.GetLength());
	if(nCount==0)
		return FALSE;
	return TRUE;
}



BOOL CPDFCFormat::IsHeadStr(const CString& strSrc)
{
	return (strSrc==CProc2DataForClipBoard::m_strHead);
}

CString CPDFCFormat::GetStringHead()
{
	return CProc2DataForClipBoard::m_strHead;
}

BOOL CPDFCFormat::GetPDFCCount(int &nCount, BOOL bCheckVer)
{
	return GetPDFCCountFromStr(m_strPDFCFormat,nCount,bCheckVer);
}

void CPDFCFormat::Clear(BOOL bProcPos)
{
	int nCount=m_vpPDFC.size();
	for(int i=0;i<nCount;i++)
	{
		delete m_vpPDFC[i];
	}
	m_vpPDFC.clear();
	if(bProcPos)
		m_vProcPos.clear();
	
}

BOOL CPDFCFormat::GetProcElement(const CString &strSrc, PDFC_ELEMENT pdfcElement,CString& strResult)
{
	CString strFind;
	strFind.Format("[%d]",pdfcElement);
	int nReturn=GetStrBetween2Char(strSrc,strFind,strResult);
	if(nReturn==1)
	{
		if(pdfcElement>PDFCE_ISBAGAGE)
		{
			if(IsBaggageProc(strSrc))
				return FALSE;
			else
				return TRUE;
		}	
		else if(pdfcElement>PDFCE_PROC2_HASPROC)
		{
			if(HasProcFromStr(strSrc))
				return FALSE;
			else
				return TRUE;
		}
		else
		 return FALSE;
	}
	else if(nReturn==2)
		return FALSE;
	
	return TRUE;

}

BOOL CPDFCFormat::HasProcFromStr(const CString &strSrc)
{
	CString strTemp;
	if(GetProcElement(strSrc,PDFCE_PROC2_HASPROC,strTemp))
	{
		GetStrAfterColon(strTemp,strTemp);
		if(strTemp.Find('1')!=-1)
			return TRUE;
	}
	return FALSE;
	
}

BOOL CPDFCFormat::IsBaggageProc(const CString &strSrc)
{
	CString strTemp;
	if(GetProcElement(strSrc,PDFCE_ISBAGAGE,strTemp))
	{
		GetStrAfterColon(strTemp,strTemp);
		if(strTemp.Find('1')!=-1)
			return TRUE;
	}
	return FALSE;
}

BOOL CPDFCFormat::GetARCVector3FromStr(
	const CString &strSrc,ARCVector3& arcVector3)
{
	CString strResult;
	if(!GetStrAfterColon(strSrc,strResult))
		return FALSE;
	DistanceUnit du[3];
	Get3DPointFromStr(strResult,du);
	ARCVector3 tempARCV(du[0],du[1],du[2]);
	arcVector3=tempARCV;
	return TRUE;	

}

BOOL CPDFCFormat::GetProcDispPropertiesStr(const CString &strSrc,CProcessor2::CProcDispProperties& arcVector3)
{
	CString strResult;
	CString strTemp;
	if(!GetStrAfterColon(strSrc,strResult))
		return FALSE;
	

	for(int i = 0;i < PDP_ARRAY_SIZE; i++)
	{
		int nPos = strResult.Find('@',0); 
		if (-1 != nPos)
		{
			strTemp = strResult.Left(nPos);	
			strResult = strResult.Right(strResult.GetLength() - nPos - 1);			
		}
		else
		{
			strTemp = strResult;
		}
		int a,b,c;
		sscanf( strTemp, "%d,%d,%d,%d", &(arcVector3.bDisplay[i]),&a,&b,&c);		
		ARCColor3 tmpColor(a,b,c);
		arcVector3.color[i] = tmpColor;
	}		

	return TRUE;	
}

BOOL CPDFCFormat::Get3DPointFromStr(const CString &strSrc, DistanceUnit *pPoint)
{
	CString strFind=",";
	std::vector<int> vPos;
	int nCount=FindCharFromStr(strSrc,strFind,&vPos);
	if(nCount!=2)
		return FALSE;
	CString strValue[3];
	vPos.insert(vPos.begin(),-1);
	vPos.insert(vPos.end(),strSrc.GetLength());
	for(int i=0;i<3;i++)
		strValue[i]=strSrc.Mid(vPos[i]+1,vPos[i+1]-vPos[i]-1);
	for(int j=0;j<3;j++)
	{
		pPoint[j]=atof(strValue[j].GetBuffer(strValue[j].GetLength()));
		strValue[j].ReleaseBuffer();
	}
	return TRUE;
}

BOOL CPDFCFormat::Parse(double fFLoorHeight,ARCVector3& arcVector)
{
		int nCoutProc2;
		if(!GetPDFCCount(nCoutProc2,TRUE))
			return FALSE;
		ARCVector3 originPos;
		if(!GetOriginFromStr(originPos))
			return FALSE;

		Clear(FALSE);
		CString strProc2,strProc2Element;
		for(int i=0;i<nCoutProc2;i++)
		{
			PROCESSOR2_DATA_FOR_CLIPBOARD* pNewPDFC=new PROCESSOR2_DATA_FOR_CLIPBOARD;
			m_vpPDFC.push_back(pNewPDFC);
			
			if(!GetProcStrFromIndex(strProc2,i))
			{
				Clear(FALSE);
				return FALSE;
			}
			double fOffsetX=0,fOffsetY=0,fOffsetXOrigin=0,fOffsetYOrigin=0;
			for(int j=PDFCE_PROC2_LOCATION;j<=PDFCE_BAGGAGE_BARRIER_LOCATION;j++)
			{
				if(!GetProcElement(strProc2,(PDFC_ELEMENT)j,strProc2Element))
				{
					Clear(FALSE);
					return FALSE;
				}
				BOOL bReturn=TRUE;
				switch(j)
				{
				case PDFCE_PROC2_LOCATION:
					{
					bReturn=GetARCVector3FromStr(strProc2Element,pNewPDFC->m_vLocation);
					//if paste the process to the new position where the right mouse clicked,
					//use the following code
//					fOffsetXOrigin=pNewPDFC->m_vLocation[0]-originPos[0];
//					fOffsetYOrigin=pNewPDFC->m_vLocation[1]-originPos[1];
					
//					ARCVector3 tempARCV(arcVector[0]+fOffsetXOrigin,arcVector[1]+fOffsetYOrigin,0);
					//calc offset value
//					fOffsetX=tempARCV[0]-pNewPDFC->m_vLocation[0];
//					fOffsetY=tempARCV[1]-pNewPDFC->m_vLocation[1];
//					pNewPDFC->m_vLocation=tempARCV;
					}
					break;
				case PDFCE_PROC2_SCALE:
					bReturn=GetARCVector3FromStr(strProc2Element,pNewPDFC->m_vScale);
					break;
				case PDFCE_PROC2_ROTATION:
					bReturn=GetDoubleFromStr(strProc2Element,pNewPDFC->m_dRotation);
					break;
				case PDFCE_SHAPE_NAME:
					bReturn=GetStrFromStr(strProc2Element,pNewPDFC->m_strShapeName);
					break;
				case PDFCE_PROC2_HASPROC:
					bReturn=GetIntFromStr(strProc2Element,pNewPDFC->bHasProcessor);
					break;
				case PDFCE_PROC_NAME:
					if(pNewPDFC->bHasProcessor)
						bReturn=GetStrFromStr(strProc2Element,pNewPDFC->ProcName);
					break;
				case PDFCE_PROC_TYPE:
					if(pNewPDFC->bHasProcessor)
						bReturn=GetIntFromStr(strProc2Element,(int&)pNewPDFC->ProcType);
					break;
				case PDFCE_PROC_LOCATION:
					if(pNewPDFC->bHasProcessor)
						bReturn=GetPathFromStr(strProc2Element,pNewPDFC->m_location,PDFCE_PROC_LOCATION,fFLoorHeight,fOffsetX,fOffsetY);
					break;
				case PDFCE_PROC_INCONSTR:
					if(pNewPDFC->bHasProcessor)
						bReturn=GetPathFromStr(strProc2Element,pNewPDFC->In_Constr,PDFCE_PROC_INCONSTR,fFLoorHeight,fOffsetX,fOffsetY);
					break;
				case PDFCE_PROC_OUTCONSTR:
					if(pNewPDFC->bHasProcessor)
						bReturn=GetPathFromStr(strProc2Element,pNewPDFC->Out_Constr,PDFCE_PROC_OUTCONSTR,fFLoorHeight,fOffsetX,fOffsetY);
					break;
				case PDFCE_QUEUE_CORNERS:
					if(pNewPDFC->bHasProcessor)
						bReturn=GetPathFromStr(strProc2Element,pNewPDFC->m_qCorners,PDFCE_QUEUE_CORNERS,fFLoorHeight,fOffsetX,fOffsetY);
					break;
				case PDFCE_ISBAGAGE:
					if(pNewPDFC->bHasProcessor)
						bReturn=GetIntFromStr(strProc2Element,pNewPDFC->bIsBaggageDevice);
					break;
				case PDFCE_BAGGAGE_PATHMOVINGON:
					if(pNewPDFC->bHasProcessor&&pNewPDFC->bIsBaggageDevice)
						bReturn=GetPathFromStr(strProc2Element,pNewPDFC->m_pathBagMovingOn,PDFCE_BAGGAGE_PATHMOVINGON,fFLoorHeight,fOffsetX,fOffsetY);
					break;
				case PDFCE_BAGGAGE_BARRIER_LOCATION:
					if(pNewPDFC->bHasProcessor&&pNewPDFC->bIsBaggageDevice)
						bReturn=GetPathFromStr(strProc2Element,pNewPDFC->m_pathBarrierSL,PDFCE_BAGGAGE_BARRIER_LOCATION,fFLoorHeight,fOffsetX,fOffsetY);
					break;
				case PDFCE_PROC_PROCDISPPROPERTIES:
					bReturn=GetProcDispPropertiesStr(strProc2Element,pNewPDFC->m_dispProperties);
					break;					
				}
				if(bReturn==FALSE)
				{
					Clear(FALSE);
					return FALSE;
				}
			}
		}
		return TRUE;	
}

BOOL CPDFCFormat::GetDoubleFromStr(const CString &strSrc, double &fDResult)
{
	CString strResult;
	if(!GetStrAfterColon(strSrc,strResult))
		return FALSE;
	fDResult=atof(strResult.GetBuffer(strResult.GetLength()));
	return TRUE;
}

BOOL CPDFCFormat::GetStrAfterColon(const CString &strSrc, CString &strResult)
{
	CString strFind=":";
	int nPos=FindCharFromStr(strSrc,strFind,NULL,TRUE);
	if(nPos==-1)
		return FALSE;
	strResult=strSrc.Mid(nPos+1);
	return TRUE;	
}

BOOL CPDFCFormat::GetIntFromStr(const CString &strSrc, int &fDResult)
{
	CString strResult;
	if(!GetStrAfterColon(strSrc,strResult))
		return FALSE;
	fDResult=atoi(strResult.GetBuffer(strResult.GetLength()));
	return TRUE;
}

BOOL CPDFCFormat::GetStrFromStr(const CString &strSrc, CString &strDest)
{
	CString strResult;
	if(!GetStrAfterColon(strSrc,strResult))
		return FALSE;
	strDest=strResult;
	return TRUE;

}

BOOL CPDFCFormat::GetPathFromStr(const CString &strSrc, Path &path,PDFC_ELEMENT pdfcElement,double fFLoorHeight,double fOffsetX,double fOffsetY)
{
	int nCountPoint;
	if(!GetPointCountFromStr(strSrc,nCountPoint))
		return FALSE;

	CString strPre,strResult,strResult2;
	Point* pPt=new Point[nCountPoint];
	for(int i=0;i<nCountPoint;i++)
	{
		strPre.Format("[%d-%d]",pdfcElement,i);
		int nReturn=GetStrBetween2Char(strSrc,strPre,strResult);
		if(nReturn!=0)
			return FALSE;
		if(!GetStrAfterColon(strResult,strResult2))
			return FALSE;
		DistanceUnit du[3];
		if(!Get3DPointFromStr(strResult2,du))
			return FALSE;
		du[2]=fFLoorHeight;
		du[0]+=fOffsetX;
		du[1]+=fOffsetY;
		pPt[i].setPoint(du[0],du[1],du[2]);
	}
	Path tempPath;
	tempPath.init(nCountPoint,pPt);
	path=tempPath;
	delete[] pPt;
	return TRUE;
}

BOOL CPDFCFormat::GetPointCountFromStr(const CString &strSrc, int &nCount)
{
	CString strFind="(";
	int nPosB=FindCharFromStr(strSrc,strFind,NULL,TRUE);
	if(nPosB==-1)
		return FALSE;
	strFind=")";
	int nPosE=FindCharFromStr(strSrc,strFind,NULL,TRUE);
	if(nPosE==-1)
		return FALSE;
	strFind=strSrc.Mid(nPosB+1,nPosE-nPosB-1);
	nCount=atoi(strFind.GetBuffer(strFind.GetLength()));
	return TRUE;
}

int CPDFCFormat::GetStrBetween2Char(const CString &strSrc, CString strFinded, CString& strResult)
{
	strResult="";
	std::vector<int> vPos;
	int nCount=FindCharFromStr(strSrc,strFinded,&vPos);
	if(nCount==0)
	{
		strResult="";
		return 1;
		
	}
	else if(nCount!=2)
	{
		return 2;//Data From Clipboard is bad
	}
	else
	{
		strResult=strSrc.Mid(vPos[0],vPos[1]-vPos[0]);
		return 0;
	}
}

BOOL CPDFCFormat::GetOriginFromStr(ARCVector3& arcVector)
{
	CString strFinded="[0]";
	CString strResult;
	if(GetStrBetween2Char(m_strPDFCFormat,strFinded,strResult)!=0)
		return FALSE;

	if(!GetARCVector3FromStr(strResult,arcVector))
		return FALSE;
	
	return TRUE;
}
