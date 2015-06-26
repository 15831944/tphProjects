#pragma once

#ifdef _DEBUG
static void check_gl_errors(const TCHAR* msg, const TCHAR* file, int line)
{
	GLenum errorGL = GL_NO_ERROR;
	while( GL_NO_ERROR!=(errorGL = glGetError()) )
	{
		CString sError;
		sError.Format( _T("OPENGL ERRORS [%s] in %s %d  when %s \n"), gluErrorString(errorGL), file, line ,msg );
		TRACE(sError);
	}
}
#define  CHECK_GL_ERRORS(msg) check_gl_errors(msg , __FILE__,__LINE__)
#else // _DEBUG
#define CHECK_GL_ERRORS(msg)	__noop
#endif