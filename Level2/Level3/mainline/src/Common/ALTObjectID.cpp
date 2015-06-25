#include "stdafx.h"
#include <afx.h>
#include "ALTObjectID.h"
#include <string>
#include "common\termfile.h"

ALTObjectID::ALTObjectID()
{

}

ALTObjectID::ALTObjectID( const ALTObjectID& other)
{
	for(int i=0;i<OBJECT_STRING_LEVEL;++i)
		m_val[i] = other.m_val[i];
}

ALTObjectID::ALTObjectID( const char * strID)
{
	FromString(strID);
}


//ALTObjectID::ALTObjectID( const ProcessorID& procID )
//{
//	ASSERT(false);
//}


BOOL ALTObjectID::readALTObjectID( ArctermFile& file )
{
	char name[256]; name[0]='\0';

	if (file.isBlankField() ||
		!file.getShortField (name, 256) )
	{
		return FALSE;
	}
	//if( strcmp("All Processors",name ) ){}
	else { *this = ALTObjectID(name); }
	return TRUE;
}

int ALTObjectID::writeALTObjectID( ArctermFile& file ) const
{
	CString str = GetIDString();
	//if( str.IsEmpty() ) str = "All Processors";
	file.writeField (_T(str));
	return 1;
}

CString ALTObjectID::GetIDString() const
{
	CString strName = _T("");
	//level1
	if(at(0).length() == 0)
		return strName;

	strName += CString(at(0).c_str());

	for (int i =1; i < OBJECT_STRING_LEVEL; ++i)
	{
		if (at(i).length() == 0)
			break;

		strName +=_T("-");
		strName +=at(i).c_str();
	}

	return strName;
}

void ALTObjectID::printID( char* buf, const char *sep /*= "-"*/ ) const
{
	
	if(at(0).empty()){
		strcpy(buf,"");
		return;
	}else{
		strcpy(buf,at(0).c_str());
	}
	for (size_t i = 1; i < OBJECT_STRING_LEVEL && !at(i).empty(); i++)
	{
		strcat (buf, sep);
		strcat (buf, at(i).c_str());
	}
}

BOOL ALTObjectID::idFits( const ALTObjectID& checkID ) const		//small fit big
{
	if(checkID.IsBlank())
		return TRUE;

	if(IsBlank())
		return FALSE;

	for (size_t i = 0; i < OBJECT_STRING_LEVEL; i++)
	{
		if( checkID.at(i).empty() )
			return TRUE;

		if ( at(i) != checkID.at(i) )
			return FALSE;
	}
	return TRUE;
}

ALTObjectID::~ALTObjectID()
{
}

std::string & ALTObjectID::at( size_t i )
{
	return m_val[i];
}

const std::string& ALTObjectID::at( size_t i ) const
{
	return m_val[i];
}

ALTObjectID& ALTObjectID::operator=( const ALTObjectID&other )
{
	for(int i=0;i<OBJECT_STRING_LEVEL;++i)
		m_val[i] = other.m_val[i];
	return *this;
}

bool ALTObjectID::IsFits( const ALTObjectIDList& idGroup )const
{	
	//level 1
	CString str1 = at(0).c_str();

	ASSERT( !str1.IsEmpty() );
	if( str1.IsEmpty() ) return false;
	
	SortedStringList strlevel1;
	idGroup.GetLevel1StringList(strlevel1);
	SortedStringList::iterator itr1 = strlevel1.find(str1);
	if(itr1 == strlevel1.end()) return false;
	
	CString str2 = at(1).c_str();
	if( str2.IsEmpty() )return true;
	//level  2
	SortedStringList strlevel2;
	idGroup.GetLevel2StringList(str1,strlevel2);
	strlevel2.find(str2);
	SortedStringList::iterator itr2 = strlevel2.find(str2);
	if(itr2 == strlevel2.end() ) return false;
	
	CString str3 = at(2).c_str();
	if(str3.IsEmpty()) return  true;
	//level 3
	SortedStringList strlevel3;
	idGroup.GetLevel3StringList(str1,str2,strlevel3);
	if( strlevel3.end() == strlevel3.find(str3) ) return false;

	CString str4 = at(3).c_str();
	if(str4.IsEmpty()) return true;

	//level4
	SortedStringList strlevel4;
	idGroup.GetLevel4StringList(str1,str2,str3,strlevel4);
	if(strlevel4.end() == strlevel4.find(str4) )return false;
	return true;
	
}

