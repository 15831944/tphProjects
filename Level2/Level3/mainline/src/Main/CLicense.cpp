/*
  Name:         CLicense.cpp (implementation)
  Copyright:    Aviation Research Corporation
  Description:  CLicense class implementation...
*/
#include "stdafx.h"

// Pre-procs...
#include "CLicense.h"
#include "../Common/ReadRegister.h"
#include "../Common/INIFileHandle.h"
// Constructor...
CLicense::CLicense()
{
    // Initialize variables to defaults...
    pLicenseBuffer      = NULL;
    dwLicenseBufferSize = 0;
}

// Get license buffer size...
unsigned int CLicense::GetLicenseBufferSize()
{
    // Return it...
    return dwLicenseBufferSize;
}
CString  CLicense::GetPath() 
{
	CString path ;
	CReadRegister  RegReader ;
	if(!RegReader.ReadRegisterPath(CString()))
		return path ;
	return RegReader.m_Path.m_AppPath ;
	
}
// Get product license time remaining in seconds. -1 unlimited...
// "AT" -> ARCTerm...
// "AC" -> ARCCapture...
int CLicense::GetProductLicenseSecondsRemaining(char *pszProductCode)
{
    // Variables...
    HKEY            hKey            = NULL;
    char            szKey[1024]     = {0};
    char            szValue[1024]   = {0};
    DWORD           dwType          = 0;
    int           dwLicenseSize   = 0;
    unsigned char  *pLicenseBuffer  = NULL;
    int    unIndex         = 0;
    time_t          TimeInstalled   = 0;
    time_t          TimeExpiration  = 0;
    bool            bIdentified     = false;
    unsigned char   MACAddress[6]   = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    unsigned char   Byte            = 0x00;
	unsigned char   SimPermission   = 0x00;
    DWORD           dwSerial        = 0;
    char            szBuffer[1024]  = {0};
    DWORD           dwRealSerial    = 0;
    DWORD           dwTemp          = 0;
    
    // Check product code length...
    if(strlen(pszProductCode) != 2)
        return 0;
    
    // A legacy ARCTerm license has been detected...
    //if(IsLegacyARCTermLicenseDetected(NULL))
    //{
    //    // Upgrade to new format, and check for error...
    //    if(!UpgradeLegacyARCTermLicense())
    //        MessageBox(NULL, "A legacy ARCport ALT license was detected, but\n"
    //                         "an error occured while upgrading it.", 
    //                   "Error", MB_OK | MB_ICONERROR);
    //    
    //    // Abort...
    //    return 0;
    //}
    
    // Format appropriate registry key and value based on product code...
        
        // ARCTerm...
        if(strcmp(pszProductCode, "AT") == 0)
        {
            // Remember...
            strcpy(szKey, CLICENSE_REGISTRY_LICENSE_ARCTERM_KEY);
            strcpy(szValue, CLICENSE_REGISTRY_LICENSE_ARCTERM_VALUE);
        }
        
        // ARCCapture...
        else if(strcmp(pszProductCode, "AC") == 0)
        {
            // Remember...
            strcpy(szKey, CLICENSE_REGISTRY_LICENSE_ARCCAPTURE_KEY);
            strcpy(szValue, CLICENSE_REGISTRY_LICENSE_ARCCAPTURE_VALUE);
        }
        
        // Unknown...
        else
            return 0;

    CINIFileForLicenseHandle  handle ;
	if(!handle.LoadFile(GetPath()))
		return 0 ;
	pLicenseBuffer = new BYTE[1024] ;
	memset(pLicenseBuffer,0,sizeof(BYTE)*1024) ;
	handle.GetLicenseFont(pLicenseBuffer,dwLicenseSize) ;

    // Summon the mighty r-decryption algorithm...
    for(unIndex = 0; unIndex < dwLicenseSize; unIndex++)
        pLicenseBuffer[unIndex] -= unIndex;       

    // Check magic bytes for Aviation Research Corporation license signature...
        
        // Seek...
        unIndex = 0;
        
        // Check...
        if(memcmp(&pLicenseBuffer[unIndex], "AL", strlen("AL")) != 0)
        {
            // Cleanup, abort...
            free(pLicenseBuffer);
            return 0;
        }
    
    // Check product signature...
    
        // Seek...
        unIndex += strlen(pszProductCode);
        
        // Check...
        if(memcmp(&pLicenseBuffer[unIndex], pszProductCode, 
                  strlen(pszProductCode)) != 0)
        {
            // Cleanup, abort...
            delete []pLicenseBuffer ;
            return 0;
        }
        
    // Extract time installed...
    
        // Seek...
        unIndex += strlen(pszProductCode);
        
        // Extract...
        memcpy(&TimeInstalled, &pLicenseBuffer[unIndex], sizeof(time_t));
    
    // Extract expiration time...
    
        // Seek...
        unIndex += sizeof(time_t);
        
        // Extract...
        memcpy(&TimeExpiration, &pLicenseBuffer[unIndex], sizeof(time_t));

		unIndex += sizeof(time_t);
		// Extract...
		if (pLicenseBuffer[unIndex] == '$')
		{
			unIndex += strlen("$");
			memcpy(&SimPermission, &pLicenseBuffer[unIndex], sizeof(unsigned char));
			unIndex += sizeof(unsigned char);
		}
		else
		{
			SimPermission = 2;
		}

    if(TimeExpiration == 0)
    {
        // Cleanup...
        delete []pLicenseBuffer ;
        
        // Done...
        return -1;
    }
    
    // Product license is past expiration...
    if(time(NULL) >= TimeExpiration)
    {
        // Cleanup...
        delete []pLicenseBuffer ;
        
        // Done...
        return 0;
    }
    // Cleanup...
    delete []pLicenseBuffer ;

    // Return time remaining in seconds...
    return TimeExpiration - time(NULL);
}

