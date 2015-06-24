#pragma once
#include "fromtoproccaculation.h"

class CFromToProcCaculationWithMode :
	public CFromToProcCaculation
{
public:
	CFromToProcCaculationWithMode(void);
	~CFromToProcCaculationWithMode(void);

public:
	void InitData( int _iFromProc,int fromMode,int _iToProc,int toMode );
	void AddValue( int _iProc ,int paxMode ,double _dValue );
};
