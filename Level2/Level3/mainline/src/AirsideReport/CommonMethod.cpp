#include "StdAfx.h"
#include "CommonMethod.h"
#include "../InputAirside/ALTObject.h"
#include "../InputAirside/IntersectionNode.h"
namespace CommonMethod
{
	CString GetObjectName(int objectType,int objID)
	{
		CString strObjName = _T("");
		if (objectType == 0)
		{
			ALTObject *pObject = ALTObject::ReadObjectByID(objID);
			if (pObject)
			{
				strObjName = pObject->GetObjNameString();
			}
		}
		else
		{
			IntersectionNode intersecNode;
			intersecNode.ReadData(objID);
			strObjName = intersecNode.GetName();
		}

		return strObjName;
	}
};