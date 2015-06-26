#pragma once

#ifdef INPUTAIRSIDE_EXPORTS
#define INPUTAIRSIDE_API __declspec(dllexport)
#else
#define INPUTAIRSIDE_API __declspec(dllimport)
#endif