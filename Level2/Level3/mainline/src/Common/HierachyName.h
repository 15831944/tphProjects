#pragma once
#include <vector>

class CHierachyName
{
public:
	CHierachyName(){}
	CHierachyName(const CString& sname);

	int getLevelCount()const{ return (int)mLevelNames.size(); }
	CString& getLevel(int i){ return mLevelNames[i]; }
	const CString& getLevel(int i)const{ return mLevelNames[i]; }

	void fromString(const CString& s,const CString& sep = "-");
	CString toString(const CString& sep="-")const;
	CString GetString(int m_id)const;
	//the key string means all names
	virtual CString GetAllString()const{ return _T("All"); }
	bool isAll()const{ return mLevelNames.empty(); }
	bool isEmpty()const{ return mLevelNames.empty(); }

	//can this name fit int the hierarchy, include ==
	bool fitIn(const CHierachyName& othername )const;
	bool fitIn(const CString& s)const;
	bool operator<(const CHierachyName& otherName)const;
	bool operator==(const CHierachyName& other)const;

	CHierachyName& PopFront();
protected:
	std::vector<CString> mLevelNames;

};
