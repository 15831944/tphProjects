#include "stdafx.h"

#include "ARCGUID.h"
#include <time.h>
#include "ARCStringConvert.h"
// IP helper API...
#include <iphlpapi.h>

CString ARCGUID::Generate( CString className )
{
	CString s;
	s.Append(slocalID);
	s.Append(_T("_"));
	s.Append(className);
	s.Append(_T("_"));
	time_t t;
	time(&t);
	s.Append(ARCStringConvert::toString((int)t));
	
	s.Append(_T("_"));
	int r = rand()%10000;
	s.Append(ARCStringConvert::toString(r));
	

	return s;
}

bool ARCGUID::GetMacAddress(byte* address)
{
	// Variables...
	IP_ADAPTER_INFO    *pAdaptersInfo       = NULL;
	IP_ADAPTER_INFO    *pAdapter            = NULL;
	unsigned long       ulAdapterInfoSize   = 0;

	// Get information on all adapters...

	// Request size of buffer needed and check for error...
	ulAdapterInfoSize = 1;
	if(ERROR_BUFFER_OVERFLOW != GetAdaptersInfo(pAdaptersInfo, 
		&ulAdapterInfoSize))
		return false;

	// Bad size...
	if(ulAdapterInfoSize <= 1)
		return false;

	// Allocate storage space...
	pAdaptersInfo = (IP_ADAPTER_INFO *) malloc(ulAdapterInfoSize);

	// Failed...
	if(!pAdaptersInfo)
		return false;

	// Make the request and check for error...
	if(ERROR_SUCCESS != GetAdaptersInfo(pAdaptersInfo, &ulAdapterInfoSize))
	{
		// Cleanup, abort...
		free(pAdaptersInfo);
		return false;
	}

	// Check each adapter's MAC address until we find a match...
	for(pAdapter = pAdaptersInfo; pAdapter; pAdapter = pAdapter->Next)
	{
		// Network adapter does not have standard 6 byte MAC address, skip...
		if(pAdapter->AddressLength != 6)
			continue;

		// Match...
		memcpy(address,pAdapter->Address,6);
	
			// Cleanup...
		free(pAdaptersInfo);

			// Done...
		return true;
		
	}
	// Not found...
	return false;
}



void ARCGUID::Init()
{
	byte macaddress[6];
	if(GetMacAddress(macaddress)){
	}
	slocalID.Format(_T("%02x%02x%02x%02x%02x%02x"),macaddress[0],macaddress[1],macaddress[2],macaddress[3],macaddress[4],macaddress[5]);	
}

CString ARCGUID::slocalID;
