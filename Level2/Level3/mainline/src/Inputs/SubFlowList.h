// SubFlowList.h: interface for the CSubFlowList class.
#pragma once
#include "..\common\dataset.h"

class CSubFlow;
class InputTerminal;
typedef std::vector<CSubFlow*> PROCESS_INTERNAL_FLOW;

class CSubFlowList  :public DataSet  
{
private:
	PROCESS_INTERNAL_FLOW m_vAllProcessUnit;
public:
	CSubFlowList( );
	virtual ~CSubFlowList();
public:

	virtual void readData (ArctermFile& p_file);
	virtual void readObsoleteData ( ArctermFile& p_file );
	virtual void writeData (ArctermFile& p_file) const;
	virtual const char *getTitle (void) const { return "Process Unit Internal Flow File" ;};
	virtual const char *getHeaders (void) const {return "Process Name,  " ;};
	virtual void clear (void);
public:
	bool IfProcessUnitExist( const CString _sProcessUnitName ) const;
	void AddProcessUnit( const CSubFlow& _processUnit );
	int GetProcessUnitCount()const { return m_vAllProcessUnit.size();	}
	CSubFlow* GetProcessUnitAt( int _iIdx );
	void DeleteProcessUnitAt( int _iIdx );

	CSubFlow* GetProcessUnit( const CString _sProcessUnitName ) const;
	bool getUsedProInfo(const ProcessorID& id,InputTerminal *_pTerm,std::vector<CString>& _strVec);
	bool IfUseThisPipe(int nCurPipeIndex,CString& strSubFlowName)const;

	//----------------------------------------------------------------
	//Summary:
	//	check every sub flow percentag valid
	//-----------------------------------------------------------------
	bool IfAllFlowValid() const;
public:
	void BuildAllProcessPossiblePath();
	void SetTransitionLink (const PassengerFlowDatabase& database, const PassengerFlowDatabase& _gateDetailFlowDB, const PassengerFlowDatabase& _stationPaxFlowDB);
};