int ALTObjectID::idLength() const
{	
	int i ;
	for(i=0;i<OBJECT_STRING_LEVEL;i++)
	{
		if(m_val[i].empty()) 
			return i;
	}
	return i;
}
void ALTObjectIDList::DeleteItem( int nidx )
{
	erase(begin() + nidx);
}

void ALTObjectIDList::Add( const ALTObjectID& newID )
{
	push_back(newID);
}

ALTObjectIDList::ALTObjectIDList()
{

}

ALTObjectIDList::~ALTObjectIDList()
{

}

void ALTObjectIDList::push_back( const ALTObjectID& _Val )
{
	return std::vector<ALTObjectID>::push_back(_Val);
}

ALTObjectID& ALTObjectIDList::at( size_type i )
{
	return std::vector<ALTObjectID>::at(i);
}

const ALTObjectID& ALTObjectIDList::at( size_type i ) const
{
	return std::vector<ALTObjectID>::at(i);

}

void ALTObjectIDList::clear()
{
	std::vector<ALTObjectID>::clear();
}

const size_t ALTObjectIDList::size() const
{
	return std::vector<ALTObjectID>::size();
}

size_t ALTObjectIDList::GetLevel1StringList( SortedStringList& strlist ) const
{
	CString str1;
	for(size_t i=0;i<size();i++){
		str1 = at(i).m_val[0].c_str();
		if(!str1.IsEmpty()){
			str1.MakeUpper();
			strlist.insert(str1);
		}
	}

	return strlist.size();
}

size_t ALTObjectIDList::GetLevel2StringList( const CString& level1str,SortedStringList& strlist ) const
{
	CString str1;
	for(size_t i=0;i<size();++i){
		str1 = at(i).m_val[0].c_str();
		if( level1str.CompareNoCase(str1) == 0 ){
            CString str2 = at(i).m_val[1].c_str();
			if(!str2.IsEmpty()){
				str2.MakeUpper();
				strlist.insert( str2 );
			}
		}
	}
	return strlist.size();
}



size_t ALTObjectIDList::GetLevel3StringList( const CString& level1str,const CString& level2str,SortedStringList& strlist ) const
{
	CString str1;
	for(size_t i=0;i<size();++i){
		str1 = at(i).m_val[0].c_str();
		if( level1str.CompareNoCase(str1) == 0 ){
			CString str2 = at(i).m_val[1].c_str();
			if(level2str.CompareNoCase(str2) == 0 ){
				CString str3 = at(i).m_val[2].c_str();
				if(!str3.IsEmpty() ){
					str3.MakeUpper();
					strlist.insert(str3);
				}
			}			
		}
	}
	return strlist.size();
}

size_t ALTObjectIDList::GetLevel4StringList( const CString& level1str,const CString& level2str,const CString& level3str, SortedStringList& strlist ) const
{
	CString str1;
	for(size_t i=0;i<size();++i){
		str1 = at(i).m_val[0].c_str();
		if( level1str.CompareNoCase(str1) == 0 ){
			CString str2 = at(i).m_val[1].c_str();
			if(level2str.CompareNoCase(str2) == 0 ){
				CString str3 = at(i).m_val[2].c_str();
				if(level3str.CompareNoCase(str3)==0){
					CString str4 = at(i).m_val[3].c_str();
					if(!str4.IsEmpty() ){
						str4.MakeUpper();
						strlist.insert(str4);
					}
				}				
			}			
		}
	}
	return strlist.size();
}
size_t ALTObjectIDList::GetStringListAtLevel(int nLevel ,SortedStringList& nameList)
{
	if (nLevel < OBJECT_STRING_LEVEL)
	{
		for(size_t i=0;i<size();++i)
		{
			CString strName = at(i).m_val[nLevel].c_str();
			strName.Trim();
			if (strName.GetLength())
			{
				nameList.insert(strName);
			}
		}
	}

	return nameList.size();
}
bool ALTObjectID::operator == (const  ALTObjectID&otherID )const
{
	return GetIDString().CompareNoCase( otherID.GetIDString()) == 0;	
}

bool ALTObjectID::IsBlank() const
{
	return m_val[0].empty();
	for(size_t i=0;i<OBJECT_STRING_LEVEL;++i)
	{
		if( !at(i).empty() )
			return false;
	}
	return true;
}

#include "ARCStringConvert.h"

