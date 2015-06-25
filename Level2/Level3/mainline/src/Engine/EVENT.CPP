#include "stdafx.h"
#include "event.h"
#include "common\exeption.h"
#include "common\ARCException.h"
#include "common\OnBoardException.h"
#include "common\WinMsg.h"
//#include "Terminal.h"
#include "common\CodeTimeTest.h"
#include "SimulationDiagnoseDelivery.h"
#include "Common/ARCTracker.h"
#include "ARCportEngine.h"
#include "Common/FileOutPut.h"


EventList * far Event::m_pEventList = NULL;

static DWORD dStart100EventTime(0);

Event::~Event()
{
    time = 0l;
}

void Event::addEvent ()
{
    m_pEventList->addItem (this);
#ifdef _DEBUG
//	static int co = 0;
//	co++;
//	FILE *fw = fopen("c:\\debugtime2.txt", "a+");
//	char buf[255];
//	sprintf(buf, "id = %d, this->GetTime() = %s, time == %s \n", co, this->getTime().printTime(), time.printTime() );
//	fprintf(fw, buf);
//	fclose(fw);
//
//	if( this->getTime() < m_pEventList->currentTime )
//	{
//		Sleep(0);
//	}
#endif
}


//It deletes all events from eventList after running simulation engine
void EventList::kill (void)
{
    Event *anEvent;

    while (getCount())
    {
        anEvent = getItem(0);
        removeItem(0);
        if (anEvent->kill())
            delete anEvent;
    }
}


/*****
*
*   Simulation Engine main loop
*
*****/

int echoFlag = 0;
long eventNumber;
long probeID;

//return current event
int Event::getCurEventNum()
{
	return eventNumber;
}

//add by adam 2007-09-26 fix bug arcport2007 NO.31
void EventList::SetSimulationStartTime(ElapsedTime estStartTime)
{
	m_estSimulationStartTime = estStartTime;
}

