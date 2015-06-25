#include "StdAfx.h"
#include ".\arcstringconvert.h"

#include "./ARCVector.h"

#include "path.h"
#include "Path2008.h"
#include <sstream>
#include <string>
#include <iosfwd>
#include <stdlib.h>



#define PATH2008_VERSION 100

CString ARCStringConvert::toString( const ARCVector3& vec3)
{
	CString strRet;
	strRet.Format("%f %f %f", vec3.x, vec3.y, vec3.z);
	return strRet;
}

CString ARCStringConvert::toString( const ARCVector2& vec2)
{
	CString strRet;
	strRet.Format("%f %f", vec2.x, vec2.y);
	return strRet;
}

CString ARCStringConvert::toString( const int& d )
{
	CString strRet;
	strRet.Format("%d",d);
	return strRet;
}

CString ARCStringConvert::toString( const Path& _path)
{
	CString strRet;
	int nCount = _path.getCount();
	strRet.Format(_T("%d"),nCount);
	CString strPoint;
	for(int i=0;i<nCount;i++)
	{
		Point pt = _path.getPoint(i);
		strPoint.Format(_T(" %f,%f,%f"),pt.getX(),pt.getY(),pt.getZ());
		strRet+=strPoint;
	}
	return strRet;
}

CString ARCStringConvert::toString( const std::vector<bool>& blist )
{
	CString strRet;
	int nCount = (int)blist.size();
	strRet.Format(_T("%d"),nCount);
	CString strPoint;
	for(int i=0;i<nCount;i++)
	{
		bool b = blist.at(i);
		strPoint.Format(_T(" %d"),b?1:0);
		strRet+=strPoint;
	}
	return strRet;
}

CString ARCStringConvert::toString( const double& d )
{
	CString strRet;
	strRet.Format("%f",d);
	return strRet;
}

CString ARCStringConvert::toDBString( const CPath2008& path )
{
	CString strPath;
	int nPointCount = path.getCount();

	CString strHead;
	strHead.Format(_T("%d;%d;"),PATH2008_VERSION,nPointCount);
	strPath += strHead; 

	CString strPoint;
	for (int nPoint = 0; nPoint < nPointCount; ++nPoint)
	{
		const CPoint2008& pt = path[nPoint];
		strPoint.Format(_T("%f,%f,%f;"),pt.getX(),pt.getY(),pt.getZ());

		strPath += strPoint;
	}
	return strPath;
}

ARCVector3 ARCStringConvert::parseVector3( const CString& vec3str )
{
	ARCVector3 vecRet;
	CString strRemain = vec3str;

	//x
	int i = strRemain.Find(" ");
	if(i>=0)
	{
		vecRet.x = atof( strRemain.Left(i).GetString() );
		int nLeft = strRemain.GetLength() - i -1;
		strRemain = strRemain.Right(nLeft);
	}

	//y
	i = strRemain.Find(" ");
	if(i>=0)
	{
		vecRet.y  = atof( strRemain.Left(i).GetString() );
		int nLeft = strRemain.GetLength() - i -1;
		strRemain = strRemain.Right(nLeft);
	}
	//z
	vecRet.z = atof( strRemain.GetString());	
	return vecRet;
	
}

ARCVector2 ARCStringConvert::parseVector2( const CString& vec2str )
{
	ARCVector2 vecRet;
	CString strRemain = vec2str;

	//x
	int i = strRemain.Find(" ");
	if(i>=0)
	{
		vecRet.x = atof( strRemain.Left(i).GetString() );
		int nLeft = strRemain.GetLength() - i -1;
		strRemain = strRemain.Right(nLeft);
	}

	//y
	vecRet.y = atof( strRemain.GetString() );
	return vecRet;
}

bool ARCStringConvert::isNumber( const CString& val, double& fout )
{
	std::istringstream str(val.GetString());	
	str >> fout;
	return !str.fail() && str.eof();
}
#define isNumeric(x)  ((x>=48&&x<=57)||x==43||x==45||x==46)

bool ARCStringConvert::isAllNumeric( const char* buf )
{
	int len = strlen(buf);
	for(int i=0; i<len; i++) {
		if(!isNumeric(buf[i]))
			return false;
	}
	return true;
}

void ARCStringConvert::NextAlphaNumeric( char* buf )
{
	int len = strlen(buf);
	if(len==1) {
		if(buf[0]=='Z')
			strcpy(buf, "AA");
		else
			buf[0]++;
	}
	else if(len==2) {
		if(buf[1]=='Z') {
			if(buf[0]!='Z') {
				buf[0]++;
				buf[1]='A';
			}
			else
				strcat(buf, "_COPY");
		}
		else {
			buf[1]++; //increment last
		}
	}
	else {
		strcat(buf, "_COPY");
	}
}

