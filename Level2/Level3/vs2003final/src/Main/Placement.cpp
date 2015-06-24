// Placement.cpp: implementation of the CPlacement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "Placement.h"
#include "ShapesManager.h"
#include "Processor2.h"
#include "..\engine\proclist.h"
#include <algorithm>
#include "../Inputs/Pipe.h"
#include "../Inputs/PipeDataSet.h"
#include "../Inputs/AreasPortals.h"
#include "../Inputs/Structure.h"
#include "../Inputs/Structurelist.h"
#include "../Inputs/WallShape.h"
#include "../Inputs/WallShapeList.h"
#include <Inputs/IN_TERM.H>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPlacement::CPlacement() : DataSet( PlacementsFile ,(float)2.4)
{
	m_fileType = PlacementsFile;
}

CPlacement::CPlacement(InputFiles fileType) : DataSet(fileType,(float)2.4)
{
	m_fileType = fileType;
}

CPlacement::~CPlacement()
{
	clear();
}

void CPlacement::clear()
{
	int nCount = m_vUndefined.size();
	for( int i=0; i<nCount; i++ )
		delete m_vUndefined[i];

	m_vUndefined.clear();


	nCount = m_vDefined.size();
	for( i=0; i<nCount; i++ )
		delete m_vDefined[i];

	m_vDefined.clear();

	//clear group list
	for(std::list<CObjectDisplayGroup*>::iterator itr = m_vGroupList.begin(); itr!= m_vGroupList.end(); itr++)
	{
		delete (*itr);
	}
	m_vGroupList.clear();

}

void CPlacement::readObsoleteData( ArctermFile& p_file )
{
	if (p_file.getVersion() < (float)2.3)
	{
		readData2_2(p_file);
	}
	else
	{
		readData2_3(p_file);
	}
}

void CPlacement::readData2_2(ArctermFile& p_file)
{
	p_file.getLine();
	
	//std::vector<Processor*> vExsitProc;
	while (!p_file.isBlank ())
	{
		CProcessor2* pProc2 = new CProcessor2;

		char name[256];
		if( p_file.isBlankField() )
			p_file.skipField( 2 );
		else
		{
			p_file.getShortField (name, 256);
			pProc2->SetShape(SHAPESMANAGER->FindShapeByName(CString(name)));
		}
		// m_vLocation
		Point aPoint;
		p_file.getPoint( aPoint );
		pProc2->SetLocation( ARCVector3(aPoint.getX(), aPoint.getY(),aPoint.getZ()) );

		// m_vScale
		p_file.getPoint( aPoint );
		pProc2->SetScale( ARCVector3(aPoint.getX(), aPoint.getY(), aPoint.getZ())/100.0 );

		// m_dRotation
		double dVal;
		p_file.getFloat( dVal );
		pProc2->SetRotation( dVal );

		int nVal;
		p_file.getInteger( nVal );
		pProc2->SetFloor( nVal );

		// processor
		//assert( m_pInTerm );
		Processor* pProc = NULL;
		if( p_file.isBlankField() )
			p_file.skipField( 1 );
		else
		{
			p_file.getShortField (name, 256);
			if(m_fileType == PlacementsFile)
				pProc = m_pInTerm->GetTerminalProcessorList()->getProcessor( name );
			else if(m_fileType == AirsidePlacementsFile)
				pProc = m_pInTerm->GetAirsideProcessorList()->getProcessor( name );
		}
		pProc2->SetProcessor( pProc );

		// m_dispProperties
		for( int i=0; i<PDP_ARRAY_SIZE; i++ )
		{
			int nVal;
			p_file.getInteger( nVal );
			pProc2->m_dispProperties.bDisplay[i] = (BOOL)nVal;
			long nLong;
			p_file.getInteger( nLong );
			pProc2->m_dispProperties.color[i] = nLong;
		}
		CString strValue(pProc2->GetShape()->Name());
		if( pProc )
		{
			if (pProc2->GetShape())
			{
				pProc2->ShapeName(pProc2->GetShape()->Name());
			}
			m_vDefined.push_back( pProc2 );
		}

		else
		{
			if( name[0] != 0 )
			{
				CString strTemp = _T("");
				strTemp = m_vUndefined.GetUnquieName(strValue);
		
				strTemp.MakeUpper();
				pProc2->ShapeName(strTemp);
				m_vUndefined.push_back(pProc2);
				m_vUndefined.InsertUnDefinePro2(std::make_pair(strTemp,pProc2));
				m_vUndefined.setNode(ALTObjectID(strTemp));
				m_vUndefined.InsertShapeName(strTemp);
			}
		}

		p_file.getLine();
	}
//	m_vDefined.BuildShapeGroupIdxTree();
//	m_vUndefined.BuildShapeGroupIdxTree();
}

