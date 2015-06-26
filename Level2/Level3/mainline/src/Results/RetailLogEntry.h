#pragma once
#include "results\logitem.h"
#include "Common\TERM_BIN.H"
enum enumRetailLogType
{
	RETAIL_BEGINSERVICE,
	RETAIL_BUY,
	RETAIL_CHECKOUT,
	RETAIL_CANCEL,
	RETAIL_SKIP,
	RETAIL_SOLDOUT,
	RETAIL_BYPASS,
};
class CRetailLogEntry: public TerminalLogItem<RetailDescStruct,RetailEventStruct>
{
public:
	CRetailLogEntry(void);
	~CRetailLogEntry(void);

	virtual void echo (ofsstream& p_stream, const CString& _csProjPath) const;

	void SetID( const ProcessorID& _strID );
	//CString GetID()const;

	void SetIndex(int idx);
	int GetIndex()const;

	void SetProcIndex(int procIndex);
	int GetProcIndex()const;

	void SetProcessorType(char ch);
	char GetProcessorType()const;

	void SetSubType(char ch);
	char GetSubType()const;
};
