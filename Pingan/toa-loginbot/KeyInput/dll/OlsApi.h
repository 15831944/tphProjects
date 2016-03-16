//-----------------------------------------------------------------------------
//     Author : hiyohiyo
//       Mail : hiyohiyo@crystalmark.info
//        Web : http://openlibsys.org/
//    License : The modified BSD license
//
//                     Copyright 2007-2008 OpenLibSys.org. All rights reserved.
//-----------------------------------------------------------------------------

#pragma once

/******************************************************************************
**
** DLL Information
**
******************************************************************************/

//-----------------------------------------------------------------------------
// GetDllStatus
//-----------------------------------------------------------------------------
DWORD // DLL Status, defined OLS_DLL_****
WINAPI GetDllStatus();

//-----------------------------------------------------------------------------
// GetDllVersion
//-----------------------------------------------------------------------------
DWORD // DLL Version, defined OLS_VERSION
WINAPI GetDllVersion(
	PBYTE major,		// major version
	PBYTE minor,		// minor version
	PBYTE revision,		// revision
	PBYTE release		// release/build
);

//-----------------------------------------------------------------------------
// GetDriverVersion
//-----------------------------------------------------------------------------
DWORD // Device Driver Version, defined OLS_DRIVER_VERSION
WINAPI GetDriverVersion(
	PBYTE major,		// major version
	PBYTE minor,		// minor version
	PBYTE revision,		// revision
	PBYTE release		// release/build
);

//-----------------------------------------------------------------------------
// GetDriverType
//-----------------------------------------------------------------------------
DWORD // Device Driver Type, defined OLS_DRIVER_TYPE_****
WINAPI GetDriverType();

//-----------------------------------------------------------------------------
// InitializeOls
//-----------------------------------------------------------------------------
DWORD // returns error code
WINAPI InitializeOls();

//-----------------------------------------------------------------------------
// DeinitializeOls
//-----------------------------------------------------------------------------
VOID WINAPI DeinitializeOls();

/******************************************************************************
**
** CPU
**
******************************************************************************/

//-----------------------------------------------------------------------------
// IsCpuid
//-----------------------------------------------------------------------------
BOOL // TRUE: support CPUID instruction, FALSE: not support CPUID instruction
WINAPI IsCpuid();

//-----------------------------------------------------------------------------
// IsMsr
//-----------------------------------------------------------------------------
BOOL // TRUE: support MSR(Model-Specific Register), FALSE: not support MSR
WINAPI IsMsr();

//-----------------------------------------------------------------------------
// IsTsc
//-----------------------------------------------------------------------------
BOOL // TRUE: support TSC(Time Stamp Counter), FALSE: not support TSC
WINAPI IsTsc();

//-----------------------------------------------------------------------------
// Rdmsr
//-----------------------------------------------------------------------------
BOOL // TRUE: success, FALSE: failure
WINAPI Rdmsr(
	DWORD index,		// MSR index
	PDWORD eax,			// bit  0-31
	PDWORD edx			// bit 32-63
);

//-----------------------------------------------------------------------------
// RdmsrEx
//-----------------------------------------------------------------------------
BOOL // TRUE: success, FALSE: failure
WINAPI RdmsrEx(
	DWORD index,		// MSR index
	PDWORD eax,			// bit  0-31
	PDWORD edx,			// bit 32-63
	DWORD_PTR affinityMask	// Thread Affinity Mask
);

//-----------------------------------------------------------------------------
// Wrmsr
//-----------------------------------------------------------------------------
BOOL // TRUE: success, FALSE: failure
WINAPI Wrmsr(
	DWORD index,		// MSR index
	DWORD eax,			// bit  0-31
	DWORD edx			// bit 32-63
);

//-----------------------------------------------------------------------------
// WrmsrEx
//-----------------------------------------------------------------------------
BOOL // TRUE: success, FALSE: failure
WINAPI WrmsrEx(
	DWORD index,		// MSR index
	DWORD eax,			// bit  0-31
	DWORD edx,			// bit 32-63
	DWORD affinityMask	// Thread Affinity Mask
);

//-----------------------------------------------------------------------------
// Rdpmc
//-----------------------------------------------------------------------------
BOOL // TRUE: success, FALSE: failure
WINAPI Rdpmc(
	DWORD index,		// PMC index
	PDWORD eax,			// bit  0-31
	PDWORD edx			// bit 32-63
);

//-----------------------------------------------------------------------------
// RdmsrEx
//-----------------------------------------------------------------------------
BOOL // TRUE: success, FALSE: failure
WINAPI RdpmcEx(
	DWORD index,			// PMC index
	PDWORD eax,				// bit  0-31
	PDWORD edx,				// bit 32-63
	DWORD_PTR affinityMask	// Thread Affinity Mask
);

//-----------------------------------------------------------------------------
// Cpuid
//-----------------------------------------------------------------------------
BOOL // TRUE: success, FALSE: failure
WINAPI Cpuid(
	DWORD index,		// CPUID index
	PDWORD eax,			// EAX
	PDWORD ebx,			// EBX
	PDWORD ecx,			// ECX
	PDWORD edx			// EDX
);

