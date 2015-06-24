#ifndef PBUFFERS_H
#define PBUFFERS_H

#include <GL/gl.h>
//#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/wglext.h>
static const CString strPBufferDebug = _T("PBufferDebug");

#define MAX_PFORMATS 256
#define MAX_ATTRIBS  32

class PBuffer
{
private:
    HDC          myDC;      // Handle to a device context.
    HGLRC        myGLctx;   // Handle to a GL context.
    HPBUFFERARB  buffer;    // Handle to a pbuffer.
    unsigned int mode;      // Flags indicating the type of pbuffer.
public:
    int          width;
    int          height;
    PBuffer( int width, int height, unsigned int mode );
    ~PBuffer();
    void HandleModeSwitch();
    bool MakeCurrent();
    bool Initialize(HDC hdc = NULL, bool share = false);
};
#endif