void CPlacement::readData2_3(ArctermFile& p_file)
{
	p_file.getLine();

	//std::vector<Processor*> vExsitProc;
	while (!p_file.isBlank ())
	{
		CProcessor2* pProc2 = new CProcessor2;

		char name[256];
		char shapeDefaultName[256];
		if( p_file.isBlankField() )
			p_file.skipField( 2 );
		else
		{
			p_file.getShortField (name, 256);
			strcpy(shapeDefaultName,name);
			pProc2->SetShape(SHAPESMANAGER->FindShapeByName(CString(name)));
		}

		char shapeName[256];
		//if (p_file.isBlankField())
		//{
		//	p_file.skipField(2);
		//}
		//else
		{
			p_file.getShortField(shapeName,256);
		}
		// m_vLocation
		Point aPoint;
		p_file.getPoint( aPoint );
		pProc2->SetLocation( ARCVector3(aPoint.getX(), aPoint.getY(),aPoint.getZ()) );

		// m_vScale
		p_file.getPoint( aPoint );
		pProc2->SetScale( ARCVector3(aPoint.getX(), aPoint.getY(), aPoint.getZ())/100.0 );

		// m_dRotation
		double dVal;
		p_file.getFloat( dVal );
		pProc2->SetRotation( dVal );

		int nVal;
		p_file.getInteger( nVal );
		pProc2->SetFloor( nVal );

		// processor
		//assert( m_pInTerm );
		Processor* pProc = NULL;
		if( p_file.isBlankField() )
			p_file.skipField( 1 );
		else
		{
			p_file.getShortField (name, 256);
			if(m_fileType == PlacementsFile)
				pProc = m_pInTerm->GetTerminalProcessorList()->getProcessor( name );
			else if(m_fileType == AirsidePlacementsFile)
				pProc = m_pInTerm->GetAirsideProcessorList()->getProcessor( name );
		}
		pProc2->SetProcessor( pProc );

		// m_dispProperties
		for( int i=0; i<PDP_ARRAY_SIZE; i++ )
		{
			int nVal;
			p_file.getInteger( nVal );
			pProc2->m_dispProperties.bDisplay[i] = (BOOL)nVal;
			long nLong;
			p_file.getInteger( nLong );
			pProc2->m_dispProperties.color[i] = nLong;
		}

		if( pProc )
		{
			if (pProc2->GetShape())
			{
				pProc2->ShapeName(CString(shapeName));
			}

			m_vDefined.push_back(pProc2);
		}

		else
		{
			if (pProc2->GetShape())
			{
				pProc2->ShapeName(CString(shapeName));
			}

			if( name[0] != 0 )
			{
				m_vUndefined.push_back(pProc2);
				CString strShapeName = shapeName;
				strShapeName.MakeUpper();
				m_vUndefined.InsertUnDefinePro2(std::make_pair(strShapeName,pProc2));
				m_vUndefined.setNode(ALTObjectID(strShapeName));
				m_vUndefined.InsertShapeName(strShapeName);
			}
		}

		p_file.getLine();
	}
}

