#ifndef ARRAYS_H
#define ARRAYS_H

#pragma warning(disable:4275)

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <vector>
#include <algorithm>
#include <cassert>
#include "replace.h"


class  TShouldDelete
{
public:
	enum DeleteType{NoDelete, Delete, Default};
};

template<class T>
class  TArrayAsVector:public stdvector<T> 
{
public:
	TArrayAsVector( int upper, int lower, int delta)
	{  
		assert(lower == 0);
		reserve(upper);	
	}

	virtual ~TArrayAsVector() {}
	//Adds a T object at the next available index at the end of an array. 
	//Adding an element beyond the upper bound leads to an overflow condition. 
	//If overflow occurs and delta is nonzero, the array is 
	//expanded (by sufficient multiples of delta bytes) to accommodate 
	//the addition. If delta is zero, Add fails. Add returns 0 if 
	//it couldn't add the object.
	int Add( const T& t ) {	push_back(t); return 1;}
	//Removes the object at the given index. The object will be 
	//destroyed.
	int Destroy( int i ) { 
		if(i==INT_MAX){ 
			ASSERT(FALSE);
			return 1; }
		erase(begin() + i );
		return 1;
	}
	//Removes the given object and destroys it.
	int Destroy( const T& t ) 
	{	
		iterator it = stdfind(begin(), end(), t);
		if (it != end()) 
			erase(it);
		return 1;
	}
	//Removes the object at loc.
	int Detach( int loc ) { 
		if(i==INT_MAX){ 
			ASSERT(FALSE);
			return 1; }
		erase(begin() + loc );	return 1;}
	//Removes the first object that compares equal to the 
	//specified object.
	int Detach( const T& t )
	{	
		iterator it = stdfind(begin(), end(), t);
		if (it != end()) 
			erase(it);
		return 1;
	}
	//Finds the specified object and returns the object's index; otherwise returns INT_MAX.
	int Find( const T& t ) const 
	{	
		const_iterator it = stdfind(begin(), end(), t);
		if (it != end()) 
			return it - begin();
		else
			return INT_MAX;
	}
	//Removes all elements from the array without destroying the array.
	void Flush() {	clear();}
	//Returns the number of items in the array, as distinguished from 
	//ArraySize, which returns the size of the array.
	unsigned GetItemsInContainer() const { return size(); }
	//Returns 1 if the array contains no elements; otherwise returns 0.
	int IsEmpty() const { return empty(); }

	reference ItemAt(size_type pos) {	return at(pos); }
    const_reference ItemAt(size_type pos) const {return at(pos); }
	//Returns a reference to the element at the location specified
	//by loc. The non-const version resizes the array if it's necessary 
	//to make loc a valid index.
	//T& operator []( int loc ) ;
	//The const throws an exception in the debugging version on an 
	//attempt to index out of bounds.
	//T& operator []( int loc ) const;
};

template<class T>
class  TSArrayAsVector:public stdvector<T> 
{
private:
	int needResort;
public:
	TSArrayAsVector( int upper, int lower, int delta) :needResort(0)
	{  
		assert(lower == 0);
		reserve(upper);	
	}

	virtual ~TSArrayAsVector() { }

	//Adds a T object at the next available index at the end of an array. 
	//Adding an element beyond the upper bound leads to an overflow condition. 
	//If overflow occurs and delta is nonzero, the array is 
	//expanded (by sufficient multiples of delta bytes) to accommodate 
	//the addition. If delta is zero, Add fails. Add returns 0 if 
	//it couldn't add the object.
	int Add( const T& t ) 
	{	
		push_back(t); 
		needResort = 1;
		return 1;
	}
	//Removes the object at the given index. The object will be 
	//destroyed.
	int Destroy( int i ) 
	{ 		
		if(i==INT_MAX){ 
			ASSERT(FALSE);
			return 1; }

		if (needResort)
			sort(begin(), end());
		needResort = 0;
		erase(begin() + i );	
		return 1;
	}
	//Removes the given object and destroys it.
	int Destroy( const T& t ) 
	{	
		iterator it = stdfind(begin(), end(), t);
		if (it != end()) 
			erase(it);
		return 1;
	}
	//Removes the object at loc.
	int Detach( int loc ) 
	{ 
		if(i==INT_MAX){ 
			ASSERT(FALSE);
			return 1; }

		if (needResort)
			sort(begin(), end());
		needResort = 0;
		erase(begin() + loc );	
		return 1;
	}
	//Removes the first object that compares equal to the 
	//specified object.
	int Detach( const T& t )
	{	
		iterator it = stdfind(begin(), end(), t);
		if (it != end()) 
			erase(it);
		return 1;
	}
	//Finds the specified object and returns the object's index; otherwise returns INT_MAX.
	int Find( const T& t ) 
	{	
		if (needResort)
			sort(begin(), end());
		needResort = 0;

		const_iterator it = stdfind(begin(), end(), t);
		if (it != end()) 
			return it - begin();
		else
			return INT_MAX;
	}
	//Removes all elements from the array without destroying the array.
	void Flush() {	clear();}
	//Returns the number of items in the array, as distinguished from 
	//ArraySize, which returns the size of the array.
	unsigned GetItemsInContainer() const { return size(); }
	//Returns 1 if the array contains no elements; otherwise returns 0.
	int IsEmpty() const { return empty(); }

	const_reference ItemAt(size_type pos) 
	{	
		if (needResort)
			sort(begin(), end());
		needResort = 0;

		return at(pos); 
	}
    
