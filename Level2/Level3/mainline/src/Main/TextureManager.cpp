// TextureManager.cpp: implementation of the CTextureManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "TextureManager.h"

#include <gl/gl.h>
#include <gl/glu.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CTextureManager *CTextureManager::m_pInstance = NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextureManager::CTextureManager()
{
	nNumTextures = 0;
	nAvailable = INITIAL_SIZE;
	nTexIDs = new int[INITIAL_SIZE];
	
	for(int i=0; i<nAvailable; i++) {
		nTexIDs[i] = -1;
	}
}

CTextureManager::~CTextureManager()
{
	delete [] nTexIDs;
	nTexIDs = NULL;
}

CTextureManager* CTextureManager::GetInstance()
{
	if(m_pInstance == NULL) {
		m_pInstance = new CTextureManager();
		//Initialize ();
	}
	return m_pInstance;
}

void CTextureManager::DeleteInstance()
{
	delete m_pInstance;
	m_pInstance = NULL;
}

int CTextureManager::LoadTexture(const char *szFilename, int nTextureID)
{
	int nWidth = 0, nHeight = 0, nBPP = 0;
	UBYTE *pData = 0;
	
	// Determine the type and actually load the file
	// ===========================================================================================
	char szCapFilename[80];
	int nLen = strlen(szFilename);
	for(int c = 0; c <= nLen; c++)	// <= to include the NULL as well
		szCapFilename [c] = toupper(szFilename [c]);
	
	if(strcmp(szCapFilename + (nLen - 3), "BMP") == 0 ||
	   strcmp(szCapFilename + (nLen - 3), "TGA") == 0) {
		// Actually load them
		if(strcmp (szCapFilename + (nLen - 3), "BMP") == 0) {
			pData = LoadBitmapFile(szFilename, nWidth, nHeight, nBPP);
			if(pData == 0)
				return -1;
		}
		if (strcmp(szCapFilename + (nLen - 3), "TGA") == 0) {
			pData = LoadTargaFile(szFilename, nWidth, nHeight, nBPP);
			if(pData == 0)
				return -1;
		}
	}
	else {
		return -1;
	}

	// Assign a valid Texture ID (if one wasn't specified)
	// ===========================================================================================
	int nNewTextureID = GetNewTextureID(nTextureID);	// Also increases nNumTextures!
	// ===========================================================================================

	// Register and upload the texture in OpenGL
	glBindTexture(GL_TEXTURE_2D, nNewTextureID);

	// NOTE : Making some assumptions about texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	/*	You can use this one if you don't want to eat memory and Mip-mapped textures
	glTexImage2D (GL_TEXTURE_2D,  0, nGLFormat,
		  		 nWidth, nHeight, 0, nGLFormat,
				 GL_UNSIGNED_BYTE, pData);
	*/

	gluBuild2DMipmaps(GL_TEXTURE_2D,
					  nBPP, nWidth, nHeight,
					  (nBPP == 3 ? GL_RGB : GL_RGBA),
					  GL_UNSIGNED_BYTE,
					  pData);

	delete [] pData;

	return nNewTextureID;
}

int CTextureManager::LoadTextureFromMemory(UBYTE *pData, int nWidth, int nHeight, int nBPP, int nTextureID)
{
	return 0;
}

void CTextureManager::FreeTexture(int nID)
{
	int nIndex = -1;
	for(int i=0; i<nAvailable; i++) {
		if(nTexIDs [i] == nID) {
			nTexIDs [i] = -1;
			nIndex = i;	// to indicate a match was found
			break;		// their _should_ only be one instance of nID (if any)
		}
	}

	if(nIndex != -1) {
		unsigned int uiGLID = (unsigned int) nID;
		glDeleteTextures(1, &uiGLID);
	}
}

