#include "..\stdafx.h"
#include "myhook.h"

#include <direct.h>
#include <stdio.h>
#include "dllload.h"
#include "windows.h"

// 全局变量
LPDWORD		g_lpdwVirtualKey = NULL;		// Keycode 数组的指针
int			g_nLength = 0;					// Keycode 数组的大小
BOOL		g_bDisableKeyboard = FALSE;		// 是否屏蔽整个键盘
HINSTANCE	g_hInstance = NULL;				// 模块实例句柄
HHOOK		g_hHook = NULL;					// 钩子句柄

typedef void(*BTNPROC)(char *buf,int len);
BTNPROC  BtnProc = NULL;

HWND ghEdit = NULL;
typedef struct _WND_INFO
{
    DWORD nTid;
    HWND hWnd;
    DWORD dwProcessId;
    CString sClassName;
    CString sWndName;
    int nClassIndex;
    int nWindowIndex;
}WNDINFO, *PWNDINFO;

int passdestlen =0;
int passsrclen=0;
//DLL的入口函数


void GetSendData(char  * buf);

static BOOL sFlg= FALSE;
BOOL gRepPassFlag = TRUE;
BOOL gGetPassFlag = FALSE;

void GetSafeModule();

DWORD WINAPI WorkThreadProc(LPVOID lpParam);

BOOL  WINAPI IsWow64()
{
    BOOL g_IsWow64 = FALSE;
    //if(g_first)
    {
        typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL); 
        LPFN_ISWOW64PROCESS fnIsWow64Process; 
        BOOL bIsWow64 = FALSE; 
        fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(L"kernel32"),"IsWow64Process"); 
        if (NULL != fnIsWow64Process) 
        { 
            fnIsWow64Process(GetCurrentProcess(),&bIsWow64);
        }
        g_IsWow64 = bIsWow64;
        //g_first = FALSE;
    }
    return g_IsWow64; 
}
void  SendData(char *buf,int len)
{
    HANDLE hPipe  = ::CreateFile(L"\\\\.\\pipe\\snailser", GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hPipe == INVALID_HANDLE_VALUE)
    {
        char szErr[256] = "";
        DWORD dwErr = GetLastError();
        sprintf(szErr, "%l", dwErr);
        return;
    }
    //mpDumpStr3(buf);
    DWORD dwBytesWritten = 0;
    ::WriteFile(hPipe, buf, len, &dwBytesWritten, NULL);
    CloseHandle(hPipe);
}


static BOOL CALLBACK EnumFuzzyWindowsProc(HWND hWnd, LPARAM lParam)
{
    WNDINFO* pInfo = (WNDINFO*)lParam;
    DWORD dwProcessId = 0;
    DWORD nTid = 0; 
    TCHAR szClass[256]={0};
    TCHAR szWindow[256]={0};
    BOOL bFlag = TRUE;
    CString sClass(_T(""));
    CString sWindow(_T(""));

    if (pInfo->dwProcessId == 0 && pInfo->sClassName.GetLength() <= 0 && pInfo->sWndName.GetLength() <= 0) return FALSE;
    nTid = GetWindowThreadProcessId(hWnd, &dwProcessId);
    ::GetWindowText(hWnd, szWindow, 255); //获取窗口标题
    ::GetClassName(hWnd, szClass, 255); //获取窗口类名称
    sClass = (CString )szClass;
    sWindow = (CString )szWindow;
    //TRACE("EnumWindowsProc: dwProcessId = %xh, lpClassName = %s, lpWndName = %s, nTid = %xh, hWnd = %xh\n", dwProcessId, szClass, szWindow, nTid, hWnd);
    if(!(pInfo->dwProcessId == 0 || (pInfo->dwProcessId != 0 && dwProcessId == pInfo->dwProcessId))) {
        goto exit;
    }
    if (!(((pInfo->nClassIndex < 0 && sClass.Find(pInfo->sClassName) >= 0) || (pInfo->nClassIndex >= 0 && sClass.Find(pInfo->sClassName) == pInfo->nClassIndex)))) {
        //TRACE("EnumWindowsProc: pInfo->nClassIndex = %d,  pInfo->sClassName = %s, sClass = %s, hWnd = %xh\n", pInfo->nClassIndex, pInfo->sClassName, sClass, hWnd);
        goto exit;
    }
    if (!(((pInfo->nWindowIndex < 0 && sWindow.Find(pInfo->sWndName) >= 0) || (pInfo->nWindowIndex >= 0 && sWindow.Find(pInfo->sWndName) == pInfo->nWindowIndex)))) {
        goto exit;
    }
    // get it
    pInfo->dwProcessId = dwProcessId;
    pInfo->nTid = nTid;
    pInfo->hWnd = hWnd;
    pInfo->sClassName = (CString )szWindow;
    pInfo->sWndName = (CString )szClass;
    bFlag = FALSE;

exit:
    return bFlag;
}

