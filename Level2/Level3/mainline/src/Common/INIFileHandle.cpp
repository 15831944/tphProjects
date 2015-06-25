#include "stdafx.h"
#include ".\inifilehandle.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void StringToIntArrary(TCHAR* val ,TCHAR* ch , BYTE* outval,int& size)
{
    
	TCHAR* code ;
	int index = 0 ;
    code = strtok(val,ch) ;
	TCHAR m_val = 0 ;
	while(code != NULL)
	{
       m_val = (TCHAR)strtoul(code,NULL,16);
       outval[index] = m_val ;
	   code = strtok(NULL,ch) ;
	   index++ ;
	}
	size = index ;
}
void StringBYTEToStringChar(const BYTE* _val ,CString& _outval,int length)
{

	TCHAR val[10]={ 0} ; 
	for (int i= 0 ;i < length ; i++)
	{
		wsprintf(val,"%02x ",_val[i]);
		_outval.Append(val) ;
	}
}
CCfgData::CCfgData()
{
	m_strGroup="ArcPort China";

}


CCfgData::~CCfgData()
{
	RemoveAll();

}

void CCfgData::RemoveAll()
{
	POSITION pos=m_StrMapMap.GetStartPosition();
	while(pos)
	{
		CMapStringToString* pStrMap;
		CString str;
		m_StrMapMap.GetNextAssoc(pos,str,(void*&)pStrMap);
		if(pStrMap!=NULL)
		{
			pStrMap->RemoveAll();
			delete pStrMap;
		}
	}
	m_StrMapMap.RemoveAll();

}

BOOL CCfgData::LoadCfgData(LPCTSTR strFileName)
{
	int iReadLen=0;
	CString str[3];
	int iState=0;//0¡êo
	unsigned char ch; 

	RemoveAll();
	CFile file;
	if(!file.Open(strFileName, CFile::modeRead))
		return FALSE ;
	file.Seek(0,CFile::begin);
	str[0]="";
	str[1]="";
	str[2]="";
	CMapStringToString* pStrMap=NULL;
	do
	{
		iReadLen=file.Read(&ch,1);
		if(iReadLen!=0)
		{
			
			if(ch>0x80)//handel chinese
			{
				str[iState]+=ch;
				iReadLen=file.Read(&ch,1);
				if(iReadLen!=0)
				{
					str[iState]+=ch;
				}
			}else
			{
				switch(ch)
				{
					
				case '[':
					str[0].TrimLeft();
					str[0].TrimRight();
					if(str[0]==""&&str[1]=="")
					{
						pStrMap=NULL;
						iState=2;
						str[2]="";
					}else
					{
						str[iState]+=ch;
					}
					break;
				case ']':
					str[2].TrimLeft();
					str[2].TrimRight();
					if(iState==2&&str[2]!="")
					{
						iState=0;
						pStrMap=new CMapStringToString;
						m_StrMapMap.SetAt(str[2],pStrMap);
					}else
					{
						str[iState]+=ch;
					}
					break;
				case '=':
					iState=1;
					str[1]="";
					break;
				case ';':
				case 0x0d:
				case 0x0a:
					iState=0;
					str[0].TrimLeft();
					str[0].TrimRight();

					if(str[0]!=""&&pStrMap!=NULL)
					{
						pStrMap->SetAt((LPCTSTR)str[0],(LPCTSTR)str[1]);
					}
					str[0]="";
					str[1]="";
					if(ch==';')
					{
						while((iReadLen=file.Read(&ch,1))>0)
						{
							if(ch==0x0d||ch==0x0a)
							{
								file.Seek(-1,CFile::current);
								break;
							}
						}
					}
					break;
				default:
					str[iState]+=ch;
					break;
				}
			}
		}
	}while(iReadLen!=0);
	file.Close();
return TRUE ;
}


void CCfgData::SetGroup(LPCTSTR strGroup)
{
	m_strGroup=strGroup;
}

