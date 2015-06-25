#pragma once
#include "template.h"
#include "TERM_BIN.H"
//class CPaxTyLevel
//{
//public:
//    CPaxTyLevel(int level ,CString& _name) ;
//	~CPaxTyLevel() ;
//private:
//	CString m_ty ;
//	int m_level ;
//public:
//	CString GetTyName() const;
//	int GetLevel() const;
//	void SetTyName(CString& _name) ;
//	void SetLevel(int _level) ;
//
//public:
//	bool Fits(const CPaxTyLevel&) const;
//	bool Equals(const CPaxTyLevel&) const;
//};

class StringDictionary;
class CPassengerType
{
public:
	CPassengerType(StringDictionary *pStrDict);
	~CPassengerType(void);

public:

	//void AddLevelPaxType(int _level , CString& _name) ;//
	//void DelLevelPaxType(int _level);//
	//BOOL FindLevelName(int _level,CString& _name) const;//
    CString PrintStringForShow() const;
	//void PrintStringForShow(char* p_str) const;
	bool screenPrint(char *p_str, int _nLevel =0, unsigned p_maxChar =70) const;
	bool screenPrint(CString& strContent, int _nLevel =0, unsigned p_maxChar =70) const;
	CString GetStringForDatabase() const;
	void FormatLevelPaxTypeFromString(CString _PaxTy);

	bool Fits(const CPassengerType& cmpPaxType) const; //big fit small
	bool IsEqual(const CPassengerType& cmpPaxType) const;

	void initDefault (void);
	int isDefault (void) const;
	void initPaxType (const MobDescStruct &p_struct);

	void setUserType (int p_level, int p_type);;
	void setUserType (const int *p_type);
	int getUserType (int p_level) const;
	void getUserType (int *p_type) const;

	int contains (int p_level, int p_index) const;
	void copyData(const CPassengerType& paxType);
	void SetStringDictionary(StringDictionary *pStrDict);

	bool operator < (const CPassengerType& paxType)const;

private:
	//----------------------------------------------------------------------------
	//Summary:
	//		retrieve user define type count
	//Return:
	//		int: user define type count
	//---------------------------------------------------------------------------
	int GetUserTypeCount()const;
private:
	int userTypes[MAX_PAX_TYPES];
	//InputTerminal* m_pInTerm;
	//comes from InputTerminal
	StringDictionary *m_pStrDict;
};
