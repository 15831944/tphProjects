#include "StdAfx.h"
#include "MobElementsDiagnosis.h"
#include <algorithm>
#include "MobElementBehavior.h"
#include "person.h"
#include "OnBoard/MobileAgent.h"
#include "Inputs/MobileElemConstraint.h"
#include "process.h"
#include "Common/fileman.h"
#include "TerminalMobElementBehavior.h"
#include "PaxOnboardBaseBehavior.h"

/**
	1. set interested mobile elements id: "CTRL + F8".
	2. run simulation.
	3. all interested mobile elements log saved in "Project Path\\Diagnosis\\"
*/

#include "Common/termfile.h"
#include "Common/exeption.h"
class MobElementDiagnosisConfig
{
public:
	void loadConfig(std::string strFileName, std::vector<long>& mobElementIdList)
	{
		try {
			ArctermFile file;
			file.openFile (strFileName.c_str(), READ); 

			file.getLine();
			int nCount = 0;
			file.getInteger(nCount);
			for (int i = 0; i < nCount; i++)
			{
				long nValue = -1;
				file.getInteger( nValue );
				if(nValue < 0 || nValue > INT_MAX)
					continue;
				mobElementIdList.push_back( nValue );
			}

			file.closeIn();
		}
		catch (FileOpenError *exception){
			delete exception;
			return;
		}


	}
};

CMobElementsDiagnosis::CMobElementsDiagnosis()
:mEnableDiagnosis(false)
{
}

/**
	Diagnosis File Path: "Project Path\\Diagnosis\\"
	MobElement Diagnosis config: "MobElementsDiagnosis.cfg"
**/
void CMobElementsDiagnosis::Initialize(const CString& strProjPath)
{
//----------------------------------------------------------------------
// Comment out the section below if want enable mobile elements diagnosis functionality
// in release version.
//#ifndef _DEBUG  
//	return;
//#endif
//
    
	mMobElementList.clear();
	mFilePath = strProjPath;
	mFilePath.append( "\\Diagnosis\\" );

	std::string strConfigFile = mFilePath;
	strConfigFile.append( "MobElementsDiagnosis.cfg" );
	MobElementDiagnosisConfig _config;
	_config.loadConfig( strConfigFile, mMobElementList);

	mEnableDiagnosis = mMobElementList.size() > 0;

	if(mEnableDiagnosis)
		cleanUp();
}

CMobElementsDiagnosis::~CMobElementsDiagnosis(void)
{
	TRACE("Singleton object: MobElementsDiagnosis() deleted!\n");
}

void CMobElementsDiagnosis::cleanUp()
{
	std::string strDelFiles = mFilePath;
	strDelFiles.append("*.log");
	FileManager::DeleteFile( strDelFiles.c_str());
}

extern long eventNumber;
void CMobElementsDiagnosis::Diagnose(Person* pMobElement, const ElapsedTime& p_time)
{
	if(!mEnableDiagnosis)
		return;

	int nMobElemID = pMobElement->getID();

	std::vector<long>::iterator iter = 
		std::find( mMobElementList.begin(), mMobElementList.end(), nMobElemID );
	if( iter == mMobElementList.end())
		return;

	MobElementBehavior::BehaviorType modeType = 
		pMobElement->getCurrentBehavior()->getBehaviorType();

	if( modeType == MobElementBehavior::AirsideBehavior)
		return DiagnoseAirsideMobElement( pMobElement, p_time );

	if( modeType == MobElementBehavior::LandsideBehavior)
		return DiagnoseLandsideMobElement( pMobElement, p_time );

	if( modeType == MobElementBehavior::TerminalBehavior )
		return DiagnoseTerminalMobElement( pMobElement, p_time);


 	if( modeType ==  MobElementBehavior::OnboardBehavior 
 	/*	|| modeType ==  MobElementBehavior::DeplaneBehavior*/ )
 		return DiagnoseOnBoardMobElement( pMobElement, p_time );

	ASSERT(FALSE);
}

