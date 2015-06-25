#pragma once


//default copy helper class
template<class T>
struct CopyHelper
{
	static T* copy(T const* p){
		if(p)
			return new T(*p);
		return 0;
	}
};

template<class T>
struct ConstructHelper
{
	static T* construct()
	{
		return new T;
	}
};

//Instance Pointer that equal to the instance
template < class T ,class CT = ConstructHelper<T>, class CPT = CopyHelper<T> > 
class InstancePtr
{
public:
	typedef InstancePtr<T> self_type;

	InstancePtr():mpContent( CT::construct() ){ }

	InstancePtr(const InstancePtr<T>& other)
	{
		mpContent = CPT::copy( other.mpContent );
	}

	InstancePtr(T* ptr)
	{
		mpContent = CPT::copy(ptr);
	}

	

	self_type & operator=(const self_type& other)
	{		
		return wrap( CPT::copy( other.mpContent ) );
	}

	self_type& operator = (const T* ptr)
	{
		return wrap( CPT::copy(ptr) );
	}

	~InstancePtr()
	{
		wrap(0);
	}

	inline T& operator*() const { return *mpContent; }
	inline T* operator->() const { return mpContent; }
	inline bool isNull() const { return mpContent==0; }

	bool operator <(const self_type& other)const
	{ 
		if( isNull() )
			return true;
		if( other.isNull() )
			return false;		
		return (*mpContent) < (*other.mpContent);
	}

	
	T* mpContent;

private:
	self_type& wrap(T* pContent)
	{ 
		delete mpContent;
		mpContent = pContent;
		return *this;
	}	
};

template<class T>
void swap( InstancePtr<T>& p1, InstancePtr<T>& p2 )
{
	std::swap(p1.mpContent,p2.mpContent);
}