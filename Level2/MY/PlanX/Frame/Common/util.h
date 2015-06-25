#ifndef UTIL_H
#define UTIL_H

#include "commondll.h"
#include "replace.h"

#ifdef __cplusplus
extern "C" {
#endif

// provides compatibility between 16 and 32 bit code
#ifdef __FLAT__
#define far
#define huge
#endif

typedef unsigned char BYTE;
#define ABS(x) ((x) > 0 ? (x) : -(x))
#define MAX(x,y) 	((x) >= (y) ? (x) : (y))
#define MIN(x,y) 	((x) <= (y) ? (x) : (y))
#define OFFSET(type,field)  ((char*)(&((type*)NULL)->field) - (char*)NULL)
#define DEGREE_TO_RAD(x) (x*M_PI/180)
#define SHORT_INVERT(d) ( ((unsigned short)d << 8) | ((unsigned short)d >> 8) )
#define U32_INVERT(d) ( ((unsigned)d << 16) | ((unsigned)d >> 16) )
#define TRUE    1
#define FALSE   0
#define SCALE_FACTOR 100.0

#define MAX_LIST    8

int getReal (char *str, float *f);
int getDouble (char *str, double *f);
int isValidFileName (char *str);
void hugeMemCopy (char huge *dest, char huge *source, int count);
int roundUp (float val, int base);
int roundDown (float val, int base);
long COMMON_TRANSFER double factorial (unsigned n);
long COMMON_TRANSFER double Pow (long double x, long double y);
void pointRotate (int* _x, int* _y,float angle);
int filecopy (char* destFileName, char* sourceFileName, int appendMode);
int countLines (char* fileName);
long countRecords (char* fileName, unsigned recordLen);
char *getSystemDate (void);
unsigned long COMMON_TRANSFER coreleft (void);

#ifdef __cplusplus
}

#include <fstream>

int operator > ( std::istream& f, char huge* buf );
int writeStream ( std::ostream& f, char huge* buf, long size );
long fileSize ( std::istream& f );

#endif


#endif
