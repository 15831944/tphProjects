#ifndef  __REFERENCED_H
#define  __REFERENCED_H

#pragma once

class Referenced
{

public:


	inline Referenced(){ _refCount=0;}

	inline Referenced(const Referenced&){_refCount=0;}

	inline Referenced& operator = (const Referenced&) { return *this; }

	/** Set whether to use a mutex to ensure ref() and unref() are thread safe.*/
	//void setThreadSafeRefUnref(bool threadSafe);

	/** Get whether a mutex is used to ensure ref() and unref() are thread safe.*/
	//bool getThreadSafeRefUnref() const { return _refMutex!=0; }

	/** Increment the reference count by one, indicating that 
	this object has another pointer which is referencing it.*/
	inline void ref() const{++_refCount;}

	/** Decrement the reference count by one, indicating that 
	a pointer to this object is referencing it.  If the
	reference count goes to zero, it is assumed that this object
	is no longer referenced and is automatically deleted.*/
	inline void unref() const{
		--_refCount;
		if(_refCount<1)delete this;	
	}

	/** Decrement the reference count by one, indicating that 
	a pointer to this object is referencing it.  However, do
	not delete it, even if ref count goes to 0.  Warning, unref_nodelete() 
	should only be called if the user knows exactly who will
	be resonsible for, one should prefer unref() over unref_nodelete() 
	as the later can lead to memory leaks.*/
	inline void unref_nodelete() const{--_refCount;}

	/** Return the number pointers currently referencing this object. */
	inline int referenceCount() const { return _refCount; }


public:

	/** Set whether reference counting should be use a mutex to create thread reference counting.*/
	//static void setThreadSafeReferenceCounting(bool enableThreadSafeReferenceCounting);

	/** Get whether reference counting is active.*/
	//static bool getThreadSafeReferenceCounting();

	//friend class DeleteHandler;

	/** Set a DeleteHandler to which deletion of all referenced counted objects
	* will be delegated to.*/
	//static void setDeleteHandler(DeleteHandler* handler);

	/** Get a DeleteHandler.*/
	//static DeleteHandler* getDeleteHandler();


protected:
	inline virtual ~Referenced(){}

	mutable int                  _refCount;

};





#endif