#include "stdafx.h"
#include "ARCCharManager.h"

#define isNumeric(x)  ((x>=48&&x<=57)||x==43||x==45||x==46)

bool isAllNumeric(const char* buf)
{
	int len = (int) strlen(buf);
	for(int i=0; i<len; i++) {
		if(!isNumeric(buf[i]))
			return false;
	}
	return true;
}

void NextAlphaNumeric(char* buf)
{
	int len = (int) strlen(buf);
	if(len==1) {
		if(buf[0]=='Z')
			strcpy(buf, "AA");
		else
			buf[0]++;
	}
	else if(len==2) {
		if(buf[1]=='Z') {
			if(buf[0]!='Z') {
				buf[0]++;
				buf[1]='A';
			}
			else
				strcat(buf, "_COPY");
		}
		else {
			buf[1]++; //increment last
		}
	}
	else {
		strcat(buf, "_COPY");
	}
}
