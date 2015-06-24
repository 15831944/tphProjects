#include "StdAfx.h"
#include "main\placement.h"
#include "main\ShapesManager.h"
#include ".\ConvertACGateToStandHelper.h"

CConvertACGateToStandHelper::CConvertACGateToStandHelper(CString& strPath)
														: m_strPrjPath(strPath)
{
	m_pFakeInputTerminal = new FakeInputTerminal();
}

CConvertACGateToStandHelper::~CConvertACGateToStandHelper(void)
{
	if (m_pFakeInputTerminal)
	{
		delete m_pFakeInputTerminal;
		m_pFakeInputTerminal = NULL;
	}
}

//Convert 
void CConvertACGateToStandHelper::Convert()
{
    
	m_pFakeInputTerminal->loadInputs(m_strPrjPath);
	
	std::vector<GateProcessorEx*> vProc = 
		m_pFakeInputTerminal->GetProcListEx()->GetACGates();
	
	//Convert Processor Type form AC Gates to Stands
	for (std::vector<GateProcessorEx*>::iterator it = vProc.begin();
		 it != vProc.end(); it++)
		 m_pFakeInputTerminal->GetAirsideInput()->GetProcessorList()->addProcessor(ConvertACGateToStand(*it), FALSE);
	
	//Move AC Gates's placement info from Terminal to Airside
	CPlacement * pAirsidePlacement = m_pFakeInputTerminal->GetAirsideInput()->GetPlacementPtr();
 	std::vector<CProcessor2*>& vPlacement = pAirsidePlacement->m_vDefined;
	std::vector<CProcessor2*>& vPlacementNew = m_pFakeInputTerminal->GetPlacement()->GetACGatesPlacement();
	
	//APPEND AC Gates placements
	vPlacement.insert(vPlacement.end(), vPlacementNew.begin(), vPlacementNew.end() );

	WriteFiles();
}

//IN ACGate Processor
//OUT Stand Processor
StandProc* CConvertACGateToStandHelper::ConvertACGateToStand(GateProcessorEx* pACGate)
{
	StandProc * pStandProc = new StandProc();
	ASSERT(pStandProc);
	
	CString sName = pACGate->getID()->GetIDString();
	ProcessorID id;
	id.SetStrDict( m_pFakeInputTerminal->inStrDict);
	id.setID(sName);

	pStandProc->init(id);

	Path* pPathSrv = pACGate->serviceLocationPath();
	Path* pPathInCons = pACGate->inConstraint();
	Path* pPathOutCons = pACGate->outConstraint();

	pStandProc->initServiceLocation(pPathSrv->getCount(), pPathSrv->getPointList());
	pStandProc->initInConstraint(pPathInCons->getCount(), pPathInCons->getPointList());
	pStandProc->initOutConstraint(pPathOutCons->getCount(), pPathOutCons->getPointList());

	return pStandProc;
}
//Save Converted Files
void CConvertACGateToStandHelper::WriteFiles()
{
	m_pFakeInputTerminal->GetProcListEx()->saveDataSet(m_strPrjPath, FALSE);
	m_pFakeInputTerminal->GetAirsideInput()->GetProcessorList()->saveDataSet(m_strPrjPath, FALSE);
	m_pFakeInputTerminal->GetPlacement()->saveDataSet(m_strPrjPath, FALSE);
	m_pFakeInputTerminal->GetAirsideInput()->GetPlacementPtr()->saveDataSet(m_strPrjPath, FALSE);
}

//-------------------------------------------------
// FakeInputTerminal Memeber functions
//
FakeInputTerminal * m_pInTerm = NULL;

FakeInputTerminal::FakeInputTerminal()
{
	m_pInTerm = this;
	m_pProcList = new ProcessorListEx( inStrDict );
	m_pPlacement = new CPlacementEx();
}

FakeInputTerminal::~FakeInputTerminal()
{
	if(m_pProcList)
	{
		delete m_pProcList;
		m_pProcList = NULL;
	}
	if (m_pPlacement)
	{
		delete m_pPlacement;
		m_pPlacement = NULL;
	}
}

