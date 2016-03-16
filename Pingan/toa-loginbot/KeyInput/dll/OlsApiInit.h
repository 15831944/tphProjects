//-----------------------------------------------------------------------------
//     Author : hiyohiyo
//       Mail : hiyohiyo@crystalmark.info
//        Web : http://openlibsys.org/
//    License : The modified BSD license
//
//                     Copyright 2007-2008 OpenLibSys.org. All rights reserved.
//-----------------------------------------------------------------------------

#pragma once

#include "OlsDef.h"
#include "OlsApiInitDef.h"

//-----------------------------------------------------------------------------
//
// Prototypes
//
//-----------------------------------------------------------------------------

BOOL InitOpenLibSys(HMODULE *hModule);
BOOL DeinitOpenLibSys(HMODULE *hModule);

//-----------------------------------------------------------------------------
//
// Funtions
//
//-----------------------------------------------------------------------------

// DLL
_GetDllStatus GetDllStatus = NULL;
_GetDllVersion GetDllVersion = NULL;
_GetDriverVersion GetDriverVersion = NULL;
_GetDriverType GetDriverType = NULL;

_InitializeOls InitializeOls = NULL;
_DeinitializeOls DeinitializeOls = NULL;

// CPU
_IsCpuid IsCpuid = NULL;
_IsMsr IsMsr = NULL;
_IsTsc IsTsc = NULL;

_Hlt Hlt = NULL;
_Rdmsr Rdmsr = NULL;
_Wrmsr Wrmsr = NULL;
_Rdpmc Rdpmc = NULL;
_Cpuid Cpuid = NULL;
_Rdtsc Rdtsc = NULL;

_HltEx HltEx = NULL;
_RdmsrEx RdmsrEx = NULL;
_WrmsrEx WrmsrEx = NULL;
_RdpmcEx RdpmcEx = NULL;
_CpuidEx CpuidEx = NULL;
_RdtscEx RdtscEx = NULL;

// I/O
_ReadIoPortByte ReadIoPortByte = NULL;
_ReadIoPortWord ReadIoPortWord = NULL;
_ReadIoPortDword ReadIoPortDword = NULL;

_ReadIoPortByteEx ReadIoPortByteEx = NULL;
_ReadIoPortWordEx ReadIoPortWordEx = NULL;
_ReadIoPortDwordEx ReadIoPortDwordEx = NULL;

_WriteIoPortByte WriteIoPortByte = NULL;
_WriteIoPortWord WriteIoPortWord = NULL;
_WriteIoPortDword WriteIoPortDword = NULL;

_WriteIoPortByteEx WriteIoPortByteEx = NULL;
_WriteIoPortWordEx WriteIoPortWordEx = NULL;
_WriteIoPortDwordEx WriteIoPortDwordEx = NULL;

// PCI
_SetPciMaxBusIndex SetPciMaxBusIndex = NULL;

_ReadPciConfigByte ReadPciConfigByte = NULL;
_ReadPciConfigWord ReadPciConfigWord = NULL;
_ReadPciConfigDword ReadPciConfigDword = NULL;

_ReadPciConfigByteEx ReadPciConfigByteEx = NULL;
_ReadPciConfigWordEx ReadPciConfigWordEx = NULL;
_ReadPciConfigDwordEx ReadPciConfigDwordEx = NULL;

_WritePciConfigByte WritePciConfigByte = NULL;
_WritePciConfigWord WritePciConfigWord = NULL;
_WritePciConfigDword WritePciConfigDword = NULL;

_WritePciConfigByteEx WritePciConfigByteEx = NULL;
_WritePciConfigWordEx WritePciConfigWordEx = NULL;
_WritePciConfigDwordEx WritePciConfigDwordEx = NULL;

_FindPciDeviceById FindPciDeviceById = NULL;
_FindPciDeviceByClass FindPciDeviceByClass = NULL;

//-----------------------------------------------------------------------------
//
// Initialize
//
//-----------------------------------------------------------------------------

