#pragma once

#include "../Common/template.h"


typedef CString (*CBGetLogFilePath)(InputFiles);

#ifdef LANDSIDEREPORT_EXPORTS
#define LANDSIDEREPORT_API __declspec(dllexport)
#else
#define LANDSIDEREPORT_API __declspec(dllimport)
#endif