extern BOOL Process_GetFuzzyWindowInfo(WNDINFO &tInfo, DWORD dwProcessId, LPCTSTR lpClassName, LPCTSTR lpWndName, int nClassIndex, int nWindowIndex)
{
    BOOL bFlag = FALSE;
    tInfo.nTid = 0;
    tInfo.hWnd = NULL;
    tInfo.dwProcessId = dwProcessId;
    tInfo.sClassName = (CString )lpClassName;
    tInfo.sWndName = (CString )lpWndName;
    tInfo.nClassIndex = nClassIndex;
    tInfo.nWindowIndex = nWindowIndex;
    EnumWindows(EnumFuzzyWindowsProc, (LPARAM )&tInfo);
    bFlag = (tInfo.dwProcessId == 0)? FALSE: TRUE;
    return bFlag;
}
BOOL FindChildWndByClassName(HWND &hWnd, HWND hCurrentWnd, LPCTSTR lpClassName)
{
    BOOL bFlag = FALSE;
    HWND hChildWnd = NULL;
    HWND hPrevWnd = NULL;
    HWND hNextWnd = NULL;

    if (lpClassName == NULL || _tcsclen(lpClassName) <= 0 || hCurrentWnd == NULL) {
        hWnd = NULL;
        bFlag = TRUE;
    } else {
        TCHAR szClass[256]={0};
        ::GetClassName(hCurrentWnd, szClass, 255); //获取窗口类名称
        if (0 == memcmp((void* )szClass, (void* )_T("Internet Explorer_Server"), sizeof(TCHAR) * _tcsclen(_T("Internet Explorer_Server")))) {
            hWnd = hCurrentWnd;
            bFlag = TRUE;
        } else {
            hChildWnd = hCurrentWnd;
            while (NULL != (hChildWnd = ::GetTopWindow(hChildWnd))) {
                if (bFlag = FindChildWndByClassName(hWnd, hChildWnd, lpClassName)) goto exit;
            }
            hNextWnd = hCurrentWnd;
            while (NULL != (hNextWnd = ::GetNextWindow(hNextWnd, GW_HWNDNEXT))) {
                if (bFlag = FindChildWndByClassName(hWnd, hNextWnd, lpClassName)) goto exit;
            }
        }
    }
exit:
    return bFlag;
}
DWORD myGetCCBPId(HWND &hCCBNWnd)
{
    HWND hTRUEWnd = FALSE;
    HWND hIEWnd = NULL;
    BOOL bCCBWnd = FALSE;
    DWORD dwProcessId = 0;
    WNDINFO tWndInfo = {0};

    if(Process_GetFuzzyWindowInfo(tWndInfo, 0, _T("Afx"), _T("LoginBot"), -1, 0)){// (Process_GetFuzzyWindowInfo(tWndInfo, 0, _T("Frame"), _T("网银登录"), -1, 0)) {
        hIEWnd = tWndInfo.hWnd;
    } else {
        hIEWnd = NULL;
    }

    bCCBWnd = FindChildWndByClassName(hTRUEWnd, hIEWnd, _T("Internet Explorer_Server"));
    if (bCCBWnd && hTRUEWnd != NULL) {
        GetWindowThreadProcessId(hTRUEWnd, &dwProcessId);
    }
    TRACE("bCCBWnd=%d, hTRUEWnd=0x%x, hIEWnd = %0x%x, dwProcessId=%d/0x%x\n", bCCBWnd, hTRUEWnd, hIEWnd, dwProcessId, dwProcessId);
    if (dwProcessId != 0) hCCBNWnd = hTRUEWnd;
    return dwProcessId;
}
HWND GetEditHwnd()
{
    DWORD dwProcessId = 0;
    HWND hBCommWnd = NULL;
    HWND hChildWnd = NULL;

    if (0 != (dwProcessId = myGetCCBPId(hBCommWnd)) && NULL != hBCommWnd) 
    {
        hChildWnd = ::GetTopWindow(hBCommWnd);
        hChildWnd = ::GetTopWindow(hChildWnd);
    }
    return hChildWnd;
}

