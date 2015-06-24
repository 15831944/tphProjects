// TextureManager.h: interface for the CTextureManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTUREMANAGER_H__851E88B1_3164_4207_B04D_9BD2F6329EB7__INCLUDED_)
#define AFX_TEXTUREMANAGER_H__851E88B1_3164_4207_B04D_9BD2F6329EB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define TEXMANAGER	CTextureManager::GetInstance()
#define DESTROY_TEXMANAGER	CTextureManager::DeleteInstance();

#define UBYTE unsigned char
#define INITIAL_SIZE	32	// initial size of the TexID array
#define TEXTURE_STEP	8	// how much the array grows by each time

class CTextureManager  
{
protected:
	CTextureManager();
	virtual ~CTextureManager();

public:
	static CTextureManager* GetInstance();
	static void DeleteInstance();

	int LoadTexture(const char *szFilename, int nTextureID = -1);
	int LoadTextureFromMemory(UBYTE *pData, int nWidth, int nHeight, int nBPP, int nTextureID = -1);

	void FreeTexture(int nID);
	void FreeAll();

	int	GetNumTextures();
	int GetAvailableSpace();
	int GetTexID(int nIndex);

private :
	UBYTE *LoadBitmapFile(const char *filename, int &nWidth, int &nHeight, int &nBPP);
	UBYTE *LoadTargaFile(const char *filename, int &nWidth, int &nHeight, int &nBPP);

	int GetNewTextureID(int nPossibleTextureID);	// get a new one if one isn't provided
	bool CheckSize(int nDimension);

private :
	static CTextureManager* m_pInstance;
	int nNumTextures;
	int nAvailable;				// available space in the nTexID array
	int *nTexIDs;

};

#endif // !defined(AFX_TEXTUREMANAGER_H__851E88B1_3164_4207_B04D_9BD2F6329EB7__INCLUDED_)