unsigned char CLicense::GetProductLicenseSimulationControl(char *pszProductCode)
{
	// Variables...
	HKEY            hKey            = NULL;
	char            szKey[1024]     = {0};
	char            szValue[1024]   = {0};
	DWORD           dwType          = 0;
	int           dwLicenseSize   = 0;
	unsigned char  *pLicenseBuffer  = NULL;
    int    unIndex         = 0;
	time_t          TimeInstalled   = 0;
	time_t          TimeExpiration  = 0;
	bool            bIdentified     = false;
	unsigned char   MACAddress[6]   = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	unsigned char   Byte            = 0x00;
	unsigned char   SimPermission   = 0x00;
	DWORD           dwSerial        = 0;
	char            szBuffer[1024]  = {0};
	DWORD           dwRealSerial    = 0;
	DWORD           dwTemp          = 0;

	// Check product code length...
	if(strlen(pszProductCode) != 2)
		return 0;

	// A legacy ARCTerm license has been detected...
	//if(IsLegacyARCTermLicenseDetected(NULL))
	//{
	//	// Upgrade to new format, and check for error...
	//	if(!UpgradeLegacyARCTermLicense())
	//		MessageBox(NULL, "A legacy ARCport ALT license was detected, but\n"
	//		"an error occured while upgrading it.", 
	//		"Error", MB_OK | MB_ICONERROR);

	//	// Abort...
	//	return 0;
	//}

	// Format appropriate registry key and value based on product code...

	// ARCTerm...
	pLicenseBuffer = new BYTE[1024];
	memset(pLicenseBuffer,0,sizeof(BYTE)*1024) ;
    CINIFileForLicenseHandle handle ;
	if(!handle.LoadFile(GetPath())) 
		return 0;
    if(!handle.GetLicenseFont(pLicenseBuffer,dwLicenseSize))
		return 0;

	// Allocate...

	// Summon the mighty r-decryption algorithm...
	for(unIndex = 0; unIndex < dwLicenseSize; unIndex++)
		pLicenseBuffer[unIndex] -= unIndex;       

	// Check magic bytes for Aviation Research Corporation license signature...

	// Seek...
	unIndex = 0;

	// Check...
	if(memcmp(&pLicenseBuffer[unIndex], "AL", strlen("AL")) != 0)
	{
		// Cleanup, abort...
		delete []pLicenseBuffer;
		return 0;
	}

	// Check product signature...

	// Seek...
	unIndex += strlen(pszProductCode);

	// Check...
	if(memcmp(&pLicenseBuffer[unIndex], pszProductCode, 
		strlen(pszProductCode)) != 0)
	{
		// Cleanup, abort...
		delete []pLicenseBuffer;
		return 0;
	}

	// Extract time installed...

	// Seek...
	unIndex += strlen(pszProductCode);

	// Extract...
	memcpy(&TimeInstalled, &pLicenseBuffer[unIndex], sizeof(time_t));

	// Extract expiration time...

	// Seek...
	unIndex += sizeof(time_t);

	// Extract...
	memcpy(&TimeExpiration, &pLicenseBuffer[unIndex], sizeof(time_t));


	unIndex += sizeof(time_t);
	// Extract...
	if (pLicenseBuffer[unIndex] == '$')
	{
		unIndex += strlen("$");
		memcpy(&SimPermission, &pLicenseBuffer[unIndex], sizeof(unsigned char));
		unIndex += sizeof(unsigned char);
	}
	else
	{
		SimPermission = 2;
	}

	// Product license is past expiration...
	if(TimeExpiration != 0)
		if(time(NULL) >= TimeExpiration)
		{
			// Cleanup...
			delete []pLicenseBuffer;

			// Done...
			return 0;
		}

	// Cleanup...
	delete []pLicenseBuffer;

	// Return time remaining in seconds...
	return SimPermission;
}