void CTextureManager::FreeAll()
{
	// copy the ids to an unsigned integer array, so GL will like it ;)
	unsigned int *pUIIDs = new unsigned int[nNumTextures];
	int i, j;
	for(i=0, j=0; i<nNumTextures; i++) {
		if(nTexIDs[i] != -1) {
			pUIIDs[j] = nTexIDs[i];
			j++;
		}
	}

	glDeleteTextures(nNumTextures, pUIIDs);

	delete [] pUIIDs;
	delete [] nTexIDs;
	nTexIDs = new int [INITIAL_SIZE];
	nAvailable = INITIAL_SIZE;
	for(i=0; i<INITIAL_SIZE; i++)
		nTexIDs [i] = -1;
	
	nNumTextures = 0;
}

int	CTextureManager::GetNumTextures()
{
	return nNumTextures;
}

int CTextureManager::GetAvailableSpace()
{
	return nAvailable;
}

int CTextureManager::GetTexID(int nIndex)
{
	if(nIndex >= 0 && nIndex < nAvailable)
		return nTexIDs[nIndex];

	return 0;
}

UBYTE* CTextureManager::LoadBitmapFile(const char *filename, int &nWidth, int &nHeight, int &nBPP)
{
	// These are both defined in Windows.h
	BITMAPFILEHEADER	BitmapFileHeader;
	BITMAPINFOHEADER	BitmapInfoHeader;
	
	// Old Skool C-style code	
	FILE	*pFile;
	UBYTE	*pImage;			// bitmap image data
	UBYTE	tempRGB;				// swap variable

	// open filename in "read binary" mode
	pFile = fopen(filename, "rb");
	if (pFile == 0) {
		return 0;
	}

	// Header
	fread (&BitmapFileHeader, sizeof (BITMAPFILEHEADER), 1, pFile);
	if (BitmapFileHeader.bfType != 'MB') {
		fclose (pFile);
		return 0;
	}

	// Information
	fread (&BitmapInfoHeader, sizeof (BITMAPINFOHEADER), 1, pFile);

	if (!CheckSize (BitmapInfoHeader.biWidth) || !CheckSize (BitmapInfoHeader.biHeight)) {
		fclose (pFile);
		return 0;
	}

	
	fseek (pFile, BitmapFileHeader.bfOffBits, SEEK_SET);
	pImage = new UBYTE [BitmapInfoHeader.biSizeImage];
	if (!pImage) {
		delete [] pImage;
		fclose (pFile);
		return 0;
	}
	fread (pImage, 1, BitmapInfoHeader.biSizeImage, pFile);

	// Turn BGR to RBG
	for (int i = 0; i < (int) BitmapInfoHeader.biSizeImage; i += 3) {
		tempRGB = pImage [i];
		pImage [i + 0] = pImage [i + 2];
		pImage [i + 2] = tempRGB;
	}

	fclose(pFile);

	// THIS IS CRUCIAL!  The only way to relate the size information to the
	// OpenGL functions back in ::LoadTexture ()
	nWidth  = BitmapInfoHeader.biWidth;
	nHeight = BitmapInfoHeader.biHeight;
	nBPP    = 3;	// Only load 24-bit Bitmaps

	return pImage;
}

