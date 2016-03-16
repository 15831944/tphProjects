//-----------------------------------------------------------------------------
//     Author : hiyohiyo
//       Mail : hiyohiyo@crystalmark.info
//        Web : http://openlibsys.org/
//    License : The modified BSD license
//
//                     Copyright 2007-2008 OpenLibSys.org. All rights reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include <tchar.h>
#include <winioctl.h>
#include <stddef.h>
#include <malloc.h>
#include <intrin.h>
#include "OlsDll.h"
#include "OlsApi.h"
#include "OlsDef.h"
#include "Driver.h"
#include "OlsIoctl.h"

//-----------------------------------------------------------------------------
//
// Defines
//
//-----------------------------------------------------------------------------

const BYTE gPciNumberOfDevice =	32;
const BYTE gPciNumberOfFunction = 8;

//-----------------------------------------------------------------------------
//
// Global
//
//-----------------------------------------------------------------------------

BYTE gPciMaxNumberOfBus = 128;

extern HANDLE gHandle;

extern BOOL gIsNT;
extern BOOL gIsCpuid;
extern BOOL gIsMsr;
extern BOOL gIsTsc;
extern BOOL gInitDll;

extern DWORD gDllStatus;
extern DWORD gDriverType;
extern TCHAR gDriverPath[MAX_PATH];

//-----------------------------------------------------------------------------
//
// DLL
//
//-----------------------------------------------------------------------------

DWORD WINAPI GetDllStatus()
{
	if(gDllStatus == OLS_DLL_NO_ERROR
	&& gDriverType >= OLS_DRIVER_TYPE_WIN_NT
	&& GetDriverVersion(NULL, NULL, NULL, NULL) == 0
	)
	{
		gDllStatus = OLS_DLL_DRIVER_UNLOADED;
	}

	return gDllStatus;
}

DWORD WINAPI GetDllVersion(PBYTE major, PBYTE minor, PBYTE revision, PBYTE release)
{
	if(major != NULL && minor != NULL && revision != NULL && release != NULL)
	{
		*major = OLS_MAJOR_VERSION;
		*minor = OLS_MINOR_VERSION;
		*revision = OLS_REVISION;
		*release = OLS_RELESE;
	}

	return OLS_VERSION;
}

