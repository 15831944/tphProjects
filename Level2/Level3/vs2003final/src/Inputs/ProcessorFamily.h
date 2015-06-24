#pragma once

class ProcessorFamily
{
public:
	ProcessorFamily(void);
	virtual ~ProcessorFamily(void);
protected:
	int m_nStartIndexInProcList;
	int m_nEndIndexInProcList;
public:
	int GetStartIndexInProcList(void);
	int GetEndIndexInProcList(void);
	void SetStartIndexInProcList( int _nIndex );
	void SetEndIndexInProcList( int _nIndex );
};
