#pragma once
#include "Terminal3DListBase.h"

#include <boost/bind.hpp>
#include <Common/Guid.h>

// template arguments:
//		ListType   - your own class derived from this template
//			sometimes you need to rewrite CreateNew3D
//
//		DataType   - the data object, typically a class derived from CBaseObject or CObjectDisplay and implements the interfaces:
//			defines a static GUID member
//			implements getGuid()
//			implements GetFloorIndex()
//
//		C3DObjType - the 3D object
//			derived from CTerminal3DObject and implements all the interfaces
//			has two typename SharedPtr and List
//

template <class ListType, class DataType, class C3DObjType>
class /*RENDERENGINE_API*/ CTerminal3DList : public CRender3DListBase
{
public:

	typedef typename C3DObjType::SharedPtr Shared3DPtr;
	typedef typename C3DObjType::List      Shared3DList;

	CTerminal3DList()
	{}

	// for Update() and Update3DObject() only, external code should not use this method
	// class derived from CTerminal3DList may sometimes need to rewrite this method
	//C3DObjType* CreateNew3D(DataType* pData, size_t /*nIndex*/) const
	//{
	//	return new C3DObjType(m_p3DScene->AddGUIDQueryableSceneNode(pData), m_p3DScene, pData->getGuid());
	//}

	// Update entire object list
	bool Update()
	{
		std::for_each(m_3DObjects.begin(), m_3DObjects.end(), boost::BOOST_BIND(&C3DNodeObject::Detach, _1)); // must detach all first
		m_3DObjects.clear();
		std::vector<DataType*> vecData ;
	/*	if (m_p3DScene->QueryDataObjectList<DataType>(vecData))
		{
			m_3DObjects.reserve(vecData.size());
			std::vector<DataType*>::iterator ite = vecData.begin();
			std::vector<DataType*>::iterator iteBg = ite;
			std::vector<DataType*>::iterator iteEn = vecData.end();
			for (;ite!=iteEn;ite++)
			{
				Shared3DPtr p3D = Shared3DPtr(static_cast<ListType*>(this)->CreateNew3D(*ite, ite - iteBg));
				CRenderFloor3D::SharedPtr pFloor3D = m_p3DScene->GetFloors().GetFloorByLevel((*ite)->GetFloorIndex());
				ASSERT(pFloor3D);
				p3D->AttachTo(*pFloor3D);
				p3D->Update3D(*ite);
				m_3DObjects.push_back(p3D);
			}
			return true;
		}*/
		return false;
	}

	// update specified 3d object with guid and update code
	// update code can be declared and implemented in C3DObjType
	bool Update3DObject(const CGuid& guid, int nUpdateCode = Terminal3DUpdateAll)
	{
		Shared3DList::iterator ite = std::find_if(m_3DObjects.begin(), m_3DObjects.end(),
			boost::BOOST_BIND(&CTerminal3DObject::GetGuid, _1) == guid);
		DataType* pData = m_p3DScene->QueryDataObject<DataType>(guid);

		if (m_3DObjects.end() != ite)
		{
			if (pData)
			{
				// update
				(*ite)->Update3D(pData, nUpdateCode);
			}
			else
			{
				// delete
				(*ite)->Detach();
				m_3DObjects.erase(ite);
			}
		}
		else if (pData)
		{
			// add
			/*int nFloorLevel = pData->GetFloorIndex();
			CRenderFloor3D::SharedPtr pFloor3D = m_p3DScene->GetFloors().GetFloorByLevel(nFloorLevel);
			if (pFloor3D)
			{
				Shared3DPtr p3D = Shared3DPtr(static_cast<ListType*>(this)->CreateNew3D(pData, m_3DObjects.size()));
				p3D->AttachTo(*pFloor3D);
				p3D->Update3D(pData);
				m_3DObjects.push_back(p3D);
			}*/
		}
		else
		{
			ASSERT(FALSE);
			return false;
		}
		return true;
	}
	Shared3DPtr Find3DObj( const CGuid& guid ) const
	{
		Shared3DList::const_iterator ite = std::find_if(m_3DObjects.begin(), m_3DObjects.end(),
			boost::BOOST_BIND(&CTerminal3DObject::GetGuid, _1) == guid);
		if (m_3DObjects.end() != ite)
			return *ite;
		return Shared3DPtr();
	}

protected:
	Shared3DList m_3DObjects;
};



template <class ListType, class DataType, class C3DObjType>
class /*RENDERENGINE_API*/ CAirside3DList : public CRender3DListBase
{
public:

	typedef typename C3DObjType::SharedPtr Shared3DPtr;
	typedef typename C3DObjType::List      Shared3DList;

	CAirside3DList()
	{}

	// for Update() and Update3DObject() only, external code should not use this method
	// class derived from CTerminal3DList may sometimes need to rewrite this method
	C3DObjType* CreateNew3D(DataType* pData, size_t /*nIndex*/) const
	{
		return new C3DObjType(m_p3DScene->AddGUIDQueryableSceneNode(pData), m_p3DScene, pData->getGuid());
	}