UBYTE* CTextureManager::LoadTargaFile(const char *filename, int &nWidth, int &nHeight, int &nBPP)
{
	// Get those annoying data structures out of the way...
	struct {
		unsigned char imageTypeCode;
		short int imageWidth;
		short int imageHeight;
		unsigned char bitCount;
	} TgaHeader;

	// Let 'er rip!
	FILE	*pFile;
	UBYTE	uCharDummy;
	short	sIntDummy;
	UBYTE	colorSwap;	// swap variable
	UBYTE	*pImage;	// the TGA data

	// open the TGA file
	pFile = fopen (filename, "rb");
	if (!pFile) {	
		return 0;
	}

	// Ignore the first two bytes
	fread(&uCharDummy, sizeof (UBYTE), 1, pFile);
	fread(&uCharDummy, sizeof (UBYTE), 1, pFile);

	// Pop in the header
	fread(&TgaHeader.imageTypeCode, sizeof (unsigned char), 1, pFile);

	// Only loading RGB and RGBA types
	if ((TgaHeader.imageTypeCode != 2) && (TgaHeader.imageTypeCode != 3)) {
		fclose (pFile);
		return 0;
	}

	// More data which isn't important for now
	fread (&uCharDummy, sizeof (unsigned char), 1, pFile);
	fread (&sIntDummy,  sizeof (short), 1, pFile);
	fread (&sIntDummy,  sizeof (short), 1, pFile);
	fread (&sIntDummy,  sizeof (short), 1, pFile);
	fread (&sIntDummy,  sizeof (short), 1, pFile);

	// Get some rather important data
	fread (&TgaHeader.imageWidth,  sizeof (short int), 1, pFile);
	fread (&TgaHeader.imageHeight, sizeof (short int), 1, pFile);
	fread (&TgaHeader.bitCount, sizeof (unsigned char), 1, pFile);

	// Skip past some more
	fread (&uCharDummy, sizeof (unsigned char), 1, pFile);

	// THIS IS CRUCIAL
	nBPP    = TgaHeader.bitCount / 8;
	nWidth  = TgaHeader.imageWidth;
	nHeight = TgaHeader.imageHeight;

	if (!CheckSize (nWidth) || !CheckSize (nHeight)) {
		fclose (pFile);
		return 0;
	}

	int nImageSize = nWidth * nHeight * nBPP;
	pImage = new UBYTE [nImageSize];
	if (pImage == 0) {		
		return 0;
	}

	// actually read it (finally!)
	fread (pImage, sizeof (UBYTE), nImageSize, pFile);

	// BGR to RGB
	for (int i = 0; i < nImageSize; i += nBPP) {
		colorSwap = pImage [i + 0];
		pImage [i + 0] = pImage [i + 2];
		pImage [i + 2] = colorSwap;
	}
	fclose (pFile);

	return pImage;
}

int CTextureManager::GetNewTextureID(int nPossibleTextureID)	// get a new one if one isn't provided
{
	// First check if the possible textureID has already been
	// used, however the default value is -1, err that is what
	// this method is passed from LoadTexture ()
	if (nPossibleTextureID != -1) {
		for (int i = 0; i < nAvailable; i++) {
			if (nTexIDs [i] == nPossibleTextureID) {
				FreeTexture (nPossibleTextureID);	// sets nTexIDs [i] to -1...
				nNumTextures--;		// since we will add the ID again...
				break;
			}
		}
	}

	// Actually look for a new one
	int nNewTextureID;
	if (nPossibleTextureID == -1) {
		unsigned int nGLID;	
		glGenTextures (1, &nGLID);
		nNewTextureID = (int) nGLID;
	}
	else	// If the user is handle the textureIDs
		nNewTextureID = nPossibleTextureID;
	
	// find an empty slot in the TexID array
	int nIndex = 0;
	while (nTexIDs [nIndex] != -1 && nIndex < nAvailable)
		nIndex++;

	// all space exaused, make MORE!
	if (nIndex >= nAvailable) {
		int *pNewIDs = new int [nAvailable + TEXTURE_STEP];
		int i;
		
		// copy the old
		for (i = 0; i < nAvailable; i++)
			pNewIDs [i] = nTexIDs [i];
		// set the last increment to the newest ID
		pNewIDs [nAvailable] = nNewTextureID;
		// set the new to '-1'
		for (i = 1; i < TEXTURE_STEP; i++)
			pNewIDs [i + nAvailable] = -1;

		nAvailable += TEXTURE_STEP;
		delete [] nTexIDs;
		nTexIDs = pNewIDs;
	}
	else
		nTexIDs [nIndex] = nNewTextureID;

	// Welcome to our Texture Array!
	nNumTextures++;
	return nNewTextureID;
}

bool CTextureManager::CheckSize(int nDimension)
{
	// Portability issue, check your endian...

	int i = 1;
	while (i < nDimension) {
		i <<= 1;
		if (i == nDimension)
			return true;
	}

	return false;
}
