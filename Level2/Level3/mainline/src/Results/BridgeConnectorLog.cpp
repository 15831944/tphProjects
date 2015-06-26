#include "StdAfx.h"
#include ".\bridgeconnectorlog.h"


CBridgeConnectorLog::CBridgeConnectorLog( EventLog<BridgeConnectorEventStruct>* _pBridgeEventLog )
: BaseLog<BridgeConnectorDescStruct,BridgeConnectorEventStruct,CBridgeConnectorLogEntry>()
{
	eventLog = _pBridgeEventLog;
}


CBridgeConnectorLog::~CBridgeConnectorLog()
{

}

void CBridgeConnectorLog::echoLogFile( const char *p_filename, const CString& _csProjPath, OutputTerminal* _pOutTerm )
{
	//ofsstream echoFile (p_filename);
	//echoFile << p_filename << '\n';
	//echoFile << "Log Echo File\n";
	//echoFile << echoTitles() << '\n';
	//CElevatorLogEntry logEntry;
	//logEntry.SetOutputTerminal( _pOutTerm );
	//int nCount = getCount();
	//for( int i=0; i<nCount; i++ )
	//{
	//	getItem (logEntry, i);
	//	logEntry.echo (echoFile, _csProjPath);
	//}
	//echoFile.close();	
}

const char * CBridgeConnectorLog::echoTitles( void ) const
{
	return "ID,Start Time,End Time "; 
}