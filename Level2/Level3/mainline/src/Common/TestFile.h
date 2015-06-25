#pragma once


#include "template.h"
#include "elaptime.h"
#include "csv_file.h"
//#include <fstream>
//#include "fsstream.h"
//class TestFile : public std::ofstream
//{
//public:
//	TestFile(void);
//	~TestFile(void);
//};

class baseTestClass
{
	public:
		baseTestClass();
   virtual ~baseTestClass(){}
protected:
	ifsstream fpIn;
	ofsstream fpOut;
	char dataLine[10240];
	int curPos;
	int lineLength;
};

class testClass : public CSVFile
{
public:
	testClass();
	~testClass(){}
protected:
	float m_fversion;
	int trimSpaces;

	//TestFile testFile;
	int nNumber;
	long lNumber;
};