	// Update entire object list
	bool Update()
	{
		std::for_each(m_3DObjects.begin(), m_3DObjects.end(), boost::BOOST_BIND(&C3DNodeObject::Detach, _1)); // must detach all first
		m_3DObjects.clear();
		std::vector<DataType*> vecData ;
		if (m_p3DScene->QueryDataObjectList<DataType>(vecData))
		{
			m_3DObjects.reserve(vecData.size());
			std::vector<DataType*>::iterator ite = vecData.begin();
			std::vector<DataType*>::iterator iteBg = ite;
			std::vector<DataType*>::iterator iteEn = vecData.end();
			for (;ite!=iteEn;ite++)
			{
				Shared3DPtr p3D = Shared3DPtr(static_cast<ListType*>(this)->CreateNew3D(*ite, ite - iteBg));

				p3D->AttachTo(m_p3DScene->GetRoot());
				p3D->Update3D(*ite);
				m_3DObjects.push_back(p3D);
			}
			return true;
		}
		return false;
	}

	// update specified 3d object with guid and update code
	// update code can be declared and implemented in C3DObjType
	bool Update3DObject(const CGuid& guid, int nUpdateCode = Terminal3DUpdateAll)
	{
		Shared3DList::iterator ite = std::find_if(m_3DObjects.begin(), m_3DObjects.end(),
			boost::BOOST_BIND(&CAirside3DObject::GetGuid, _1) == guid);
		DataType* pData = m_p3DScene->QueryDataObject<DataType>(guid);

		if (m_3DObjects.end() != ite)
		{
			if (pData)
			{
				// update
				(*ite)->Update3D(pData, nUpdateCode);
			}
			else
			{
				// delete
				(*ite)->Detach();
				m_3DObjects.erase(ite);
			}
		}
		else if (pData)
		{
			// add

			Shared3DPtr p3D = Shared3DPtr(static_cast<ListType*>(this)->CreateNew3D(pData, m_3DObjects.size()));
			p3D->AttachTo(m_p3DScene->GetRoot());
			p3D->Update3D(pData);
			m_3DObjects.push_back(p3D);
			
		}
		else
		{
			ASSERT(FALSE);
			return false;
		}
		return true;
	}
	Shared3DPtr Find3DObj( const CGuid& guid ) const
	{
		Shared3DList::const_iterator ite = std::find_if(m_3DObjects.begin(), m_3DObjects.end(),
			boost::BOOST_BIND(&CAirside3DObject::GetGuid, _1) == guid);
		if (m_3DObjects.end() != ite)
			return *ite;
		return Shared3DPtr();
	}

protected:
	Shared3DList m_3DObjects;
};


class CRenderAirside3D;

template <class ListType, class DataType, class C3DObjType>
class /*RENDERENGINE_API*/ CAirportObject3DList : public CAirportObject3DListBase
{
public:

	typedef typename C3DObjType::SharedPtr Shared3DPtr;
	typedef typename C3DObjType::List      Shared3DList;

	CAirportObject3DList()
	{}

	// for Update() and Update3DObject() only, external code should not use this method
	// class derived from CTerminal3DList may sometimes need to rewrite this method
	C3DObjType* CreateNew3D(DataType* pData, size_t /*nIndex*/) const
	{
		return new C3DObjType(m_p3DScene->AddGUIDQueryableSceneNode(pData), m_p3DScene, m_pAirport3D, pData->getGuid());
	}

	// Update entire object list
	bool Update()
	{
		std::for_each(m_3DObjects.begin(), m_3DObjects.end(), boost::BOOST_BIND(&C3DNodeObject::Detach, _1)); // must detach all first
		m_3DObjects.clear();
		std::vector<DataType*> vecData ;
		if (m_pAirport3D->QueryDataObjectList<DataType>(vecData))
		{
			m_3DObjects.reserve(vecData.size());
			std::vector<DataType*>::iterator ite = vecData.begin();
			std::vector<DataType*>::iterator iteBg = ite;
			std::vector<DataType*>::iterator iteEn = vecData.end();
			for (;ite!=iteEn;ite++)
			{
				Shared3DPtr p3D = Shared3DPtr(static_cast<ListType*>(this)->CreateNew3D(*ite, ite - iteBg));

				p3D->AttachTo(m_p3DScene->GetRoot());
				p3D->Update3D(*ite);
				m_3DObjects.push_back(p3D);
			}
			return true;
		}
		return false;
	}

	// update specified 3d object with guid and update code
	// update code can be declared and implemented in C3DObjType
	bool Update3DObject(const CGuid& guid, int nUpdateCode = Terminal3DUpdateAll)
	{
		Shared3DList::iterator ite = std::find_if(m_3DObjects.begin(), m_3DObjects.end(),
			boost::BOOST_BIND(&CAirport3DObject::GetGuid, _1) == guid);
		DataType* pData = m_pAirport3D->QueryDataObject<DataType>(guid);

		if (m_3DObjects.end() != ite)
		{
			if (pData)
			{
				// update
				(*ite)->Update3D(pData, nUpdateCode);
			}
			else
			{
				// delete
				(*ite)->Detach();
				m_3DObjects.erase(ite);
			}
		}
		else if (pData)
		{
			// add

			Shared3DPtr p3D = Shared3DPtr(static_cast<ListType*>(this)->CreateNew3D(pData, m_3DObjects.size()));
			p3D->AttachTo(m_p3DScene->GetRoot());
			p3D->Update3D(pData);
			m_3DObjects.push_back(p3D);

		}
		else
		{
			ASSERT(FALSE);
			return false;
		}
		return true;
	}
	Shared3DPtr Find3DObj( const CGuid& guid ) const
	{
		Shared3DList::const_iterator ite = std::find_if(m_3DObjects.begin(), m_3DObjects.end(),
			boost::BOOST_BIND(&CAirport3DObject::GetGuid, _1) == guid);
		if (m_3DObjects.end() != ite)
			return *ite;
		return Shared3DPtr();
	}

private:
	Shared3DList m_3DObjects;
};
