#pragma once
class CACType ;
class CAcTypeAlias
{
public:
	CAcTypeAlias(void);
	~CAcTypeAlias(void);
protected:
	 CACType* m_AcTypeInstance ;
	std::vector<CString> m_AliasNames ;
	int m_ID ;
public:
	void SetActypeInstance( CACType* _actype) ;
	CACType* GetActypeInstance() ;
    
	void SetID(int _id) { m_ID = _id ;} ;
	int GetID() { return m_ID ;} ;

	void AddAliasName(CString& _strname) ;
	void RemoveAliasName(CString& _aliasname) ;

	CString FormatAliasName() ;

	BOOL CheckAliasName(CString& _aliasname) ;


	std::vector<CString>* GetAliasNames() { return &m_AliasNames ;} ;
	static void ReadDataFromDB(CAcTypeAlias* _PActypeAlias , int _ID) ;
	static void WriteDataFromDB(CAcTypeAlias* _PActypeAlias , int _ID) ;
};
