//-----------------------------------------------------------------------------
//     Author : hiyohiyo
//       Mail : hiyohiyo@crystalmark.info
//        Web : http://openlibsys.org/
//    License : The modified BSD license
//
//                     Copyright 2007-2008 OpenLibSys.org. All rights reserved.
//-----------------------------------------------------------------------------

#pragma once

#include "OlsApiInitDef.h"

//-----------------------------------------------------------------------------
//
// Externs
//
//-----------------------------------------------------------------------------

// DLL
extern _GetDllStatus GetDllStatus;
extern _GetDllVersion GetDllVersion;
extern _GetDriverVersion GetDriverVersion;
extern _GetDriverType GetDriverType;

extern _InitializeOls InitializeOls;
extern _DeinitializeOls DeinitializeOls;

// CPU
extern _IsCpuid IsCpuid;
extern _IsMsr IsMsr;
extern _IsTsc IsTsc;

extern _Hlt Hlt;
extern _Rdmsr Rdmsr;
extern _Wrmsr Wrmsr;
extern _Rdpmc Rdpmc;
extern _Cpuid Cpuid;
extern _Rdtsc Rdtsc;

extern _HltEx HltEx;
extern _RdmsrEx RdmsrEx;
extern _WrmsrEx WrmsrEx;
extern _RdpmcEx RdpmcEx;
extern _CpuidEx CpuidEx;
extern _RdtscEx RdtscEx;

// I/O
extern _ReadIoPortByte ReadIoPortByte;
extern _ReadIoPortWord ReadIoPortWord;
extern _ReadIoPortDword ReadIoPortDword;

extern _ReadIoPortByteEx ReadIoPortByteEx;
extern _ReadIoPortWordEx ReadIoPortWordEx;
extern _ReadIoPortDwordEx ReadIoPortDwordEx;

extern _WriteIoPortByte WriteIoPortByte;
extern _WriteIoPortWord WriteIoPortWord;
extern _WriteIoPortDword WriteIoPortDword;

extern _WriteIoPortByteEx WriteIoPortByteEx;
extern _WriteIoPortWordEx WriteIoPortWordEx;
extern _WriteIoPortDwordEx WriteIoPortDwordEx;

// PCI
extern _SetPciMaxBusIndex SetPciMaxBusIndex;

extern _ReadPciConfigByte ReadPciConfigByte;
extern _ReadPciConfigWord ReadPciConfigWord;
extern _ReadPciConfigDword ReadPciConfigDword;

extern _ReadPciConfigByteEx ReadPciConfigByteEx;
extern _ReadPciConfigWordEx ReadPciConfigWordEx;
extern _ReadPciConfigDwordEx ReadPciConfigDwordEx;

extern _WritePciConfigByte WritePciConfigByte;
extern _WritePciConfigWord WritePciConfigWord;
extern _WritePciConfigDword WritePciConfigDword;

extern _WritePciConfigByteEx WritePciConfigByteEx;
extern _WritePciConfigWordEx WritePciConfigWordEx;
extern _WritePciConfigDwordEx WritePciConfigDwordEx;

extern _FindPciDeviceById FindPciDeviceById;
extern _FindPciDeviceByClass FindPciDeviceByClass;
