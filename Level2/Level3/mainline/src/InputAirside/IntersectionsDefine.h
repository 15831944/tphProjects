#pragma once
#include <vector>
#include "ALTObject.h"

class INPUTAIRSIDE_API InsecObjectPart //iop
{
	
public:
	int nALTObjectID;//object id 
	int part;                    //part of the stretch
	double pos;                  //relative position of the stretch path(0-1)
	ALTObject::RefPtr m_pObj;
	
	

	ALTObject * GetObject();
	int GetObjectID()const;
	void SetObject(ALTObject * pObj);
	

	InsecObjectPart();
	~InsecObjectPart(void);
	InsecObjectPart(const InsecObjectPart&);
};

typedef std::vector<InsecObjectPart> InsecObjectPartVector;
