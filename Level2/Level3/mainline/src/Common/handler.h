#ifndef EXCEPTION_HANDLERS_H
#define EXCEPTION_HANDLERS_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#ifdef __cplusplus
extern "C" {
#endif

int textQueryMessage (const char *string);
void textErrorMessage (const char *string);

// Throw a fatal error exception for "c" code.
void C_ThrowFatalError (const char *p_msg);
void C_ThrowExit (int exitNo);

#ifdef __cplusplus
}
#endif

#endif