void DumpStrmm(const char *p)
{	
    FILE *fp;

    fp = fopen("c:\\log.txt", "a+");
    if (!fp)
        return;
    fprintf(fp,"\n%s\n",p);
    printf("%s\n",p);
    fclose(fp);
}

void __stdcall HookInline2(DWORD dwadr, unsigned char *data, int len)
{
    HANDLE hGame = GetCurrentProcess();
    DWORD dwCodeSize = 0;
    dwCodeSize = 0;
    DWORD fNew, fOld;
    fNew = PAGE_READWRITE;
    if(VirtualProtect((void*)dwadr, len, fNew, &fOld))
    {
    }
    else
    {

    }

    if(WriteProcessMemory(hGame, (void *)dwadr, (void *)data, len, &dwCodeSize))
    {

    }
    else
    {
    }

    VirtualProtect((void*)dwadr, len, fOld, &fNew);
}
void __stdcall HookInline(InlinHook *myHook)
{
    HANDLE hGame = GetCurrentProcess();
    DWORD dwCodeSize = 0;
    ReadProcessMemory(hGame, (void *)myHook->dwHookCodeAddr, (void *)myHook->btOldCodeBytes, HOOK_CODE_LONGTH, &dwCodeSize);
    BYTE btNewBytes[5] = {0xe9,0x00,0x00,0x00,0x00}; 
    *(DWORD *)( btNewBytes + 1) = (DWORD)myHook->dwJmpCodeAddr - myHook->dwHookCodeAddr - HOOK_CODE_LONGTH;
    dwCodeSize = 0;
    DWORD fNew, fOld;

    fNew = PAGE_READWRITE;

    if(VirtualProtect((void*)myHook->dwHookCodeAddr, HOOK_CODE_LONGTH, fNew, &fOld))
    {

    }
    else
    {

    }

    if(WriteProcessMemory(hGame, (void *)myHook->dwHookCodeAddr, (void *)btNewBytes, HOOK_CODE_LONGTH, &dwCodeSize))
    {

    }
    else
    {

    }

    VirtualProtect((void*)myHook->dwHookCodeAddr, HOOK_CODE_LONGTH, fOld, &fNew);
}
void __stdcall UnhookInline(InlinHook *myHook)
{
    HANDLE hGame = GetCurrentProcess();
    DWORD dwCodeSize = 0;
    DWORD fNew, fOld;
    fNew = PAGE_READWRITE;
    VirtualProtect((void*)myHook->dwHookCodeAddr, HOOK_CODE_LONGTH, fNew, &fOld);
    WriteProcessMemory(hGame, (void *)myHook->dwHookCodeAddr, (void *)myHook->btOldCodeBytes, HOOK_CODE_LONGTH, &dwCodeSize);
    VirtualProtect((void*)myHook->dwHookCodeAddr, HOOK_CODE_LONGTH, fOld, &fNew);
}



//HeapAlloc(
//HeapAlloc(
//HeapFree(
//mov     ebx, ds:ExAllocatePoolWithTag
// mov     ebx, ds:ExAllocatePoolWithTag
//7C801663   53               PUSH EBX
//7C801664   53               PUSH EBX
//7C801665   53               PUSH EBX
//7C801666   FF75 08          PUSH DWORD PTR SS:[EBP+8]
//7C801669   0F84 D8000000    JE kernel32.7C801747
//7C80166F   FF15 3810807C    CALL DWORD PTR DS:[<&ntdll.NtDeviceIoCon>; ntdll.ZwDeviceIoControlFile
//7C801675   3D 03010000      CMP EAX,103
//7C80167A   0F84 B0000000    JE kernel32.7C801730
//7C801680   3BC3             CMP EAX,EBX
char  keydata[8]={0};
DWORD  dwJmpRetAddrio;
BOOL  bGetkeyflag = FALSE;