void CPlacement::readData(ArctermFile& p_file)
{
	p_file.getLine();

	//std::vector<Processor*> vExsitProc;
	while (!p_file.isBlank ())
	{
		CProcessor2* pProc2 = new CProcessor2;

		char name[256];
		char shapeDefaultName[256];
		if( p_file.isBlankField() )
			p_file.skipField( 2 );
		else
		{
			p_file.getShortField (name, 256);
			strcpy(shapeDefaultName,name);
			pProc2->SetShape(SHAPESMANAGER->FindShapeByName(CString(name)));
		}

		char shapeName[256];
		if (p_file.isBlankField())
		{
			p_file.skipField(2);
		}
		else
		{
			p_file.getShortField(shapeName,256);
		}
		// m_vLocation
		Point aPoint;
		p_file.getPoint( aPoint );
		pProc2->SetLocation( ARCVector3(aPoint.getX(), aPoint.getY(),aPoint.getZ()) );

		// m_vScale
		p_file.getPoint( aPoint );
		pProc2->SetScale( ARCVector3(aPoint.getX(), aPoint.getY(), aPoint.getZ())/100.0 );

		// m_dRotation
		double dVal;
		p_file.getFloat( dVal );
		pProc2->SetRotation( dVal );

		int nVal;
		p_file.getInteger( nVal );
		pProc2->SetFloor( nVal );

		// processor
		//assert( m_pInTerm );
		Processor* pProc = NULL;
		if( p_file.isBlankField() )
			p_file.skipField( 1 );
		else
		{
			p_file.getShortField (name, 256);
			if(m_fileType == PlacementsFile)
				pProc = m_pInTerm->GetTerminalProcessorList()->getProcessor( name );
			else if(m_fileType == AirsidePlacementsFile)
				pProc = m_pInTerm->GetAirsideProcessorList()->getProcessor( name );
		}
		pProc2->SetProcessor( pProc );

		// m_dispProperties
		for( int i=0; i<PDP_ARRAY_SIZE; i++ )
		{
			int nVal;
			p_file.getInteger( nVal );
			pProc2->m_dispProperties.bDisplay[i] = (BOOL)nVal;
			long nLong;
			p_file.getInteger( nLong );
			pProc2->m_dispProperties.color[i] = nLong;
		}

		if( pProc )
		{
			if (pProc2->GetShape())
			{
				pProc2->ShapeName(CString(shapeName));
			}
			
			m_vDefined.push_back(pProc2);
		}

		else
		{
			if (pProc2->GetShape())
			{
				pProc2->ShapeName(CString(shapeName));
			}

			if( name[0] != 0 )
			{
				m_vUndefined.push_back(pProc2);
				CString strShapeName = shapeName;
				strShapeName.MakeUpper();
				m_vUndefined.InsertUnDefinePro2(std::make_pair(strShapeName,pProc2));
				m_vUndefined.setNode(ALTObjectID(strShapeName));
				m_vUndefined.InsertShapeName(strShapeName);
			}
		}


		p_file.getLine();
	}
}
void CPlacement::writeData (ArctermFile& p_file) const
{
	int nCount = m_vUndefined.size();
    for( int i = 0; i < nCount; i++ )
    {
		CProcessor2* pProc2 = m_vUndefined[i];
		writeProc( pProc2, p_file );
    }

	nCount = m_vDefined.size();
    for( i = 0; i < nCount; i++ )
    {
		CProcessor2* pProc2 = m_vDefined[i];
		writeProc( pProc2, p_file );
    }
}


void CPlacement::writeProc( CProcessor2* _pProc2, ArctermFile& p_file ) const
{
	assert( _pProc2 );

	// shape
	CShape* pShape = _pProc2->GetShape();
	if(pShape != NULL)
		p_file.writeField( pShape->Name() );
	else
		p_file.writeBlankField();

	if (pShape != NULL)
	{
		if (_pProc2->ShapeName().GetIDString().IsEmpty())
		{
			p_file.writeField("ALL PROCESSORS");
		}
		else
		{
			p_file.writeField(_pProc2->ShapeName().GetIDString());
		}
	}
	else
		p_file.writeBlankField();
	// m_vLocation
	ARCVector3 location;
	location = _pProc2->GetLocation(  );
	Point aPoint1( location[VX], location[VY], location[VZ] );
	p_file.writePoint( aPoint1 );		

	// m_vScale
	ARCVector3 scale;
	_pProc2->GetScale( scale );
	scale = scale*100;
	Point aPoint2( scale[VX], scale[VY], scale[VZ] );
	p_file.writePoint( aPoint2 );		

	// m_dRotation
	double dRotation;
	_pProc2->GetRotation( dRotation );
	p_file.writeFloat( static_cast<float>(dRotation) );

	// m_nFloor
	p_file.writeFloat( static_cast<float>(_pProc2->GetFloor()) );

	// processor
	Processor* pProc = _pProc2->GetProcessor();
	if( pProc == NULL )
		p_file.writeBlankField();
	else
	{
		const ProcessorID* pId = pProc->getID();
		char szName[128];
		pId->printID( szName );
		p_file.writeField( szName );
	}

	// m_dispProperties
	for( int i=0; i<PDP_ARRAY_SIZE; i++ )
	{
		p_file.writeInt( (int)_pProc2->m_dispProperties.bDisplay[i] );
		p_file.writeInt( (long)_pProc2->m_dispProperties.color[i] );
	}
	p_file.writeLine();
}