DWORD WINAPI GetDriverVersion(PBYTE major, PBYTE minor, PBYTE revision, PBYTE release)
{
	if(gHandle == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	DWORD	returnedLength = 0;
	BOOL	result = FALSE;
	DWORD	value = 0;

	result = DeviceIoControl(
                        gHandle,
                        IOCTL_OLS_GET_DRIVER_VERSION,
                        NULL,     
                        0,
                        &value,
                        sizeof(value),
                        &returnedLength,
                        NULL
                        );

	if(result)
	{
		if(major != NULL && minor != NULL && revision != NULL && release != NULL)
		{
			*major = (BYTE)((value >> 24) & 0xFF);
			*minor = (BYTE)((value >> 16) & 0xFF);
			*revision = (BYTE)((value >> 8) & 0xFF);
			*release = (BYTE)((value) & 0xFF);		
		}
	}
	else
	{
		value = 0;
	}
	
	return value;
}

DWORD WINAPI GetDriverType()
{
	return gDriverType;
}

//-----------------------------------------------------------------------------
//
// CPU
//
//-----------------------------------------------------------------------------

BOOL WINAPI IsCpuid()
{
	return gIsCpuid;
}

BOOL WINAPI IsMsr()
{
	return gIsMsr;
}

BOOL WINAPI IsTsc()
{
	return gIsTsc;
}

BOOL WINAPI Rdmsr(DWORD index, PDWORD eax, PDWORD edx)
{
	if(gHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	if(eax == NULL || edx == NULL || gIsMsr == FALSE)
	{
		return FALSE;
	}

	DWORD	returnedLength = 0;
 	BOOL	result = FALSE;
	BYTE	outBuf[8] = {0};

	result = DeviceIoControl(
					gHandle,
					IOCTL_OLS_READ_MSR,
					&index,
					sizeof(index),
					&outBuf,
					sizeof(outBuf),
					&returnedLength,
					NULL
					);

	if(result)
	{
		memcpy(eax, outBuf, 4);
		memcpy(edx, outBuf + 4, 4);
	}

	return result;
}

BOOL WINAPI RdmsrEx(DWORD index, PDWORD eax, PDWORD edx, DWORD_PTR affinityMask)
{
	BOOL		result = FALSE;
	DWORD_PTR	mask = 0;

	if(gIsNT)
	{
		mask = SetThreadAffinityMask(GetCurrentThread(), affinityMask);
	}

	result = Rdmsr(index, eax, edx);

	if(gIsNT && mask)
	{
		SetThreadAffinityMask(GetCurrentThread(), mask);
	}

	return result;
}

BOOL WINAPI Wrmsr(DWORD index, DWORD eax, DWORD edx)
{
	if(gHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	if(gIsMsr == FALSE)
	{
		return FALSE;
	}

	DWORD	returnedLength = 0;
 	BOOL	result = FALSE;
	DWORD	outBuf;
	OLS_WRITE_MSR_INPUT inBuf;

	inBuf.Register = index;
	inBuf.Value.HighPart = edx;
	inBuf.Value.LowPart = eax;

	result = DeviceIoControl(
					gHandle,
					IOCTL_OLS_WRITE_MSR,
					&inBuf,
					sizeof(inBuf),
					&outBuf,
					sizeof(outBuf),
					&returnedLength,
					NULL
					);

	return result;
}

BOOL WINAPI WrmsrEx(DWORD index, DWORD eax, DWORD edx, DWORD_PTR affinityMask)
{
	BOOL		result = FALSE;
	DWORD_PTR	mask = 0;

	if(gIsNT)
	{
		mask = SetThreadAffinityMask(GetCurrentThread(), affinityMask);
	}

	result = Wrmsr(index, eax, edx);

	if(gIsNT && mask)
	{
		SetThreadAffinityMask(GetCurrentThread(), mask);
	}

	return result;
}

BOOL WINAPI Rdpmc(DWORD index, PDWORD eax, PDWORD edx)
{
	if(gHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	if(eax == NULL || edx == NULL)
	{
		return FALSE;
	}

	DWORD	returnedLength = 0;
	BOOL	result = FALSE;
	BYTE	outBuf[8] = {0};

	result = DeviceIoControl(
					gHandle,
					IOCTL_OLS_READ_PMC,
					&index,	
					sizeof(index),
					&outBuf,
					sizeof(outBuf),
					&returnedLength,
					NULL
					);

	if(result)
	{
		memcpy(eax, outBuf, 4);
		memcpy(edx, outBuf + 4, 4);
	}

	return result;
}

BOOL WINAPI RdpmcEx(DWORD index, PDWORD eax, PDWORD edx, DWORD_PTR affinityMask)
{
	BOOL		result = FALSE;
	DWORD_PTR	mask = 0;

	if(gIsNT)
	{
		mask = SetThreadAffinityMask(GetCurrentThread(), affinityMask);
	}

	result = Rdpmc(index, eax, edx);

	if(gIsNT && mask)
	{
		SetThreadAffinityMask(GetCurrentThread(), mask);
	}

	return result;
}

BOOL WINAPI Cpuid(DWORD index, PDWORD eax, PDWORD ebx, PDWORD ecx, PDWORD edx)
{
	if(eax == NULL || ebx == NULL || ecx == NULL || edx == NULL || gIsCpuid == FALSE)
	{
		return FALSE;
	}

	int info[4];
	__cpuid(info, index);
	*eax = info[0];
	*ebx = info[1];
	*ecx = info[2];
	*edx = info[3];

	return TRUE;
}

BOOL WINAPI CpuidEx(DWORD index, PDWORD eax, PDWORD ebx, PDWORD ecx, PDWORD edx, DWORD_PTR affinityMask)
{
	BOOL		result = FALSE;
	DWORD_PTR	mask = 0;

	if(gIsNT)
	{
		mask = SetThreadAffinityMask(GetCurrentThread(), affinityMask);
	}

	result = Cpuid(index, eax, ebx, ecx, edx);

	if(gIsNT && mask)
	{
		SetThreadAffinityMask(GetCurrentThread(), mask);
	}

	return result;
}

BOOL WINAPI Rdtsc(PDWORD eax, PDWORD edx)
{
	if(eax == NULL || edx == NULL || gIsTsc == FALSE)
	{
		return FALSE;
	}

	ULONGLONG value = 0;
	
	value = __rdtsc();
	*eax = (DWORD)((value >>  0) & 0xFFFFFFFF);
	*edx = (DWORD)((value >> 32) & 0xFFFFFFFF);

	return TRUE;
}

BOOL WINAPI RdtscEx(PDWORD eax, PDWORD edx, DWORD_PTR affinityMask)
{
	BOOL		result = FALSE;
	DWORD_PTR	mask = 0;

	if(gIsNT)
	{
		mask = SetThreadAffinityMask(GetCurrentThread(), affinityMask);
	}

	result = Rdtsc(eax, edx);

	if(gIsNT && mask)
	{
		SetThreadAffinityMask(GetCurrentThread(), mask);
	}

	return result;
}

BOOL WINAPI Hlt()
{
	if(gHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	DWORD	returnedLength = 0;
 	BOOL	result = FALSE;

	result = DeviceIoControl(
					gHandle,
					IOCTL_OLS_HALT,	
					NULL,
					0,
					NULL,
					0,
					&returnedLength,
					NULL
					);

	return result;
}

BOOL WINAPI HltEx(DWORD_PTR affinityMask)
{
	BOOL		result = FALSE;
	DWORD_PTR	mask = 0;

	if(gIsNT)
	{
		mask = SetThreadAffinityMask(GetCurrentThread(), affinityMask);
	}

	result = Hlt();

	if(gIsNT && mask)
	{
		SetThreadAffinityMask(GetCurrentThread(), mask);
	}

	return result;
}

//-----------------------------------------------------------------------------
//
// I/O
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// I/O Read (No Error Check)
//-----------------------------------------------------------------------------

BYTE WINAPI ReadIoPortByte(WORD port)
{
	if(gHandle == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	DWORD	returnedLength = 0;
	BOOL	result = FALSE;
	WORD	value = 0;

	result = DeviceIoControl(
						gHandle,
						IOCTL_OLS_READ_IO_PORT_BYTE,
						&port,
						sizeof(port),
						&value,
						sizeof(value),
						&returnedLength,
						NULL
						);

	return (BYTE)value;
}

WORD WINAPI ReadIoPortWord(WORD port)
{
	if(gHandle == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	DWORD	returnedLength = 0;
	BOOL	result = FALSE;
	WORD	value = 0;

	result = DeviceIoControl(
						gHandle,
						IOCTL_OLS_READ_IO_PORT_WORD,
						&port,     
						sizeof(port),
						&value,
						sizeof(value),
						&returnedLength,
						NULL
						);

	return value;
}

DWORD WINAPI ReadIoPortDword(WORD port)
{
	if(gHandle == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	DWORD	returnedLength = 0;
	BOOL	result = FALSE;
	DWORD	value = port;

	result = DeviceIoControl(
						gHandle,
						IOCTL_OLS_READ_IO_PORT_DWORD,
						&value,     
						sizeof(value),	// required 4 bytes
						&value,
						sizeof(value),
						&returnedLength,
						NULL
						);

	return value;
}

//-----------------------------------------------------------------------------
// I/O Read (Error Check)
//-----------------------------------------------------------------------------

BOOL WINAPI ReadIoPortByteEx(WORD port, PBYTE value)
{
	if(gHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	DWORD	returnedLength = 0;
	BOOL	result = FALSE;
	WORD	val = 0;

	result = DeviceIoControl(
						gHandle,
						IOCTL_OLS_READ_IO_PORT_BYTE,
						&port,
						sizeof(port),
						&val,
						sizeof(val),
						&returnedLength,
						NULL
						);

	if(result)
	{
		*value = (BYTE)val;
	}

	return result;
}

BOOL WINAPI ReadIoPortWordEx(WORD port, PWORD value)
{
	if(gHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	DWORD	returnedLength = 0;
	BOOL	result = FALSE;
	WORD	val = 0;

	result = DeviceIoControl(
						gHandle,
						IOCTL_OLS_READ_IO_PORT_WORD,
						&port,     
						sizeof(port),
						&val,
						sizeof(val),
						&returnedLength,
						NULL
						);

	if(result)
	{
		*value = val;
	}

	return result;
}

BOOL WINAPI ReadIoPortDwordEx(WORD port, PDWORD value)
{
	if(gHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	DWORD	returnedLength = 0;
	BOOL	result = FALSE;
	DWORD	val = port;

	result = DeviceIoControl(
						gHandle,
						IOCTL_OLS_READ_IO_PORT_DWORD,
						&val,     
						sizeof(val),	// required 4 bytes
						&val,
						sizeof(val),
						&returnedLength,
						NULL
						);

	if(result)
	{
		*value = val;
	}

	return result;
}

//-----------------------------------------------------------------------------
// I/O Write (No Error Check)
//-----------------------------------------------------------------------------

VOID WINAPI WriteIoPortByte(WORD port, BYTE value)
{
	if(gHandle == INVALID_HANDLE_VALUE){return ;}

	DWORD	returnedLength = 0;
	BOOL	result = FALSE;
	DWORD   length = 0;
    OLS_WRITE_IO_PORT_INPUT inBuf;

	inBuf.CharData = value;
	inBuf.PortNumber = port;
	length = offsetof(OLS_WRITE_IO_PORT_INPUT, CharData) +
						 sizeof(inBuf.CharData);

	result = DeviceIoControl(
						gHandle,
						IOCTL_OLS_WRITE_IO_PORT_BYTE,
						&inBuf,
						length,
						NULL,
						0,
						&returnedLength,
						NULL
						);
}

VOID WINAPI WriteIoPortWord(WORD port, WORD value)
{
	if(gHandle == INVALID_HANDLE_VALUE){return ;}

	DWORD	returnedLength = 0;
	BOOL	result = FALSE;
	DWORD   length = 0;
    OLS_WRITE_IO_PORT_INPUT inBuf;

	inBuf.ShortData = value;
	inBuf.PortNumber = port;
	length = offsetof(OLS_WRITE_IO_PORT_INPUT, CharData) +
						 sizeof(inBuf.ShortData);

	result = DeviceIoControl(
						gHandle,
						IOCTL_OLS_WRITE_IO_PORT_WORD,
						&inBuf,
						length,
						NULL,
						0,
						&returnedLength,
						NULL
						);
}

VOID WINAPI WriteIoPortDword(WORD port, DWORD value)
{
	if(gHandle == INVALID_HANDLE_VALUE){return ;}

	DWORD	returnedLength = 0;
	BOOL	result = FALSE;
	DWORD   length = 0;
    OLS_WRITE_IO_PORT_INPUT inBuf;

	inBuf.LongData = value;
	inBuf.PortNumber = port;
	length = offsetof(OLS_WRITE_IO_PORT_INPUT, CharData) +
						 sizeof(inBuf.LongData);

	result = DeviceIoControl(
						gHandle,
						IOCTL_OLS_WRITE_IO_PORT_DWORD,
						&inBuf,
						length,
						NULL,
						0,
						&returnedLength,
						NULL
						);
}

//-----------------------------------------------------------------------------
// I/O Write (Error Check)
//-----------------------------------------------------------------------------

BOOL WINAPI WriteIoPortByteEx(WORD port, BYTE value)
{
	if(gHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	DWORD	returnedLength = 0;
	DWORD   length = 0;
    OLS_WRITE_IO_PORT_INPUT inBuf;

	inBuf.CharData = value;
	inBuf.PortNumber = port;
	length = offsetof(OLS_WRITE_IO_PORT_INPUT, CharData) +
						 sizeof(inBuf.CharData);

	return DeviceIoControl(
						gHandle,
						IOCTL_OLS_WRITE_IO_PORT_BYTE,
						&inBuf,
						length,
						NULL,
						0,
						&returnedLength,
						NULL
						);
}

BOOL WINAPI WriteIoPortWordEx(WORD port, WORD value)
{
	if(gHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	DWORD	returnedLength = 0;
	DWORD   length = 0;
    OLS_WRITE_IO_PORT_INPUT inBuf;

	inBuf.ShortData = value;
	inBuf.PortNumber = port;
	length = offsetof(OLS_WRITE_IO_PORT_INPUT, CharData) +
						 sizeof(inBuf.ShortData);

	return DeviceIoControl(
						gHandle,
						IOCTL_OLS_WRITE_IO_PORT_WORD,
						&inBuf,
						length,
						NULL,
						0,
						&returnedLength,
						NULL
						);
}

BOOL WINAPI WriteIoPortDwordEx(WORD port, DWORD value)
{
	if(gHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	DWORD	returnedLength = 0;
	BOOL	result = FALSE;
	DWORD   length = 0;
    OLS_WRITE_IO_PORT_INPUT inBuf;

	inBuf.LongData = value;
	inBuf.PortNumber = port;
	length = offsetof(OLS_WRITE_IO_PORT_INPUT, CharData) +
						 sizeof(inBuf.LongData);

	return DeviceIoControl(
						gHandle,
						IOCTL_OLS_WRITE_IO_PORT_DWORD,
						&inBuf,
						length,
						NULL,
						0,
						&returnedLength,
						NULL
						);
}

//-----------------------------------------------------------------------------
//
// PCI Configuration Read
//
//-----------------------------------------------------------------------------

BOOL pciConfigRead(DWORD pciAddress, DWORD regAddress, PBYTE value, DWORD size)
{
	if(gHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	if(value == NULL)
	{
		return FALSE;
	}
	// alignment check
	if(size == 2 && (regAddress & 1) != 0)
	{
		return FALSE;
	}
	if(size == 4 && (regAddress & 3) != 0)
	{
		return FALSE;
	}

	DWORD	returnedLength = 0;
	BOOL	result = FALSE;
	OLS_READ_PCI_CONFIG_INPUT inBuf;

	inBuf.PciAddress = pciAddress;
	inBuf.PciOffset = regAddress;

	return DeviceIoControl(
                            gHandle,
                            IOCTL_OLS_READ_PCI_CONFIG,
                            &inBuf,
                            sizeof(inBuf),
                            value,
                            size,
                            &returnedLength,
                            NULL
                            );
}

//-----------------------------------------------------------------------------
// PCI Configuration Read (No Error Check)
//-----------------------------------------------------------------------------

BYTE WINAPI ReadPciConfigByte(DWORD pciAddress, BYTE regAddress)
{
	BYTE ret;
	if(pciConfigRead(pciAddress, regAddress, (PBYTE)&ret, sizeof(ret)))
	{
		return ret;
	}
	else
	{
		return 0xFF;
	}
}

WORD WINAPI ReadPciConfigWord(DWORD pciAddress, BYTE regAddress)
{
	WORD ret;
	if(pciConfigRead(pciAddress, regAddress, (PBYTE)&ret, sizeof(ret)))
	{
		return ret;
	}
	else
	{
		return 0xFFFF;
	}
}

DWORD WINAPI ReadPciConfigDword(DWORD pciAddress, BYTE regAddress)
{
	DWORD ret;
	if(pciConfigRead(pciAddress, regAddress, (PBYTE)&ret, sizeof(ret)))
	{
		return ret;
	}
	else
	{
		return 0xFFFFFFFF;
	}
}

//-----------------------------------------------------------------------------
// PCI Configuration Read (Error Check)
//-----------------------------------------------------------------------------

BOOL WINAPI ReadPciConfigByteEx(DWORD pciAddress, DWORD regAddress, PBYTE value)
{
	return pciConfigRead(pciAddress, regAddress, (PBYTE)value, sizeof(BYTE));
}

BOOL WINAPI ReadPciConfigWordEx(DWORD pciAddress, DWORD regAddress, PWORD value)
{
	return pciConfigRead(pciAddress, regAddress, (PBYTE)value, sizeof(WORD));
}

BOOL WINAPI ReadPciConfigDwordEx(DWORD pciAddress, DWORD regAddress, PDWORD value)
{
	return pciConfigRead(pciAddress, regAddress, (PBYTE)value, sizeof(DWORD));
}

//-----------------------------------------------------------------------------
//
// PCI Configuration Write
//
//-----------------------------------------------------------------------------

BOOL pciConfigWrite(DWORD pciAddress, DWORD regAddress, PBYTE value, DWORD size)
{
	DWORD	returnedLength = 0;
	BOOL	result = FALSE;
	int		inputSize = 0;
	OLS_WRITE_PCI_CONFIG_INPUT	*inBuf;

	if(gHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	if(value == NULL)
	{
		return FALSE;
	}
	// alignment check
	if(size == 2 && (regAddress & 1) != 0)
	{
		return FALSE;
	}
	if(size == 4 && (regAddress & 3) != 0)
	{
		return FALSE;
	}

	inputSize = offsetof(OLS_WRITE_PCI_CONFIG_INPUT, Data) + size;
	inBuf = (OLS_WRITE_PCI_CONFIG_INPUT*)malloc(inputSize);
	if(inBuf == NULL)
	{
		return FALSE;
	}
	memcpy(inBuf->Data, value, size);
	inBuf->PciAddress = pciAddress;
	inBuf->PciOffset = regAddress;
	result = DeviceIoControl(
                            gHandle,
                            IOCTL_OLS_WRITE_PCI_CONFIG,
                            inBuf,
                            inputSize,
                            NULL,
                            0,
                            &returnedLength,
                            NULL
                            );
	free(inBuf);

	if(result)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//-----------------------------------------------------------------------------
// PCI Configuration Write (No Error Check)
//-----------------------------------------------------------------------------

VOID WINAPI WritePciConfigByte(DWORD pciAddress, BYTE regAddress, BYTE value)
{
	pciConfigWrite(pciAddress, regAddress, (PBYTE)&value , sizeof(value));
}

VOID WINAPI WritePciConfigWord(DWORD pciAddress, BYTE regAddress, WORD value)
{
	pciConfigWrite(pciAddress, regAddress, (PBYTE)&value , sizeof(value));
}

VOID WINAPI WritePciConfigDword(DWORD pciAddress, BYTE regAddress, DWORD value)
{
	pciConfigWrite(pciAddress, regAddress, (PBYTE)&value , sizeof(value));
}

//-----------------------------------------------------------------------------
// PCI Configuration Write (Error Check)
//-----------------------------------------------------------------------------

BOOL WINAPI WritePciConfigByteEx(DWORD pciAddress, DWORD regAddress, BYTE value)
{
	return pciConfigWrite(pciAddress, regAddress, (PBYTE)&value , sizeof(value));
}

BOOL WINAPI WritePciConfigWordEx(DWORD pciAddress, DWORD regAddress, WORD value)
{
	return pciConfigWrite(pciAddress, regAddress, (PBYTE)&value , sizeof(value));
}

BOOL WINAPI WritePciConfigDwordEx(DWORD pciAddress, DWORD regAddress, DWORD value)
{
	return pciConfigWrite(pciAddress, regAddress, (PBYTE)&value , sizeof(value));
}

//-----------------------------------------------------------------------------
//
// FindPciDevice
//
//-----------------------------------------------------------------------------

VOID WINAPI SetPciMaxBusIndex(BYTE max)
{
	gPciMaxNumberOfBus = max;
}

DWORD WINAPI FindPciDeviceById(WORD vendorId, WORD deviceId, BYTE index)
{
	if(gHandle == INVALID_HANDLE_VALUE)
	{
		return 0xFFFFFFFF;
	}

	DWORD bus = 0, dev = 0, func = 0;
	DWORD count = 0;
	DWORD pciAddress = 0xFFFFFFFF;
	DWORD id = 0;
	DWORD ret = 0;	
	BOOL multiFuncFlag = FALSE;
	BYTE type = 0;
	count = 0;

	if(vendorId == 0xFFFF)
	{
		return 0xFFFFFFFF;
	}

	for(bus = 0; bus <= gPciMaxNumberOfBus; bus++)
	{
		for(dev = 0; dev < gPciNumberOfDevice; dev++)
		{
			multiFuncFlag = FALSE;
			for(func = 0; func < gPciNumberOfFunction; func++)
			{
				if(multiFuncFlag == 0 && func > 0)
				{
					break;
				}

				pciAddress = PciBusDevFunc(bus, dev, func);
				if(pciConfigRead(pciAddress, 0, (PBYTE)&id, sizeof(id)))
				{
					if(func == 0) // Is Multi Function Device
					{
						if(pciConfigRead(pciAddress, 0x0E, (PBYTE)&type, sizeof(type)))
						{
							if(type & 0x80)
							{
								multiFuncFlag = TRUE;
							}
						}
					}

					if(id == (vendorId | ((DWORD)deviceId << 16)))
					{
						if(count == index) 
						{
							return pciAddress;
						}
						count++;
						continue;
					}
				}
				else if(ret == OLS_ERROR_PCI_BUS_NOT_EXIST)
				{
					return 0xFFFFFFFF; 
				}
				else if(ret == OLS_ERROR_PCI_NO_DEVICE && func == 0)
				{
					break;
				}
			}
		}
	}
	return 0xFFFFFFFF;
}

DWORD WINAPI FindPciDeviceByClass(BYTE baseClass, BYTE subClass, BYTE programIf, BYTE index)
{
	if(gHandle == INVALID_HANDLE_VALUE)
	{
		return 0xFFFFFFFF;
	}

	DWORD bus = 0, dev = 0, func = 0;
	DWORD count = 0;
	DWORD pciAddress = 0xFFFFFFFF;
	DWORD conf[3] = {0};
	DWORD ret = 0;	
	BOOL multiFuncFlag = FALSE;
	BYTE type = 0;
	count = 0;

	for(bus = 0; bus <= gPciMaxNumberOfBus; bus++)
	{
		for(dev = 0; dev < gPciNumberOfDevice; dev++)
		{
			multiFuncFlag = FALSE;
			for(func = 0; func < gPciNumberOfFunction; func++)
			{
				if(multiFuncFlag == FALSE && func > 0)
				{
					break;
				}
				pciAddress = PciBusDevFunc(bus, dev, func);
				if(pciConfigRead(pciAddress, 0, (BYTE*)conf, sizeof(conf)))
				{
					if(func == 0) // Is Multi Function Device
					{ 
						if(pciConfigRead(pciAddress, 0x0E, (BYTE*)&type, sizeof(type)))
						{
							if(type & 0x80)
							{
								multiFuncFlag = TRUE;
							}
						}
					}
					if((conf[2] & 0xFFFFFF00) == 
							(((DWORD)baseClass << 24) |
							((DWORD)subClass << 16) |
							((DWORD)programIf << 8))
						)
					{
						if(count == index)
						{
							return pciAddress;
						}
						count++;
						continue;
					}
				}
				else if(ret == OLS_ERROR_PCI_BUS_NOT_EXIST)
				{
					return 0xFFFFFFFF;
				}
				else if(ret == OLS_ERROR_PCI_NO_DEVICE && func == 0)
				{
					break;
				}
			}
		}
	}
	return 0xFFFFFFFF;
}

