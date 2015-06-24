// IntegratedStation.cpp: implementation of the IntegratedStation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\Inputs\SingleCarSchedule.h"
#include ".\Results\TrainLogEntry.h"
#include ".\Inputs\AllCarSchedule.h"
#include "MovingTrain.h"
#include "IntegratedStation.h"
#include "..\common\termfile.h"
#include "inputs\miscproc.h"
#include "hold.h"
#include "engine\terminal.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IntegratedStation::IntegratedStation()
:m_pTrain(NULL)
{
	m_pPreBoardingArea=NULL;

	m_nOrientation = 0;
} 

IntegratedStation::IntegratedStation( const IntegratedStation& _anotherStation):Processor( _anotherStation )
{
	//Processor::Processor( _anotherStation );

	m_pTrain = _anotherStation.m_pTrain;
	m_port1Path.init( _anotherStation.m_port1Path );
	m_port2Path.init( _anotherStation.m_port2Path );
	m_pPreBoardingArea = new HoldingArea( *( _anotherStation.m_pPreBoardingArea) );
	
	m_nOrientation = _anotherStation.m_nOrientation;
	CAR::const_iterator iter = _anotherStation.m_cars.begin();
	CAR::const_iterator iterEnd = _anotherStation.m_cars.end();
	
	for( ; iter!=iterEnd ; ++iter )
	{
		CCar* pCar= new CCar ( **iter );
		m_cars.push_back( pCar );
	}
	
	m_layout = _anotherStation.m_layout;
	m_layout.ComputeLayoutInfo();
}


IntegratedStation& IntegratedStation::operator =  ( const IntegratedStation& _anotherStation)
{
	if( this!= &_anotherStation )
	{
		m_port1Path.init( _anotherStation.m_port1Path );
		m_port2Path.init( _anotherStation.m_port2Path );
		m_nOrientation = _anotherStation.m_nOrientation;
		if( m_pPreBoardingArea )
			delete m_pPreBoardingArea;
		m_pPreBoardingArea = new HoldingArea ( *( _anotherStation.m_pPreBoardingArea ) );

		CAR::const_iterator iter = m_cars.begin();
		CAR::const_iterator iterEnd = m_cars.end();	
		for( ; iter!=iterEnd ; ++iter )
			delete (*iter);
		m_cars.clear();

		iter = _anotherStation.m_cars.begin();
		iterEnd = _anotherStation.m_cars.end();
		for (; iter!=iterEnd; ++iter )
		{
			CCar* pCar= new CCar( **iter );
			m_cars.push_back( pCar );
			
			
		}
		//Processor::operator = (_anotherStation);
		m_pTerm = _anotherStation.m_pTerm;
		m_location.init( _anotherStation.m_location );
		m_layout = _anotherStation.m_layout;
		m_layout.ComputeLayoutInfo();
	}
	return *this;
}

IntegratedStation::~IntegratedStation()
{
	
	for(CAR::iterator iter=m_cars.begin();iter!=m_cars.end();++iter)
		delete (*iter);
	m_cars.clear();

	if( m_pPreBoardingArea )
		delete m_pPreBoardingArea;
	
}

//write IntegratedStation's special info
int IntegratedStation::writeSpecialProcessor(ArctermFile& procFile) const
{
	procFile.writeInt( m_nOrientation );
	procFile.writeLine();
	return TRUE;
}

//read special info from files
int IntegratedStation::readSpecialProcessor(ArctermFile& procFile)
{
	//procFile.getLine();
	procFile.getInteger( m_nOrientation );
	procFile.getLine();

	InitLocalData();
	return TRUE;
}