// Install a license...
// "AT" -> ARCTerm...
// "AC" -> ARCCapture...
bool CLicense::InstallProductLicense(char *pszProductCode, int nSeconds)
{
    // Variables...
    time_t              Time                = 0;
    IP_ADAPTER_INFO    *pAdaptersInfo       = NULL;
    IP_ADAPTER_INFO    *pAdapter            = NULL;
    unsigned long       ulAdapterInfoSize   = 0;
    unsigned int        unIndex             = 0;
    HKEY                hKey                = NULL;
    HKEY                hTempKey            = NULL;
    char                szKey[128]          = {0};
    char                szValue[128]        = {0};
    unsigned char       Byte                = 0x00;
	unsigned char		SimPermission		= 0x00;
    char                szBuffer[1024]      = {0};
    DWORD               dwSerial            = 0;
    DWORD               dwTemp              = 0;

    // Try to build license data buffer and store...
    try
    {
        // Format product registry key and value...

            // ARCTerm...
            if(strcmp(pszProductCode, "AT") == 0)
            {
                // Remember...
                strcpy(szKey, CLICENSE_REGISTRY_LICENSE_ARCTERM_KEY);
                strcpy(szValue, CLICENSE_REGISTRY_LICENSE_ARCTERM_VALUE);
            }

            // ARCCapture...
            else if(strcmp(pszProductCode, "AC") == 0)
            {
                // Remember...
                strcpy(szKey, CLICENSE_REGISTRY_LICENSE_ARCCAPTURE_KEY);
                strcpy(szValue, CLICENSE_REGISTRY_LICENSE_ARCCAPTURE_VALUE);
            }

            // Unknown...
            else
                throw "Error: Unknown product code...";

        // Start with magic signature and check for error...
        if(!PushLicenseData((unsigned char *) "AL", strlen("AL")))
            throw "Error: Insufficient memory...";

        // Add product code and check for error...
        if(!PushLicenseData(pszProductCode, 2))
            throw "Error: Insufficient memory...";

        // Add todays time as install time and check for error...
        time(&Time);
        if(!PushLicenseData((unsigned char *) &Time, sizeof(time_t)))
            throw "Error: Insufficient memory...";

        // Calculate time till expiration...
            
            // Unlimited...
            if(nSeconds == -1)
                memset(&Time, 0, sizeof(time_t));
            
            // Limited...
            else
                Time += nSeconds;

        // Add time till expiration and check for error...
        if(!PushLicenseData((unsigned char *) &Time, sizeof(time_t)))
            throw "Error: Insufficient memory...";

		// Add Simulation Permission mark...
		if(!PushLicenseData((unsigned char *) "$", strlen("$")))
			throw "Error: Insufficient memory...";

		//Add Simulation Permission and check for error...
		SimPermission = 0;
		if(!PushLicenseData((unsigned char *) &SimPermission, 
			sizeof(unsigned char)))
			throw "Error: Insufficient memory...";

        // Add all adapters MAC addresses found...

            // Request size of buffer needed and check for error...
            ulAdapterInfoSize = 1;
            if(ERROR_BUFFER_OVERFLOW != GetAdaptersInfo(pAdaptersInfo, 
                                                        &ulAdapterInfoSize))
                throw "Error: Problem while querying hardware...";

                // Bad size...
                if(ulAdapterInfoSize <= 1)
                    throw "Error: Problem while querying hardware...";

            // Allocate storage space...
            pAdaptersInfo = (IP_ADAPTER_INFO *) malloc(ulAdapterInfoSize);

                // Failed...
                if(!pAdaptersInfo)
                    throw "Error: Insufficient memory...";

            // Make the request and check for error...
            if(ERROR_SUCCESS != GetAdaptersInfo(pAdaptersInfo, 
                                                &ulAdapterInfoSize))
            {
                // Cleanup, abort...
                free(pAdaptersInfo);
                throw "Error: Problem while querying hardware...";
            }

            // Parse each adapter's information while there are some...
            for(pAdapter = pAdaptersInfo; pAdapter; pAdapter = pAdapter->Next)
            {
                // Network adapter does not have standard 6 byte MAC, skip...
                if(pAdapter->AddressLength != 6)
                    continue;

                // Add ID type byte and check for error...
                if(!PushLicenseData((unsigned char *) "N", strlen("N")))
                    throw "Error: Insufficient memory...";

                // Add token length and check for error...
                Byte = 6;
                if(!PushLicenseData((unsigned char *) &Byte, 
                                    sizeof(unsigned char)))
                    throw "Error: Insufficient memory...";

                // Add adapter MAC address and check for error...
                if(!PushLicenseData(pAdapter->Address, pAdapter->AddressLength))
                    throw "Error: Insufficient memory...";
            }

        // Try and get hard drive volume serial... (not hardware serial)

            // Format drive letter we are running on...
            GetModuleFileName(GetModuleHandle(NULL), szBuffer, 
                              sizeof(szBuffer));
            szBuffer[3] = '\x0';

            // Get serial and add if successful...
            if(GetVolumeInformation(szBuffer, NULL, 0, &dwSerial, &dwTemp, &dwTemp,
                                    NULL, 0))
            {
                // Add ID type byte and check for error...
                if(!PushLicenseData((unsigned char *) "H", strlen("H")))
                    throw "Error: Insufficient memory...";

                // Add token length and check for error...
                Byte = sizeof(dwSerial);
                if(!PushLicenseData((unsigned char *) &Byte, 
                                    sizeof(unsigned char)))
                    throw "Error: Insufficient memory...";

                // Add and check for error...
                if(!PushLicenseData(&dwSerial, sizeof(dwSerial)))
                    throw "Error: Insufficient memory...";
            }

        // Summon the mighty r-encryption algorithm...
        for(unIndex = 0; unIndex < GetLicenseBufferSize(); unIndex++)
            pLicenseBuffer[unIndex] += unIndex;
        
		//write license to INI File 
		CINIFileForLicenseHandle  handle ;
		handle.LoadFile(GetPath());
		handle.SetLicenseFONT(pLicenseBuffer,dwLicenseBufferSize) ;
		handle.SaveData(GetPath()) ;
    }
        // Failed...
        catch(const char *pszReason)
        {
            // Cleanup license buffer data, if any...
            if(GetLicenseBufferSize())
                PushLicenseData(NULL, 0);
            
            // Cleanup registry key, if open...
            if(hKey)
                RegCloseKey(hKey);
            
            // Display error message...
            MessageBox(NULL, pszReason, "License Error", MB_OK | MB_ICONERROR);
            
            // Abort...
            return false;
        }

    // Cleanup...
    PushLicenseData(NULL, 0);

    // Done...
    return true;
}

