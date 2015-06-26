#include "StdAfx.h"
#include ".\landsideplacement.h"
#include "termplan.h"
#include "Placement.h"
#include "ShapesManager.h"
#include "Processor2.h"
#include "..\engine\proclist.h"
#include "LandProcessor2.h"
CLandsidePlacement::CLandsidePlacement(ProcessorList* proclist, InputFiles fileType):CPlacement(fileType)
{
	ASSERT(proclist);
	m_pProcList = proclist;
}

CLandsidePlacement::~CLandsidePlacement(void)
{
}
void CLandsidePlacement::readData (ArctermFile& p_file)
{
	p_file.getLine();

	//std::vector<Processor*> vExsitProc;
	while (!p_file.isBlank ())
	{
		CProcessor2* pProc2 = new CLandProcessor2();

		char name[256];
		char shapeDefaultName[256];
		if( p_file.isBlankField() )
			p_file.skipField( 2 );
		else
		{
			p_file.getShortField (name, 256);
			strcpy(shapeDefaultName,name);
			pProc2->SetShape(SHAPESMANAGER->GetShapeByName(CString(name)));
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
			pProc = m_pProcList->getProcessor(name);
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
			//	if( std::find( vExsitProc.begin(), vExsitProc.end(), pProc ) != vExsitProc.end() )
			//		throw new FileFormatError( "Placements.txt " );

			//			vExsitProc.push_back( pProc );
			m_vDefined.push_back( pProc2 );
		}

		


		p_file.getLine();
	}
}


void CLandsidePlacement::writeData (ArctermFile& p_file) const
{
	int nCount = m_vDefined.size();
	for( int i = 0; i < nCount; i++ )
	{
		CProcessor2* pProc2 = m_vDefined[i];
		writeProc( pProc2, p_file );
	}	
}

Processor* _globlePTestProc = NULL;
bool IsTheSameLandProcessor( CProcessor2* _pFirst )
{
	return _pFirst->GetProcessor() == _globlePTestProc;
}
void CLandsidePlacement::BuildFromProcList(ProcessorList* _pProcList, CProcessor2::CProcDispProperties& _dispProp)
{
	
	for(int i=0; i<_pProcList->getProcessorCount(); i++) 
	{
		_globlePTestProc = _pProcList->getProcessor(i);

		int iDefinedSize = m_vDefined.size();
		int j=0; 
		for(; j<iDefinedSize; ++j )
		{
			if( _globlePTestProc == m_vDefined[j]->GetProcessor() )
			{
				break;
			}
		}


		if( j<iDefinedSize-1 )// find at least one
		{
			// remove duplicated proc
			CPROCESSOR2LIST::iterator iterLast = std::remove_if( m_vDefined.begin() + j + 1, m_vDefined.end(),IsTheSameLandProcessor );
			m_vDefined.erase( iterLast, m_vDefined.end() );
		}
		else if( j==iDefinedSize )// not find
		{
			CProcessor2* pProc2 = new CLandProcessor2();
			pProc2->SetProcessor(_globlePTestProc);
			//pProc2->SetShape( NULL );
			pProc2->SetShape( SHAPESMANAGER->GetShapeByName( "Default" ) );
			pProc2->SetFloor(static_cast<int>(_globlePTestProc->getFloor() / SCALE_FACTOR));
			Path* pSLoc = _globlePTestProc->serviceLocationPath();
			ASSERT(pSLoc->getCount() > 0);
			Point p = pSLoc->getPoint(0);
			pProc2->SetLocation(ARCVector3(p.getX(), p.getY(),0.0));
			pProc2->SetScale(ARCVector3(1.0,1.0,1.0));
			pProc2->SetRotation(0);
			pProc2->m_dispProperties = _dispProp;

			m_vDefined.push_back(pProc2);	
		}
		
	}

	ASSERT( m_vDefined.size() == _pProcList->getProcessorCount() );
}