BOOL CCfgData::GetStrData(LPCTSTR strKey, CString &strValue)
{
	CMapStringToString* pStrMap=NULL;
	if(m_StrMapMap.Lookup(m_strGroup,(void*&)pStrMap))
	{
		if(pStrMap->Lookup(strKey,strValue))
			return TRUE;
		return FALSE;
	}
	return FALSE;
}
BOOL CCfgData::GetLongData(LPCTSTR strKey, long &lValue)
{
	CString str;
	if(CCfgData::GetStrData(strKey, str))
	{
		lValue=atoi((LPCTSTR)str);
		return TRUE;
	}
	return FALSE;
}
BOOL CCfgData::SetData(LPCTSTR strKey, LPCTSTR strValue)
{
	CMapStringToString* pStrMap=NULL;
	if(m_StrMapMap.Lookup(m_strGroup,(void*&)pStrMap))
	{
		pStrMap->SetAt(strKey,strValue);
		return TRUE;
	}else
	{
		pStrMap=new CMapStringToString;
		m_StrMapMap.SetAt(m_strGroup,pStrMap);
		pStrMap->SetAt(strKey,strValue);
		return TRUE;
	}

}

BOOL CCfgData::SaveCfgData(LPCTSTR strFileName)
{
	CFile file;
	if(!file.Open(strFileName,CFile::modeCreate|CFile::modeWrite))
		return FALSE;
	POSITION pos=m_StrMapMap.GetStartPosition();
	char ch[6]="[]\r\n=";
	CString str[3];
	while(pos)
	{
		CMapStringToString* pStrMap;
		m_StrMapMap.GetNextAssoc(pos,str[2],(void*&)pStrMap);
		if(pStrMap!=NULL)
		{
			file.Write(&ch[0],1);
			file.Write((LPSTR)(LPCTSTR)str[2],str[2].GetLength());
			file.Write(&ch[1],1);
			file.Write(&ch[2],2);
			POSITION pos1=pStrMap->GetStartPosition();
			while(pos1)
			{
				pStrMap->GetNextAssoc(pos1,str[0],str[1]);
				file.Write((LPSTR)(LPCTSTR)str[0],str[0].GetLength());
				file.Write(&ch[4],1);
				file.Write((LPSTR)(LPCTSTR)str[1],str[1].GetLength());
				file.Write(&ch[2],2);
			}
		}
	}
	file.Close();
	return TRUE;
}


BOOL CCfgData::SaveToStr(CString *pstr)
{
	if(pstr==NULL)return FALSE;
	*pstr="";
	POSITION pos=m_StrMapMap.GetStartPosition();
	CString str[4];
	while(pos)
	{
		CMapStringToString* pStrMap;
		m_StrMapMap.GetNextAssoc(pos,str[2],(void*&)pStrMap);
		if(pStrMap!=NULL)
		{
			str[3]=*pstr;
			pstr->Format("%s[%s]\r\n",str[3],str[2]);
			POSITION pos1=pStrMap->GetStartPosition();
			while(pos1)
			{
				pStrMap->GetNextAssoc(pos1,str[0],str[1]);
				str[3]=*pstr;
				pstr->Format("%s%s=%s\r\n",str[3],str[0],str[1]);

			}
		}
	}
	return TRUE;

}
CINIFileHandle::CINIFileHandle()
{

}
CINIFileHandle::~CINIFileHandle()
{

}
BOOL CINIFileHandle::LoadFile(const CString& _ProPath)
{
	m_Path.Empty() ;
	m_Path.Append(_ProPath) ;
	m_Path.Append("//") ;
	m_Path.Append(ARCTERM_INIFILE_NAME) ;
	if(! m_File.LoadCfgData(m_Path) )
		return FALSE ;
	m_File.SetGroup(m_currentGroup) ;
	return TRUE ;
}
BOOL CINIFileHandle::SaveData(const CString& _ProPath)
{
	m_Path.Empty() ;
	m_Path.Append(_ProPath) ;
	m_Path.Append("//") ;
	m_Path.Append(ARCTERM_INIFILE_NAME) ;
	return m_File.SaveCfgData(m_Path) ;
}
void CINIFileHandle::SetData(CString key , CString _val)
{
	m_File.SetData(key,_val) ;
}
BOOL CINIFileHandle::GetData(CString key,CString& _val)
{
   return m_File.GetStrData(key,_val) ;
}
void CINIFileHandle::SetCurrentGroup(CString _groupName)
{
	m_File.SetGroup(_groupName) ;
}
CINIFileForDBHandle::CINIFileForDBHandle(void) : CINIFileHandle()
{
	m_currentGroup = ARCTERM_DATABASE ;
	m_File.SetGroup(m_currentGroup) ;
}

CINIFileForDBHandle::~CINIFileForDBHandle(void)
{
}

