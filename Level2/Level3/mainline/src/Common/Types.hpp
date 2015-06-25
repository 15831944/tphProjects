#pragma once
#include <common/Types.h>
#include <vector>


template<typename T>
class class_type_t : public class_type
{
public:
	typedef class_type_t<T> self_type;	

	static typename self_type* Protype()
	{
		static self_type mProtype;
		return &mProtype;
	}
};

#define __TYPE(T) class_type_t<T>::Protype()

#define DynamicClassImpl(T)	class_type* T::_TYPE(){ return __TYPE(T);}
							

#define DynamicClassStatic(T)	public:	\
									inline bool IsA(class_type* ct)const{ return type() == ct; }\
									inline bool IsKindof(class_type* ct)const{ return type()->IsDeriveFrom(ct); }\
									static class_type * _TYPE(){ return __TYPE(T); }\
								private:\
									virtual class_type* type()const{ return _TYPE(); }


class _regInherit
{
public:
	_regInherit(class_type* ,class_type* );	
};

template<typename TD, typename TP>
RegInheritT<TD, TP>::RegInheritT()
{
	static TP*p(( TD*)0);//compile error: TD is not derived from TP
	static _regInherit reg( __TYPE(TD), __TYPE(TP) );reg; //make sure is used make this call once
}