//check if the station definition is valid
BOOL IntegratedStation::IsAValidStation()
{
	if( inConstraintLength() <= 0 )
		return false;
	if( outConstraintLength() <=0 )
		return false;

	if ( m_pPreBoardingArea->serviceLocationLength() <=0)
		return false;
	
	for (CAR ::const_iterator iter=m_cars.begin();iter!=m_cars.end();++iter)
	{
		if(!(*iter)->IsAValidCar())
			return false;
	}

	return true;

}
void IntegratedStation::InitLocalData()
{
	DistanceUnit dSeviceZ = getServicePoint(0).getZ();
	//DistanceUnit _x = serviceLocationPath()->getPoint(0).getX() - m_pTerm->pStationLayout->GetAdjustPoint().getX();
	DistanceUnit _x = serviceLocationPath()->getPoint( 0 ).getX();
	_x = _x - GetStationLayout()->GetAdjustPoint().getX();
	DistanceUnit _y = serviceLocationPath()->getPoint(0).getY() - GetStationLayout()->GetAdjustPoint().getY();
	
	m_port1Path.init( GetStationLayout()->GetPort1Path() );
	Point* pTemp =m_port1Path.getPointList();
	if( pTemp )
	 //pTemp->setZ( getServicePoint(0).getZ()*100 );
	 pTemp->setZ( dSeviceZ );
	m_port1Path.DoOffset( _x, _y, 0 );

	m_port2Path.init( GetStationLayout()->GetPort2Path() );
	pTemp =m_port2Path.getPointList();
	if( pTemp )
		pTemp->setZ( dSeviceZ);
	m_port2Path.DoOffset( _x, _y ,0 );
	
	CAR::iterator iter = m_cars.begin();
	CAR::iterator iterEnd = m_cars.end();
	
	for( ; iter!=iterEnd ; ++iter )
		delete (*iter);
	m_cars.clear();

	iter = GetStationLayout()->GetAllCars().begin();
	iterEnd = GetStationLayout()->GetAllCars().end();

	for(; iter!=iterEnd; ++iter)
	{
		CCar* pCar= new CCar ( **iter );
		pCar->CorrectZ( dSeviceZ );
		pCar->DoOffest( _x, _y );
		m_cars.push_back( pCar );
	}

	if( m_pPreBoardingArea )
		delete m_pPreBoardingArea;

	m_pPreBoardingArea = new HoldingArea ( *(GetStationLayout()->GetPreBoardingArea()) );
	m_pPreBoardingArea->DoOffset( _x , _y );
	int iServiceCount = m_pPreBoardingArea->serviceLocationLength();
	Point* pService  = m_pPreBoardingArea->serviceLocationPath()->getPointList();
	for( int i=0; i<iServiceCount; ++i )
	{
		pService[i].setZ( dSeviceZ );
	}

	
	Path* pInconstraint = m_pPreBoardingArea->inConstraint();
	if( pInconstraint )
	{
		Point* pPlist = pInconstraint->getPointList();
		int iCount = pInconstraint->getCount();		
		for( int i=0; i<iCount; ++i )
		{
			pPlist[ i ].setZ( dSeviceZ );
		}
	}

	
	Rotate( m_nOrientation );

	UpdateMinMax();
}

void IntegratedStation::UpdateTempInfo( CStationLayout* _pTempLayout)
{
	DistanceUnit dSeviceZ = getServicePoint(0).getZ();
	DistanceUnit _x = serviceLocationPath()->getPoint( 0 ).getX();
	_x = _x - _pTempLayout->GetAdjustPoint().getX();
	DistanceUnit _y = serviceLocationPath()->getPoint(0).getY() - _pTempLayout->GetAdjustPoint().getY();
	
	m_port1Path.init( _pTempLayout->GetPort1Path() );
	Point* pTemp =m_port1Path.getPointList();
	if( pTemp )
	 pTemp->setZ( dSeviceZ);
	m_port1Path.DoOffset( _x, _y, 0 );

	m_port2Path.init( (int)dSeviceZ );
	pTemp =m_port2Path.getPointList();
	if( pTemp )
	 pTemp->setZ( dSeviceZ );
	m_port2Path.DoOffset( _x, _y ,0 );
	
	CAR::iterator iter = m_cars.begin();
	CAR::iterator iterEnd = m_cars.end();
	
	for( ; iter!=iterEnd ; ++iter )
		delete (*iter);
	m_cars.clear();

	iter = _pTempLayout->GetAllCars().begin();
	iterEnd = _pTempLayout->GetAllCars().end();

	for(; iter!=iterEnd; ++iter)
	{
		CCar* pCar= new CCar ( **iter );
		pCar->CorrectZ( dSeviceZ );
		pCar->DoOffest( _x, _y );
		m_cars.push_back( pCar );
	}

	if( m_pPreBoardingArea )
		delete m_pPreBoardingArea;

	m_pPreBoardingArea = new HoldingArea ( *(_pTempLayout->GetPreBoardingArea()) );
	m_pPreBoardingArea->DoOffset( _x , _y );
	int iServiceCount = m_pPreBoardingArea->serviceLocationLength();
	Point* pService  = m_pPreBoardingArea->serviceLocationPath()->getPointList();
	for( int i=0; i<iServiceCount; ++i )
	{
		pService[i].setZ( dSeviceZ );
	}

	Path* pInconstraint = m_pPreBoardingArea->inConstraint();
	if( pInconstraint )
	{
		Point* pPlist = pInconstraint->getPointList();
		int iCount = pInconstraint->getCount();		
		for( int i=0; i<iCount; ++i )
		{
			pPlist[ i ].setZ( dSeviceZ );
		}
	}
	//Rotate(m_nOrientation );

	UpdateMinMax();
}