// Is legacy license detected for ARCTerm?...
bool CLicense::IsLegacyARCTermLicenseDetected(char *pszLegacyLicensePath)
{
    // Variables...
    HKEY    hKey            = NULL;
    char    szKey[1024]     = {0};
    char    szValue[1024]   = {0};
    char    szBuffer[1024]  = {0};
    DWORD   dwTemp          = 0;
    FILE   *hFile           = NULL;
    
    // Format product registry key and value...
    strcpy(szKey, CLICENSE_REGISTRY_PATH_ARCTERM_KEY);
    strcpy(szValue, CLICENSE_REGISTRY_PATH_ARCTERM_VALUE);
    
    // Open ARCTerm paths key and check for error...
    if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
                                     "SOFTWARE\\ARC\\ARCTerm\\Paths", 0, 
                                     KEY_READ, &hKey))
    {
        // Try deprecated registry location instead...
        if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
                                         "SOFTWARE\\ARCTerm\\Paths", 0, 
                                         KEY_READ, &hKey))
        {
            // Try one last deprecated location and abort if not found...
            if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
                                             szKey, 0, KEY_READ, &hKey))
                return false;
        }
    }
    
    // Get path to ARCTerm and check for error...
    dwTemp = sizeof(szBuffer);
    if(ERROR_SUCCESS != RegQueryValueEx(hKey, szValue, NULL, NULL, 
                                        (BYTE *) szBuffer, &dwTemp))
    {
        // Cleanup...
        RegCloseKey(hKey);
        
        // Done...
        return false;
    }
    
        // Failed...
        if(!strlen(szBuffer))
            return false;
    
    // Cleanup...
    RegCloseKey(hKey);
    
    // Format path to lock...
    strcat(szBuffer, "\\lock");
    
    // Store path to legacy ARCTerm lock file for caller, if requested...
    if(pszLegacyLicensePath)
        strcpy(pszLegacyLicensePath, szBuffer);
    
    // Attempt to open lock file...
    hFile = fopen(szBuffer, "rb");
    
        // Failed, assume non-existent...
        if(!hFile)
            return false;

    // Cleanup...
    fclose(hFile);

    // Done...
    return true;
}

