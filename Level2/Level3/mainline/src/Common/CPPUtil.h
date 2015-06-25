#pragma once

#include <iosfwd>
#include <vector>
#include <list>
#include <algorithm>


namespace cpputil
{//noncopyable
	class noncopyable
	{
	protected:
		noncopyable() {}
		~noncopyable() {}
	private:  // emphasize the following members are private
		noncopyable( const noncopyable& );
		const noncopyable& operator=( const noncopyable& );
	};

	//auto ptr set function : delete ptr object, set ptr to 0/new ptr
	template<class T,class D>
	inline void autoPtrReset(T*& p, D* newp)
	{
		if(p==newp)
			return;
		if(p)
			delete p;
		p = newp;
	}

	template<class T>
	inline void autoPtrReset(T*& p, T* newp=0)
	{
		if(p==newp)
			return;
		if(p)
			delete p;
		p = newp;
	}


	template<class T>
	inline	void deletePtrMap(T& map)
	{
		for(T::iterator itr = map.begin();itr!=map.end();++itr)
		{
			delete itr->second;
		}
		map.clear();
	}

	template<class T>
	inline	void deletePtrVector(T& vec)
	{
		for(T::iterator itr = vec.begin();itr!=vec.end();++itr)
		{
			delete *itr;
		}
		vec.clear();
	}

	template<class T>
	class TPtrVector : public std::vector<T*>
	{
	public:
		bool add(T*t)
		{
			if(bHas(t))
				return false;
			push_back(t);
			return true;
		}
		bool remove(T* t)
		{
			iterator itr = std::find(begin(),end(),t);
			if(itr!=end())
			{
				erase(itr);
				return true;
			}
			return false;
		}
		bool bHas(T* t)const{  return std::find(begin(),end(),t)!=end(); }

		void deepClear()
		{
			for(iterator itr = begin();itr!=end();++itr)
			{
				delete *itr;
			}
			clear();
		}

		int count()const{ return (int)size(); }
	};


	//map with no auto sort 
	template<typename _Kty, typename _Vty> 
	class nosort_map
	{
	public:
		typedef std::pair<_Kty,_Vty > _ItemType;
		typedef std::list< _ItemType > _InternalTy;
		typedef typename _InternalTy::iterator iterator;
		typedef typename _InternalTy::const_iterator const_iterator;

		inline iterator begin(){ return m_data.begin(); }
		inline const_iterator begin()const{ return m_data.begin(); }
		inline iterator end(){ return m_data.end(); }
		inline const_iterator end()const{ return m_data.end(); }
		inline size_t size()const{ return m_data.size(); }
		inline bool empty()const{ return m_data.empty(); }
		const_iterator find(const _Kty& k)const 
		{
			for(const_iterator itr = begin();itr!=end();++itr)
			{
				if( itr->first == k ){
					return itr;
				}
			}
			return end();
		}
		inline bool has(const _Kty& k)const{ return find(k)!=end(); }

		_Vty& operator[](const _Kty& key)
		{
			for(iterator itr = begin();itr!=end();++itr)
			{
				if( itr->first == key ){
					return itr->second;
				}
			}
			m_data.push_back(_ItemType(key,_Vty()));
			return m_data.back().second;
		}

	protected:
		_InternalTy m_data;
	};


	//set with no auto sort
	template<class _Ty> 
	class nosort_set
	{
	public:
		typedef std::vector< _Ty > _InternalTy;
		typedef typename _InternalTy::iterator iterator;
		typedef typename _InternalTy::const_iterator const_iterator;

		inline iterator begin(){ return m_data.begin(); }
		inline const_iterator begin()const{ return m_data.begin(); }
		inline iterator end(){ return m_data.end(); }
		inline const_iterator end()const{ return m_data.end(); }
		inline size_t size()const{ return m_data.size(); }
		inline bool empty()const{ return m_data.empty(); }

		iterator add(const _Ty& t )
		{
			iterator itr = std::find(m_data.begin(),m_data.end(),t);
			if(itr!=m_data.end())
				return itr;
			
			m_data.push_back(t);
			return m_data.rbegin();
		}
	protected:
		_InternalTy m_data;
	};

	//auto sort when adding new data
	template<class _Ty, class _Pr = less<_Kty> >
	class sort_list : public std::list<_Ty>
	{
	public:
		void add(const _Ty& t , bool blower = false )
		{
			iterator pos = blower? std::lower_bound(begin(),end(),_Pr() ) : std::upper_bound(begin(),end(),_Pr() );
			insert(pos,t);
		}
		inline void sort(){ std::sort(begin(),end(),_Pr() );	}	
	};


	//auto sort when adding new data
	template<class _Ty, class _Pr = less<_Kty> >
	class sort_vector : std::vector<_Ty>
	{
	public:
		void add(const _Ty& t , bool blower = false )
		{
			iterator pos = blower? std::lower_bound(begin(),end(),_Pr() ) : std::upper_bound(begin(),end(),_Pr() );
			insert(pos,t);
		}
		inline void sort(){ std::sort(begin(),end(),_Pr() );	}	
	};


	//object have scope
	class Object;
	class Scope : public noncopyable
	{
	public:
		void add(Object* o)
		{
			m_objlist.add(o);
		}
		void release(Object* o)
		{
			m_objlist.remove(o);
		}
		~Scope()
		{
			TPtrVector<Object> olist;
			m_objlist.swap(olist);			
			olist.deepClear();
		}
	protected:
		TPtrVector<Object> m_objlist;
	};

	class Object : public noncopyable
	{
	public:
		Object():m_pScope(NULL){}
		
		virtual ~Object()
		{
			if(m_pScope)
				m_pScope->release(this);
		}
		
		void setScope(Scope* s)
		{ 
			if(m_pScope==s)
				return ;

			if(m_pScope)
				m_pScope->release(this);
		
			m_pScope = s; 
			
			if(m_pScope)
				m_pScope->add(this);
			
			return ;
		};
	protected:
		Scope* m_pScope;
	};
	
}