//Load Data
void FakeInputTerminal::loadInputs( const CString& _pProjPath )
{
	//Layout
	m_pProcList->SetInputTerminal(this);
	m_pProcList->loadDataSet(_pProjPath);

	m_AirsideInput->GetProcessorList()->SetInputTerminal(this);
	m_AirsideInput->GetProcessorList()->loadDataSet(_pProjPath);

	//Placements
	m_pPlacement->SetInputTerminal(this);
	m_pPlacement->SetACGates(m_pProcList->GetACGates());
	m_pPlacement->loadDataSet(_pProjPath);

	m_AirsideInput->GetPlacementPtr()->SetInputTerminal(this);
	m_AirsideInput->GetPlacementPtr()->loadDataSet(_pProjPath);
}




//---------------------------------------------
//ProcessorListEx MemberFunctions 
//

ProcessorListEx::ProcessorListEx( StringDictionary* _pStrDict ): ProcessorList(_pStrDict)
{

}

ProcessorListEx::ProcessorListEx( StringDictionary* _pStrDict,InputFiles fileType):ProcessorList(_pStrDict, fileType)
{

}

Processor *ProcessorListEx::readOldProcessorVer22(ArctermFile& procFile, bool _bFirst /*=false*/ )
{
	ProcessorEx *proc;
	ProcessorID procID;

	assert( m_pInTerm );
	procID.SetStrDict( m_pInTerm->inStrDict );
	if (!procID.readProcessorID (procFile))
		throw new StringError ("Invalid processor IDs, ProcessorList::getProcessor");

	char string[SMALL];
	procFile.getField (string, SMALL);


	if (!_stricmp (string, "DPND_SOURCE"))
		proc = (ProcessorEx *)new DependentSource;
	else if (!_stricmp (string, "DPND_SINK"))
		proc = (ProcessorEx *)new DependentSink;
	else if (!_stricmp (string, "GATE"))
		proc = (ProcessorEx *)new GateProcessorEx;
	else if (!_stricmp (string, "LINE"))
		proc = (ProcessorEx *)new LineProcessor;
	else if (!_stricmp (string, "BAGGAGE"))
		proc = (ProcessorEx *)new BaggageProcessor;
	else if (!_stricmp (string, "HOLD_AREA"))
		proc = (ProcessorEx *)new HoldingArea;
	else if (!_stricmp (string, "BARRIER"))
		proc = (ProcessorEx *)new Barrier;
	else if (!_stricmp (string, "FLOOR")) 
		proc = (ProcessorEx *)new FloorChangeProcessor;
	else if (!_stricmp(string,"INTEGRATED_STATION"))
		proc =(ProcessorEx *) new IntegratedStation ;
	else if (!_stricmp(string,"MOVING_SIDE_WALK"))
		proc =(ProcessorEx *) new MovingSideWalk();
	else if(!_stricmp(string,"Elevator"))
		proc =(ProcessorEx *)new ElevatorProc;
	else if(!_stricmp(string,"Conveyor"))
		proc =(ProcessorEx *)new Conveyor;
	else if(!_stricmp(string,"Stair"))
		proc =(ProcessorEx *)new Stair;
	else if(!_stricmp(string,"Escalator"))
		proc =(ProcessorEx *)new Escalator;
	else if(!_stricmp(string,"ARP"))
		proc =(ProcessorEx *) new ArpProc();
	else if(!_stricmp(string,"RUNWAY"))
		proc =(ProcessorEx *) new RunwayProc();
	else if(!_stricmp(string,"TAXIWAY"))
		proc =(ProcessorEx *) new TaxiwayProc();
	else if(!_stricmp(string,"STAND"))
		proc =(ProcessorEx *) new StandProc();
	else if(!_stricmp(string,"NODE"))
		proc =(ProcessorEx *) new NodeProc();
	else if(!_stricmp(string,"DEICE_STATION"))
		proc =(ProcessorEx *) new DeiceBayProc();
	else if(!_stricmp(string,"FIX"))
		proc =(ProcessorEx *) new FixProc();
	else if (!_stricmp(string,"CONTOUR"))
		proc =(ProcessorEx *) new ContourProc;
	else if(!_stricmp(string ,"APRON"))
		proc=(ProcessorEx *) new ApronProc;	
	else
		proc =(ProcessorEx *) new Processor;


	proc->init (procID);
	proc->SetTerminal((Terminal *)m_pInTerm); // add by tutu 2002-9-26

	Point aPoint;
	Point inConst[100], outConst[100];
	int svcCount = 0, inCount = 0, queueCount = 0, outCount = 0;

	if (!procFile.getPoint (aPoint))
		throw new StringError ("All processors must have a service location");


	if( _bFirst )
	{
		m_bDoOffset = false;
		static DistanceUnit THRESHOLD_FOR_DOOFFSET = 30000000;
		DistanceUnit x = aPoint.getX();
		DistanceUnit y = aPoint.getY();
		if( x  > THRESHOLD_FOR_DOOFFSET || x < -THRESHOLD_FOR_DOOFFSET
			|| y > THRESHOLD_FOR_DOOFFSET || y < -THRESHOLD_FOR_DOOFFSET )
		{
			m_bDoOffset = true;			
			m_minX = x;
			m_maxX = x;
			m_minY = y;
			m_maxY = y;			
		}
	}

	if( m_bDoOffset )
	{
		DistanceUnit x = aPoint.getX();
		if( x < m_minX )
			m_minX = x;
		else if( x > m_maxX )
			m_maxX = x;

		DistanceUnit y = aPoint.getY();
		if( y < m_minY )
			m_minY = y;
		else if( y > m_maxY )
			m_maxY = y;		
	}

	m_pServicePoints[svcCount++] = aPoint;

	if (procFile.getPoint (aPoint))
		inConst[inCount++] = aPoint;

	if (procFile.getPoint (aPoint))
		m_pQueue[queueCount++] = aPoint;

	if (procFile.getPoint (aPoint))
		outConst[outCount++] = aPoint;

	char fixedQueue = 'N';
	if (queueCount)
		procFile.getChar (fixedQueue);


	proc->readSpecialField( procFile );
	// make sure point limits are not exceeded
	for (int i = 1; procFile.getLine() && !procFile.isNewEntryLine(); i++)
	{
		if (i + 1 >= MAX_POINTS)
		{
			procID.printID (string);
			throw new TwoStringError ("maximum point count exceeded by ", string);
		}

		// skip ID and Processor Type fields
		procFile.skipField (2);

		if (procFile.getPoint (aPoint))
			m_pServicePoints[svcCount++] = aPoint;

		if (procFile.getPoint (aPoint))
			inConst[inCount++] = aPoint;

		if (procFile.getPoint (aPoint))
			m_pQueue[queueCount++] = aPoint;

		if (procFile.getPoint (aPoint))
			outConst[outCount++] = aPoint;
	}


	char c[256];
	proc->getID()->printID( c );

	proc->initInConstraint (inCount, inConst);
	proc->initServiceLocation (svcCount, m_pServicePoints);
	proc->initOutConstraint (outCount, outConst);
	proc->initQueue (fixedQueue == 'Y', queueCount, m_pQueue);

	///proc->readSpecialProcessor(procFile);
	return proc;
}

