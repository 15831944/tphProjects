#include "StdAfx.h"
#include ".\actypealias.h"
#include "Aircraft.h"
#include <algorithm>
#include "../Database/ADODatabase.h"
CAcTypeAlias::CAcTypeAlias():m_AcTypeInstance(NULL)
{}
CAcTypeAlias::~CAcTypeAlias()
{

}
void CAcTypeAlias::SetActypeInstance(CACType* _actype)
{
	m_AcTypeInstance = _actype ;
}
CACType* CAcTypeAlias::GetActypeInstance()
{
	return m_AcTypeInstance ;
}
void CAcTypeAlias::AddAliasName(CString& _strname)
{
	if(stdfind(m_AliasNames.begin(),m_AliasNames.end(),_strname) == m_AliasNames.end())
		m_AliasNames.push_back(_strname) ;
}
void CAcTypeAlias::RemoveAliasName(CString& _aliasname)
{
	std::vector<CString> ::iterator iter = std::find(m_AliasNames.begin(),m_AliasNames.end(),_aliasname) ;
	if(iter != m_AliasNames.end())
		m_AliasNames.erase(iter) ;
}
BOOL CAcTypeAlias::CheckAliasName(CString& _aliasname)
{
	std::vector<CString> ::iterator iter = std::find(m_AliasNames.begin(),m_AliasNames.end(),_aliasname) ;
	if(iter != m_AliasNames.end())
		return TRUE ;
	else
		return FALSE ;
}
CString CAcTypeAlias::FormatAliasName()
{
	CString FromatName ;
	for (int i = 0 ; i < (int)m_AliasNames.size() ; i++)
	{
		if(i != 0)
			FromatName.Append(_T(",")) ;
		FromatName.Append(m_AliasNames[i]) ;
	}
	return FromatName ;
}
void CAcTypeAlias::ReadDataFromDB(CAcTypeAlias* _PActypeAlias , int _ID) 
{
	if(_PActypeAlias == NULL || _ID == -1)
		return ;
	CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_ACTYPE_ALIAS WHERE RELATION_ACTYPE_ID = %d"),_ID) ;
	CADORecordset dataset ;
	long count = 0 ;
	CADODatabase::ExecuteSQLStatement(SQL,count,dataset) ;
	while(!dataset.IsEOF())
	{
		CString name ;
		dataset.GetFieldValue(_T("ALIASNAME") ,name) ;
		_PActypeAlias->AddAliasName(name) ;
		dataset.MoveNextData();
	}
}
void CAcTypeAlias::WriteDataFromDB(CAcTypeAlias* _PActypeAlias , int _ID)
{
	if(_ID == -1 || _PActypeAlias == NULL)
		return ;
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_ACTYPE_ALIAS WHERE RELATION_ACTYPE_ID = %d"),_ID) ;

	CADODatabase::ExecuteSQLStatement(SQL) ;
	for (int i = 0 ; i < (int)_PActypeAlias->GetAliasNames()->size();i++)
	{
		SQL.Format(_T("INSERT INTO TB_ACTYPE_ALIAS (RELATION_ACTYPE_ID,ALIASNAME) VALUES(%d,'%s')"),_ID,_PActypeAlias->GetAliasNames()->at(i)) ;
		CADODatabase::ExecuteSQLStatement(SQL) ;
	}
}