// Does this MAC address exist in the system on any network adapter?
bool CLicense::IsMACPresentInSystem(unsigned char *pMACAddress)
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
        if(memcmp(pAdapter->Address, pMACAddress, 6) == 0)
        {
            // Cleanup...
            free(pAdaptersInfo);
    
            // Done...
            return true;
        }
    }
    
    // Cleanup...
    free(pAdaptersInfo);
    
    // Not found...
    return false;
}

/* Is product installed?...
// "AT" -> ARCTerm...
// "AC" -> ARCCapture...
bool CLicense::IsProductInstalled(char *pszProductCode)
{
    // Variables...
    HKEY    hKey            = NULL;
    char    szKey[1024]     = {0};
    char    szValue[1024]   = {0};
    char    szBuffer[1024]  = {0};
    DWORD   dwTemp          = 0;

    // Format product registry key and value...

        // ARCTerm...
        if(strcmp(pszProductCode, "AT") == 0)
        {
            // Remember...
            strcpy(szKey, CLICENSE_REGISTRY_PATH_ARCTERM_KEY);
            strcpy(szValue, CLICENSE_REGISTRY_PATH_ARCTERM_VALUE);
        }

        // ARCCapture...
        else if(strcmp(pszProductCode, "AC") == 0)
        {
            // Remember...
            strcpy(szKey, CLICENSE_REGISTRY_PATH_ARCCAPTURE_KEY);
            strcpy(szValue, CLICENSE_REGISTRY_PATH_ARCCAPTURE_VALUE);
        }

        // Unknown...
        else
            return false;

    // Open product paths key and check for error...
    if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, szKey, 0, KEY_READ, 
                                     &hKey))
        return false;

    // Get path to ARCTerm and check for error...
    dwTemp = sizeof(szBuffer);
    if(ERROR_SUCCESS != RegQueryValueEx(hKey, szValue, NULL, NULL, 
                                        (BYTE *) szBuffer, &dwTemp))
    {
        // Cleanup...
        RegCloseKey(hKey);

        // Done...
        return false;
    }
    
        // Failed...
        if(!strlen(szBuffer))
            return false;
    
    // Done...
    return true;
}*/

