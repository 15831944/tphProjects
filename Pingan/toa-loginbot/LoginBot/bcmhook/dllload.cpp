// dllload.cpp : 定义控制台应用程序的入口点。
//

#include "..\stdafx.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <malloc.h>
#include "MemoryModule.h"
#include "myhook.h"

typedef int(*testProc)(void* ptrConfig, HWND msgWindow);
#define DLL_FILE TEXT("K:\\pass\\dllload\\Debug\\BocomKeyFlt.sys")
typedef int (  __stdcall *ENCODE)( char *pass, int passlen,  int flag, int *plen,char *key,int keylen);
typedef int (  __stdcall *ENCODE130BE)( int index, char * buf1,  int buf1len, char *outbuf,int * NumberOfBytes);
typedef int (  __stdcall *ENCODE132E6)( int index, int  index2,char * buf1,  char *buf2, int NumberOfBytes,char *key, int keylen);

ENCODE130BE sub_130BE;
ENCODE132E6 sub_132E6;

HMEMORYMODULE  dllhandle = NULL;

DWORD  base = 0;
int  sub_14F40(char *a1, int a2, int a3, int * a4, char * a5, int a6)
{
    char  v15[100] = {0}; 
    int  NumberOfBytes; 
    int  v17;

    NumberOfBytes = a2 + 8;
    char v6[400]= {0};
    char v7[400]={0};
    char * v13 = v6;
    memcpy(v13, a1, a2);
    if ( !sub_130BE(2, v13, a2, v15, &NumberOfBytes) )
    {
    }
    v17 = NumberOfBytes + 1;
    char  v10[400] ={0};
    char * v14 = v10;

    //memset(v10, 0, v17);
    if ( !sub_132E6(0, 0, v15, v14, NumberOfBytes, a5, a6) )
    {
    }
    memset((void *)a1, 0, 400);
    memcpy((void *)a1, (const void *)v14, NumberOfBytes);
    *(int  *)a4 = NumberOfBytes;

    return 0;
}
int  PassEncode3(char *a1, int  a2, int a3, int  a4, char * a5, int a6)
{
    int NumberOfBytes;
    NumberOfBytes = a2 + 8;
    char v6[400]={0};
    memset(v6, 0, NumberOfBytes);
    char v7[400] = {0};
    memset(v7, 0, NumberOfBytes);
    memcpy(v6, a1, a2);
    if ( !sub_130BE(2, v6, a2, v7, (&NumberOfBytes) ))
    {

    }
    char v10[400] ={0}; 
    memset(v10, 0, NumberOfBytes + 1);
    if ( !sub_132E6(1, 0, v7, v10, NumberOfBytes, a5, a6) )
    {
    }
    *((BYTE *)v10 + a4) = 0;
    memset((void *)a1, 0,400);
    memcpy((void *)a1, v10, a4);

    return 0;
}
unsigned char mkey[] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10,0xf0,0xe1,0xd2,0xc3};
typedef int (  __stdcall *ENCODE)( char *pass, int passlen,  int flag, int *plen,char *key,int keylen);
typedef int (__stdcall *KEYCODE)( char *key, int keylen, char *enKey,int * enLen);


typedef void(__cdecl *MEMCPY)();
typedef void(__cdecl *MEMSET)(void* dst, int i ,int len);

MEMCPY  mmMemcpy = (MEMCPY)memcpy;
MEMCPY  mmMemset = (MEMCPY)memset;

KEYCODE  keycode;
ENCODE  myencode;
int jmpsendnext;
int jmpsendnext2;
int jmpsendnext3;
int jmpsendnext4;


int nextjmpmemset3;
int nextjmpmemset4;
int nextjmpmemset5;
int nextjmpmemset6;
int nextjmpmemset7;
int nextjmpmemset8;
void __declspec(naked) jmpPacket_send()
{
    //::MessageBoxA(NULL,"232323","dafds",0);
    __asm 
    {
        /*	pushad
        push ebx
        push edi
        push eax*/
        call  mmMemcpy //call memcpy
            /*popad*/
            //retn
            jmp jmpsendnext
    }
}

