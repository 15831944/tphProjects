#include "StdAfx.h"
#include ".\onetooneprocess.h"


OneToOneProcess::OneToOneProcess(ALTObjectID altObject,ALTObjectIDList altList)
:m_altObject(altObject)
,m_altList(altList)
{
}

OneToOneProcess::~OneToOneProcess(void)
{
}

bool OneToOneProcess::GetOneToOneObject( ALTObjectID& resultObject,int _nDestIdLength )
{
	int nDeltaFromLeaf = 0;

	ALTObjectIDList arrayResult;
	while( m_altList.size() > 0 )
	{ 
		int nCount = (int)m_altList.size();
		if( nCount == 1 )
		{
			resultObject = m_altList.at( 0 );
			return true;
		}

		ALTObjectIDList arrayTemp( m_altList );
		m_altList.clear();
		char szSourceLevelName[128];
		int nSourceIdLength = m_altObject.idLength();
		nSourceIdLength -= nDeltaFromLeaf + 1;
		if( nSourceIdLength == 0 )
			break;	

		strcpy(szSourceLevelName,m_altObject.at( nSourceIdLength ).c_str());
		nCount = (int)arrayTemp.size();
		for( int i=0; i<nCount; i++ )
		{
			ALTObjectID objID = arrayTemp.at( i );

			int nThisIdLenght = objID.idLength();
			nThisIdLenght -= nDeltaFromLeaf;

			if( nThisIdLenght <= _nDestIdLength )
			{
				arrayResult.push_back( objID );
			}
			else
			{
				char szLevelName[128];
				strcpy(szLevelName,objID.at(nThisIdLenght-1 ).c_str());
				if( strcmp( szLevelName, szSourceLevelName ) == 0 )
				{
					m_altList.push_back( objID );
				}
			}
		}

		if( m_altList.size() == 0 )
		{
			return false;
		}
		nDeltaFromLeaf++;
	}
	int nCount = (int)arrayResult.size();
	if( nCount == 0 || nCount > 1 )
		return false;

	resultObject = arrayResult.at(0);
	return true;
		
}
