// Rule.h: interface for the CRule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RULE_H__0D100E0B_17D1_4DB1_8F9B_7728D7E0F1EF__INCLUDED_)
#define AFX_RULE_H__0D100E0B_17D1_4DB1_8F9B_7728D7E0F1EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Person;

class CRule  
{
public:
	CRule(){};
	virtual ~CRule(){};
public:
	virtual bool Vote(const Person& _man) const=0;
	virtual CRule* Clone() const =0;//to new a rule instance;

};

class CSkipProcRule : public CRule
{
private:
	int m_nSkipTime;//minutes
public:
	CSkipProcRule(int _nSkipTime);
	CSkipProcRule(const CSkipProcRule& _other);
	virtual ~CSkipProcRule(){};
public:
	bool Vote(const Person& _man) const;
	CRule* Clone() const;
};

#endif // !defined(AFX_RULE_H__0D100E0B_17D1_4DB1_8F9B_7728D7E0F1EF__INCLUDED_)