void __declspec(naked) jmpPacket_send2()
{
    __asm 
    {
        /*	pushad
        push ebx
        push edi
        push eax*/
        call mmMemcpy
            /*popad*/
            jmp jmpsendnext2
    }
}
int jmpMemcpynext1;
void __declspec(naked) jmpMemcpy1()
{
    __asm 
    {
        /*	pushad
        push ebx
        push edi
        push eax*/
        call mmMemcpy
            /*popad*/
            jmp jmpMemcpynext1
    }
}

int jmpMemcpynext2;
void __declspec(naked) jmpMemcpy2()
{
    __asm 
    {
        call mmMemcpy
            jmp jmpMemcpynext2
    }
}

int jmpMemcpynext3;
void __declspec(naked) jmpMemcpy3()
{
    __asm 
    {
        /*	pushad
        push ebx
        push edi
        push eax*/
        call mmMemcpy
            /*popad*/
            jmp jmpMemcpynext3
    }
}

int jmpMemcpynext4;
void __declspec(naked) jmpMemcpy4()
{
    __asm 
    {
        /*	pushad
        push ebx
        push edi
        push eax*/
        call mmMemcpy
            /*popad*/
            jmp jmpMemcpynext4
    }
}

int jmpMemcpynext5;
void __declspec(naked) jmpMemcpy5()
{
    __asm 
    {
        /*	pushad
        push ebx
        push edi
        push eax*/
        call mmMemcpy
            /*popad*/
            jmp jmpMemcpynext5
    }
}

int jmpMemcpynext6;
void __declspec(naked) jmpMemcpy6()
{
    __asm 
    {
        /*	pushad
        push ebx
        push edi
        push eax*/
        call mmMemcpy
            /*popad*/
            jmp jmpMemcpynext6
    }
}

int jmpMemcpynext7;
void __declspec(naked) jmpMemcpy7()
{
    __asm 
    {
        /*	pushad
        push ebx
        push edi
        push eax*/
        call mmMemcpy
            /*popad*/
            jmp jmpMemcpynext7
    }
}

void __declspec(naked) jmpmemset()
{
    __asm 
    {
        /*	pushad
        push ebx
        push edi
        push eax*/
        call mmMemset
            /*popad*/
            jmp jmpsendnext3
    }
}
void __declspec(naked) jmpmemset2()
{
    __asm 
    {
        /*	pushad
        push ebx
        push edi
        push eax*/
        call mmMemset
            /*popad*/
            jmp jmpsendnext4
    }
}

void __declspec(naked) jmpmemset3()
{
    __asm 
    {
        /*	pushad
        push ebx
        push edi
        push eax*/
        call mmMemset
            /*popad*/
            jmp nextjmpmemset3
    }
}
void __declspec(naked) jmpmemset4()
{
    __asm 
    {
        /*	pushad
        push ebx
        push edi
        push eax*/
        call mmMemset
            /*popad*/
            jmp nextjmpmemset4
    }
}
void __declspec(naked) jmpmemset5()
{
    __asm 
    {
        /*	pushad
        push ebx
        push edi
        push eax*/
        call mmMemset
            /*popad*/
            jmp nextjmpmemset5
    }
}
void __declspec(naked) jmpmemset6()
{
    __asm 
    {
        /*	pushad
        push ebx
        push edi
        push eax*/
        call mmMemset
            /*popad*/
            jmp nextjmpmemset6
    }
}
void __declspec(naked) jmpmemset7()
{
    __asm 
    {
        /*	pushad
        push ebx
        push edi
        push eax*/
        call mmMemset
            /*popad*/
            jmp nextjmpmemset7
    }
}
void __declspec(naked) jmpmemset8()
{
    __asm 
    {
        /*	pushad
        push ebx
        push edi
        push eax*/
        call mmMemset
            /*popad*/
            jmp nextjmpmemset8
    }
}
InlinHook MySendHook = { {0x00,0x00,0x00,0x00,0x00},0x01904D4C,(DWORD)jmpPacket_send,0};
InlinHook MySendHook2 = { {0x00,0x00,0x00,0x00,0x00},0x01904D4C,(DWORD)jmpPacket_send2,0};
InlinHook Mymemset = { {0x00,0x00,0x00,0x00,0x00},0x01904D4C,(DWORD)jmpmemset,0};
InlinHook Mymemset2 = { {0x00,0x00,0x00,0x00,0x00},0x01904D4C,(DWORD)jmpmemset2,0};
InlinHook Mymem4a52 = { {0x00,0x00,0x00,0x00,0x00},0x01904D4C,(DWORD)0x90909090,0};