Path ARCStringConvert::parsePath( CString str )
{
	const static char divChar = ' ';
	const int nStrLen = str.GetLength();
	//Count
	int nPrePos = 0;
	int nCurDivPos = str.Find(divChar,nPrePos);

	if(nCurDivPos<0)nCurDivPos=nStrLen;
	int nCount = atoi(str.Mid(nPrePos,nCurDivPos-nPrePos));
	nPrePos=nCurDivPos+1;

	int nRealCount = 0;
	std::vector<Point> vPts;
	while(nPrePos<str.GetLength())
	{
		int nCurDivPos = str.Find(divChar,nPrePos);
		if(nCurDivPos<0)nCurDivPos = nStrLen;
		Point pt = paresePoint(str.Mid(nPrePos,nCurDivPos-nPrePos));	
		vPts.push_back(pt);

		nPrePos=nCurDivPos+1;
	}
	ASSERT(vPts.size()==nCount);

	Path ret;
	if(vPts.size())
	{
		ret.init(vPts.size(),&vPts[0]);
	}
	return ret;
	
}

static CString GetFirstString( CString& strString, const CString& strSeparator )
{
	CString strRet = _T("");
	int nPos = strString.Find(strSeparator);
	if (nPos >= 0)
	{
		strRet = strString.Left(nPos);

		strString = strString.Mid(nPos+1,strString.GetLength());
	}

	return strRet;

}


bool ARCStringConvert::parsePathFromDBString( CPath2008& path, CString strPath )
{
	if(strPath.GetLength() == 0)
	{
		ASSERT(FALSE);
		return false;
	}

	//version;
	CString strVersion = GetFirstString(strPath,_T(";"));
	if(atoi(strVersion) != PATH2008_VERSION)
	{
		ASSERT(FALSE);
		return false;
	}

	//point count
	CString strCount = GetFirstString(strPath,_T(";"));
	int nPointCount = atoi(strCount);

	if(nPointCount <= 0 )
		return false;

	path.init(nPointCount);
	for (int nPoint = 0; nPoint < nPointCount; ++nPoint)
	{
		CString strPoint = GetFirstString(strPath,_T(";"));
		ASSERT(strPoint.GetLength() > 0);

		CString strX = GetFirstString(strPoint, _T(","));
		CString strY = GetFirstString(strPoint, _T(","));
		CString strZ = strPoint;

		path[nPoint] = CPoint2008(atof(strX),atof(strY),atof(strZ));
	}

	return true;

}
Point ARCStringConvert::paresePoint( CString str)
{
	Point pt(0,0,0);
	const static char divChar = ',';
	const int nStrLen = str.GetLength();
	//x	
	do 
	{
		int nPrePos = 0;
		int nCurDivPos = str.Find(divChar,nPrePos);	
		if(nCurDivPos<0)nCurDivPos=nStrLen;
		double tempf = atof(str.Mid(nPrePos,nCurDivPos-nPrePos));
		pt.setX(tempf);

		nPrePos=nCurDivPos+1;
		if(nPrePos>=nStrLen)break;

		nCurDivPos = str.Find(divChar,nPrePos);	
		if(nCurDivPos<0)nCurDivPos=nStrLen;
		tempf = atof(str.Mid(nPrePos,nCurDivPos-nPrePos));
		pt.setY(tempf);

		nPrePos=nCurDivPos+1;
		if(nPrePos>=nStrLen)break;

		nCurDivPos = str.Find(divChar,nPrePos);	
		if(nCurDivPos<0)nCurDivPos=nStrLen;
		tempf = atof(str.Mid(nPrePos,nCurDivPos-nPrePos));
		pt.setZ(tempf);

	} while (false);
	
	return pt;
}

std::vector<bool> ARCStringConvert::parseBoolList( CString str)
{
	const static char divChar = ' ';
	const int nStrLen = str.GetLength();
	//Count
	int nPrePos = 0;
	int nCurDivPos = str.Find(divChar,nPrePos);

	if(nCurDivPos<0)nCurDivPos=nStrLen;
	int nCount = atoi(str.Mid(nPrePos,nCurDivPos-nPrePos));
	nPrePos=nCurDivPos+1;

	int nRealCount = 0;
	std::vector<bool> vPts;
	while(nPrePos<str.GetLength())
	{
		int nCurDivPos = str.Find(divChar,nPrePos);
		if(nCurDivPos<0)nCurDivPos = nStrLen;
		bool b = (atoi(str.Mid(nPrePos,nCurDivPos-nPrePos))!=0);	
		vPts.push_back(b);

		nPrePos=nCurDivPos+1;
	}
	ASSERT(vPts.size()==nCount);	
	return vPts;
}






