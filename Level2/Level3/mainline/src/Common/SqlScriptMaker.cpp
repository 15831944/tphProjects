#include "StdAfx.h"
#include ".\sqlscriptmaker.h"

SqlSelectScriptMaker::SqlSelectScriptMaker(const CString& strTableName)
{
	m_strTableName = strTableName;

	m_strTableName.Trim();
	ASSERT(m_strTableName.GetLength() > 0);


}

SqlSelectScriptMaker::~SqlSelectScriptMaker(void)
{
	m_vColName.clear();
}

void SqlSelectScriptMaker::AddColumn( const CString& strColName )
{
	CString strTemp = strColName;
	strTemp.Trim();
	ASSERT(strTemp.GetLength() > 0);

	m_vColName.push_back(strTemp);
}

void SqlSelectScriptMaker::SetCondition( const CString& strCondition )
{

	CString strTemp = strCondition;
	strTemp.Trim();
	m_strCondition = strTemp;

}

CString SqlSelectScriptMaker::GetScript()
{
	CString strSQL = _T("SELECT ");

	//columns
	//ASSERT(m_vColName.size() > 0);

	CString strCols;
	std::vector<CString>::iterator iter = m_vColName.begin();
	for (; iter != m_vColName.end(); ++ iter)
	{
		strCols += *iter;
		strCols += _T(" ,");
	}

	strCols.TrimRight(_T(","));
	strCols.Trim();

	if(strCols.GetLength() == 0)
		strCols = _T(" * ");


	strSQL += strCols; //columns
	strSQL += _T(" FROM ");

	strSQL += m_strTableName;//table

	m_strCondition.Trim();
	if(m_strCondition.GetLength()) //condition
	{
		strSQL += _T(" WHERE ");
		strSQL += m_strCondition;
	}


	strSQL.Trim();

	return strSQL;
}


//////////////////////////////////////////////////////////////////////////
//SqlUpdateScriptMaker::SqlUpdateColumn
SqlScriptColumn::SqlScriptColumn( const CString& strColName, int nVaule )
{
	m_strColName = strColName;
	m_strColName.Trim();
	ASSERT(m_strColName.GetLength() > 0);



	m_strValue.Empty();

	m_strValue.Format(_T("%d"),nVaule);
}

SqlScriptColumn::SqlScriptColumn( const CString& strColName, double dVaule )
{	
	m_strColName = strColName;
	m_strColName.Trim();
	ASSERT(m_strColName.GetLength() > 0);

	m_strValue.Empty();

	m_strValue.Format(_T("%f"),dVaule);
}

SqlScriptColumn::SqlScriptColumn( const CString& strColName, const CString& strVaule )
{	
	m_strColName = strColName;

	m_strColName.Trim();
	ASSERT(m_strColName.GetLength() > 0);


	m_strValue.Empty();
	m_strValue.Format(_T("'%s'"),strVaule);

	m_strValue.Trim();

}

//SqlScriptColumn::SqlScriptColumn( const CString& strColName, bool bVaule )
//{
//	m_strColName = strColName;
//
//	m_strColName.Trim();
//	ASSERT(m_strColName.GetLength() > 0);
//
//
//	m_strValue.Empty();
//	m_strValue.Format(_T("%d"),bVaule?1:0);
//
//	m_strValue.Trim();
//}
CString SqlScriptColumn::GetName()
{
	return m_strColName;
}

CString SqlScriptColumn::GetValue()
{
	return m_strValue;
}

CString SqlScriptColumn::EqualCondition()
{
	CString str;
	str.Format(_T("(%s = %s)"), m_strColName.GetString(), m_strValue.GetString() );
	return str;
}



//////////////////////////////////////////////////////////////////////////
//SqlInsertScriptMaker
SqlInsertScriptMaker::SqlInsertScriptMaker( const CString& strTableName )
{	
	m_strTableName = strTableName;

	m_strTableName.Trim();
	ASSERT(m_strTableName.GetLength() > 0);


}