BOOL InitOpenLibSys(HMODULE *hModule)
{
#ifdef _M_X64
	*hModule = LoadLibrary(_T("WinRing0x64.dll"));
#else
	*hModule = LoadLibrary(_T("WinRing0.dll"));
#endif

	if(*hModule == NULL)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------------
	// GetProcAddress
	//-----------------------------------------------------------------------------
	// DLL
	GetDllStatus =			(_GetDllStatus)			GetProcAddress (*hModule, "GetDllStatus");
	GetDllVersion =			(_GetDllVersion)		GetProcAddress (*hModule, "GetDllVersion");
	GetDriverVersion =		(_GetDriverVersion)		GetProcAddress (*hModule, "GetDriverVersion");
	GetDriverType =			(_GetDriverType)		GetProcAddress (*hModule, "GetDriverType");

	InitializeOls =			(_InitializeOls)		GetProcAddress (*hModule, "InitializeOls");
	DeinitializeOls =		(_DeinitializeOls)		GetProcAddress (*hModule, "DeinitializeOls");

	// CPU
	IsCpuid =				(_IsCpuid)				GetProcAddress (*hModule, "IsCpuid");
	IsMsr =					(_IsMsr)				GetProcAddress (*hModule, "IsMsr");
	IsTsc =					(_IsTsc)				GetProcAddress (*hModule, "IsTsc");
	Hlt =					(_Hlt)					GetProcAddress (*hModule, "Hlt");
	Rdmsr =					(_Rdmsr)				GetProcAddress (*hModule, "Rdmsr");
	Wrmsr =					(_Wrmsr)				GetProcAddress (*hModule, "Wrmsr");
	Rdpmc =					(_Rdpmc)				GetProcAddress (*hModule, "Rdpmc");
	Cpuid =					(_Cpuid)				GetProcAddress (*hModule, "Cpuid");
	Rdtsc =					(_Rdtsc)				GetProcAddress (*hModule, "Rdtsc");
	HltEx =					(_HltEx)				GetProcAddress (*hModule, "HltEx");
	RdmsrEx =				(_RdmsrEx)				GetProcAddress (*hModule, "RdmsrEx");
	WrmsrEx =				(_WrmsrEx)				GetProcAddress (*hModule, "WrmsrEx");
	RdpmcEx =				(_RdpmcEx)				GetProcAddress (*hModule, "RdpmcEx");
	CpuidEx =				(_CpuidEx)				GetProcAddress (*hModule, "CpuidEx");
	RdtscEx =				(_RdtscEx)				GetProcAddress (*hModule, "RdtscEx");

	// I/O
	ReadIoPortByte =		(_ReadIoPortByte)		GetProcAddress (*hModule, "ReadIoPortByte");
	ReadIoPortWord =		(_ReadIoPortWord)		GetProcAddress (*hModule, "ReadIoPortWord");
	ReadIoPortDword =		(_ReadIoPortDword)		GetProcAddress (*hModule, "ReadIoPortDword");

	ReadIoPortByteEx =		(_ReadIoPortByteEx)		GetProcAddress (*hModule, "ReadIoPortByteEx");
	ReadIoPortWordEx =		(_ReadIoPortWordEx)		GetProcAddress (*hModule, "ReadIoPortWordEx");
	ReadIoPortDwordEx =		(_ReadIoPortDwordEx)	GetProcAddress (*hModule, "ReadIoPortDwordEx");

	WriteIoPortByte =		(_WriteIoPortByte)		GetProcAddress (*hModule, "WriteIoPortByte");
	WriteIoPortWord =		(_WriteIoPortWord)		GetProcAddress (*hModule, "WriteIoPortWord");
	WriteIoPortDword =		(_WriteIoPortDword)		GetProcAddress (*hModule, "WriteIoPortDword");

	WriteIoPortByteEx =		(_WriteIoPortByteEx)	GetProcAddress (*hModule, "WriteIoPortByteEx");
	WriteIoPortWordEx =		(_WriteIoPortWordEx)	GetProcAddress (*hModule, "WriteIoPortWordEx");
	WriteIoPortDwordEx =	(_WriteIoPortDwordEx)	GetProcAddress (*hModule, "WriteIoPortDwordEx");

	// PCI
	SetPciMaxBusIndex =		(_SetPciMaxBusIndex)	GetProcAddress (*hModule, "SetPciMaxBusIndex");

	ReadPciConfigByte =		(_ReadPciConfigByte)	GetProcAddress (*hModule, "ReadPciConfigByte");
	ReadPciConfigWord =		(_ReadPciConfigWord)	GetProcAddress (*hModule, "ReadPciConfigWord");
	ReadPciConfigDword =	(_ReadPciConfigDword)	GetProcAddress (*hModule, "ReadPciConfigDword");

	ReadPciConfigByteEx =	(_ReadPciConfigByteEx)	GetProcAddress (*hModule, "ReadPciConfigByteEx");
	ReadPciConfigWordEx =	(_ReadPciConfigWordEx)	GetProcAddress (*hModule, "ReadPciConfigWordEx");
	ReadPciConfigDwordEx =	(_ReadPciConfigDwordEx)	GetProcAddress (*hModule, "ReadPciConfigDwordEx");

	WritePciConfigByte =	(_WritePciConfigByte)	GetProcAddress (*hModule, "WritePciConfigByte");
	WritePciConfigWord =	(_WritePciConfigWord)	GetProcAddress (*hModule, "WritePciConfigWord");
	WritePciConfigDword =	(_WritePciConfigDword)	GetProcAddress (*hModule, "WritePciConfigDword");

	WritePciConfigByteEx =	(_WritePciConfigByteEx)	GetProcAddress (*hModule, "WritePciConfigByteEx");
	WritePciConfigWordEx =	(_WritePciConfigWordEx)	GetProcAddress (*hModule, "WritePciConfigWordEx");
	WritePciConfigDwordEx =	(_WritePciConfigDwordEx)GetProcAddress (*hModule, "WritePciConfigDwordEx");

	FindPciDeviceById =		(_FindPciDeviceById)	GetProcAddress (*hModule, "FindPciDeviceById");
	FindPciDeviceByClass =	(_FindPciDeviceByClass)	GetProcAddress (*hModule, "FindPciDeviceByClass");

	//-----------------------------------------------------------------------------
	// Check Functions
	//-----------------------------------------------------------------------------
	if(!(
		GetDllStatus
	&&	GetDllVersion
	&&	GetDriverVersion
	&&	GetDriverType
	&&	InitializeOls
	&&	DeinitializeOls
	&&	IsCpuid
	&&	IsMsr
	&&	IsTsc
	&&	Hlt
	&&	HltEx
	&&	Rdmsr
	&&	RdmsrEx
	&&	Wrmsr
	&&	WrmsrEx
	&&	Rdpmc
	&&	RdpmcEx
	&&	Cpuid
	&&	CpuidEx
	&&	Rdtsc
	&&	RdtscEx
	&&	ReadIoPortByte
	&&	ReadIoPortWord
	&&	ReadIoPortDword
	&&	ReadIoPortByteEx
	&&	ReadIoPortWordEx
	&&	ReadIoPortDwordEx
	&&	WriteIoPortByte
	&&	WriteIoPortWord
	&&	WriteIoPortDword
	&&	WriteIoPortByteEx
	&&	WriteIoPortWordEx
	&&	WriteIoPortDwordEx
	&&	SetPciMaxBusIndex
	&&	ReadPciConfigByte
	&&	ReadPciConfigWord
	&&	ReadPciConfigDword
	&&	ReadPciConfigByteEx
	&&	ReadPciConfigWordEx
	&&	ReadPciConfigDwordEx
	&&	WritePciConfigByte
	&&	WritePciConfigWord 
	&&	WritePciConfigDword
	&&	WritePciConfigByteEx
	&&	WritePciConfigWordEx 
	&&	WritePciConfigDwordEx
	&&	FindPciDeviceById
	&&	FindPciDeviceByClass
	))
	{
		return FALSE;
	}

	if(InitializeOls() != OLS_DLL_NO_ERROR)
	{
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
//
// Deinitialize
//
//-----------------------------------------------------------------------------

BOOL DeinitOpenLibSys(HMODULE *hModule)
{
	BOOL result = FALSE;

	if(*hModule == NULL)
	{
		return TRUE;
	}
	else
	{
		DeinitializeOls();
		result = FreeLibrary(*hModule);
		*hModule = NULL;

		return result;
	}
}
