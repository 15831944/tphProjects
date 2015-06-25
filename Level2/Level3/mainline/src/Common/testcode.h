#pragma once

#include "template.h"
#include "elaptime.h"
#include "csv_file.h"


class  testcode 
{

public:
	testcode(void);
	~testcode(void);

protected:

	ifsstream fpIn;
	ofsstream fpOut;
	int _curPos;
	int _lineLength;
	char* dataLine;
	
	//ifsstream fpIn;
	//ofsstream fpOut;
	//int curPos;
	//int lineLength;
	//char dataLine[LARGE];
	int a;
	int b;
	int c;

	float e;
	float f;

	
};