// Is product license detected?...
// "AT" -> ARCTerm...
// "AC" -> ARCCapture...
bool CLicense::IsProductLicenseDetected(char *pszProductCode)
{
    // Variables...
    HKEY    hKey            = NULL;
    char    szKey[1024]     = {0};
    char    szValue[1024]   = {0};
    DWORD   dwType          = 0;
    DWORD   dwTemp          = 0;
    
    // Requested ARCTerm query
    if(strcmp(pszProductCode, "AT") == 0)
    {
        // Legacy license detected...
        if(IsLegacyARCTermLicenseDetected(NULL))
        {
            // Upgrade to new format, and check for error...
            if(!UpgradeLegacyARCTermLicense())
                MessageBox(NULL, "A legacy ARCTerm license was detected, but\n"
                                 "an error occured while upgrading it.", 
                           "Error", MB_OK | MB_ICONERROR);
        }
    }

    // Format product registry key and value...
        
        // ARCTerm...
        if(strcmp(pszProductCode, "AT") == 0)
        {
            // Remember...
            strcpy(szKey, CLICENSE_REGISTRY_LICENSE_ARCTERM_KEY);
            strcpy(szValue, CLICENSE_REGISTRY_LICENSE_ARCTERM_VALUE);
        }
        
        // ARCCapture...
        else if(strcmp(pszProductCode, "AC") == 0)
        {
            // Remember...
            strcpy(szKey, CLICENSE_REGISTRY_LICENSE_ARCCAPTURE_KEY);
            strcpy(szValue, CLICENSE_REGISTRY_LICENSE_ARCCAPTURE_VALUE);
        }
        
        // Unknown...
        else
            return false;
    
    // Open ARCCapture license key and check for error...
    if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, szKey, 0, KEY_READ, 
                                     &hKey))
        return false;
    
    // Free old license buffer...
    PushLicenseData(NULL, 0);
    
    // Allocate storage space for ARCCapture license...
	// Allocate...
	pLicenseBuffer = (unsigned char *) malloc(dwLicenseBufferSize);
         CINIFileForLicenseHandle handle ;
		 if(!handle.LoadFile(GetPath()))
			 return FALSE;
		 if(!handle.GetLicenseFont(pLicenseBuffer,dwLicenseBufferSize))
			 return FALSE ;           
    // Cleanup...
    PushLicenseData(NULL, 0);   
    // Done.
    return true;
}