Processor *ProcessorListEx::readProcessor (ArctermFile& procFile, bool _bFirst /*=false*/ )
{
	ProcessorEx *proc;
	ProcessorID procID;

	assert( m_pInTerm );
	procID.SetStrDict( m_pInTerm->inStrDict );
	if (!procID.readProcessorID (procFile))
		throw new StringError ("Invalid processor IDs, ProcessorList::getProcessor");

	char string[SMALL];
	procFile.getField (string, SMALL);


	if (!_stricmp (string, "DPND_SOURCE"))
		proc =(ProcessorEx *) new DependentSource;
	else if (!_stricmp (string, "DPND_SINK"))
		proc = (ProcessorEx *)new DependentSink;
	else if (!_stricmp (string, "GATE"))
		proc =(ProcessorEx *) new GateProcessorEx;
	else if (!_stricmp (string, "LINE"))
		proc =(ProcessorEx *) new LineProcessor;
	else if (!_stricmp (string, "BAGGAGE"))
		proc =(ProcessorEx *) new BaggageProcessor;
	else if (!_stricmp (string, "HOLD_AREA"))
		proc =(ProcessorEx *) new HoldingArea;
	else if (!_stricmp (string, "BARRIER"))
		proc =(ProcessorEx *) new Barrier;
	else if (!_stricmp (string, "FLOOR")) 
		proc =(ProcessorEx *) new FloorChangeProcessor;
	else if (!_stricmp(string,"INTEGRATED_STATION"))
		proc =(ProcessorEx *) new IntegratedStation ;
	else if (!_stricmp(string,"MOVING_SIDE_WALK"))
		proc =(ProcessorEx *) new MovingSideWalk();
	else if(!_stricmp(string,"Elevator"))
		proc =(ProcessorEx *)new ElevatorProc;
	else if(!_stricmp(string,"Conveyor"))
		proc =(ProcessorEx *)new Conveyor;
	else if(!_stricmp(string,"Stair"))
		proc =(ProcessorEx *)new Stair;
	else if(!_stricmp(string,"Escalator"))
		proc =(ProcessorEx *)new Escalator;
	else if(!_stricmp(string,"ARP"))
		proc = (ProcessorEx *)new ArpProc();
	else if(!_stricmp(string,"RUNWAY"))
		proc =(ProcessorEx *) new RunwayProc();
	else if(!_stricmp(string,"TAXIWAY"))
		proc =(ProcessorEx *) new TaxiwayProc();
	else if(!_stricmp(string,"STAND"))
		proc = (ProcessorEx *)new StandProc();
	else if(!_stricmp(string,"NODE"))
		proc =(ProcessorEx *) new NodeProc();
	else if(!_stricmp(string,"DEICE_STATION"))
		proc =(ProcessorEx *) new DeiceBayProc();
	else if(!_stricmp(string,"FIX"))
		proc =(ProcessorEx *) new FixProc();
	else if (!_stricmp(string,"CONTOUR"))
		proc =(ProcessorEx *) new ContourProc;
	else if(!_stricmp(string ,"APRON"))
		proc=(ProcessorEx *) new ApronProc;	
	else
		proc =(ProcessorEx *) new Processor;


	proc->init (procID);
	proc->SetTerminal((Terminal *)m_pInTerm); // add by tutu 2002-9-26
	//add by hans 2005-12-30 add backup flag
	int nBackup;
	procFile.getInteger(nBackup);
	if (nBackup > 0)
	{
		proc->SetBackup(true);
	}
	else
	{
		proc->SetBackup(false);
	}

	Point aPoint;
	Point inConst[100], outConst[100];
	int svcCount = 0, inCount = 0, queueCount = 0, outCount = 0;

	if (!procFile.getPoint (aPoint))
		throw new StringError ("All processors must have a service location");


	if( _bFirst )
	{
		m_bDoOffset = false;
		static DistanceUnit THRESHOLD_FOR_DOOFFSET = 30000000;
		DistanceUnit x = aPoint.getX();
		DistanceUnit y = aPoint.getY();
		if( x  > THRESHOLD_FOR_DOOFFSET || x < -THRESHOLD_FOR_DOOFFSET
			|| y > THRESHOLD_FOR_DOOFFSET || y < -THRESHOLD_FOR_DOOFFSET )
		{
			m_bDoOffset = true;			
			m_minX = x;
			m_maxX = x;
			m_minY = y;
			m_maxY = y;			
		}
	}

	if( m_bDoOffset )
	{
		DistanceUnit x = aPoint.getX();
		if( x < m_minX )
			m_minX = x;
		else if( x > m_maxX )
			m_maxX = x;

		DistanceUnit y = aPoint.getY();
		if( y < m_minY )
			m_minY = y;
		else if( y > m_maxY )
			m_maxY = y;		
	}

	m_pServicePoints[svcCount++] = aPoint;

	if (procFile.getPoint (aPoint))
		inConst[inCount++] = aPoint;

	if (procFile.getPoint (aPoint))
		m_pQueue[queueCount++] = aPoint;

	if (procFile.getPoint (aPoint))
		outConst[outCount++] = aPoint;

	char fixedQueue = 'N';
	if (queueCount)
		procFile.getChar (fixedQueue);


	proc->readSpecialField( procFile );
	// make sure point limits are not exceeded
	for (int i = 1; procFile.getLine() && !procFile.isNewEntryLine(); i++)
	{
		if (i + 1 >= MAX_POINTS)
		{
			procID.printID (string);
			throw new TwoStringError ("maximum point count exceeded by ", string);
		}

		// skip ID and Processor Type fields
		procFile.skipField (2);

		if (procFile.getPoint (aPoint))
			m_pServicePoints[svcCount++] = aPoint;

		if (procFile.getPoint (aPoint))
			inConst[inCount++] = aPoint;

		if (procFile.getPoint (aPoint))
			m_pQueue[queueCount++] = aPoint;

		if (procFile.getPoint (aPoint))
			outConst[outCount++] = aPoint;
	}


	char c[256];
	proc->getID()->printID( c );

	proc->initInConstraint (inCount, inConst);
	proc->initServiceLocation (svcCount, m_pServicePoints);
	proc->initOutConstraint (outCount, outConst);
	proc->initQueue (fixedQueue == 'Y', queueCount, m_pQueue);

	///proc->readSpecialProcessor(procFile);
	return proc;
}

