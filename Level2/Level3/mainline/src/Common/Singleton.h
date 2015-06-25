#pragma once 

/**
* Class to provide simple singleton functionality.
*/

template <class T>
class Singleton
{
public:
	virtual ~Singleton(){}

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

template <class T>
T* Singleton< T >::s_pTheInstance = NULL;
