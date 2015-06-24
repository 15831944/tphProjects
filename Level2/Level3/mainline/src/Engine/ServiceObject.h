#pragma once 

/**
* Class to provide a base class with a virtual destructor that can be added
* to a simple list of objects to delete. Used for singleton patterns.
*/
// used in simEngine service objects.
class DelObject
{
public:
	DelObject() : m_pNextToDelete(NULL){}
	virtual ~DelObject(){}
	DelObject* m_pNextToDelete;
};

/**
* Class to provide managed singleton/orphan deletion.
*/
class DeletionManager
{
public:
	static void Register(DelObject* pObject);
	static void UnRegister(DelObject* pObject);
	static void DeleteAll();
	static DelObject* s_pFirst;
	static DelObject* s_pLast;
};

/**
* Class to provide simple singleton functionality.
*/
template <class T, bool t_bAutoRegister = true>
class ServiceObject : public DelObject
{
public:
	virtual ~ServiceObject(){}

	static bool HasInstance()
	{
		return s_pTheInstance != NULL;
	}

	static T* GetInstance()
	{
		if(!s_pTheInstance)
		{
			s_pTheInstance = T::CreateTheInstance();
		}
		return s_pTheInstance;
	}

	static T* CreateTheInstance()
	{
		T* pT = new T;
		//if(t_bAutoRegister)
		//{
		//	DeletionManager::Register(pT);
		//}
		return pT;
	}

	static T& GetInstanceRef()
	{
		return *GetInstance();
	}

	static void ReleaseInstance()
	{
		if(s_pTheInstance)
		{
			delete s_pTheInstance;
			s_pTheInstance = NULL;
		}
	}

protected:
		static T* s_pTheInstance;
};

template <class T, bool t_bAutoRegister>
		T* ServiceObject< T, t_bAutoRegister >::s_pTheInstance = NULL;