Processor* globlePTestProc = NULL;
bool IsTheSameProcessor( CProcessor2* _pFirst )
{
	return _pFirst->GetProcessor() == globlePTestProc;
}
void CPlacement::BuildFromProcList(ProcessorList* _pProcList, CProcessor2::CProcDispProperties& _dispProp )
{
	for(int i=0; i<_pProcList->getProcessorCount(); i++) 
	{
		globlePTestProc = _pProcList->getProcessor(i);
		
		int iDefinedSize = m_vDefined.size();
		for( int j=0; j<iDefinedSize; ++j )
		{
			if( globlePTestProc == m_vDefined[j]->GetProcessor() )
			{
				break;
			}
		}

		
		if( j<iDefinedSize-1 )// find at least one
		{
			// remove duplicated proc
			CPROCESSOR2LIST::iterator iterLast = std::remove_if( m_vDefined.begin() + j + 1, m_vDefined.end(),IsTheSameProcessor );
			m_vDefined.erase( iterLast, m_vDefined.end() );
		}
		else if( j==iDefinedSize )// not find
		{
			CProcessor2* pProc2 = new CProcessor2();
			pProc2->SetProcessor(globlePTestProc);
			//pProc2->SetShape( NULL );
			pProc2->SetShape( SHAPESMANAGER->FindShapeByName( "Default" ) );
			pProc2->SetFloor(static_cast<int>(globlePTestProc->getFloor() / SCALE_FACTOR));
			Path* pSLoc = globlePTestProc->serviceLocationPath();
			ASSERT(pSLoc->getCount() > 0);
			Point p = pSLoc->getPoint(0);
			pProc2->SetLocation(ARCVector3(p.getX(), p.getY(),0.0));
			pProc2->SetScale(ARCVector3(1.0,1.0,1.0));
			pProc2->SetRotation(0);
			pProc2->m_dispProperties = _dispProp;
			
			m_vDefined.push_back(pProc2);	
		}
		/* needless to do anything
		if( j== iDefinedSize-1 )
		{
			
		}
		*/
	}

	ASSERT( m_vDefined.size() == _pProcList->getProcessorCount() );
}

void CPlacement::RefreshAssociateProcs(ProcessorList  * _pProcList)
{
	int iSize = m_vDefined.size();
	for(int i=0;i<iSize;i++)
	{
		m_vDefined[i]->SetProcessor( _pProcList->getProcessor(m_vDefined[i]->GetProcessorName()));
	}
}

void CPlacement::ReplaceProcssor( Processor* _pOldProc, Processor* _pNewProc )
{
	int iSize = m_vDefined.size();
	for( int i=0; i<iSize; ++i )
	{
		if( m_vDefined[ i ]->GetProcessor() == _pOldProc )
		{
			m_vDefined[ i ]->SetProcessor( _pNewProc );
		}
	}
}

void CPlacement::RemoveGroup( CObjectDisplayGroup* pObjGroup )
{
	m_vGroupList.remove(pObjGroup);
	delete pObjGroup;
}



CPipeDisplay * CPlacement::GetPipeDislplay( CPipeDataSet* pPipeData, int index )
{
	for(int i=0;i< m_vPipeList.GetCount(); ++i)
	{
		CPipeDisplay * pPipe =  m_vPipeList.GetItem(i);
		if(pPipe->m_nIndex == index && pPipe->m_pPipes == pPipeData){
			return pPipe;
		}
	}
	if(index < pPipeData->GetPipeCount())
	{
		CPipeDisplay * newPipeDisplay =new CPipeDisplay(pPipeData, index);
		m_vPipeList.AddItem(newPipeDisplay);
		return newPipeDisplay;
	}
	return NULL;

}

void CPlacement::DeletePipe( CPipeDisplay* pPipeDisplay )
{
	std::list<CObjectDisplayGroup*>::iterator iter = m_vGroupList.begin();
	for (; iter != m_vGroupList.end(); ++iter)
	{
		(*iter)->RemoveItem(pPipeDisplay);
	}


	int nIndex = pPipeDisplay->m_nIndex;

	m_vPipeList.DelItem(pPipeDisplay);


	int nCount = m_vPipeList.GetCount();
	for (int j =0; j < nCount; ++j )
	{
		if(m_vPipeList.GetItem(j)->m_nIndex > nIndex)
		{
			m_vPipeList.GetItem(j)->m_nIndex -= 1; 
		}
	}

}