void IntegratedStation::DeleteCar( int _nIdx )
{
	ASSERT( _nIdx>=0 && (UINT)_nIdx<m_cars.size() );
	CAR::iterator iter = m_cars.begin() + _nIdx;
	delete *iter;
	m_cars.erase( iter );
}
void IntegratedStation::DoOffset( DistanceUnit _x,DistanceUnit _y)
{
		
	m_port1Path.DoOffset( _x, _y, 0 );	
	m_port2Path.DoOffset( _x, _y ,0 );
	
	CAR::iterator iter = m_cars.begin();
	CAR::iterator iterEnd = m_cars.end();
	for(; iter!=iterEnd; ++iter)
	{	
		(*iter)->DoOffest( _x, _y );
		
	}

	m_pPreBoardingArea->DoOffset( _x , _y );
	Processor::DoOffset( _x,_y );

}
void IntegratedStation::Rotate (DistanceUnit degrees)
{
	DistanceUnit x = serviceLocationPath()->getPoint( 0 ).getX();
	DistanceUnit y = serviceLocationPath()->getPoint(0).getY();
	
	DoOffset( -x, -y );
	//degrees = degrees* 180.0 /M_PI;
	m_port1Path.Rotate( degrees );
	m_port2Path.Rotate( degrees );
	
	if( m_pPreBoardingArea )
		m_pPreBoardingArea->Rotate( degrees );

	CAR::iterator iter = m_cars.begin();
	CAR::iterator iterEnd = m_cars.end();	
	for( ; iter!=iterEnd ; ++iter )
		(*iter)->Rotate( degrees );

	Processor::Rotate( degrees );

	DoOffset( x,y );	
}

void IntegratedStation::Mirror( const Path* _p )
{
	m_port1Path.Mirror(_p);
	m_port2Path.Mirror(_p);
	if(m_pPreBoardingArea)
		m_pPreBoardingArea->Mirror(_p);

	for(CAR::iterator iter = m_cars.begin(); iter!=m_cars.end(); ++iter )
		(*iter)->Mirror(_p);

	Processor::Mirror(_p);
}

int IntegratedStation::readSpecialField(ArctermFile& procFile)
{

    procFile.getInteger(m_nOrientation);
	if(procFile.getVersion() < 2.51)
	{
		//old structure of integrate station
		//need to read layout information from file
		//GetTerminal()->pStationLayout
		m_layout.SetInputTerminal(GetTerminal());
		m_layout.setProjectPath(GetTerminal()->pStationLayoutSample->getProjectPath());
		m_layout.loadDataSet(GetTerminal()->pStationLayoutSample->getProjectPath());
	}
	else
	{
		m_layout.SetInputTerminal(GetTerminal());
		m_layout.setProjectPath(GetTerminal()->pStationLayoutSample->getProjectPath());
		//the layout information are located in the same file
		procFile.getLine();//1
		//procFile.getLine();//2 is inside of readData
		m_layout.readData(procFile);
		procFile.getLine();//4

	}

	return TRUE;
}
int IntegratedStation::writeSpecialField(ArctermFile& procFile) const
{
	procFile.writeInt( m_nOrientation );

	//write the file in the same processor file
	procFile.writeLine();//1
	procFile.writeField(_T("***************************Station Layout Start**********************"));
	procFile.writeLine();//2
	m_layout.writeData(procFile);
	procFile.writeField(_T("***************************Station Layout End************************"));



	return TRUE;
}