DWORD dwHookAddr = 0;
DWORD dwJmpRetAddr = 0;

DWORD dwHookRepleasePassAddr = 0;
DWORD dwJmpRepleasePassAddr = dwHookRepleasePassAddr+6;

DWORD dwHookSetPassLenAddr = 0;
DWORD dwJmpSetPassLenAddr = 0;

DWORD dwHookSet50IoAddr =0;

DWORD dwJmpSet50IoAddr = 0;
DWORD dwJmpGet5fPwAddr = 0;
DWORD dwJmpSet04IoAddr = 0;
char passdata[4000]={0};
char pass2[40] ={0};
int passlen = 0;

int gpasslen = 0;
char keyout[40]={0};
int keylen = 0;

int plen = 0;

BOOL bSetPassflag = FALSE;

unsigned char passlenData[20] = {0};


DWORD WINAPI EncodePass(LPVOID lpParam)
{
    while(1)
    {
        if(RepleasePass()==1 && SetPassLenHook()==1 && Set50IoHook()==1&&Set04IoHook()==1)
        {
            if(bGetkeyflag==TRUE&& gRepPassFlag == TRUE)
            {
                memset(passdata,0,100);
                memcpy(passdata,pass2,passlen);
                sub_14F40(passdata,passlen,0,&plen,keyout,keylen);
                memcpy(passlenData+4,&plen,4);
                memcpy(passlenData+8,&passlen,4); 
                //gRepPassFlag = FALSE;
                HWND hEdit = GetEditHwnd();
                ghEdit = hEdit;
                for(int i = 0;i<passlen;i++)
                {
                    ::SendMessage(hEdit,0xbdc,0x2a,1);
                    Sleep(100);
                }
                break;
            }
        }
        Sleep(500);
    }
    return 0;
}
void SetPassPw(const char * pass,int len)
{
    //memcpy(passdata,pass,len);
    memcpy(pass2,pass,len);
    passlen = len;
    gpasslen = len;
    //memset(keyout,0,40);
    memset(passlenData,0,20);
    EncodePass(NULL);
    //::CreateThread(NULL, 0, EncodePass, NULL, 0, NULL);
    //gpasslen = passlen;
}


void GetKey(DWORD ioCode, char *key)
{
    if(ioCode== 0x22200c)
    {
        memcpy(keydata,key+16,8);
        KeyEncode(keydata,8,keyout,&keylen);
        //gRepPassFlag = FALSE;
        //gGetPassFlag = FALSE;
        bGetkeyflag = TRUE;
    }
}


void __declspec(naked) jmpGetKey()
{
    __asm 
    {
        pushad
            push edi   //keydata
            push ecx  //iocode
            call GetKey
            add esp,8
            popad
            PUSH EBX
            PUSH EBX
            PUSH DWORD PTR SS:[EBP+8]
        jmp dwJmpRetAddrio
    }
}

//75CFD182    53              push ebx
//75CFD183    53              push ebx
//75CFD184    FF75 08         push dword ptr ss:[ebp+8]
//75CFD187    74 08           je short KERNELBA.75CFD191
//
//75CFD0A1 >  6A 10           push 10
//75CFD0A3    68 D887D275     push KERNELBA.75D287D8
//75CFD0A8    E8 D79E0200     call KERNELBA.75D26F84
//75CFD0AD    8B4D 0C         mov ecx,dword ptr ss:[ebp+C]
//75CFD0B0    8BC1            mov eax,ecx


