#include "stdafx.h"
#include "pbuffer.h"
//#include "glh_extensions.h"
#define PBUFFERLOGDEBUG( STR ) 

PBuffer::PBuffer( int w, int h, unsigned int mode ) : width(w), height(h), mode(mode), myDC(NULL), myGLctx(NULL), buffer(NULL)
{
}

PBuffer::~PBuffer()
{
	CString strInfo;
    if ( buffer )
    {
		if(! wglDeleteContext( myGLctx ) )
		{
			PBUFFERLOGDEBUG ( "did not delete the glContext\n" )
		}
		if(! wglReleasePbufferDCARB( buffer, myDC ) )
		{
			PBUFFERLOGDEBUG ( "did not release the pbuffer \n" )
		}
		if(! wglDestroyPbufferARB( buffer ) )
		{
            PBUFFERLOGDEBUG ( "did not Destory the Pbuffer \n" )
		}
		else{
			/*CString strDebug;
			strDebug.Format("PBuffer %d x %d Delete",width,height);
            PBUFFERLOGDEBUG ( strDebug );*/
		}
		// TRACE("PBuffer destroyed\n");
    }
}
// Check to see if the pbuffer was lost.
// If it was lost, destroy it and then recreate it.
void PBuffer::HandleModeSwitch()
{
    int lost = 0;

    wglQueryPbufferARB( buffer, WGL_PBUFFER_LOST_ARB, &lost );

    if ( lost )
        {
        this->~PBuffer();
        Initialize();
        }
}

// This function actually does the creation of the p-buffer.
// It can only be called once a window has already been created.
bool PBuffer::Initialize(HDC hdc,bool share)
{
    if(hdc == NULL)
		hdc = wglGetCurrentDC();
	HGLRC hglrc = wglGetCurrentContext();

    // Query for a suitable pixel format based on the specified mode.
    int   iattributes[2*MAX_ATTRIBS];
    float fattributes[2*MAX_ATTRIBS];
    int nfattribs = 0;
    int niattribs = 0;

    // Attribute arrays must be "0" terminated - for simplicity, first
    // just zero-out the array entire, then fill from left to right.
    for ( int a = 0; a < 2*MAX_ATTRIBS; a++ )
        {
        iattributes[a] = 0;
        fattributes[a] = 0;
        }

    // Since we are trying to create a pbuffer, the pixel format we
    // request (and subsequently use) must be "p-buffer capable".
    iattributes[2*niattribs  ] = WGL_DRAW_TO_PBUFFER_ARB;
    iattributes[2*niattribs+1] = true;
    niattribs++;
	iattributes[2*niattribs  ] = WGL_PIXEL_TYPE_ARB;
	iattributes[2*niattribs+1] = WGL_TYPE_RGBA_ARB;
	niattribs++;
	iattributes[2*niattribs  ] = WGL_DEPTH_BITS_ARB;
	iattributes[2*niattribs+1] = 1;
	niattribs++;
	iattributes[2*niattribs  ] = WGL_SUPPORT_OPENGL_ARB;
	iattributes[2*niattribs+1] = true;
    niattribs++;
	iattributes[2*niattribs  ] = WGL_STENCIL_BITS_ARB;
	iattributes[2*niattribs+1] = 1;
	niattribs++;

    int format;
    int pformat[MAX_PFORMATS];
    unsigned int nformats;
	CString strInfo;         //debuf information
    if ( !wglChoosePixelFormatARB( hdc, iattributes, fattributes, MAX_PFORMATS, pformat, &nformats ) )
    {
		return false; //exit( -1 );       
    }
	if( nformats <= 0 )
	{
		//choose pixel format without stencil bits
		iattributes[8] = WGL_STENCIL_BITS_ARB;
		iattributes[9] = 0;
		if ( !wglChoosePixelFormatARB( hdc, iattributes, fattributes, MAX_PFORMATS, pformat, &nformats ) )
		{
			return false;//	exit( -1 ); 
		}
		
	}	
	if ( nformats <= 0 )
	{
		return false; //exit( -1 );			
	}
    format = pformat[0];

    // Create the p-buffer.
    iattributes[0] = 0;
    buffer = wglCreatePbufferARB( hdc, format, width, height, iattributes );
    if ( !buffer )
        {
        DWORD err = GetLastError();
		return false; //exit( -1 );       
		}
    // Get the device context.
    myDC = wglGetPbufferDCARB( buffer );
    if ( !myDC )
        {
			return false; //exit( -1 );
        }

    // Create a gl context for the p-buffer.
    myGLctx = wglCreateContext( myDC );
    if ( !myGLctx )
        {
			return false; //exit( -1 );
        }

	if( share )
		{
		if( !wglShareLists(hglrc, myGLctx) )
			{
				return false; //exit(-1);
			}
		}

    // Determine the actual width and height we were able to create.
    wglQueryPbufferARB( buffer, WGL_PBUFFER_WIDTH_ARB, &width );
    wglQueryPbufferARB( buffer, WGL_PBUFFER_HEIGHT_ARB, &height );
	
		return true;
}

bool PBuffer::MakeCurrent()
{
	CString strInfo;
    if ( !wglMakeCurrent( myDC, myGLctx ) )
    {
		return false ;//	 exit( -1 );
    }
	return true;
}
