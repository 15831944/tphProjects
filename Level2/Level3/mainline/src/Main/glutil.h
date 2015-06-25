#pragma once

#ifdef _DEBUG
static void check_gl_errors(const TCHAR* file, int line)
{
	GLenum errorGL = GL_NO_ERROR;
	while( GL_NO_ERROR!=(errorGL = glGetError()) )
	{
		CString sError;
		sError.Format( _T("OPENGL ERRORS [%s] in %s %d \n"), gluErrorString(errorGL), file, line  );
		TRACE(sError);
	}
}
#define  CHECK_GL_ERRORS(msg) check_gl_errors(__FILE__,__LINE__)
#else // _DEBUG
#define CHECK_GL_ERRORS(msg)	__noop
#endif