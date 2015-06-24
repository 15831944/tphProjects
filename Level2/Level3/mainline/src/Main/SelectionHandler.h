#ifndef __SELECTION_HANDLER_H
#define __SELECTION_HANDLER_H

#define SELECTION_MAX_HITS		256
#define SELECTION_BUFFER_SIZE	1024
static const GLuint SELECTOFFSET = 65536;	//2^16
static const GLuint LANDSIDEACTAGSOFFSET = 0;
static const GLuint AIRSIDEACTAGSOFFSET = SELECTOFFSET;

// selection id data
// ids are 32-bit unsigned
// high-order 16-bits encode object type (2 8-bit numbers: major type, minor type)
// low-order 16-bits encode idx within the object type
static const BYTE SELTYPE_PROCESSOR = 0;
static const BYTE SELTYPE_RAILWAY = 1;
static const BYTE SELTYPE_PIPE = 2;
static const BYTE SELTYPE_STRUCTURE = 3;
static const BYTE SELTYPE_TAXIWAY = 4;
static const BYTE SELTYPE_LANDSIDETRACER = 5;
static const BYTE SELTYPE_AIRSIDETRACER = 6;
static const BYTE SELTYPE_TERMINALTRACER = 7;
static const BYTE SELTYPE_WALLSHAPE=8;
static const BYTE SELTYPE_AIRROUTE = 9;
static const BYTE SELTYPE_LANDSIDEPROCESSOR = 10;
static const BYTE SELTYPE_ALTOBJECT =11;
static const BYTE SELTYPE_HOLDSHORTLINE = 12;
static const BYTE SELTYPE_TERMINALAREA = 13;
static const BYTE SELTYPE_TERMINALPORTAL = 14;

static const BYTE SELSUBTYPE_MAIN = 0;
static const BYTE SELSUBTYPE_POINT = 1;
static const BYTE SELSUBTYPE_WIDTHPOINT = 2;

/*
inline static BYTE GetSelectPriority(GLuint id)
{
static const BYTE PRIORITIES[][3] = { 
{ 0x00, 0xFF, 0xFF, },	//processor
{ 0xFF, 0x00, 0xFF, },	//railway
{ 0xFF, 0x00, 0x00, },	//pipe
{ 0xFF, 0xFF, 0xFF, },	//structure
{ 0xFF, 0x00, 0x00, },  //taxiway
{ 0xFF, 0xFF, 0xFF, },	//landside tracer
{ 0xFF, 0xFF, 0xFF, },	//airside tracer
{ 0xFF, 0xFF, 0xFF, },	//terminal tracer
};
BYTE type = id >> 24;
BYTE subtype = 0x00FF & (id >> 16);

return PRIORITIES[type][subtype];
}
*/

inline static BYTE GetSelectType(GLuint id)
{
	return id >> 24;
}

inline static BYTE GetSelectSubType(GLuint id)
{
	return 0x00FF & (id >> 16);
}

inline static unsigned short GetSelectIdx(GLuint id)
{
	return static_cast<unsigned short>(id & 0x0000FFFF);
}

inline static GLuint GenerateSelectionID(BYTE type, BYTE subtype, unsigned short idx)
{
	GLuint t = type;
	GLuint s = subtype;
	GLuint i = idx;

	return (t << 24) + (s << 16) + i;
}

//only suitable for landside                lower order 2 8bit number, major idx, min idx.
inline static GLuint NewGenerateSelectionID(BYTE type, BYTE subtype, unsigned short idx,unsigned short subidx)
{
	GLuint t = type ;
	GLuint s = subtype;
	GLuint ti = idx;
	GLuint si = subidx;
	return (t<<24) + (s<<16) + (ti<<8) + si;
}

inline static unsigned short NewGetSelectIdx(GLuint id)
{
	id /= 256;    //2^8
	return static_cast<unsigned short> (id & 0x000000FF);
}
inline static unsigned short NewGetSubSelectIdx(GLuint id)
{
	return static_cast<unsigned short> (id & 0x000000FF);
}



#endif