//-----------------------------------------------------------------------------
// CpuidEx
//-----------------------------------------------------------------------------
BOOL // TRUE: success, FALSE: failure
WINAPI CpuidEx(
	DWORD index,			// CPUID index
	PDWORD eax,				// EAX
	PDWORD ebx,				// EBX
	PDWORD ecx,				// ECX
	PDWORD edx,				// EDX
	DWORD_PTR affinityMask	// Thread Affinity Mask
);

//-----------------------------------------------------------------------------
// Rdtsc
//-----------------------------------------------------------------------------
BOOL // TRUE: success, FALSE: failure
WINAPI Rdtsc(
	PDWORD eax,			// bit  0-31
	PDWORD edx			// bit 32-63
);

//-----------------------------------------------------------------------------
// RdmsrEx
//-----------------------------------------------------------------------------
BOOL // TRUE: success, FALSE: failure
WINAPI RdtscEx(
	PDWORD eax,				// bit  0-31
	PDWORD edx,				// bit 32-63
	DWORD_PTR affinityMask	// Thread Affinity Mask
);

//-----------------------------------------------------------------------------
// Hlt
//-----------------------------------------------------------------------------
BOOL // TRUE: success, FALSE: failure
WINAPI Hlt();

//-----------------------------------------------------------------------------
// HltEx
//-----------------------------------------------------------------------------
BOOL // TRUE: success, FALSE: failure
WINAPI HltEx(
	DWORD_PTR affinityMask
);

/******************************************************************************
**
** I/O
**
******************************************************************************/

//-----------------------------------------------------------------------------
// ReadIoPortByte
//-----------------------------------------------------------------------------
BYTE // Read Value
WINAPI ReadIoPortByte(
	WORD port			// I/O port address
);

//-----------------------------------------------------------------------------
// ReadIoPortWord
//-----------------------------------------------------------------------------
WORD // Read Value
WINAPI ReadIoPortWord(
	WORD port			// I/O port address
);

//-----------------------------------------------------------------------------
// ReadIoPortDword
//-----------------------------------------------------------------------------
DWORD // Read Value
WINAPI ReadIoPortDword(
	WORD port			// I/O port address
);

//-----------------------------------------------------------------------------
// ReadIoPortByteEx
//-----------------------------------------------------------------------------
BOOL // TRUE: success, FALSE: failure
WINAPI ReadIoPortByteEx(
	WORD port,			// I/O port address
	PBYTE value			// Read Value
);
//-----------------------------------------------------------------------------
// ReadIoPortWordEx
//-----------------------------------------------------------------------------
BOOL // TRUE: success, FALSE: failure
WINAPI ReadIoPortWordEx(
	WORD port,			// I/O port address
	PWORD value			// Read Value
);
//-----------------------------------------------------------------------------
// ReadIoPortDwordEx
//-----------------------------------------------------------------------------
BOOL // TRUE: success, FALSE: failure
WINAPI ReadIoPortDwordEx(
	WORD port,			// I/O port address
	PDWORD value		// Read Value
);

//-----------------------------------------------------------------------------
// WriteIoPortByte
//-----------------------------------------------------------------------------
VOID
WINAPI WriteIoPortByte(
	WORD port,			// I/O port address
	BYTE value			// Write Value
);

//-----------------------------------------------------------------------------
// WriteIoPortDword
//-----------------------------------------------------------------------------
VOID
WINAPI WriteIoPortDword(
	WORD port,			// I/O port address
	DWORD value			// Write Value
);


//-----------------------------------------------------------------------------
// WriteIoPortWord
//-----------------------------------------------------------------------------
VOID 
WINAPI WriteIoPortWord(
	WORD port,			// I/O port address
	WORD value			// Write Value
);

//-----------------------------------------------------------------------------
// WriteIoPortByteEx
//-----------------------------------------------------------------------------
BOOL // TRUE: success, FALSE: failure
WINAPI WriteIoPortByteEx(
	WORD port,			// I/O port address
	BYTE value			// Write Value
);

//-----------------------------------------------------------------------------
// WriteIoPortWordEx
//-----------------------------------------------------------------------------
BOOL // TRUE: success, FALSE: failure
WINAPI WriteIoPortWordEx(
	WORD port,			// I/O port address
	WORD value			// Write Value
);


//-----------------------------------------------------------------------------
// WriteIoPortDwordEx
//-----------------------------------------------------------------------------
BOOL // TRUE: success, FALSE: failure
WINAPI WriteIoPortDwordEx(
	WORD port,			// I/O port address
	DWORD value			// Write Value
);

/******************************************************************************
**
** PCI
**
******************************************************************************/
// pciAddress
//  0- 2: Function Number
//  3- 7: Device Number
//  8-15: PCI Bus Number
// 16-31: Reserved
// 0xFFFFFFFF : Error

