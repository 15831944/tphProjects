#pragma once


template<class T>
class ref_ptr
{

public:
	typedef T element_type;

	ref_ptr() :_ptr(0L) {}
	ref_ptr(T* t):_ptr(t)              { if (_ptr) _ptr->ref(); }
	ref_ptr(const ref_ptr& rp):_ptr(rp._ptr)  { if (_ptr) _ptr->ref(); }
	~ref_ptr()                           { if (_ptr) _ptr->unref(); _ptr=0; }

	inline ref_ptr& operator = (const ref_ptr& rp)
	{
		if (_ptr==rp._ptr) return *this;
		T* tmp_ptr = _ptr;
		_ptr = rp._ptr;
		if (_ptr) _ptr->ref();
		// unref second to prevent any deletion of any object which might
		// be referenced by the other object. i.e rp is child of the
		// original _ptr.
		if (tmp_ptr) tmp_ptr->unref();
		return *this;
	}

	inline ref_ptr& operator = (T* ptr)
	{
		if (_ptr==ptr) return *this;
		T* tmp_ptr = _ptr;
		_ptr = ptr;
		if (_ptr) _ptr->ref();
		// unref second to prevent any deletion of any object which might
		// be referenced by the other object. i.e rp is child of the
		// original _ptr.
		if (tmp_ptr) tmp_ptr->unref();
		return *this;
	}

	// comparison operators for ref_ptr.
	inline bool operator == (const ref_ptr& rp) const { return (_ptr==rp._ptr); }
	inline bool operator != (const ref_ptr& rp) const { return (_ptr!=rp._ptr); }
	inline bool operator < (const ref_ptr& rp) const { return (_ptr<rp._ptr); }
	inline bool operator > (const ref_ptr& rp) const { return (_ptr>rp._ptr); }

	// comparison operator for const T*.
	inline bool operator == (const T* ptr) const { return (_ptr==ptr); }
	inline bool operator != (const T* ptr) const { return (_ptr!=ptr); }
	inline bool operator < (const T* ptr) const { return (_ptr<ptr); }
	inline bool operator > (const T* ptr) const { return (_ptr>ptr); }


	inline T& operator*()  { return *_ptr; }

	inline const T& operator*() const { return *_ptr; }

	inline T* operator->() { return _ptr; }

	inline const T* operator->() const   { return _ptr; }

	inline bool operator!() const   { return _ptr==0L; }

	inline bool valid() const       { return _ptr!=0L; }

	inline T* get() { return _ptr; }

	inline const T* get() const { return _ptr; }

	/** take control over the object pointed to by ref_ptr, unreference but do not delete even if ref count goes to 0,
	* return the pointer to the object.
	* Note, do not use this unless you are 100% sure your code handles the deletion of the object correctly, and
	* only use when absolutely required.*/
	inline T* take() { return release();}

	inline T* release() { T* tmp=_ptr; if (_ptr) _ptr->unref_nodelete(); _ptr=0; return tmp;}

private:
	T* _ptr;
};