void CINIFileForDBHandle::SetDataBaseName(const CString& _name)
{
	m_File.SetData(KEY_ARC_DB_NAME,_name) ;
}
BOOL CINIFileForDBHandle::GetDataBaseName(CString& _name)
{
	_name.Format("%s","ARCPORT") ;
	return TRUE ;
}
void CINIFileForDBHandle::SetDBPassword(const CString& _PW) 
{
	
	m_File.SetData(KEY_ARC_DB_PW,_PW) ;
}
BOOL CINIFileForDBHandle::GetDBPassword(CString& _PW)
{
	_PW.Format("%s","!ARCport@2007") ;
	//return m_File.GetStrData(KEY_ARC_DB_PW,_PW) ;
	return TRUE ;
}
void CINIFileForDBHandle::SetDBServer(const CString& _Ser)
{
	m_File.SetData(KEY_ARC_DB_SERVER,_Ser) ;
}
BOOL CINIFileForDBHandle::GetDBServer(CString& _ser)
{
	_ser.Format("%s",_T("(local)\\ARCPORT")) ;
	//return m_File.GetStrData(KEY_ARC_DB_SERVER,_ser) ;
	return TRUE;
}
void CINIFileForDBHandle::SetDBUserName(const CString& _name)
{
	m_File.SetData(KEY_ARC_DB_USERNAME ,_name) ;
}
BOOL CINIFileForDBHandle::GetDBUserName(CString& _name)
{
	_name.Format("%s","SA") ;
	//return m_File.GetStrData(KEY_ARC_DB_USERNAME,_name) ;
	return TRUE ;
}
void CINIFileForDBHandle::SetDBVersion(const CString& _ver)
{
	m_File.SetData(KEY_ARC_DB_VERSION,_ver) ;
}
BOOL CINIFileForDBHandle::GetDBVersion(CString& _ver)
{
	return m_File.GetStrData(KEY_ARC_DB_VERSION,_ver) ;
}

CINIFileForPathsHandle::CINIFileForPathsHandle():CINIFileHandle()
{
	m_currentGroup = ARCTERM_PATH ;
	m_File.SetGroup(m_currentGroup) ;
}
CINIFileForPathsHandle::~CINIFileForPathsHandle()
{

}
void CINIFileForPathsHandle::SetPathVal(const CString& _val)
{
	m_File.SetData(KEY_ARC_PATH,_val) ;
}
BOOL CINIFileForPathsHandle::GetPathVal(CString& _val)
{
	return m_File.GetStrData(KEY_ARC_PATH,_val) ;
}

CINIFileForResourseHandle::CINIFileForResourseHandle() : CINIFileHandle()
{
	m_currentGroup = ARCTERM_RESOURCE ;
	m_File.SetGroup(m_currentGroup) ;
}
CINIFileForResourseHandle::~CINIFileForResourseHandle()
{

}
void CINIFileForResourseHandle::SetResourseVersion(const CString& _version)
{
	m_File.SetData(KEY_ARC_RES_VERSION,_version) ;
}
BOOL CINIFileForResourseHandle::GetResoueseVersion(CString& _version)
{
	return m_File.GetStrData(KEY_ARC_RES_VERSION,_version) ;
}

CINIFileForUNITHandle::CINIFileForUNITHandle():CINIFileHandle()
{
	m_currentGroup = ARCTERM_UNITSMANGER ;
	m_File.SetGroup(m_currentGroup) ;
}
CINIFileForUNITHandle::~CINIFileForUNITHandle()
{

}
void CINIFileForUNITHandle::SetUnitLenght(const CString& _val)
{

	DWORD val = 0 ;
	memcpy(&val,_val.GetString(),_val.GetLength()*sizeof(BYTE)) ;
	TCHAR m_val[1024] = {0} ;
	wsprintf(m_val,"%d",val) ;
	m_File.SetData(KEY_ARC_UNIT_LEN,m_val) ;
}
BOOL CINIFileForUNITHandle::GetUnitLenght(CString& _val)
{

	if(!m_File.GetStrData(KEY_ARC_UNIT_LEN,_val)) 
		return false;
	return true ;

}
void CINIFileForUNITHandle::SetUnitWeight(const CString& _val)
{

	DWORD val = 0 ;
	memcpy(&val,_val.GetString(),_val.GetLength()*sizeof(BYTE)) ;
	TCHAR m_val[1024] = {0} ;
	wsprintf(m_val,"%d",val) ;
	m_File.SetData(KEY_ARC_UNIT_WEIGHT,m_val) ;
}
BOOL CINIFileForUNITHandle::GetUnitWeight(CString& _val)
{

	if(!m_File.GetStrData(KEY_ARC_UNIT_WEIGHT,_val)) 
		return false;
	return true ;

}