void ProcessorListEx::readObsoleteData ( ArctermFile& p_file )
{
	if (p_file.getVersion() <= 2.21)
	{
		p_file.getLine();
		Processor *aProc=NULL;
		m_pServicePoints = new Point[MAX_POINTS];
		m_pQueue = new Point[MAX_POINTS];

		bool bFirst = true;
		int co = 0;
		while (!p_file.isBlank ())
		{
			co ++;
			try
			{
				aProc = readOldProcessorVer22(p_file, bFirst);
			}catch (...) 
			{
				Sleep(0);
			}
			if(aProc==NULL)return;
			bFirst = false;
			if( aProc->getProcessorType() == Elevator )
			{
				((ElevatorProc*)aProc)->InitLayout();
			}
			else if( aProc->getProcessorType() == ConveyorProc )
			{
				((Conveyor*)aProc)->CalculateTheBisectLine();
			}
			else if( aProc->getProcessorType() == StairProc )
			{
				((Stair*)aProc)->CalculateTheBisectLine();
			}
			else if( aProc->getProcessorType() == EscalatorProc )
			{
				((Escalator*)aProc)->CalculateTheBisectLine();
			}

			if( aProc->getProcessorType() == GateProc &&
				((GateProcessorEx*)aProc)->getACStandGateFlag())
				m_vACGate.push_back((GateProcessorEx*)aProc);
			else
				addProcessor (aProc,false);
		}

		BuildProcGroupIdxTree();
		if( m_bDoOffset )
			DoOffset();

		delete m_pServicePoints;
		delete m_pQueue;
		setIndexes();

	}
	else
	if (p_file.getVersion()>2.21 && p_file.getVersion()<2.4)
		{
			p_file.getLine();
			Processor *aProc=NULL;
			m_pServicePoints = new Point[MAX_POINTS];
			m_pQueue = new Point[MAX_POINTS];

			bool bFirst = true;
			int co = 0;
			while (!p_file.isBlank ())
			{
				co ++;
				try
				{
					aProc = readProcessor (p_file, bFirst);
				}catch (...) 
				{
					Sleep(0);
				}
				if(aProc==NULL)return;
				bFirst = false;
				if( aProc->getProcessorType() == Elevator )
				{
					((ElevatorProc*)aProc)->InitLayout();
				}
				else if( aProc->getProcessorType() == ConveyorProc )
				{
					((Conveyor*)aProc)->CalculateTheBisectLine();
				}
				else if( aProc->getProcessorType() == StairProc )
				{
					((Stair*)aProc)->CalculateTheBisectLine();
				}
				else if( aProc->getProcessorType() == EscalatorProc )
				{
					((Escalator*)aProc)->CalculateTheBisectLine();
				}

				addProcessor (aProc,false);
			}

			BuildProcGroupIdxTree();
			if( m_bDoOffset )
				DoOffset();

			delete m_pServicePoints;
			delete m_pQueue;
			setIndexes();
		}
}


