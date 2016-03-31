#pragma once

#include <stdlib.h>
#include <string>

using namespace std;
string ToAnsiString(const wstring & wstr)
{
	string strResult;
	int expectedBufferLength = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	if (expectedBufferLength > 0) {
		char * pszBuffer = (char *)malloc(expectedBufferLength);
		if (pszBuffer) {
			WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, pszBuffer, expectedBufferLength, NULL, NULL);
			pszBuffer[expectedBufferLength-1] = '\0';
			strResult = pszBuffer;
			free(pszBuffer);
			pszBuffer = NULL;
		}
	}
	return strResult;
}

wstring ToUnicodeString(const string & str)
{
	wstring wstrResult;
	int expectedBufferLength = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	if (expectedBufferLength) {
		wchar_t * pwszBuffer = (wchar_t *)malloc(sizeof(wchar_t) * expectedBufferLength);
		if (pwszBuffer) {
			MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, pwszBuffer, expectedBufferLength);
			pwszBuffer[expectedBufferLength-1] = L'\0';
			wstrResult = pwszBuffer;
			free(pwszBuffer);
			pwszBuffer = NULL;
		}
	}
	return wstrResult;
}