//Insert into + table +(column,...) Values(value,...)
CString SqlInsertScriptMaker::GetScript()
{

	//update
	CString strSQL = _T("INSERT INTO ");
	strSQL += m_strTableName; //table name

	CString strColumns;
	CString strValues;

	ParseColumnAndValue(strColumns,strValues);

	ASSERT(strColumns.GetLength() > 0);
	ASSERT(strValues.GetLength() > 0);

	strSQL += _T(" ("); //column
	strSQL += strColumns;
	strSQL += _T(" )");

	strSQL += _T("  VALUES(");
	strSQL += strValues;
	strSQL += _T(" )");

	strSQL.Trim();

	return strSQL;

}

void SqlInsertScriptMaker::ParseColumnAndValue( CString& strColNames, CString& strValues )
{
	ASSERT(m_vColumnList.size() > 0);

	strColNames.Empty();
	strValues.Empty();

	std::vector<SqlScriptColumn>::iterator iter = m_vColumnList.begin();

	for(; iter != m_vColumnList.end(); ++iter)
	{
		strColNames += (*iter).GetName();
		strColNames += _T(" ,");


		strValues += (*iter).GetValue();
		strValues += _T(" ,");
	}

	strColNames.Trim();
	strColNames.TrimRight(_T(","));
	strColNames.Trim();

	strValues.Trim();
	strValues.TrimRight(_T(","));
	strValues.Trim();

}













//////////////////////////////////////////////////////////////////////////
//SqlUpdateScriptMaker
SqlUpdateScriptMaker::SqlUpdateScriptMaker( const CString& strTableName )
{
	m_strTableName = strTableName;

	m_strTableName.Trim();
	ASSERT(m_strTableName.GetLength() > 0);
}





void SqlUpdateScriptMaker::SetCondition( const CString& strCondition )
{
	m_strCondition = strCondition;
	m_strCondition.Trim();
}
//update + tablename + SET colname = colvalue, ... + WHERE +condition
CString SqlUpdateScriptMaker::GetScript()
{
	CString strSQL = _T("UPDATE ");

	strSQL += m_strTableName; //table name


	strSQL += _T(" SET ");

	CString strColValues = _T("");
	ParseColumnAndValue(strColValues);

	strSQL += strColValues;

	if(m_strCondition.GetLength() > 0)
	{
		strSQL += _T(" WHERE ");
		strSQL += m_strCondition;
	}

	strSQL.Trim();

	return strSQL;
}

void SqlUpdateScriptMaker::ParseColumnAndValue( CString& strColValues)
{
	strColValues.Empty();

	std::vector<SqlScriptColumn>::iterator iter = m_vColumnList.begin();

	for(; iter != m_vColumnList.end(); ++iter)
	{
		strColValues += (*iter).GetName();

		strColValues += _T(" = ");

		strColValues += (*iter).GetValue();

		strColValues += _T(" ,");
	}


	strColValues.Trim();
	strColValues.TrimRight(_T(","));
	strColValues.Trim();

	ASSERT(strColValues.GetLength() > 0);

}














//////////////////////////////////////////////////////////////////////////
//SqlDeleteScriptMaker
SqlDeleteScriptMaker::SqlDeleteScriptMaker( const CString& strTableName )
{
	m_strTableName = strTableName;

	m_strTableName.Trim();
	ASSERT(m_strTableName.GetLength() > 0);
}

SqlDeleteScriptMaker::~SqlDeleteScriptMaker()
{

}

void SqlDeleteScriptMaker::SetCondition( const CString& strCondition )
{
	m_strCondition = strCondition;
	m_strCondition.Trim();
}
//delete from + table name + where + condition
CString SqlDeleteScriptMaker::GetScript()
{
	CString strSQL = _T("DELETE FROM ");

	strSQL += m_strTableName;

	if(m_strCondition.GetLength() > 0)
	{
		strSQL += _T(" WHERE ");
		strSQL += m_strCondition;
	}

	strSQL.Trim();

	return strSQL;
}






void SqlScriptColumnList::AddColumn( SqlScriptColumn& columnUpdate )
{
	m_vColumnList.push_back(columnUpdate);
}
