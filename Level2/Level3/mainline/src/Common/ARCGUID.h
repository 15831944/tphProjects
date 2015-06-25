#pragma once


#pragma comment(lib,"Iphlpapi.lib") 
//arc guid , generated from license info and class type and int
class ARCGUID 
{
public:
	static void Init();
	static CString Generate(CString className);
	static bool GetMacAddress(byte* address);
protected:
	static CString slocalID;
};