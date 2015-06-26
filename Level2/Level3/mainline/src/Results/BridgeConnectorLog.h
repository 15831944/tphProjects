#pragma once


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "common\term_bin.h"
#include "results\baselog.h"
#include "BridgeConnectorLogEntry.h"


class CBridgeConnectorLog: public BaseLog<BridgeConnectorDescStruct,BridgeConnectorEventStruct,CBridgeConnectorLogEntry>
{
public:


	CBridgeConnectorLog(EventLog<BridgeConnectorEventStruct>* _pBridgeEventLog);
	virtual ~CBridgeConnectorLog();

	virtual void echoLogFile (const char *p_filename, const CString& _csProjPath, OutputTerminal* _pOutTerm);

	virtual const char *echoTitles (void) const;


};
