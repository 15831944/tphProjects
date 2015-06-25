#if !defined(MUTILREPORT_INCLUDE_)
#define MUTILREPORT_INCLUDE_

const CString strSimParaFile   = "SimPara.par";
const CString strOtherInfo	   = "OtherInfo.inf";
const CString strSimResult     = "SimResult";
const CString strLogPath       = "Logs";
const CString strReportSetName = "Report";
const CString strReportParaFile= "ReportPara.par";

typedef struct _tagLOGToFileName
{
	int _iLogType;
	CString _strFileName;
	
	_tagLOGToFileName(int _t, CString& _str)
	{
		_iLogType = _t;
		_strFileName = _str;
	}
} LOGTOFILENAME;

#endif // MUTILREPORT_INCLUDE_