// Add data to license buffer. NULL to clear...
void *CLicense::PushLicenseData(void *pData, unsigned int unSize)
{
    // Variables...
    unsigned char  *pTemp   = NULL;
    
    // Passed NULL, free buffer...
    if(!pData)
    {
        // Clear buffer, if any...
        if(pLicenseBuffer)
            free(pLicenseBuffer);
        
        // Remember buffer is empty...
        pLicenseBuffer      = NULL;
        dwLicenseBufferSize = 0;
        
        // Done...
        return NULL;
    }
    
    // Reallocate buffer to accomodate new data...
    pTemp = (unsigned char *) realloc(pLicenseBuffer, 
                                      GetLicenseBufferSize() + unSize);
    
        // Failed...
        if(!pTemp)
            return NULL;
    
    // Add our new data...
    memcpy(&pTemp[GetLicenseBufferSize()], pData, unSize);
    
    // Update buffer pointer and size...
    pLicenseBuffer = pTemp;
    dwLicenseBufferSize += unSize;
    
    // Done...
    return pLicenseBuffer;
}

// Remove product license...
// "AT" -> ARCTerm...
// "AC" -> ARCCapture...
bool CLicense::RemoveLicense(char *pszProductCode)
{       
    // Variables...
    HKEY    hKey            = NULL;
    char    szKey[1024]     = {0};
    char    szValue[1024]   = {0};

    // Format product registry key and value...
     CINIFileForLicenseHandle handle ;
	 if(!handle.LoadFile(GetPath()))
		 return FALSE ;
	 BYTE val[28] = {0} ;
	 handle.SetLicenseFONT(val,28);
	 handle.SaveData(GetPath()) ;
        // ARCTerm...
    return true;
}

// Convert winblows system time to normal C time_t format...
time_t CLicense::SystemTimeToUnixTime(SYSTEMTIME *pSystemTime)
{
    // Variables...
    FILETIME        FileTime;
    LARGE_INTEGER   Temp;
    LARGE_INTEGER   Offset;
    LARGE_INTEGER   Result;

    // Convert system time file time...
    if(!SystemTimeToFileTime((CONST SYSTEMTIME *) pSystemTime, &FileTime))
      return 0;

    // Convert...
    Temp.HighPart   = (LONG) FileTime.dwHighDateTime;
    Temp.LowPart    = (ULONG) FileTime.dwLowDateTime;
    Offset.HighPart = 0x019db1de;
    Offset.LowPart  = 0xd53e8000;
    Result.QuadPart = (Temp.QuadPart - Offset.QuadPart) / 10000000;
    
    // Return time...
    return (ULONG) Result.LowPart;
}

