#pragma once
#include "../Database/ADODatabase.h"
class CUndoOptionHander
{
public:
	CUndoOptionHander(void);
	~CUndoOptionHander(void);
public:
	static BOOL ReadData(int& _length , int& _time) ;
	static BOOL SaveData(int _length , int _time) ;

};
