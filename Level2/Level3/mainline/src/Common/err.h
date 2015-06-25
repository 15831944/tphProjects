#ifndef ERROR_H
#define ERROR_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define INSUFFICIENT_MEMORY     "Insufficient memory"
#define FILE_OPEN               "Cannot open file"
#define FILE_READ               "Cannot read file"
#define FILE_WRITE              "Cannot write to file"
#define FILE_ERROR              "File problem"
#define ENVIRONMENT_ERROR       "Cannot read environment"

#ifdef __cplusplus
    extern "C"
    {
#endif

#ifdef __cplusplus
    }
void error (const char *errMessage, const char *module);
void error (const char *errMessage, short num);
void error (const char *errMessage);
void end (void);

#endif


#endif