// Clean all sim engine related data, for the new sim process
void IntegratedStation::CleanData()
{
	Processor::CleanData();

	int nCarCount = GetCarCount();
	for( int i=0; i<nCarCount; i++ )
	{
		CCar* pCar = GetCar( i );
		pCar->CleanData();
	}

	m_pPreBoardingArea->CleanData();	
}


int IntegratedStation::clearQueue (ElapsedTime _curTime)
{
	int nResCount = Processor::clearQueue( _curTime );

	int nCarCount = GetCarCount();
	for( int i=0; i<nCarCount; i++ )
	{
		CCar* pCar = GetCar( i );
		nResCount += pCar->clearQueue( _curTime );
	}

	nResCount += m_pPreBoardingArea->clearQueue( _curTime );
	return nResCount;
}


// Removes Person from the processor's occupants list.
void IntegratedStation::removePerson( const Person* _pPerson )
{
	Processor::removePerson( _pPerson );

	int nCarCount = GetCarCount();
	for( int i=0; i<nCarCount; i++ )
	{
		CCar* pCar = GetCar( i );
		pCar->removePerson( _pPerson );
	}
	m_pPreBoardingArea->removePerson( _pPerson );
}
void IntegratedStation::initSpecificMisc (const MiscData *miscData) 
{
	if( miscData )
	{				
		MiscIntegratedStation* pData = (MiscIntegratedStation *)miscData;
		ProcessorIDList* pIDList = pData->GetStationList();
		for( int i=0; i<pIDList->getCount(); ++i )
		{
			GroupIndex gIndex = m_pTerm->procList->getGroupIndex( *pIDList->getID( i ) );
			ASSERT( gIndex.start >= 0 );
			for( int j=gIndex.start; j<= gIndex.end; ++j )
			{
				IntegratedStation* pStation = (IntegratedStation*)m_pTerm->procList->getProcessor( j );
				m_vPreBoardingShareWithStation.push_back( pStation );
				pStation->GetSharePreboardingAreaStation().push_back( this );
			}
		}
	}	
	else
	{
		m_vPreBoardingShareWithStation.clear();
	}
}

void IntegratedStation::UpdateMinMax()
{
	Processor::UpdateMinMax();
	GetMinMax(m_min,m_max,m_port1Path);
	GetMinMax(m_min,m_max,m_port2Path);
}

void IntegratedStation::SetTrain(MovingTrain* pTrain)
{
	m_pTrain = pTrain;
}

MovingTrain* IntegratedStation::GetTrain() const
{
	return m_pTrain;
}

void IntegratedStation::ClearStationTrain(MovingTrain* pTrain)
{
	if (pTrain == NULL)
	{
		m_pTrain = NULL;
	}
	else if (m_pTrain == pTrain)
	{
		m_pTrain = NULL;
	}

}

CStationLayout* IntegratedStation::GetStationLayout()
{
	return &m_layout;
}

int IntegratedStation::GetCarCount() const
{
	return (int)m_cars.size();
}

Path* IntegratedStation::GetPreBoardAreaInConstrain()
{
	return GetPreBoardingArea()->inConstraint();
}

Path* IntegratedStation::GetPreBoardAreaServPath()
{
	return GetPreBoardingArea()->serviceLocationPath();
}

Path* IntegratedStation::GetCarAreaServPath( int icar )
{
	return GetCar(icar)->GetCarArea()->serviceLocationPath();
}

int IntegratedStation::GetCarEntryDoorCount( int icar ) 
{
	return GetCar(icar)->GetEntryDoorCount();
}

int IntegratedStation::GetCarExitDoorCount( int icar ) 
{
	return GetCar(icar)->GetExitDoorCount();
}

Path* IntegratedStation::GetCarEntryDoorServPath( int icar,int idoor )
{
	return GetCar(icar)->GetEntryDoor(idoor)->serviceLocationPath();
}

Path* IntegratedStation::GetCarExitDoorServPath( int icar,int idoor )
{
	return GetCar(icar)->GetExitDoor(idoor)->serviceLocationPath();
}

CStationLayout * IntegratedStation::GetLayout()
{
	return &m_layout;
}

CCar* IntegratedStation::GetCar( int _nIdx )
{
	if(_nIdx >=0 && _nIdx < static_cast<int>(m_cars.size()))
		return m_cars[ _nIdx ];
	return NULL;
}
