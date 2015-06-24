#if !defined(_ALTOBJECTCOMMANDUTIL_H__INCLUDED_)
#define _ALTOBJECTCOMMANDUTIL_H__INCLUDED_
#include "ALTObject3D.h"

inline
static void FillALTObjectIDVector(std::vector<UINT>& altobjectidlist, std::vector< ref_ptr<ALTObject3D> >& altobjectlist)
{
	altobjectidlist.clear();
	altobjectidlist.reserve(altobjectlist.size());
	for(std::vector< ref_ptr<ALTObject3D> >::iterator it=altobjectlist.begin(); it!=altobjectlist.end(); ++it) {
		altobjectidlist.push_back( (*it)->GetID() );
	}
}

inline
static void FillALTObjectVector(std::vector< ref_ptr<ALTObject3D> >& altobjectlist, std::vector<UINT>& altobjectidlist)
{
	altobjectlist.clear();
	altobjectlist.reserve(altobjectidlist.size());
	for(std::vector<UINT>::iterator it=altobjectidlist.begin(); it!=altobjectidlist.end(); ++it) {
		ALTObject* pALTObject;
		pALTObject = ALTObject::ReadObjectByID(*it);
		if(pALTObject)	
		{
			ALTObject3D* pALTObject3D = ALTObject3D::NewALTObject3D(pALTObject);
			pALTObject3D->SetID(*it);
			altobjectlist.push_back(pALTObject3D);	
		}
	}
}

#endif