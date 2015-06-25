#pragma once

#include <set>
class class_type
{
public:
	bool IsDeriveFrom(class_type*)const;	

	typedef std::set<class_type*> parentList_type;
	parentList_type mplist;
};


#define DynamicClass	public:inline bool IsA(class_type* ct)const{ return type() == ct; }\
						inline bool IsKindof(class_type* ct)const{ return type()->IsDeriveFrom(ct); }\
						static class_type * _TYPE();\
						private: virtual class_type* type()const{ return _TYPE(); }


#define typeof(x) x::_TYPE()




template<typename TD, typename TP>
class RegInheritT 
{
public:
	RegInheritT();
};

#define RegisterInherit(TD, TP) private: RegInheritT<TD,TP> _reg##TD##TP;