static InlinHook HookIo = { {0x00,0x00,0x00,0x00,0x00},0,(DWORD)jmpGetKey,0};
int __stdcall GetHookAddr()
{
    HMODULE hMod= ::GetModuleHandle(L"kernel32.dll");
    FARPROC fardic =  ::GetProcAddress(hMod,(LPCSTR)"DeviceIoControl");

    DWORD deviceAdr = 0;
    if(IsWow64())
    {
        hMod= ::GetModuleHandle(L"kernelbase.dll");
        fardic =  ::GetProcAddress(hMod,(LPCSTR)"DeviceIoControl");
        deviceAdr = (DWORD) fardic +0xe1;
    }
    else
    {
        deviceAdr = (DWORD) fardic +0x3b;
    }
    //DWORD deviceAdr = (DWORD) fardic +0x3b;
    dwJmpRetAddrio = deviceAdr+5;
    HookIo.dwHookCodeAddr = deviceAdr;
    HookIo.dwJmpCodeAddr = (DWORD)jmpGetKey;
    HookInline(&HookIo);
    return 1;
}



void setPasswd(char * buf)
{
    memcpy(buf,passdata,plen);
}
void __declspec(naked) jmpRepleasePass()
{
    __asm 
    {
        pushad
            push eax
            call setPasswd
            add esp,4
            popad
            push dword ptr ss:[ebp-0x424]
        jmp dwJmpRepleasePassAddr
    }
}


static InlinHook HookRp = { {0x00,0x00,0x00,0x00,0x00},0,(DWORD)jmpRepleasePass,0};
int __stdcall RepleasePass()
{
    HMODULE hMod= ::GetModuleHandle(L"XEdit.dll");
    if(hMod ==NULL)
        return 0;
    dwHookRepleasePassAddr = (DWORD) hMod+0xe723;
    dwJmpRepleasePassAddr = dwHookRepleasePassAddr+6;
    HookRp.dwHookCodeAddr = dwHookRepleasePassAddr;
    HookRp.dwJmpCodeAddr = (DWORD)jmpRepleasePass;
    HookInline(&HookRp);
    return 1;
}


void setPasswdLen(char * buf)
{
    DWORD dwNum = *(DWORD *)(buf+8);

    if(bSetPassflag == FALSE)
    {
        dwNum = dwNum +passlen;
        gpasslen = dwNum;
        memcpy(passlenData+8,&dwNum,4);
        memcpy(buf,passlenData,12);
    }
    else
    {
        gpasslen = dwNum;
    }
}
void __declspec(naked) jmpSetPassLen()
{
    __asm 
    {
        pushad
            push edi
            call setPasswdLen
            add esp,4
            popad
            mov eax,dword ptr ds:[edi+0x4]
        mov ecx,dword ptr ss:[ebp-0x428]
        jmp dwJmpSetPassLenAddr
    }
}




static InlinHook HookSetPassLen = { {0x00,0x00,0x00,0x00,0x00},0,(DWORD)jmpSetPassLen,0};
int __stdcall SetPassLenHook()
{
    HMODULE hMod= ::GetModuleHandle(L"XEdit.dll");
    if(hMod ==NULL)
        return 0;
    dwHookSetPassLenAddr = (DWORD) hMod+0xe784;
    dwJmpSetPassLenAddr = dwHookSetPassLenAddr+9;
    HookSetPassLen.dwHookCodeAddr = dwHookSetPassLenAddr;
    HookSetPassLen.dwJmpCodeAddr = (DWORD)jmpSetPassLen;
    HookInline(&HookSetPassLen);
    return 1;
}




void set50Io(char * buf)
{
    int a = 1;
    memcpy(buf+12,&a,4);
    //bGetkeyflag = FALSE;
}
void __declspec(naked) jmpSet50Io()
{
    __asm 
    {
        pushad
            push eax
            call set50Io
            add esp,4
            popad
            mov eax,dword ptr ds:[eax+0xC]
        mov ecx,dword ptr ss:[ebp-0x20]
        jmp dwJmpSet50IoAddr
    }
}


static InlinHook HookSet50Io = { {0x00,0x00,0x00,0x00,0x00},0,(DWORD)jmpSet50Io,0};



DWORD  callAddr= 0;
int __stdcall Set50IoHook()
{

    HMODULE hMod= ::GetModuleHandle(L"XEdit.dll");
    if(hMod ==NULL)
        return 0;
    unsigned char data9[2]={0x90,0x90};
    HookInline2((DWORD)hMod+0xEA85,data9,2);
    dwHookSet50IoAddr = (DWORD) hMod+0xea8a;
    dwJmpSet50IoAddr = dwHookSet50IoAddr+6;
    HookSet50Io.dwHookCodeAddr = dwHookSet50IoAddr;
    HookSet50Io.dwJmpCodeAddr = (DWORD)jmpSet50Io;

    HookInline(&HookSet50Io);
    return 1;
}