CObjectDisplay* CPlacement::GetStructureDisplay( CStructureList* pStructureList,int nIndex )
{
	CObjectDisplay *pRetDisplay = NULL;

	if(pStructureList && static_cast<int>(pStructureList->getStructureNum()) > nIndex&& nIndex>=0)
	{
		CStructure *pStructure = pStructureList->getStructureAt(nIndex);
		pRetDisplay = m_vStructureList.GetItem(pStructure);
		if (pRetDisplay == NULL)
		{
			CObjectDisplay *pObjectDisplay = new CObjectDisplay();

			pObjectDisplay->SetBaseObject(pStructure);

			m_vStructureList.AddItem(pObjectDisplay);

			pRetDisplay = pObjectDisplay;
		}
	}

	return pRetDisplay;
}
void CPlacement::DeleteStructureDisplay( CObjectDisplay *pObjectDisplay )
{
	std::list<CObjectDisplayGroup*>::iterator iter = m_vGroupList.begin();
	for (; iter != m_vGroupList.end(); ++iter)
	{
		(*iter)->RemoveItem(pObjectDisplay);
	}

	m_vStructureList.DelItem(pObjectDisplay);


}


CObjectDisplay* CPlacement::GetWallShapeDisplay( WallShapeList *pWallShapes, int nIndex )
{
	CObjectDisplay *pRetDisplay = NULL;

	if(pWallShapes &&static_cast<int>(pWallShapes->getShapeNum()) > nIndex&& nIndex>=0)
	{
		WallShape *pWallShape = pWallShapes->getShapeAt(nIndex);
		pRetDisplay = m_vWallShapeList.GetItem(pWallShape);
		if (pRetDisplay == NULL)
		{
			CObjectDisplay *pObjectDisplay = new CObjectDisplay();

			pObjectDisplay->SetBaseObject(pWallShape);

			m_vWallShapeList.AddItem(pObjectDisplay);

			pRetDisplay = pObjectDisplay;
		}
	}
	
	return pRetDisplay;
}
void CPlacement::DeleteWallShapeDisplay( CObjectDisplay *pObjectDisplay )
{
	std::list<CObjectDisplayGroup*>::iterator iter = m_vGroupList.begin();
	for (; iter != m_vGroupList.end(); ++iter)
	{
		(*iter)->RemoveItem(pObjectDisplay);
	}

	m_vWallShapeList.DelItem(pObjectDisplay);
}

CObjectDisplay* CPlacement::GetAreaDisplay( CAreas *pAreas, int nIndex )
{
	CObjectDisplay *pRetDisplay = NULL;

	if(pAreas && static_cast<int>(pAreas->m_vAreas.size()) > nIndex&& nIndex>=0)
	{
		CArea *pArea = pAreas->m_vAreas.at(nIndex);
		pRetDisplay = m_vAreaList.GetItem(pArea);
		if (pRetDisplay == NULL)
		{
			CObjectDisplay *pObjectDisplay = new CObjectDisplay();

			pObjectDisplay->SetBaseObject(pArea);

			m_vAreaList.AddItem(pObjectDisplay);

			pRetDisplay = pObjectDisplay;
		}
	}
	return pRetDisplay;
}
void CPlacement::DeleteAreaDisplay( CObjectDisplay *pObjectDisplay )
{
	std::list<CObjectDisplayGroup*>::iterator iter = m_vGroupList.begin();
	for (; iter != m_vGroupList.end(); ++iter)
	{
		(*iter)->RemoveItem(pObjectDisplay);
	}

	m_vAreaList.DelItem(pObjectDisplay);
}

CObjectDisplay* CPlacement::GetPortalDisplay( CPortals *pPortals, int nIndex )
{

	CObjectDisplay *pRetDisplay = NULL;

	if(pPortals && static_cast<int>(pPortals->m_vPortals.size()) > nIndex&& nIndex>=0)
	{
		CPortal *pPortal = pPortals->m_vPortals.at(nIndex);
		pRetDisplay = m_vPortalList.GetItem(pPortal);
		if (pRetDisplay == NULL)
		{
			CObjectDisplay *pObjectDisplay = new CObjectDisplay();

			pObjectDisplay->SetBaseObject(pPortal);

			m_vPortalList.AddItem(pObjectDisplay);

			pRetDisplay = pObjectDisplay;
		}
	}

	return pRetDisplay;

}
void CPlacement::DeletePortalDisplay( CObjectDisplay *pObjectDisplay )
{
	std::list<CObjectDisplayGroup*>::iterator iter = m_vGroupList.begin();
	for (; iter != m_vGroupList.end(); ++iter)
	{
		(*iter)->RemoveItem(pObjectDisplay);
	}

	m_vPortalList.DelItem(pObjectDisplay);
}