void ProcessorListEx::readData(ArctermFile& p_file)
{
	p_file.getLine();
	Processor *aProc=NULL;
	m_pServicePoints = new Point[MAX_POINTS];
	m_pQueue = new Point[MAX_POINTS];

	bool bFirst = true;
	int co = 0;
	while (!p_file.isBlank ())
	{
		co ++;
		try
		{
			aProc = readProcessor (p_file, bFirst);
		}catch (...) 
		{
			Sleep(0);
		}
		if(aProc==NULL)return;
		bFirst = false;
		if( aProc->getProcessorType() == Elevator )
		{
			((ElevatorProc*)aProc)->InitLayout();
		}
		else if( aProc->getProcessorType() == ConveyorProc )
		{
			((Conveyor*)aProc)->CalculateTheBisectLine();
		}
		else if( aProc->getProcessorType() == StairProc )
		{
			((Stair*)aProc)->CalculateTheBisectLine();
		}
		else if( aProc->getProcessorType() == EscalatorProc )
		{
			((Escalator*)aProc)->CalculateTheBisectLine();
		}

		if( aProc->getProcessorType() == GateProc &&
			((GateProcessorEx*)aProc)->getACStandGateFlag())
			m_vACGate.push_back((GateProcessorEx*)aProc);
		else
			addProcessor (aProc,false);
	}

	BuildProcGroupIdxTree();
	if( m_bDoOffset )
		DoOffset();

	delete m_pServicePoints;
	delete m_pQueue;
	setIndexes();
	//InitOldBaggageProcData();
}


