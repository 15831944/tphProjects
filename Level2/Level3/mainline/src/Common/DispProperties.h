#pragma once

#include <common/ARCColor.h>
#include <map>
struct  DispProp 
{
	DispProp()
	{
	}

	DispProp(BOOL b, ARCColor3 c)
		:bOn(b)
		,cColor(c)
	{
	}
	BOOL bOn;
	ARCColor3 cColor ;
};

class CDispProperties
{
public:
	CDispProperties(void);
	~CDispProperties(void);	

	DispProp& operator[](int idx);
	//serialization
	CString ToString()const;
	void FromString(CString str);

protected:
	typedef std::map<int , DispProp> MapType;
	MapType mDispPropMap;
	void ParseDSP(CString str);
	
private:
	MapType mDefaultDispPropMap;
	DispProp& getDefault(int idx);


};