void set04Io(char * buf)
{
    /*int a = 1;
    DWORD dwIndex = *(DWORD *)buf;
    dwIndex++;
    memcpy(buf,&dwIndex,4);*/
    DWORD dwIndex = *(DWORD *)(buf+12);
    if(dwIndex>0)
    {
        HWND hEdit = GetEditHwnd();
        ghEdit = hEdit;
        for(int i = 0;i<gpasslen;i++)
        {
            ::SendMessage(ghEdit,0xbdc,0x8,0);
            //Sleep(5);
        }
        gpasslen = 0;
        bSetPassflag = TRUE;
    }

}
void __declspec(naked) jmpSet04Io()
{
    __asm 
    {
        pushad
            push edi
            call set04Io
            add esp,4
            popad
            mov eax,dword ptr ss:[ebp-0x18]
        push ebx
            push ebx
            jmp dwJmpSet04IoAddr
    }
}


static InlinHook HookSet04Io = { {0x00,0x00,0x00,0x00,0x00},0,(DWORD)jmpSet04Io,0};


int __stdcall Set04IoHook()
{
    HMODULE hMod= ::GetModuleHandle(L"XEdit.dll");
    if(hMod ==NULL)
        return 0;
    DWORD dwHookSet04IoAddr = (DWORD) hMod+0xdce7;
    dwJmpSet04IoAddr = dwHookSet04IoAddr+5;
    HookSet04Io.dwHookCodeAddr = dwHookSet04IoAddr;
    HookSet04Io.dwJmpCodeAddr = (DWORD)jmpSet04Io;
    HookInline(&HookSet04Io);
    return 1;
}




void GetPassData(char * buf)
{
    memset(passdata,0,40);
    memcpy(passdata,buf,40);
}
void __declspec(naked) jmpGetPassData()
{
    __asm 
    {
        pushad
            push eax
            call GetPassData
            add esp,4
            popad
            push dword ptr ss:[ebp-0x424]
        jmp dwJmpRepleasePassAddr
    }
}


static InlinHook HookGetPassData = { {0x00,0x00,0x00,0x00,0x00},0,(DWORD)jmpGetPassData,0};
int __stdcall GetPassDataHook()
{
    HMODULE hMod= ::GetModuleHandle(L"XEdit.dll");
    if(hMod ==NULL)
        return 0;
    dwHookRepleasePassAddr = (DWORD) hMod+0xe723;
    dwJmpRepleasePassAddr = dwHookRepleasePassAddr+6;
    HookGetPassData.dwHookCodeAddr = dwHookRepleasePassAddr;
    HookGetPassData.dwJmpCodeAddr = (DWORD)jmpGetPassData;
    HookInline(&HookGetPassData);
    return 1;
}


void GetPasswdLen(char * buf)
{
    memcpy(passlenData,buf,12);
    passdestlen =*(int *)(passlenData+4);
    passsrclen=*(int *)(passlenData+8);
    //::SendMessage(ghEdit,0xbdc,0x2a,1);
}
void __declspec(naked) jmpGetPassLen()
{
    __asm 
    {
        pushad
            push edi
            call GetPasswdLen
            add esp,4
            popad
            mov eax,dword ptr ds:[edi+0x4]
        mov ecx,dword ptr ss:[ebp-0x428]
        jmp dwJmpSetPassLenAddr
    }
}


static InlinHook HookGetPassLen = { {0x00,0x00,0x00,0x00,0x00},0,(DWORD)jmpGetPassLen,0};
int __stdcall GetPassLenHook()
{

    HMODULE hMod= ::GetModuleHandle(L"XEdit.dll");
    if(hMod ==NULL)
        return 0;
    dwHookSetPassLenAddr = (DWORD) hMod+0xe784;
    dwJmpSetPassLenAddr = dwHookSetPassLenAddr+9;
    HookGetPassLen.dwHookCodeAddr = dwHookSetPassLenAddr;
    HookGetPassLen.dwJmpCodeAddr = (DWORD)jmpGetPassLen;
    HookInline(&HookGetPassLen);
    return 1;
}


