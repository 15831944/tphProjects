#ifndef __REF_PTR_BEE_DEF
#define __REF_PTR_BEE_DEF

template<typename T>
class ref_ptr_bee
{
public:
	ref_ptr_bee(void) :_ptr(0L),_ref_count(0L) {}
	ref_ptr_bee(T* t):_ptr(t),_ref_count(0L){ if (_ptr)ref(); }
	ref_ptr_bee(const ref_ptr_bee& rp):_ptr(rp._ptr),_ref_count(rp._ref_count) { if (_ptr)ref(); }
	~ref_ptr_bee(void) { if (_ptr)unref(); _ptr = 0;_ref_count = 0; }

	inline ref_ptr_bee& operator = (const ref_ptr_bee& rp)
	{
		if (_ptr==rp._ptr) return *this;
		unref();
		_ptr = rp._ptr;
		_ref_count = rp._ref_count;
		if (_ptr)ref();
		return *this;
	}

	inline ref_ptr_bee& operator = (T* ptr)
	{
		if (_ptr==ptr) return *this;
		unref();
		_ptr = ptr;
		_ref_count = 0L;
		if (_ptr)ref();
		return *this;
	}
	
	inline bool operator == (const ref_ptr_bee& rp) const { return (_ptr==rp._ptr); }
	inline bool operator != (const ref_ptr_bee& rp) const { return (_ptr!=rp._ptr); }
	inline bool operator < (const ref_ptr_bee& rp) const { return (_ptr<rp._ptr); }
	inline bool operator > (const ref_ptr_bee& rp) const { return (_ptr>rp._ptr); }

	inline bool operator == (const T* ptr) const { return (_ptr==ptr); }
	inline bool operator != (const T* ptr) const { return (_ptr!=ptr); }
	inline bool operator < (const T* ptr) const { return (_ptr<ptr); }
	inline bool operator > (const T* ptr) const { return (_ptr>ptr); }

	inline T& operator*() { return *_ptr; }

	inline const T& operator*() const { return *_ptr; }

	inline T* operator->()   { return _ptr; }

	inline const T* operator->() const   { return _ptr; }

	inline bool operator!() const   { return _ptr==0L; }

	inline bool valid(void) const       { return _ptr!=0L; }
	
protected:
	inline void ref(void)
	{
		if(_ref_count)
			(*_ref_count)++;
		else {
			_ref_count = new unsigned long;
			if(_ref_count)
				*_ref_count = 1;
			else{
				if(_ptr){
					delete _ptr;
					_ptr = 0;
				}
			}				
		}
	}

	inline void unref(void)
	{
		if(_ref_count){
			(*_ref_count)--;
			if(*_ref_count < 1){
				if(_ptr){
					delete _ptr;
					_ptr = 0;
				}
				delete _ref_count;
				_ref_count = 0;
			}
		}else {
			if(_ptr){
				delete _ptr;
				_ptr = 0;
			}	
		}
	}
private:
	T* _ptr;
	mutable unsigned long * _ref_count;
};

#endif