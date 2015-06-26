#pragma once


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "results\logitem.h"
#include "Common\point.h"
#include "Common\TERM_BIN.H"
class CBridgeConnectorLogEntry: public TerminalLogItem<BridgeConnectorDescStruct,BridgeConnectorEventStruct>
{
public:
	CBridgeConnectorLogEntry(void);
	~CBridgeConnectorLogEntry(void);

public:
	void SetID( const ProcessorID& id );

	void SetStartTime( const ElapsedTime& _time );

	void SetEndTime( const ElapsedTime& _time );

	ElapsedTime GetStartTime() const;
	ElapsedTime GetEndTime() const;

	void SetIndex( long _lIdx );
	long GetIndex()const;

	virtual void echo (ofsstream& p_stream, const CString& _csProjPath) const;

	//short name[MAX_PROC_IDS];   // full name of processor
	//short index;                // processor's index in list
	//char procType;              // processor class

	void SetLocation(const Point& ptLocation);

	//width
	void SetWidth(double dWidth);

	//Length
	void SetLength(double dLength);

	//direction from
	//direction to	
	void SetDirFromTo(const Point& ptDirFrom, const Point& ptDirTo);

	//long	startPos;			// pointer to start of track in binary file
	//long    endPos;             // pointer to end of track in binary file

	//float	speed;				// speed of the resource element moves




};