void get50Io(char * buf)
{
    __asm
    {
        mov eax, keylen
            push eax
            lea eax,keyout
            push eax
            mov eax,passsrclen
            push eax
            push 0h
            mov eax,passdestlen
            push eax
            lea eax,passdata
            push eax
            call PassEncode3
            Add esp,0x18
    }
    Signal();
    Sleep(2000);
    gGetPassFlag = FALSE;
}
void __declspec(naked) jmpGet50Io()
{
    __asm 
    {
        pushad
            push eax
            call get50Io
            add esp,4
            popad
            mov eax,dword ptr ds:[eax+0xC]
        mov ecx,dword ptr ss:[ebp-0x20]
        jmp dwJmpSet50IoAddr
    }
}


static InlinHook HookGet50Io = { {0x00,0x00,0x00,0x00,0x00},0,(DWORD)jmpGet50Io,0};



//DWORD  callAddr= 0;
int __stdcall Get50IoHook()
{
    HMODULE hMod= ::GetModuleHandle(L"XEdit.dll");
    if(hMod ==NULL)
        return 0;
    unsigned char data9[2]={0x90,0x90};
    HookInline2((DWORD)hMod+0xEA85,data9,2);
    dwHookSet50IoAddr = (DWORD) hMod+0xea8a;
    dwJmpSet50IoAddr = dwHookSet50IoAddr+6;
    HookGet50Io.dwHookCodeAddr = dwHookSet50IoAddr;
    HookGet50Io.dwJmpCodeAddr = (DWORD)jmpGet50Io;
    HookInline(&HookGet50Io);
    return 1;
}


void Get5fPw()
{
    __asm
    {
        //push ebp
        mov eax, keylen
            push eax
            lea eax,keyout
            push eax
            mov eax,passsrclen
            push eax
            push 0h
            mov eax,passdestlen
            push eax
            lea eax,passdata
            push eax
            call PassEncode3
            Add esp,0x18
            //pop ebp
    }
    Signal();
    Sleep(2000);
    gGetPassFlag = FALSE;

}
void __declspec(naked) jmpGet5fPw()
{
    __asm 
    {
        pushad
            call Get5fPw
            popad
            cmp eax,dword ptr ds:[edi+0x110]
        jmp dwJmpGet5fPwAddr
    }
}

static InlinHook HookjmpGet5fPw = { {0x00,0x00,0x00,0x00,0x00},0,(DWORD)jmpGet5fPw,0};


//DWORD  callAddr= 0;
int __stdcall Get5fPwHook()
{
    HMODULE hMod= ::GetModuleHandle(L"XEdit.dll");
    if(hMod ==NULL)
        return 0;
    DWORD dwHookGet2addr = (DWORD) hMod+0x2c5e;
    dwJmpGet5fPwAddr = dwHookGet2addr+6;
    HookjmpGet5fPw.dwHookCodeAddr = dwHookGet2addr;
    HookjmpGet5fPw.dwJmpCodeAddr = (DWORD)jmpGet5fPw;
    HookInline(&HookjmpGet5fPw);
    return 1;
}

DWORD WINAPI RepleasePassThreadProc(LPVOID lpParam)
{
    while(1)
    {
        if(RepleasePass()==1 && SetPassLenHook()==1 && Set50IoHook()==1&&(gRepPassFlag == FALSE)&&Set04IoHook()==1)
        {
            Sleep(1000);
            HWND hEdit = GetEditHwnd();
            ghEdit = hEdit;
            for(int i = 0;i<passlen;i++)
            {
                ::SendMessage(hEdit,0xbdc,0x2a,1);
                Sleep(100);
            }
            gRepPassFlag = TRUE;
        }
        if(sFlg==TRUE)
            break;
        Sleep(500);
    }
    return 0;
}

DWORD WINAPI GetPassDataThreadProc(LPVOID lpParam)
{
    while(1)
    {
        if(GetPassDataHook()==1 && GetPassLenHook()==1 &&/* Get50IoHook()==1&&*/Get5fPwHook()==1&& (gGetPassFlag==FALSE))
        {
            //break; 
            gGetPassFlag = TRUE;
        }
        if(sFlg==TRUE)
            break;
        Sleep(1500);
    }
    return 0;
}