InlinHook Mymemset3 = { {0x00,0x00,0x00,0x00,0x00},0x01904D4C,(DWORD)jmpmemset3,0};
InlinHook Mymemset4 = { {0x00,0x00,0x00,0x00,0x00},0x01904D4C,(DWORD)jmpmemset4,0};
InlinHook Mymemset5 = { {0x00,0x00,0x00,0x00,0x00},0x01904D4C,(DWORD)jmpmemset5,0};
InlinHook Mymemset6 = { {0x00,0x00,0x00,0x00,0x00},0x01904D4C,(DWORD)jmpmemset6,0};
InlinHook Mymemset7 = { {0x00,0x00,0x00,0x00,0x00},0x01904D4C,(DWORD)jmpmemset7,0};
InlinHook Mymemset8 = { {0x00,0x00,0x00,0x00,0x00},0x01904D4C,(DWORD)jmpmemset8,0};

InlinHook Mymemcpy1 = { {0x00,0x00,0x00,0x00,0x00},0x01904D4C,(DWORD)jmpMemcpy1,0};
InlinHook Mymemcpy2 = { {0x00,0x00,0x00,0x00,0x00},0x01904D4C,(DWORD)jmpMemcpy2,0};
InlinHook Mymemcpy3 = { {0x00,0x00,0x00,0x00,0x00},0x01904D4C,(DWORD)jmpMemcpy3,0};
InlinHook Mymemcpy4 = { {0x00,0x00,0x00,0x00,0x00},0x01904D4C,(DWORD)jmpMemcpy4,0};
InlinHook Mymemcpy5 = { {0x00,0x00,0x00,0x00,0x00},0x01904D4C,(DWORD)jmpMemcpy5,0};
InlinHook Mymemcpy6 = { {0x00,0x00,0x00,0x00,0x00},0x01904D4C,(DWORD)jmpMemcpy6,0};
InlinHook Mymemcpy7 = { {0x00,0x00,0x00,0x00,0x00},0x01904D4C,(DWORD)jmpMemcpy7,0};

DWORD  GetResData(HMODULE  hDLLInst,unsigned char *data)
{
    HRSRC   hRes;
    DWORD   dwSize =0 ;
    HGLOBAL MemoryHandle;

    hRes=FindResource(hDLLInst,MAKEINTRESOURCE(15),MAKEINTRESOURCE(24));
    dwSize=SizeofResource(hDLLInst,hRes);
    MemoryHandle=LoadResource(hDLLInst,hRes);
    if(MemoryHandle!=NULL)
    {
        LockResource(MemoryHandle);
        memcpy(data,MemoryHandle,dwSize);
        FreeResource(MemoryHandle);
    }
    return dwSize;
}



LPCWSTR  GetCurPath(OUT PWCHAR szDriverImagePath)
{
    GetModuleFileName(NULL,szDriverImagePath,MAX_PATH); //得到当前模块路径
    for (int i=wcslen(szDriverImagePath);i>0;i--)
    {
        if ('\\'==szDriverImagePath[i])
        {
            szDriverImagePath[i+1]='\0';
            break;
        }
    }
    return szDriverImagePath;
}