	//Returns a reference to the element at the location specified
	//by loc. The non-const version resizes the array if it's necessary 
	//to make loc a valid index.
	const_reference operator []( int loc ) 
	{
		if (needResort)
			stdsort(begin(), end());
		needResort = 0;
		return at(loc);
	}
	//The const throws an exception in the debugging version on an 
	//attempt to index out of bounds.
	//T& operator []( int loc ) const;
};

template<class T>
class  TIArrayAsVector:public stdvector<T*>
{
private:
	int ownsElements;
public:
	TIArrayAsVector( int upper, int lower, int delta)
		:ownsElements(1)
	{
		assert(lower == 0);
		reserve(upper);
	}

	virtual ~TIArrayAsVector() {	Flush(TShouldDelete::Default);}
	//Adds a pointer to a T object at the next available index at 
	//the end of an array. Adding an element beyond the upper 
	//bound leads to an overflow condition. If overflow occurs 
	//and delta is nonzero, the array is expanded (by sufficient
	//multiples of delta bytes) to accommodate the addition. 
	//If delta is zero, Add fails. Add returns 0 if the object 
	//couldn't be added.
	virtual int Add( T *t ) {	push_back(t); return 1;}
	//Removes the object at the given index. The object will be 
	//destroyed.
	int Destroy( int i )  
	{ 
		if(i==INT_MAX){ 
			ASSERT(FALSE);
			return 1; }

		T* ele = at(i);
		delete ele;
		erase(begin() + i );	
		return 1;
	}

	//Removes the given object and destroys it.
	int Destroy( T *t )
	{	
		if (empty())
			return 1;
		iterator it = begin();
		while (it != end())
		{
			if (*at(it-begin()) == *t)
			{
				T* temp = at(it-begin());
				erase(it);	
				delete temp;
				break;
			}
			it++;
		}
		return 1;
	}
	//Removes the object at loc.
	int Detach( int loc, TShouldDelete::DeleteType type = TShouldDelete::Default)
	{ 
		if(loc==INT_MAX){ 
			ASSERT(FALSE);
			return 1; }

		if (  (type == TShouldDelete::Delete)
			||( type == TShouldDelete::Default && ownsElements) )
			Destroy(loc);
		else
			erase(begin() + loc);
		return 1;
	}
	//Removes the first object that compares equal to the 
	//specified object.
	int Detach( T* t, TShouldDelete::DeleteType type = TShouldDelete::Default)
	{	
		if (empty())
			return 1;

		iterator it = begin();
		while (it != end())
		{
			if (**it == *t)
			{
				if (  (type == TShouldDelete::Delete)
					||( type == TShouldDelete::Default && ownsElements) )
					Destroy(it - begin());
				else
					erase(it);	
				break;
			}
			it++;
		}
		return 1;
	}
	//Finds the specified object and returns the object's index; otherwise returns INT_MAX.
	int Find( const T* t ) const
	{	
		if (empty())
			return INT_MAX;

		const_iterator it = begin(); 
		while (it != end())
		{
			if (**it == *t)
			{
				return it - begin();
			}
			else
				it++;
		}
		return INT_MAX;
	}
	//Removes all elements from the array without destroying the array.
	void Flush(TShouldDelete::DeleteType type=TShouldDelete::Default )
	{
		if (  (type == TShouldDelete::Delete)
			||( type == TShouldDelete::Default && ownsElements) )
		{
			iterator it = begin();
			while (it != end())
			{
				delete at(it-begin()) ;
				it++;
			}
		}
		clear();
	}
	//Returns the number of items in the array, as distinguished from 
	//ArraySize, which returns the size of the array.
	size_t GetItemsInContainer() const {	return size();}
	//Returns 1 if the array contains no elements; otherwise returns 0.
	int IsEmpty() const {	return empty();}
	
    const_reference ItemAt(size_type pos){return at(pos); }
    const_reference ItemAt(size_type pos) const {return at(pos); }
	//Returns a reference to the element at the location specified
	//by loc. The non-const version resizes the array if it's necessary 
	//to make loc a valid index.
	//T * &  operator []( int loc )
	//The const throws an exception in the debugging version on an 
	//attempt to index out of bounds.
	//T * &  operator []( int loc ) const;
	void OwnsElements(int owns) { ownsElements = owns; }
};

template<class T>
class  TISArrayAsVector:public TIArrayAsVector<T>
{
public:
	TISArrayAsVector( int upper, int lower, int delta)
		:TIArrayAsVector<T>(upper, lower, delta)
	{ ;}

	virtual ~TISArrayAsVector() { Flush(TShouldDelete::Default);}
	//Adds a pointer to a T object at the next available index at 
	//the end of an array. Adding an element beyond the upper 
	//bound leads to an overflow condition. If overflow occurs 
	//and delta is nonzero, the array is expanded (by sufficient
	//multiples of delta bytes) to accommodate the addition. 
	//Add returns 0 if the object couldn't be added.
	int Add( T *t ) 
	{	
		if (empty())
		{
			insert(begin(), t);
			return 1;
		}
		int i = 0, j = (int)(end()-1-begin());
		while (j-i >1)
		{
			if (*t < *(at( (i+j)/2)))
				j = (i+j) / 2;
			else
				i = (i+j) / 2;
		}
		if ( *t < *(at(i)))
			insert(begin()+i, t);
		else 
		{	
			if ( *t < *(at(j)))
				insert(begin()+j, t);
			else
				insert(begin() + j + 1, t);
		}
		return 1;
	}
};



#endif