//----------------------------------------------------------------------------
// (A) Mobile Element In Airside Mode
void CMobElementsDiagnosis::DiagnoseAirsideMobElement(Person* pMobElement, const ElapsedTime& p_time)
{
	if(pMobElement->getState() >= EVENT_ENUM_MAX ){
		ASSERT(FALSE); return; 
	}

	CString csMobType;
	pMobElement->getType().screenPrint(csMobType,0,256);

	int nMobElemID = pMobElement->getID();
	CString strLogFile;
	strLogFile.Format(_T("%sMobElement.%d.@Airside.log"), mFilePath.c_str(), nMobElemID);

	ofsstream echoFile (strLogFile.GetBuffer(0), stdios::app);
	echoFile	
		<< "type: "		<< csMobType.GetString()
	/*	<< ",x = "		<< pMobElement->getPoint().getX()
		<< ",y = "		<< pMobElement->getPoint().getY()
		<< ",z = "		<< pMobElement->getPoint().getZ()*/
		<< "\nEvent,"	<< eventNumber 
		<< ",Airside Pax:," << pMobElement->getID() 
		//<< ",State:,"	<< STATE_NAMES[pMobElement->getState()]
		<< ','			<< p_time<<"\r\n\r\n";

	echoFile.close();
}

//----------------------------------------------------------------------------
// (L) Mobile Element In Landside Mode
void CMobElementsDiagnosis::DiagnoseLandsideMobElement(Person* pMobElement, const ElapsedTime& p_time)
{


	////int nMobElemID = pMobElement->getID();
	////CString strLogFile;
	////strLogFile.Format(_T("%sMobElement.%d.@Landside.log"), mFilePath.c_str(), nMobElemID);


}

//----------------------------------------------------------------------------
// (T) Mobile Element In Terminal Mode
#include "Common/states.h"
extern char *STATE_NAMES[];

void CMobElementsDiagnosis::DiagnoseTerminalMobElement(Person* pMobElement, const ElapsedTime& p_time)
{
	if(pMobElement->getState() >= EVENT_ENUM_MAX ){
		ASSERT(FALSE); return; 
	}

	TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior*)pMobElement->getBehavior(MobElementBehavior::TerminalBehavior);

	if (spTerminalBehavior == NULL){
		ASSERT(FALSE); return;
	}
	
	if( NULL == spTerminalBehavior->getProcessor() ){
		ASSERT(FALSE); return;
	} 
	
	char csProcID[256];
	spTerminalBehavior->getProcessor()->getID()->printID( csProcID );

	CString csMobType;
	pMobElement->getType().screenPrint(csMobType,0,256);

	int nMobElemID = pMobElement->getID();
	CString strLogFile;
	strLogFile.Format(_T("%sMobElement.%d.@Terminal.log"), mFilePath.c_str(), nMobElemID);

	ofsstream echoFile (strLogFile.GetBuffer(0), stdios::app);
	echoFile	
		<< "type: "		<< csMobType.GetString()
		//<< ",x = "		<< pMobElement->getPoint().getX()
		//<< ",y = "		<< pMobElement->getPoint().getY()
		//<< ",z = "		<< pMobElement->getPoint().getZ()
		<< "\nEvent,"	<< eventNumber 
		<< ",Cur Pax:," << pMobElement->getID() 
		<< ",State:,"	<< STATE_NAMES[pMobElement->getState()]
		<< ",@,"		<< csProcID
		<< ','			<< p_time
		<<','			<<(long)p_time
		<<"\r\n\r\n";
		
	echoFile.close();

}

//----------------------------------------------------------------------------
// (O) Mobile Element In OnBoard Mode
#include "Common/OnBoardStates.h"
extern char *ONBOARD_STATE_NAMES[];

void CMobElementsDiagnosis::DiagnoseOnBoardMobElement(Person* pMobElement, const ElapsedTime& p_time)
{
	//MobileAgent* pMobAgent = (MobileAgent*)pMobElement;

	int nMobElemID = pMobElement->getID();
	CString strLogFile;
	strLogFile.Format(_T("%sMobElement.%d.@OnBoard.log"), mFilePath.c_str(), nMobElemID);

	CString csAgentType ;//= pMobElement->screenMobAgent();
	ofsstream echoFile (strLogFile.GetBuffer(0), stdios::app);
	echoFile
		<< "type: "			<< csAgentType.GetString()
		<< ",x =  "			<< (pMobElement->getOnboardBehavior()->getPoint()).n[VX]
		<< ",y =  "			<< (pMobElement->getOnboardBehavior()->getPoint()).n[VY]
		<< ",z =  "			<< (pMobElement->getOnboardBehavior()->getPoint()).n[VZ]
		<< "\nEvent, "		<< eventNumber 
		<< ",Cur Agent:, "	<< pMobElement->getID() 
		<< ",State:,"		<< STATE_NAMES[pMobElement->getState()]
		<< ','				<< p_time
		<<','				<<(long)p_time	
		<<"\r\n\r\n";

	echoFile.close();

}

bool CMobElementsDiagnosis::isInList( int nPaxID ) const
{
	return std::find( mMobElementList.begin(), mMobElementList.end(), nPaxID )!= mMobElementList.end();
}



