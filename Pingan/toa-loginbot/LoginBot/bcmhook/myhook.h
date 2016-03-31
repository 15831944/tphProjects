#pragma once
#include "windows.h"
typedef struct _InlinHook
{
	 BYTE btOldCodeBytes[5];
	 DWORD dwHookCodeAddr;
	 DWORD dwJmpCodeAddr;
	 DWORD Ret;
}InlinHook;

#define HOOK_CODE_LONGTH 5

void __stdcall HookInline(InlinHook *myHook);
void __stdcall UnhookInline(InlinHook *myHook);

void __stdcall HookInline2(DWORD dwadr, unsigned char *data, int len);



int __stdcall GetHookAddr();
void SetPassPw(const char * pass,int len);

int __stdcall RepleasePass();
int __stdcall SetPassLenHook();
int __stdcall Set50IoHook();
int __stdcall Set04IoHook();


int __stdcall GetPassDataHook();
int __stdcall GetPassLenHook();
int __stdcall Get50IoHook();
int __stdcall Get5fPwHook();
HWND GetEditHwnd();


void __stdcall RepPass();
void __stdcall GetPass();

void __stdcall UnRepPass();
void __stdcall UnGetPass();

DWORD WINAPI RepleasePassThreadProc(LPVOID lpParam);
DWORD WINAPI GetPassDataThreadProc(LPVOID lpParam);




DWORD WINAPI RecvPass(LPVOID lpParam);

DWORD WINAPI EncodePass(LPVOID lpParam);
void CEvent();
void UEvent();
void Wait(int timeout);
void Signal();
void Destroy();


int InitHook(const wchar_t * filename);