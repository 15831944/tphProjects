#ifndef __MUTIREPORTSHOWDATA_H
#define __MUTIREPORTSHOWDATA_H

#include "ReportParameter.h"
#include<vector>
typedef std::pair<CString,ENUM_REPORT_TYPE>REPORT_FILE_AND_TYPE;
typedef std::vector<REPORT_FILE_AND_TYPE>REPORTS_OF_EACH_SIM_RESULT;
typedef std::vector<REPORTS_OF_EACH_SIM_RESULT>ALL_COMPARE_REPORT_DATA;

#endif