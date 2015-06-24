#include "StdAfx.h"
#include "carrierSpaceDiagnosis.h"
#include "carrierSpace/carrierDefs.h"
#include "MobileAgent.h"

carrierSpaceDiagnosisHelper::carrierSpaceDiagnosisHelper()
:mEnableDiagnosis(false)
{
	mFilePath.append(_T("c:\\OnBoardSimEngine\\"));
}

carrierSpaceDiagnosisHelper::~carrierSpaceDiagnosisHelper(void)
{
}

void carrierSpaceDiagnosisHelper::traceDeckGroundLayout(carrierDeck* pDeck)
{
	if(!mEnableDiagnosis)
		return;

}

void carrierSpaceDiagnosisHelper::registerDiagnosisAgent(int nTargetID, MobileAgent* pMobAgent)
{
	if(!mEnableDiagnosis || nTargetID < 0)
		return;
	if(nTargetID != pMobAgent->getID())
		return;


}



//--------------------------------------------------------------------------------
//C24BitsBmp

#include <fstream>
namespace OBDiagnosis
{
	C24BitsBmp::C24BitsBmp(BYTE* pData, int nwidth, int nheight)
		: m_pBits(pData)
		, m_nWidth(nwidth)
		, m_nHeight(nheight)
		, m_nBitsCount(24)
		, m_bNeedRelease(false)
	{
		m_nStride  = WIDTHBYTEEX( m_nWidth * m_nBitsCount); // to be modified as the m_nBitsCount and 4 BYTEs aligned;
	}

	C24BitsBmp::C24BitsBmp(LPCTSTR lpszFileName)
		: m_bNeedRelease(true)
		, m_nBitsCount(24)
	{
		using namespace std;

		if(m_pBits != NULL)
			return;

		ifstream file(lpszFileName, ios::in|ios::binary);
		if(file.bad())
			return ;

		BITMAPFILEHEADER bfh;
		file.read((char*)&bfh, sizeof(BITMAPFILEHEADER));

		BITMAPINFOHEADER bih;
		file.read((char*)&bih, sizeof(BITMAPINFOHEADER));
		m_nWidth = bih.biWidth;
		m_nHeight = bih.biHeight;
		m_nStride  = WIDTHBYTEEX( m_nWidth * m_nBitsCount);   

		long cur_pos = file.tellg(); 
		file.seekg( ios::beg, ios::end ); 
		long nLen = file.tellg(); 
		file.seekg( cur_pos, ios::beg ); 

		nLen -= (sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER));

		if (m_bNeedRelease)
			delete[] m_pBits;

		m_pBits = new BYTE[nLen];

		file.read((char*)m_pBits, nLen);
		file.close();
	}

	C24BitsBmp::~C24BitsBmp()
	{
		if (m_bNeedRelease)
			delete[] m_pBits;
	}

	void C24BitsBmp::SavetoFile(LPCTSTR lpszFileName)
	{
		using namespace std;
		ofstream file(lpszFileName, ios::out|ios::binary);
		if (!file)
			return;

		int nBufferSize = m_nStride * m_nHeight;

		// write out the file header
		BITMAPFILEHEADER bfh;
		memset( &bfh, 0, sizeof( bfh ) );
		bfh.bfType = 'MB';
		bfh.bfSize = sizeof( bfh ) + nBufferSize + sizeof( BITMAPINFOHEADER );
		bfh.bfOffBits = sizeof( BITMAPINFOHEADER ) + sizeof( BITMAPFILEHEADER );

		DWORD dwWritten = 0;
		file.write((const char*)&bfh, sizeof(bfh));

		// and the bitmap format
		BITMAPINFOHEADER bih;
		memset( &bih, 0, sizeof( bih ) );
		bih.biSize = sizeof( bih );
		bih.biWidth = m_nWidth;
		bih.biHeight = m_nHeight;
		bih.biPlanes = 1;
		bih.biBitCount = m_nBitsCount;

		dwWritten = 0;
		file.write((const char*)&bih, sizeof(bih));

		// and the bits themselves
		file.write((const char*)m_pBits, nBufferSize);
		file.close();
	};

	//--------------------------------------------------------------------------------
	//

};







