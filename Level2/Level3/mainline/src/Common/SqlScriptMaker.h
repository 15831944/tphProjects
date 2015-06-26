#pragma once
#include <vector>

class SqlSelectScriptMaker
{
public:
	SqlSelectScriptMaker(const CString& strTableName);
	~SqlSelectScriptMaker(void);
public:
	void AddColumn(const CString& strColName);

	void SetCondition(const CString& strCondition);

	CString GetScript();

protected:
	CString m_strTableName;
	CString m_strCondition;
	std::vector<CString> m_vColName;
};

class SqlScriptColumn
{
public:
	SqlScriptColumn(const CString& strColName, int nVaule);
	SqlScriptColumn(const CString& strColName, double dVaule);
	SqlScriptColumn(const CString& strColName, const CString& nVaule);
	//true 1, false 0
	//SqlScriptColumn(const CString& strColName, bool bVaule);

	CString EqualCondition();
public:
	CString GetName();
	CString GetValue();
protected:
	CString m_strColName;
	CString m_strValue;
private:
};



class SqlScriptColumnList
{
public:
	void AddColumn( SqlScriptColumn& columnUpdate );
	void AddColumn(const CString& strColName, int nVaule){ AddColumn(SqlScriptColumn(strColName,nVaule)); }
	void AddColumn(const CString& strColName, double dVaule){ AddColumn(SqlScriptColumn(strColName,dVaule)); }
	void AddColumn(const CString& strColName, const CString& nVaule){ AddColumn(SqlScriptColumn(strColName,nVaule)); }
protected:
	std::vector<SqlScriptColumn> m_vColumnList;

};



class SqlInsertScriptMaker : public SqlScriptColumnList
{
public:
	SqlInsertScriptMaker(const CString& strTableName);
	

public:

	CString GetScript();
protected:
	void ParseColumnAndValue(CString& strColNames, CString& strValues);
protected:
	CString m_strTableName;

private:
};


class SqlUpdateScriptMaker : public SqlScriptColumnList
{
public:
	SqlUpdateScriptMaker(const CString& strTableName);
	

public:	
	
	void SetCondition(const CString& strCondition);

	CString GetScript();
protected:

	void ParseColumnAndValue( CString& strColValues);
protected:
	CString m_strTableName;
	CString m_strCondition;
private:
};


class SqlDeleteScriptMaker
{
public:
	SqlDeleteScriptMaker(const CString& strTableName);
	~SqlDeleteScriptMaker();

public:
	void SetCondition(const CString& strCondition);
	CString GetScript();

protected:
	CString m_strTableName;
	CString m_strCondition;
private:
};
