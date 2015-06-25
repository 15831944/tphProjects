#include "StdAfx.h"
#include ".\dispproperties.h"

CDispProperties::CDispProperties(void)
{
	//DSPTYPE_NAME = 0,
	//	DSPTYPE_SHAPE,
	//	DSPTYPE_QUEUE,
	//	DSPTYPE_INCONS,
	//	DSPTYPE_OUTCONS,
	//	DSPTYPE_DIRECTINARROW,


		mDefaultDispPropMap[0] = DispProp(TRUE,ARCColor3( 0, 0, 0 ));
		mDefaultDispPropMap[1] = DispProp(TRUE,ARCColor3( 55, 145, 170 ));
		mDefaultDispPropMap[2] = DispProp(TRUE,ARCColor3( 64, 210, 64 ));
		mDefaultDispPropMap[3] = DispProp(TRUE,ARCColor3( 255, 100, 0 ));
		mDefaultDispPropMap[4] = DispProp(TRUE,ARCColor3( 230, 210, 25 ));
		mDefaultDispPropMap[5] = DispProp(TRUE,ARCColor3( 0, 100, 200  ));
		//mDefaultDispPropMap[CAircraftElmentShapeDisplay::DSPTYPE_NAME] = DispProp(TRUE,ARCColor3( 0, 0, 0 ));
		//mDefaultDispPropMap[CAircraftElmentShapeDisplay::DSPTYPE_SHAPE] = DispProp(TRUE,ARCColor3( 55, 145, 170 ));
		//mDefaultDispPropMap[CAircraftElmentShapeDisplay::DSPTYPE_QUEUE] = DispProp(TRUE,ARCColor3( 64, 210, 64 ));
		//mDefaultDispPropMap[CAircraftElmentShapeDisplay::DSPTYPE_INCONS] = DispProp(TRUE,ARCColor3( 255, 100, 0 ));
		//mDefaultDispPropMap[CAircraftElmentShapeDisplay::DSPTYPE_OUTCONS] = DispProp(TRUE,ARCColor3( 230, 210, 25 ));
		//mDefaultDispPropMap[CAircraftElmentShapeDisplay::DSPTYPE_DIRECTINARROW] = DispProp(TRUE,ARCColor3( 0, 100, 200  ));


		//[AreasColor] = ARCColor3( 170, 25, 25 );
		//m_colorRef[PortalsColor] = ARCColor3( 235, 185, 30 );
		//m_colorRef[ProcShapeColor] = ARCColor3( 55, 145, 170 );
		//m_colorRef[ProcServLocColor] = ARCColor3( 0, 100, 200 );
		//m_colorRef[ProcQueueColor] = ARCColor3( 64, 210, 64 );
		//m_colorRef[ProcInConstraintColor] = ARCColor3( 255, 100, 0 );
		//m_colorRef[ProcOutConstraintColor] = ARCColor3( 230, 210, 25 );
		//m_colorRef[ProcNameColor] = ARCColor3( 0, 0, 0 );


		//m_boolVal[ProcShapeOn] = TRUE;
		//m_boolVal[ProcServLocOn] = TRUE;
		//m_boolVal[ProcQueueOn] = TRUE;
		//m_boolVal[ProcInConstraintOn] = TRUE;
		//m_boolVal[ProcOutConstraintOn] = TRUE;
		//m_boolVal[ProcNameOn] = FALSE;
	
}

CDispProperties::~CDispProperties(void)
{
}

void CDispProperties::ParseDSP( CString str )
{
	int iIndex = 0;
	int nItemIndex = 0;
	int nDspIndex = -1;
	while(iIndex< str.GetLength())
	{
		int nextIdx = str.Find(' ',iIndex);
		if(nextIdx<0)
		{
			nextIdx = str.GetLength();
		}
		CString strItem  = str.Mid(iIndex,nextIdx-iIndex);
		if(nItemIndex==0) //index
		{
			nDspIndex  = atoi(strItem.GetString());			
		}
		if(nItemIndex ==1)//bool
		{
			mDispPropMap[nDspIndex].bOn = atoi(strItem.GetString()); 
		}
		if(nItemIndex ==2) //color
		{
			mDispPropMap[nDspIndex].cColor = (COLORREF)atoi(strItem.GetString());
		}

		iIndex = nextIdx+1;
		nItemIndex++;
	}
}
const static char SplitChar = ';';
CString CDispProperties::ToString() const
{
	CString strRet;
	for(MapType::const_iterator itr=mDispPropMap.begin();itr!=mDispPropMap.end();++itr)
	{
		CString str;
		str.Format(_T("%d %d %d;"),itr->first,itr->second.bOn,(COLORREF)itr->second.cColor);
		strRet+=str;
	}
	return strRet;
}


void CDispProperties::FromString( CString str )
{
	int iIndex=0;
	while (iIndex<str.GetLength())
	{
		int nextIdx = str.Find(SplitChar,iIndex);
		if(nextIdx<0)
		{
			nextIdx = str.GetLength();
		}
		ParseDSP( str.Mid(iIndex,nextIdx-iIndex) );
		iIndex = nextIdx+1;
	}
}

DispProp& CDispProperties::operator[]( int idx )
{
	if(mDispPropMap.find(idx) != mDispPropMap.end())
		return mDispPropMap[idx];
	else
		return getDefault( idx );

}

DispProp& CDispProperties::getDefault( int idx )
{
	if(mDefaultDispPropMap.find(idx) != mDefaultDispPropMap.end())
	{
		mDispPropMap[idx] = mDefaultDispPropMap[idx];
		return mDispPropMap[idx];
	}
	else
	{
		ASSERT(0);
		mDispPropMap[idx] = DispProp(TRUE,ARCColor3( 0, 0, 0 ));
		return mDispPropMap[idx];
	}
}