//-----------------------------------------------------------------------------
// SetPciMaxBusNo
//-----------------------------------------------------------------------------
VOID
WINAPI SetPciMaxBusIndex(
	BYTE max			// Max PCI Bus to Scan
);

//-----------------------------------------------------------------------------
// ReadPciConfigByte
//-----------------------------------------------------------------------------
BYTE // Read Value
WINAPI ReadPciConfigByte(
	DWORD pciAddress,	// PCI Device Address
	BYTE regAddress		// Configuration Address 0-255
);

//-----------------------------------------------------------------------------
// ReadPciConfigWord
//-----------------------------------------------------------------------------
WORD // Read Value
WINAPI ReadPciConfigWord(
	DWORD pciAddress,	// PCI Device Address
	BYTE regAddress		// Configuration Address 0-255
);

//-----------------------------------------------------------------------------
// ReadPciConfigDword
//-----------------------------------------------------------------------------
DWORD // Read Value
WINAPI ReadPciConfigDword(
	DWORD pciAddress,	// PCI Device Address
	BYTE regAddress		// Configuration Address 0-255
);

//-----------------------------------------------------------------------------
// ReadPciConfigByteEx
//-----------------------------------------------------------------------------
BOOL // TRUE: success, FALSE: failure
WINAPI ReadPciConfigByteEx(
	DWORD pciAddress,	// PCI Device Address
	DWORD regAddress,	// Configuration Address 0-whatever
	PBYTE value			// Read Value
);

//-----------------------------------------------------------------------------
// ReadPciConfigWordEx
//-----------------------------------------------------------------------------
BOOL // TRUE: success, FALSE: failure
WINAPI ReadPciConfigWordEx(
	DWORD pciAddress,	// PCI Device Address
	DWORD regAddress,	// Configuration Address 0-whatever
	PWORD value			// Read Value
);

//-----------------------------------------------------------------------------
// ReadPciConfigDwordEx
//-----------------------------------------------------------------------------
BOOL // TRUE: success, FALSE: failure
WINAPI ReadPciConfigDwordEx(
	DWORD pciAddress,	// PCI Device Address
	DWORD regAddress,	// Configuration Address 0-whatever
	PDWORD value		// Read Value
);

//-----------------------------------------------------------------------------
// WritePciConfigByte
//-----------------------------------------------------------------------------
VOID
WINAPI WritePciConfigByte(
	DWORD pciAddress,	// PCI Device Address
	BYTE regAddress,	// Configuration Address 0-255
	BYTE value			// Write Value
);

//-----------------------------------------------------------------------------
// WritePciConfigWord
//-----------------------------------------------------------------------------
VOID
WINAPI WritePciConfigWord(
	DWORD pciAddress,	// PCI Device Address
	BYTE regAddress,	// Configuration Address 0-255
	WORD value			// Write Value
);

//-----------------------------------------------------------------------------
// WritePciConfigDword
//-----------------------------------------------------------------------------
VOID
WINAPI WritePciConfigDword(
	DWORD pciAddress,	// PCI Device Address
	BYTE regAddress,	// Configuration Address 0-255
	DWORD value			// Write Value
);

//-----------------------------------------------------------------------------
// WritePciConfigByteEx
//-----------------------------------------------------------------------------
BOOL // TRUE: success, FALSE: failure
WINAPI WritePciConfigByteEx(
	DWORD pciAddress,	// PCI Device Address
	DWORD regAddress,	// Configuration Address 0-whatever
	BYTE value			// Write Value
);

//-----------------------------------------------------------------------------
// WritePciConfigWordEx
//-----------------------------------------------------------------------------
BOOL // TRUE: success, FALSE: failure
WINAPI WritePciConfigWordEx(
	DWORD pciAddress,	// PCI Device Address
	DWORD regAddress,	// Configuration Address 0-whatever
	WORD value			// Write Value
);

//-----------------------------------------------------------------------------
// WritePciConfigDwordEx
//-----------------------------------------------------------------------------
BOOL // TRUE: success, FALSE: failure
WINAPI WritePciConfigDwordEx(
	DWORD pciAddress,	// PCI Device Address
	DWORD regAddress,	// Configuration Address 0-whatever
	DWORD value			// Write Value
);

//-----------------------------------------------------------------------------
// FindPciDeviceById
//-----------------------------------------------------------------------------
DWORD // pciAddress, 0xFFFFFFFF: failure
WINAPI FindPciDeviceById(
	WORD vendorId,		// Vendor ID
	WORD deviceId,		// Device ID
	BYTE index			// Index
);

//-----------------------------------------------------------------------------
// FindPciDeviceByClass
//-----------------------------------------------------------------------------
DWORD // pciAddress, 0xFFFFFFFF: failure
WINAPI FindPciDeviceByClass(
	BYTE baseClass,		// Base Class
	BYTE subClass,		// Sub Class
	BYTE programIf,		// Program Interface
	BYTE index			// Index
);
