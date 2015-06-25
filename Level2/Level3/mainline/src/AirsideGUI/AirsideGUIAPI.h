#pragma once


#ifdef AIRSIDEGUI_EXPORTS
#define AIRSIDEGUI_API __declspec(dllexport)
#else
#define AIRSIDEGUI_API __declspec(dllimport)
#endif