// Upgrade old legacy ARCTerm license to new one...
bool CLicense::UpgradeLegacyARCTermLicense()
{
    // Variables...
    char            szLegacyLicensePath[1024]   = {0};
    FILE           *hFile                       = NULL;
    long            lTemp                       = 0;
    SYSTEMTIME      WinTimeTemp;
    time_t          TimeInstalled               = 0;
    time_t          TimeExpire                  = 0;
    unsigned char   MACAddress[6]               = {0x00, 0x00, 0x00, 0x00, 
                                                   0x00, 0x00};
    unsigned char  *pNewLicenseBuffer           = NULL;
    int           dwLicenseBufferSize         = 0;
     int    unOffset                    = 0;
    unsigned char   Byte                        = 0x00;
	unsigned char   SimPermission				= 0x00;
    HKEY            hKey                        = NULL;
    DWORD           dwTemp                      = 0;
    HKEY            hTempKey                    = NULL;

    // No legacy license detected, abort...
    if(!IsLegacyARCTermLicenseDetected(szLegacyLicensePath))
        return false;

    // Some debugging information...
    OutputDebugString("Legacy ARCport ALTOCEF lock license detected, upgrading...");
    
    // Open legacy license lock file...
    hFile = fopen(szLegacyLicensePath, "rb");

        // Failed...
        if(!hFile)
            return false;
    
    // Read some random undocumented legacy variable and check for error...
    if(1 != fread(&lTemp, sizeof(long), 1, hFile))
    {
        // Cleanup, abort...
        fclose(hFile);
        return false;
    }
    
    // Read MAC address and check for error...
    if(1 != fread(&MACAddress, sizeof(MACAddress), 1, hFile))
    {
        // Cleanup, abort...
        fclose(hFile);
        return false;
    }
    
    // Remember new installation time...
    time(&TimeInstalled);
    
    // Read expiration time...
    
        // Extract and check for error...
        if(1 != fread(&WinTimeTemp, sizeof(SYSTEMTIME), 1, hFile))
        {
            // Cleanup, abort...
            fclose(hFile);
            return false;
        }
        
        // Convert to our C time_t format...
        
            // Unlimited...
            if(0x00101101 == lTemp)
                TimeExpire = 0;
            
            // Limited...
            else
                TimeExpire = SystemTimeToUnixTime(&WinTimeTemp);

	if(1 != fread(&SimPermission, sizeof(unsigned char), 1, hFile))
	{
		// Cleanup, abort...
		fclose(hFile);
		return false;
	}
    // Cleanup...
    fclose(hFile);
    
    // Create new license...
        
        // Calculate size of license buffer structure...
        dwLicenseBufferSize = strlen("AL") + 
                              strlen("AT") + 
                              sizeof(TimeInstalled) +
                              sizeof(TimeExpire) + 
							  strlen("$") +
							  sizeof(SimPermission) +
                              strlen("N") +
                              sizeof(Byte) + 
                              sizeof(MACAddress);
        
        // Allocate...
        pNewLicenseBuffer = (unsigned char *) malloc(dwLicenseBufferSize);
        
            // Failed...
            if(!pNewLicenseBuffer)
                return false;
        
        // Initialize...
            
            // Aviation Research Corporation license signature...
            unOffset = 0;
            memcpy(&pNewLicenseBuffer[unOffset], "AL", strlen("AL"));
            
            // ARCTerm license signature...
            unOffset += strlen("AL");
            memcpy(&pNewLicenseBuffer[unOffset], "AT", strlen("AT"));
            
            // Installation time...
            unOffset += strlen("AT");
            memcpy(&pNewLicenseBuffer[unOffset], &TimeInstalled, 
                   sizeof(time_t));
            
            // Expiration time...
            unOffset += sizeof(TimeInstalled);
            memcpy(&pNewLicenseBuffer[unOffset], &TimeExpire, sizeof(time_t));

			// Simulation Permission...
			unOffset += sizeof(TimeExpire);
            memcpy(&pNewLicenseBuffer[unOffset], "$", strlen("$"));

			unOffset += strlen("$");
			memcpy(&pNewLicenseBuffer[unOffset], &SimPermission, sizeof(SimPermission));
 

            
            // Network adapter...
            
                // Token ID...
                unOffset += sizeof(SimPermission);	
                memcpy(&pNewLicenseBuffer[unOffset], "N", strlen("N"));
                
                // Token length...
                unOffset += strlen("N");
                Byte = sizeof(MACAddress);
                memcpy(&pNewLicenseBuffer[unOffset], &Byte, sizeof(Byte));
                
                // MAC address token data...
                unOffset += sizeof(Byte);
                memcpy(&pNewLicenseBuffer[unOffset], &MACAddress, sizeof(MACAddress));
    
    // Summon the mighty r-encryption algorithm...
    for(unOffset = 0; unOffset < dwLicenseBufferSize; unOffset++)
        pNewLicenseBuffer[unOffset] += unOffset;
    
    // Write new license data to INIFIle...
        CINIFileForLicenseHandle handle ;
		if(!handle.LoadFile(GetPath()))
			return FALSE ;
		handle.SetLicenseFONT(pNewLicenseBuffer,dwLicenseBufferSize);
		handle.SaveData(GetPath()) ;

    free(pNewLicenseBuffer);

    // Delete legacy ARCTerm license lock file...
        // Unhide...
        SetFileAttributes(szLegacyLicensePath, FILE_ATTRIBUTE_NORMAL);
        
        // Delete and check for error...
        if(!DeleteFile(szLegacyLicensePath))
            return false;
    
    // Done...
    return true;
}

// Deconstructor...
CLicense::~CLicense()
{
    
}