//---------------------------------------------
//GateProcessorEx Member functions IMPL
//

GateProcessorEx::GateProcessorEx()
{
	m_bEmergentFlag = FALSE;
	fCanBeACStandGate = FALSE;
}

GateProcessorEx::~GateProcessorEx()
{

}

int GateProcessorEx::readSpecialField(ArctermFile& procFile)
{
	// for back compare if read field failed, set fCanBeACStandGate = TRUE
	char chFlag;
	procFile.getChar( chFlag );
	fCanBeACStandGate = ( chFlag == 'N' ? FALSE : TRUE );

	procFile.getChar( chFlag );
	m_bEmergentFlag = ( chFlag == 'Y' ? TRUE : FALSE );

	return TRUE;
}


//----------------------------------------------
// CPlacementEx Member functions
//
void CPlacementEx::readData (ArctermFile& p_file)
{
	p_file.getLine();

	//std::vector<Processor*> vExsitProc;
	while (!p_file.isBlank ())
	{
		BOOL bACGate = FALSE;
		Processor *pACGateProc = NULL;

		CProcessor2* pProc2 = new CProcessor2;

		char name[256];
		if( p_file.isBlankField() )
			p_file.skipField( 2 );
		else
		{
			p_file.getShortField (name, 256);
			pProc2->SetShape( SHAPESMANAGER->FindShapeByName( CString(name) ) );
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
		assert( m_pInTerm );
		Processor* pProc = NULL;
		if( p_file.isBlankField() )
			p_file.skipField( 1 );
		else
		{
			p_file.getShortField (name, 256);
			if(m_fileType == PlacementsFile)
			{
				pProc = ((FakeInputTerminal*)m_pInTerm)->GetProcListEx()->getProcessor( name );
						
				//--------------------------------------------------------------------------
				//Find ACGate
				for(std::vector<GateProcessorEx*>::iterator it = m_vACGate.begin();
					it != m_vACGate.end(); it++)
				{ 
					if ( !strcmp( (*it)->getID()->GetIDString(), name) )
					{
						bACGate = TRUE;
						pACGateProc = (*it);
						break;
					}
				}
			}
			else if(m_fileType == AirsidePlacementsFile)
				pProc = ((FakeInputTerminal*)m_pInTerm)->GetAirsideInput()->GetProcessorList()->getProcessor( name );
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

		if(bACGate)
		{
			pProc2->SetProcessor(pACGateProc);
			m_vACGateProc2.push_back(pProc2);
		}
		else
		{
		
			if( pProc )
			{
				//	if( std::find( vExsitProc.begin(), vExsitProc.end(), pProc ) != vExsitProc.end() )
				//		throw new FileFormatError( "Placements.txt " );

				//			vExsitProc.push_back( pProc );
					m_vDefined.push_back( pProc2 );		
			}

			else
			{
				if( name[0] != 0 )
				// TRACE( "%s\n", name );
				m_vUndefined.push_back( pProc2 );
			}

		}
		p_file.getLine();
	}
}


void CPlacementEx::AppendACGatesPlacement(std::vector<CProcessor2*>& vACGateProc2)
{
	m_vDefined.insert(m_vDefined.end(), vACGateProc2.begin(), vACGateProc2.end());
}



