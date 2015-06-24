#pragma once
/**
* Debug helper Class to provide diagnosis functionality in carrier space.
*
*/
#include "../ServiceObject.h"

class MobileAgent;
class carrierDeck;
class carrierSpaceDiagnosisHelper : public ServiceObject < carrierSpaceDiagnosisHelper>
{
public:
	carrierSpaceDiagnosisHelper();
	~carrierSpaceDiagnosisHelper(void);
	
	void enableDiagnosis(){	mEnableDiagnosis = true; }
	void traceDeckGroundLayout(carrierDeck* pDeck);
	void registerDiagnosisAgent(int nTargetID, MobileAgent* pMobAgent);

private:
	bool mEnableDiagnosis;
	std::string mFilePath;
};


carrierSpaceDiagnosisHelper* carrierSpaceDiagnosis();
AFX_INLINE carrierSpaceDiagnosisHelper* carrierSpaceDiagnosis()
{
	return carrierSpaceDiagnosisHelper::GetInstance();

}

#define WIDTHBYTEEX(bits)    (((bits) + 31) / 32 * 4)
namespace OBDiagnosis
{
	//--------------------------------------------------------------------------------
	//C24BitsBmp

	class C24BitsBmp
	{
		int m_nWidth;
		int m_nHeight;
		const int m_nBitsCount;
		int m_nStride;
		BYTE* m_pBits;
		bool m_bNeedRelease;

	public:
		C24BitsBmp(BYTE* pData, int nwidth, int nheight);
		C24BitsBmp(LPCTSTR lpszFileName);
		~C24BitsBmp();

		BYTE GetPixelR(int x, int y)
		{
			return m_pBits[3 * x + (m_nHeight - 1 - y) * m_nStride + 2];
		};

		BYTE GetPixelG(int x, int y)
		{
			return m_pBits[3 * x + (m_nHeight - 1 - y) * m_nStride + 1];
		};

		BYTE GetPixelB(int x, int y)
		{
			return m_pBits[3 * x + (m_nHeight - 1 - y) * m_nStride ];
		};

		BYTE* GetpBits()
		{
			return m_pBits; 
		}

		void SavetoFile(LPCTSTR lpszFileName);
	};
};