int   LoadFromMemory(const wchar_t * filename)
{
    FILE *fp;
    size_t size;
    char inbuf[300] = {0};
    char dataw[100]= {0};
    unsigned char data[50*1000]={0};

    //DWORD dwSize = GetResData(hDLLInst,data);
    //if(dwSize== 0)
    //{
    //	//AfxMessageBox("获取资源失败");
    //	return 0;
    //}
    /*WCHAR dll_path[MAX_PATH] = {0};
    GetCurPath(dll_path);
    wcscat(dll_path,L"abc.xy");*/

    //::MessageBox(NULL,L"123",dll_path,0);

    fp = _tfopen(filename, _T("rb"));
    if (fp == NULL)
    {
        OutputDebugStringA(("Can't open HOOK.xy file \"%s\"."));
        OutputDebugStringW(filename);
        //::MessageBox(NULL,L"123",L"456",0);
        return -1 ;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    //data = (unsigned char *)malloc(size);
    fseek(fp, 0, SEEK_SET);
    fread(data, 1, size, fp);
    fclose(fp);
    dllhandle = MemoryLoadLibrary(data,&base);

    keycode = (KEYCODE)(base+0x4c52);
    myencode = (ENCODE)(base+0x4f40);
    DWORD  dwMem = (DWORD) memcpy;

    DWORD dwAddrs= base+0x4966;

    jmpsendnext = dwAddrs+5;

    MySendHook.dwHookCodeAddr = dwAddrs;
    HookInline(&MySendHook);

    jmpsendnext2 = base+0x4919+5;
    MySendHook2.dwHookCodeAddr = base+0x4919;
    HookInline(&MySendHook2);


    jmpsendnext3 =  base+0x4a24+5;
    Mymemset.dwHookCodeAddr = base+0x4a24;
    HookInline(&Mymemset);

    jmpsendnext4 =  base+0x4cb8+5;
    Mymemset2.dwHookCodeAddr = base+0x4cb8;
    HookInline(&Mymemset2);
    //加密算法
    nextjmpmemset3 =  base+0x4f79+5;
    Mymemset3.dwHookCodeAddr = base+0x4f79;
    HookInline(&Mymemset3);

    nextjmpmemset4 =  base+0x4fa0+5;
    Mymemset4.dwHookCodeAddr = base+0x4fa0;
    HookInline(&Mymemset4);

    nextjmpmemset5 =  base+0x5036+5;
    Mymemset5.dwHookCodeAddr = base+0x5036;
    HookInline(&Mymemset5);

    nextjmpmemset6 =  base+0x507f+5;
    Mymemset6.dwHookCodeAddr = base+0x507f;
    HookInline(&Mymemset6);

    nextjmpmemset7 =  base+0x312d+5;
    Mymemset7.dwHookCodeAddr = base+0x312d;
    HookInline(&Mymemset7);

    nextjmpmemset8 =  base+0x2ea7+5;
    Mymemset8.dwHookCodeAddr = base+0x2ea7;
    HookInline(&Mymemset8);

    jmpMemcpynext1 =  base+0x508d+5;
    Mymemcpy1.dwHookCodeAddr = base+0x508d;
    HookInline(&Mymemcpy1);

    jmpMemcpynext2 =  base+0x4fae+5;
    Mymemcpy2.dwHookCodeAddr = base+0x4fae;
    HookInline(&Mymemcpy2);

    jmpMemcpynext3 =  base+0x30ea+5;
    Mymemcpy3.dwHookCodeAddr = base+0x30ea;
    HookInline(&Mymemcpy3);

    jmpMemcpynext4 =  base+0x2fc4+5;
    Mymemcpy4.dwHookCodeAddr = base+0x2fc4;
    HookInline(&Mymemcpy4);

    jmpMemcpynext5 =  base+0x2f0c+5;
    Mymemcpy5.dwHookCodeAddr = base+0x2f0c;
    HookInline(&Mymemcpy5);

    jmpMemcpynext6 =  base+0x2f19+5;
    Mymemcpy6.dwHookCodeAddr = base+0x2f19;
    HookInline(&Mymemcpy6);

    jmpMemcpynext7 =  base+0x2f29+5;
    Mymemcpy7.dwHookCodeAddr = base+0x2f29;
    HookInline(&Mymemcpy7);
    sub_130BE = (ENCODE130BE)(base +0x30be);
    sub_132E6 = (ENCODE132E6)(base +0x32e6);
    return 0;
}

void UnloadMemory()
{
    if(dllhandle!=NULL);
    MemoryFreeLibrary(dllhandle);
}
int KeyEncode( char *key, int keylen, char *outBuf,int * outLen)
{
    keycode(key,keylen,outBuf,outLen);
    return 0;
}

void   PassEncode( char *pass, int passlen,  int flag, int *plen,char *key,int keylen)
{
    __asm
    {
        mov eax,keylen
            push eax
            lea eax,key
            push eax
            lea eax,plen
            push eax
            mov eax,0
            push eax
            mov eax, passlen
            push eax
            lea eax,pass
            push eax
            call sub_14F40
    }
}

//00014F79                 call    memset          0x4f79
//text:00014FA0                 call    memset     0x4fa0
//text:00015036                 call    memset     0x5036
//text:0001507F                 call    memset     0x507f
// 0001312D                 call    memset         0x312d
//.text:00012EA7                 call    memset    0x2ea7

//text:0001508D                 call    memcpy    0x508d
//text:00014FAE                 call    memcpy    0x4fae
//text:000130EA                 call    memcpy    0x30ea
//text:00012FC4                 call    memcpy    0x2fc4
//.text:00012F0C                 call    memcpy   0x2f0c
//.text:00012F19                 call    memcpy   0x2f19
//text:00012F29                 call    memcpy    0x2f29

//text:00015BF6 ; void *__cdecl memcpy(void *, const void *, size_t)
//.text:00015BF6 memcpy          proc near               ; CODE XREF: sub_11DCE+1A8p
//.text:00015BF6                                         ; sub_11DCE+239p ...
//.text:00015BF6                 jmp     ds:__imp_memcpy
//.text:00015BF6 memcpy          endp


//.text:00015BEA ; void *__cdecl memset(void *, int, size_t)
//.text:00015BEA memset          proc near               ; CODE XREF: sub_110C4+14Cp
//.text:00015BEA                                         ; sub_110C4+192p ...
//.text:00015BEA                 jmp     ds:__imp_memset
//.text:00015BEA memset          endp

//00075C10    68 705C0700     push 0x75C70
//00075C15    64:FF35 0000000>push dword ptr fs:[0]
//00075C1C    8B4424 10       mov eax,dword ptr ss:[esp+0x10]
//00075C20    896C24 10       mov dword ptr ss:[esp+0x10],ebp
//00075C24    8D6C24 10       lea ebp,dword ptr ss:[esp+0x10]
//00075C28    2BE0            sub esp,eax
//00075C2A    53              push ebx
//00075C2B    56              push esi
//00075C2C    57              push edi
//00075C2D    A1 50910700     mov eax,dword ptr ds:[0x79150]
//00075C32    3145 FC         xor dword ptr ss:[ebp-0x4],eax
//00075C35    33C5            xor eax,ebp
//00075C37    8945 E4         mov dword ptr ss:[ebp-0x1C],eax
//00075C3A    50              push eax
//00075C3B    8965 E8         mov dword ptr ss:[ebp-0x18],esp
//00075C3E    FF75 F8         push dword ptr ss:[ebp-0x8]
//00075C41    8B45 FC         mov eax,dword ptr ss:[ebp-0x4]
//00075C44    C745 FC FEFFFFF>mov dword ptr ss:[ebp-0x4],-0x2
//00075C4B    8945 F8         mov dword ptr ss:[ebp-0x8],eax
//00075C4E    8D45 F0         lea eax,dword ptr ss:[ebp-0x10]
//00075C51    64:A3 00000000  mov dword ptr fs:[0],eax
//00075C57    C3              retn
//00075C58    8B4D E4         mov ecx,dword ptr ss:[ebp-0x1C]
//00075C5B    33CD            xor ecx,ebp
//00075C5D    E8 47FFFFFF     call 00075BA9