void EventList::SetSimulationEndTime(ElapsedTime estEndTime)
{
	m_estSimulationEndTime = estEndTime;
}
//End add by adam 2007-09-26 fix bug arcport2007 NO.31
//_pEngine could be NULL, please check the pointer before use that
//if NULL, it maybe used in report
ElapsedTime EventList::start (CARCportEngine *_pEngine, bool* _pBCancel, bool _bShowMsg )
{
	PLACE_METHOD_TRACK_STRING();

	int echo = 1;
    Event *nextEvent;

    currentTime = 0l;

    long displayInterval = 100;
    long maxDisplay = 100000000l;
    long displayType = 10000000l;
    long testIteration = -1;

    long probeIndex = -1;
    probeID = -1;
	eventNumber = 0;
	//CFileOutput debugfile("D:\\Event.log");
	
    try {
        do {

			int prevcount = getCount ();
			if( prevcount == 0 )
				break;
            
			nextEvent = getItem(0);
            removeItem(0);
			int postcount = getCount ();

            // check for empty event list
            if (!nextEvent)
                break;

		
            // check for events scheduled before the current time
//            if (echo && nextEvent->getTime() < currentTime)
 //               cout << "\nTime travel attempt at " << (long)currentTime << '\n';

			//add by adam 2007-09-26 fix bug arcport2007 NO.31
			//if the event time is out of range
			//if ((nextEvent->getTime() > m_estSimulationEndTime))
			//{
			//	continue;
			//}
			//End add by adam 2007-09-26 fix bug arcport2007 NO.31
            currentTime = nextEvent->getTime();

			/*LogEventToFile.Log(currentTime.printTime());
			LogEventToFile.Log(nextEvent->getTypeName());
			LogEventToFile.Log(currentTime.getTicks());*/
			//LogEventToFile.EndLine();
//Debug it is OK by ainvin, test time.
			if( nextEvent->getEventType() == MobileMovement )
			{
				endTime = currentTime;
				if( m_bSetStartTime )
				{					
					startTime = currentTime;
					m_bSetStartTime = false;	
				}
				
			}
			++eventNumber;
          // error checking conditional breakpoint
//            if (++eventNumber == testIteration)
  //              cout << '\r';

            if (probeIndex == eventNumber)
                echoFlag = 1;
			
			if( _bShowMsg )
			{

				// if all conditions are go, write the event
				if( !(eventNumber % displayInterval) || eventNumber > maxDisplay ||
						nextEvent->getEventType() == displayType )
				{
					if(_pEngine)
						_pEngine->SendEventMessage( eventNumber, currentTime, nextEvent->getTypeName() );

				}
			}

	/*		{
				
				CString strLogFile;
				strLogFile.Format(_T("%sEvent.log"), _T("F:\\landsidedebug\\"));

				ofsstream echoFile (strLogFile.GetBuffer(0), stdios::app);
				echoFile	
					<< currentTime.getPrecisely()<<", "
					<<eventNumber<<", "
					<<nextEvent->getEventType() <<", "
					<<nextEvent->getTypeName() <<", "
					<<currentTime.PrintDateTime()
					<<"\r\n";

					echoFile.close();
			}*/
		


			//TRACE("E-----------: %d\r\n",eventNumber);
		 
			//debugfile.Log(eventNumber).Log("{").Log(nextEvent->getTypeName());
            // process the event
            if (nextEvent->process(_pEngine))
                delete nextEvent;

			//debugfile.Log("}").Line();

        } while ( !(*_pBCancel) );    // runs until no events left to process
    }
	catch( ARCSystemError* sys_exception)
	{
		kill();
//		cout << '\r' << eventNumber << '\t' << (long)currentTime;
//		cout << "\n ARCTerm system error.";
		if(_pEngine)
			_pEngine->SendSimFormatMessage( sys_exception->getFormatErrorMsg() );

		delete sys_exception;
		return currentTime;
	}
    catch(ARCSimEngineException *sim_exception)
    {
        kill();
//        cout << '\r' << eventNumber << '\t' << (long)currentTime;
//        cout << "\n ARCTerm runtime error.";
        throw sim_exception;
    }

	//TODO:
	//catch (TerminalException* terminal_exeption)
	//{
	//	;
	//	return currentTime;
	//}
	catch (AirsideException* airside_exeption)
	{
		kill();

		AirsideDiagnose* pDiagnose = (AirsideDiagnose*)airside_exeption->GetDiagnose();
		
		DiagnoseDelivery()->DeliveryAirsideDiagnose(pDiagnose);


		delete airside_exeption;
		return currentTime;
	}
	//TODO:
	//catch (LandsideException* landside_exeption)
	//{
	//	;
	//	return currentTime;
	//}
	//TODO:
	catch (OnBoardSimEngineFatalError* onboard_exeption)
	{
		kill();

		OnBoardDiagnose* pDiagnose = 
			(OnBoardDiagnose*)onboard_exeption->GetDiagnose();
		DiagnoseDelivery()->DeliveryOnBoardDiagnose(pDiagnose);

		delete onboard_exeption;
		return currentTime;
	}

	kill();
//    if (echo)
//        cout << '\r' << eventNumber << '\t' << (long)currentTime;
    return currentTime;
}


// in the sorted list find the first match.
// return -1 for can not found.
int EventList::FindFirstMatch( ElapsedTime _eTime, int _nStart /*= 0*/, int _nEnd /*= -1*/  )
{
	int nEnd = _nEnd;

	if( _nEnd == -1 )
	{
		nEnd = getCount() - 1;
		if( nEnd == -1 )
			return -1;
	}


	if( getItem(_nStart)->getTime() == _eTime )
		return _nStart;

	if( _nStart == nEnd )
		return -1;

	int nMid = _nStart + ( nEnd - _nStart ) / 2;
	int nTestIdx = nMid;
	while( getItem(nTestIdx)->getTime() == _eTime )		// _nStart is the terminater.
		nTestIdx--;

	if( nTestIdx != nMid )
		return nTestIdx+1;

	if( _eTime < getItem(nMid)->getTime() )
		return FindFirstMatch( _eTime, _nStart, nMid );
	else
		return FindFirstMatch( _eTime, nMid+1, nEnd );
}