CINIFileForLicenseHandle::CINIFileForLicenseHandle():CINIFileHandle()
{
	m_currentGroup = ARCTERM_LICENSE ;
	m_File.SetGroup(m_currentGroup) ;
}
CINIFileForLicenseHandle::~CINIFileForLicenseHandle()
{

}
void CINIFileForLicenseHandle::SetLicenseFONT(const BYTE* _val,int size)
{

	CString m_val ;
	StringBYTEToStringChar(_val,m_val,size) ;
	m_File.SetData(KEY_ARC_POSI_FONT,m_val) ;
}
BOOL CINIFileForLicenseHandle::GetLicenseFont(BYTE* _val,int& size
											  )
{
	TCHAR val[1024] = {0} ;
	CString  strval ;
	if(!m_File.GetStrData(KEY_ARC_POSI_FONT,strval)) 
		return false;
	TCHAR m_val[1024] = {0} ;
	memcpy(m_val,strval.GetString(),strval.GetLength()) ;
	StringToIntArrary(m_val," ",_val,size) ;
	return true ;
}

void CINIFileForLicenseHandle::SetLicenseHeight(const CString& _val)
{
	DWORD val = 0 ;
	memcpy(&val,_val.GetString(),_val.GetLength()*sizeof(BYTE)) ;
	TCHAR m_val[1024] = {0} ;
	wsprintf(m_val,"%d",val) ;
	m_File.SetData(KEY_ARC_POSI_HEIGHT,m_val) ;
}
BOOL CINIFileForLicenseHandle::GetLicenseHeight(CString& _val)
{

	if(!m_File.GetStrData(KEY_ARC_POSI_HEIGHT,_val)) 
		return false;
	return true ;

}
void CINIFileForLicenseHandle::SetLicenseLeft(const CString& _val)
{

	DWORD val = 0 ;
	memcpy(&val,_val.GetString(),_val.GetLength()*sizeof(BYTE)) ;
	TCHAR m_val[1024] = {0} ;
	wsprintf(m_val,"%d",val) ;
	m_File.SetData(KEY_ARC_POSI_LEFT,m_val) ;
}
BOOL CINIFileForLicenseHandle::GetLicenseLeft(CString& _val)
{
	if(!m_File.GetStrData(KEY_ARC_POSI_LEFT,_val)) 
		return false;
	return true ;

}
void CINIFileForLicenseHandle::SetLicenseSetting(const BYTE* _val,int size)
{
	CString m_val ;
	StringBYTEToStringChar(_val,m_val,size) ;
	m_File.SetData(KEY_ARC_POSI_SETTING,m_val) ;
}
BOOL CINIFileForLicenseHandle::GetlicenseSetting(BYTE* _val,int& size)
{
	TCHAR val[1024] = {0} ;
	CString  strval ;
	if(!m_File.GetStrData(KEY_ARC_POSI_SETTING,strval)) 
		return false;
	TCHAR m_val[1024] = {0} ;
	memcpy(m_val,strval.GetString(),strval.GetLength()) ;
	StringToIntArrary(m_val," ",_val,size) ;
	return true ;

}
void CINIFileForLicenseHandle::SetLicenseTop(const CString& _val)
{

	DWORD val = 0 ;
	memcpy(&val,_val.GetString(),_val.GetLength()*sizeof(BYTE)) ;
	TCHAR m_val[1024] = {0} ;
	wsprintf(m_val,"%d",val) ;
	m_File.SetData(KEY_ARC_POSI_TOP,m_val) ;
}
BOOL CINIFileForLicenseHandle::GetLicenseTop(CString& _val)
{
	if(!m_File.GetStrData(KEY_ARC_POSI_TOP,_val)) 
		return false;
	return true ;

}
void CINIFileForLicenseHandle::SetLicenseWidth(const CString& _val)
{

	DWORD val = 0 ;
	memcpy(&val,_val.GetString(),_val.GetLength()*sizeof(BYTE)) ;
	TCHAR m_val[1024] = {0} ;
	wsprintf(m_val,"%d",val) ;
	m_File.SetData(KEY_ARC_POSI_WIDTH,m_val) ;
}
BOOL CINIFileForLicenseHandle::GetLicenseWidth(CString& _val)
{

	if(!m_File.GetStrData(KEY_ARC_POSI_WIDTH,_val)) 
		return false;
	return true ;

}