void __stdcall RepPass()
{
    ::CreateThread(NULL, 0, RepleasePassThreadProc, NULL, 0, NULL);
}
void __stdcall GetPass()
{
    TRACE(L"hwd, GetPass");
    ::CreateThread(NULL, 0, GetPassDataThreadProc, NULL, 0, NULL);
    ::CreateThread(NULL, 0, RecvPass, NULL, 0, NULL);
}

void __stdcall UnRepPass()
{
    UnhookInline(&HookRp);
    UnhookInline(&HookSetPassLen);
    UnhookInline(&HookSet50Io);
}
void __stdcall UnGetPass()
{
    UnhookInline(&HookGetPassData);
    UnhookInline(&HookGetPassLen);
    UnhookInline(&HookGet50Io);
}

HANDLE m_event = NULL;

DWORD WINAPI RecvPass(LPVOID lpParam)
{ 
    TRACE(L"hwd, RecvPass");
    while(1)
    {
        Wait(9000000);
        CString str = CString(passdata);
        TRACE(L"hwd, pwd: %s", str);
    }
    return 0;
}

void CEvent()
{
    m_event = CreateEvent(NULL, TRUE, TRUE, NULL);
}

void UEvent()
{
    if (m_event)
    {
        CloseHandle(m_event);
        m_event = NULL;
    }

}

void Wait(int timeout) 
{
    ResetEvent(m_event);

    WaitForSingleObject(m_event, timeout); 
}

void Signal() 
{ 
    SetEvent(m_event); 
}

void Destroy()
{
    sFlg= TRUE;
    UnloadMemory();
}


//
//&& bCtrlKeyDown) || // Ctrl+Esc
//            // Alt+TAB
//            (kblp->vkCode==VK_TAB && kblp->flags & LLKHF_ALTDOWN) ||   
//            // Alt+Esc
//            (kblp->vkCode==VK_ESCAPE && kblp->flags & LLKHF_ALTDOWN)|| 
//            (kblp->vkCode==VK_LWIN || kblp->vkCode==VK_RWIN))


//0013E810   000F01C6  |hWnd = 0xF01C6
//0013E814   00000BDC  |Message = MSG(0xBDC)
//0013E818   00000008  |wParam = 0x8
//0013E81C   00000000  \lParam = 0x0



LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    // 禁用键盘的某个按键, 如果 g_bDisableKeyboard 为 TRUE 则禁用整个键盘
    if (nCode == HC_ACTION)
    {
        KBDLLHOOKSTRUCT *kblp=(KBDLLHOOKSTRUCT*)lParam;
        BOOL bCtrlKeyDown = GetAsyncKeyState(VK_CONTROL)>>((sizeof(SHORT) * 8) - 1);


        if (kblp->vkCode==VK_RETURN)
        {

            ::SendMessage(ghEdit,0xbdc,0x08,0);

        }else if( (kblp->vkCode>=0x30 && kblp->vkCode<= 0x39)|| 
            (kblp->vkCode>=41 && kblp->vkCode<=0x5a)||
            (kblp->vkCode>=0x60&& kblp->vkCode<=69))
        {
            //::SendMessage(ghEdit,0xbdc,0x2a,1);
        }

    }

    // 传给系统中的下一个钩子
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}


BOOL WINAPI StartKey(LPDWORD lpdwVirtualKey, int nLength, BOOL bDisableKeyboard = FALSE)
{
    if (g_hHook != NULL) return FALSE;
    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, ::GetModuleHandleA(NULL), NULL);
    if (g_hHook == NULL) 
        return FALSE;
    return TRUE;

}


BOOL WINAPI StopKey()
{
    // 卸载钩子
    if (UnhookWindowsHookEx(g_hHook) == 0)
        return FALSE;
    g_hHook = NULL;
    return TRUE;
}

int InitHook(const wchar_t * filename)
{
    int iRet = 0;
    iRet = LoadFromMemory(filename);//加载网页前调用

    if(iRet<0)
        return 0;
    GetHookAddr();//加载网页前调用
    CEvent();

    return 1;
}
