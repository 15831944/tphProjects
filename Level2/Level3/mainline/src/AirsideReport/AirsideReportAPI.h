#pragma once


#ifdef AIRSIDEREPORT_EXPORTS
#define AIRSIDEREPORT_API __declspec(dllexport)
#else
#define AIRSIDEREPORT_API __declspec(dllimport)
#endif