ALTObjectID ALTObjectID::GetNext() const
{
	ALTObjectID nameDest = *this;
	int lastLevel = 0;
	for(int i =0 ;i<(int) OBJECT_STRING_LEVEL;i++)
	{
		if( nameDest.m_val[i].empty() )
		{ 
			lastLevel = i - 1;
			break; 
		}else if (i == (int) OBJECT_STRING_LEVEL-1 )
		{
			lastLevel = i ;
		}
	}
	if(lastLevel<0)
		return nameDest;
	/////
	//char buf[256] = {0};
	//strcpy(buf,nameDest.m_val[lastLevel].c_str());

	//check if last level of ID is numeric or alphanumeric
	//char c = toupper(buf[0]);
	nameDest.m_val[lastLevel] = GetNextString(nameDest.m_val[lastLevel]);	
	return nameDest;
}

void ALTObjectID::FromString( const char * strID )
{
	CString strTemp = strID;

	if (!strTemp.MakeUpper().Compare("ALLPROCESSORS") || !strTemp.MakeUpper().Compare("ALL PROCESSORS"))
	{
		return;
	}

	char stridx[256];
	strcpy(stridx,strID);


	//val_type::iterator itr = m_val.begin();
	int itr=0;

	char * starChar = stridx;
	char * charitr = stridx;

	char a ;
	do{
		a = *charitr;
		if( ('-' == a) || ('\0' ==a)  ){
			*charitr = 0;
			m_val[itr] = std::string(starChar);
			starChar = ++charitr;
			itr++;
		}else ++charitr;

	}while(a!= 0 && itr < OBJECT_STRING_LEVEL );
}

//map char A-Z 0-9 to int
static bool increaseChar(char& c){
	if( c>='0' && c<'9' )
	{
		c++; return true;
	}
	if( c>= 'A' && c<'Z')
	{
		c++; return true;
	}
	if( c>='a' && c<'z' )
	{
		c++; return true;
	}
	return false;
}
static bool increaseString(std::string& str)
{
	for(std::string::reverse_iterator itr=str.rbegin();itr!=str.rend();++itr)
	{
		if( increaseChar(*itr) )
		{
			return true;
		}
	}
	return false;
}

std::string ALTObjectID::GetNextString( const std::string& from )
{
	//check if last level of ID is numeric or alphanumeric
	//char c = toupper(buf[0]);
	int len = (int)from.length();
	const char* buf = from.c_str();
	if(ARCStringConvert::isAllNumeric(buf))
	{
		//numeric
		int i = atoi(buf);
		i++;
		return ARCStringConvert::toString(i).GetString();		
	}//if all string_number
	
	std::string retstr = from;
	if(increaseString(retstr))
		return retstr;

	CString strNextName;
	CString strLastLevel = CString(buf);

	int nPos = strLastLevel.ReverseFind('_');
	if(nPos == -1)
	{
		strNextName.Format(_T("%s_%d"),buf,0);
	}
	else
	{
		//get the string after "_"
		CString strAfter_ = strLastLevel.Mid(nPos);
		strAfter_.TrimLeft(_T("_"));

		CString strBefore_ = strLastLevel.Left(nPos);

		if (strAfter_.GetLength() == 0)
		{
			//CString strNextName;
			strNextName.Format(_T("%s%d"),buf,0);
		}
		else
		{
			if(ARCStringConvert::isAllNumeric(strAfter_.GetBuffer()))
			{
				int i = atoi(strAfter_);
				i++;
				strNextName.Format(_T("%s_%d"),strBefore_,i);
			}
			else
			{
				strNextName.Format(_T("%s_%d"),buf,0);
			}
		}

	}
	return strNextName.GetString();
	
}

bool ALTObjectID::operator<( const ALTObjectID& id2) const
{
	const ALTObjectID& id1 = *this;
	for(int i=0;i<OBJECT_STRING_LEVEL;i++ )	
	{
		CString s1 = id1.at(i).c_str();
		CString s2 = id2.at(i).c_str();
		if( s1.CompareNoCase(s2)==0 )
			continue;

		if( ARCStringConvert::isAllNumeric(s1.GetString()) && ARCStringConvert::isAllNumeric(s2.GetString()) )
		{
			int i1 = atoi(s1);
			int i2 = atoi(s2);
			return (i1<i2);					
		}

		s1.MakeLower();
		s2.MakeLower();
		return s1<s2;
	}
	return false;
}