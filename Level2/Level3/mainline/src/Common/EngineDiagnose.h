#if !defined(ENGINE_DIAGNOSE_FILE_)
#define ENGINE_DIAGNOSE_FILE_

enum DiagnoseType{
	NON_DIAGNOSE,
	MISS_FLIGHT,
	PAXDEST_DIAGNOS,
	DIAGNOSE_TYPE_MAX
};


class DiagnoseEntry
{
public:
	DiagnoseType	eType;
	int				iSimResultIndex;
	long			lData;
	// constructor
	DiagnoseEntry():eType( NON_DIAGNOSE ),iSimResultIndex(-1),lData(-1l)
	{	}
};


typedef struct _tagFORMATMSG
{
	CString strTime;
	CString strMsgType;
	CString strMobType;
	CString strProcessor; 
	CString strOtherInfo;
	/////////////////////////
	DiagnoseEntry diag_entry;
}FORMATMSG;


typedef struct _tagFORMATMSGEX
{
	CString strTime;
	CString strMsgType;
	CString strProcessorID;
	CString strPaxID;
	CString strFlightID;
	CString strMobileType;
	CString strOther;
	////////////////////////
	DiagnoseEntry diag_entry;
}FORMATMSGEX;

#endif //ENGINE_DIAGNOSE_FILE_