#include "StdAfx.h"
#include ".\newpassengertype.h"
#include "../Common/strdict.h"
//#include "strdict.h"

///////////////////////////////////////////////////////////////////////////////
//CPaxTyLevel::CPaxTyLevel(int level ,CString& _name):m_level(level),m_ty(_name)
//{
//
//}
//CPaxTyLevel::~CPaxTyLevel()
//{
//
//}
//CString CPaxTyLevel::GetTyName() const
//{
//	return m_ty ;
//}
//int CPaxTyLevel::GetLevel() const
//{
//	return m_level ;
//}
//void CPaxTyLevel::SetTyName(CString& _name)
//{
//	m_ty = _name ;
//}
//void CPaxTyLevel::SetLevel(int _level)
//{
//	m_level = _level ;
//}
//
//bool CPaxTyLevel::Fits( const CPaxTyLevel& paxTypelevel) const
//{
//
//	if(m_ty.GetLength() == 0)//default 
//		return true;
//	CString passedPaxType = paxTypelevel.GetTyName();
//	if(m_ty.CompareNoCase(passedPaxType) == 0)
//		return true;
//
//	return false;
//}
//
//bool CPaxTyLevel::Equals( const CPaxTyLevel& paxTypelevel) const
//{
//	return 0 == m_ty.CompareNoCase(paxTypelevel.GetTyName());
//}



//////////////////////////////////////////////////////////////////////////
//CPassengerType
CPassengerType::CPassengerType(StringDictionary *pStrDict):m_pStrDict(pStrDict)
{
	initDefault();
}
CPassengerType::~CPassengerType()
{

}

CString CPassengerType::PrintStringForShow() const
{
	if (isDefault())
	{
		return "DEFAULT";
	}

	CString strType = "";
	for (int i = 0; i < MAX_PAX_TYPES; i++)
	{
		if (userTypes[i] > 0)
		{
			if(strType !="")
				strType += "-";
			strType += m_pStrDict->getString (userTypes[i]);

		}
	}
	return strType;
}

CString CPassengerType::GetStringForDatabase() const
{

	CString strType = "";
	for (int i = 0; i < MAX_PAX_TYPES; i++)
	{
		if (userTypes[i] > 0)
			strType += m_pStrDict->getString (userTypes[i]);
		if(i < MAX_PAX_TYPES -1)
			strType += "-";
	}
	return strType;
}

void CPassengerType::FormatLevelPaxTypeFromString(CString _PaxTy)
{

	if (_PaxTy.IsEmpty())
	{
		initDefault();
		return;
	}

	char p_str[1024];
	strcpy(p_str, _PaxTy.GetString());

    int i,ndx = -1;
    char name[32];
	for (int j = 0; j < MAX_PAX_TYPES; j++)
	{
		for (i = 0, ndx++; p_str[ndx] != '-' && ndx< (int)strlen(p_str) ; i++, ndx++)
			name[i] = p_str[ndx];
		name[i] = '\0';
		userTypes[j] = (i)? m_pStrDict->findString (name): 0;
	}

}

//check the pax type passed is contained by the current pax type
//regular 
//if each level of the current pax is contain the level in the cmpPaxtype,
//that is all levels are lager than paxtype passed, return true
bool CPassengerType::Fits( const CPassengerType& cmpPaxType ) const
{
	if(isDefault())
		return true;

	for (int i = 0; i < MAX_PAX_TYPES; i++)
		if (userTypes[i] && userTypes[i] != cmpPaxType.userTypes[i])
			return false;

	return true;
}

bool CPassengerType::IsEqual( const CPassengerType& cmpPaxType ) const
{
	for (int i = 0; i < MAX_PAX_TYPES; i++)
		if (userTypes[i] != cmpPaxType.userTypes[i])
			return false;

	return true;

}

void CPassengerType::initDefault( void )
{
	for (int i = 0; i < MAX_PAX_TYPES; i++)
		userTypes[i] = 0;

}

int CPassengerType::isDefault( void ) const
{
	for (int i = 0; i < MAX_PAX_TYPES; i++)
		if (userTypes[i])
			return 0;

	return 1;
}

void CPassengerType::setUserType( int p_level, int p_type )
{
	userTypes[p_level] = p_type;
}

void CPassengerType::setUserType( const int *p_type )
{
	for (int i = 0; i < MAX_PAX_TYPES; i++)
		userTypes[i] = p_type[i];
}

int CPassengerType::getUserType( int p_level ) const
{
	return userTypes[p_level];
}

void CPassengerType::getUserType( int *p_type ) const
{
	for (int i = 0; i < MAX_PAX_TYPES; i++)
		p_type[i] = userTypes[i];
}

int CPassengerType::contains( int p_level, int p_index ) const
{
	return userTypes[p_level] == p_index;
}

void CPassengerType::copyData( const CPassengerType& paxType )
{
	memcpy((void*)userTypes,(const void*)paxType.userTypes,sizeof(int)*MAX_PAX_TYPES);
}

void CPassengerType::initPaxType( const MobDescStruct &p_struct )
{
	for (int i = 0; i < MAX_PAX_TYPES; i++)
		userTypes[i] = p_struct.paxType[i];
}

bool CPassengerType::screenPrint( char *p_str, int _nLevel, unsigned p_maxChar ) const
{
	for (int i = 0; i < MAX_PAX_TYPES; i++)
	{
		if (userTypes[i] >0 )
		{
			//if p_str not enough to load the string ,returned  //matt changed
			const char * psNextString = m_pStrDict->getString (userTypes[i]);
			if ((strlen (p_str) + strlen(psNextString)) > p_maxChar)
			{
				if(strlen(psNextString) >4)
				{
					strcpy(p_str + strlen(p_str)-4,"...");
				}
				else
				{
					strcpy (p_str + (p_maxChar-4), "...");
				}
				return false;
			}
			
			if(strlen(p_str)>0 )
				strcat (p_str, "-");
	        strcat (p_str, m_pStrDict->getString (userTypes[i]));

		}
	}

	return true;
}

bool CPassengerType::screenPrint( CString& strContent, int _nLevel, unsigned p_maxChar ) const
{
	CString strTemp;
	for (int i = 0; i < MAX_PAX_TYPES; i++)
	{
		if (userTypes[i] >0 )
		{
			//if p_str not enough to load the string ,return false  
			strTemp = m_pStrDict->getString (userTypes[i]);
			if (strContent.GetLength() + strTemp.GetLength() > (int)p_maxChar)
			{
				strContent +="...";
				return false;
			}

			if(strContent.GetLength() >0)
				strContent+="-";

			strContent +=m_pStrDict->getString (userTypes[i]);
		}
	}

	return true;
}

void CPassengerType::SetStringDictionary( StringDictionary *pStrDict )
{
	m_pStrDict = pStrDict;
}

//----------------------------------------------------------------------------
//Summary:
//		retrieve user define type count
//Return:
//		int: user define type count
//---------------------------------------------------------------------------
int CPassengerType::GetUserTypeCount() const
{
	int nCount = 0;
	for (int i = 0; i < MAX_PAX_TYPES; i++)
	{
		if (userTypes[i] == 0 )
			return nCount;
		else
			nCount++;
	}

	return nCount;
}

//--------------------------------------------------------------------------
//Summary:
//		compare passenger type method
//Parameters:
//		paxType: compare object
//Return:
//		user type count is greater than 'paxtype'
//--------------------------------------------------------------------------
bool CPassengerType::operator<( const CPassengerType& paxType ) const
{
	int nThisCount = GetUserTypeCount();
	int nOtherCount = paxType.GetUserTypeCount();
	
	return nThisCount > nOtherCount;
}