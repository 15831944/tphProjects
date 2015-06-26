// FormatFloat.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>


int _tmain(int argc, _TCHAR* argv[])
{
    char buf[] = ".236589";
    char buf2[] = "256325.";
    char buf3[] = "-.05";
    char buf4[] = "-.";
    char buf5[] = "-5.";
    char buf6[] = "-5.-";
    char buf7[] = "--5.0-09";
    char buf8[] = ".-50";
    float f = atof(buf);
    float f2 = atof(buf2);
    float f3 = atof(buf3);
    float f4 = atof(buf4);
    float f5 = atof(buf5);
    float f6 = atof(buf6);
    float f7 = atof(buf7);
    float f8 = atof(buf8);
	return 0;
}

