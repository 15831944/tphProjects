#ifndef __AIRSIDE_ENTITY_DEF
#define __AIRSIDE_ENTITY_DEF
#include "../EngineDll.h"
#include "../EngineConfig.h"
#include "../../Common/Referenced.h"
#include "../../Common/ref_ptr.h"
#include <vector>

NAMESPACE_AIRSIDEENGINE_BEGINE


//abstract class for simulation entity
class ENGINE_TRANSFER Entity : public Referenced{
public:
	Entity(){}
	virtual ~Entity();
	 
	CString m_strID;
	void setStringID(CString strID){ m_strID = strID; }
	CString getStringID()const{ return m_strID; }

};

template <class T> 
class EntContainer : public std::vector< ref_ptr<T> >{
public:
	T * getEntity(const CString& strID){
		for(iterator itr= begin();itr!=end();itr++){
			if(strID == (*itr)->getStringID()){ return (*itr).get(); }
		}
		return NULL;
	}

	bool addEntity(T  *newEnt){
		for(iterator itr = begin();itr!=end();itr++){
			if( (*itr)->getStringID() == newEnt->getStringID() ) return false;
		}
		push_back(newEnt);
		return true;
	}
};


NAMESPACE_AIRSIDEENGINE_END
#endif