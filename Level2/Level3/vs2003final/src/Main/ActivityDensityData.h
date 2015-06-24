// ActivityDensityData.h: interface for the CActivityDensityData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTIVITYDENSITYDATA_H__C67ACF4F_B2AA_403D_97E3_57CA2129C9AD__INCLUDED_)
#define AFX_ACTIVITYDENSITYDATA_H__C67ACF4F_B2AA_403D_97E3_57CA2129C9AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common\term_bin.h"
#include "AnimationData.h"
#include "ActivityDensityParams.h"

class Terminal;

class CActivityDensityData  
{
public:
	CActivityDensityData();
	virtual ~CActivityDensityData();

	//unsigned char At(int x, int y, int z) { return m_pData[x + y*m_nSizeX + z*m_nSizeXY]; }
	//unsigned char At(int idx) { return m_pData[idx]; }
	//RGBQUAD ColorAt(int x, int y, int z) { return m_pColorTable[m_pData[x + y*m_nSizeX + z*m_nSizeXY]]; }
	//RGBQUAD ColorAt(int idx) { return m_pColorTable[m_pData[idx]]; }
	
	void SetSizeX(long nSizeX) { m_nSizeX = nSizeX; m_nSizeXY = m_nSizeY*m_nSizeX; }
	void SetSizeY(long nSizeY) { m_nSizeY = nSizeY; m_nSizeXY = m_nSizeY*m_nSizeX; }
	void SetSizeZ(long nSizeZ) { m_nSizeZ = nSizeZ; }
	long GetSizeX() { return m_nSizeX; }
	long GetSizeY() { return m_nSizeY; }
	long GetSizeZ() { return m_nSizeZ; }

	//RGBQUAD* GetColorTable() { return m_pColorTable; }
	//unsigned char GetColorTableSize() { return m_nColorTableSize; }

	float* Get3DData() { return m_pData; }
	float* Get2DData(int z) { return m_pData + z*m_nSizeXY; }

	BOOL BuildData(AnimationData& animData, Terminal* pTerminal, CActivityDensityParams& adParams);

protected:
	//unsigned char GetColorIdx(RGBQUAD rgbquad);


	float* m_pData;	//array representing 3d data;
	long m_nSizeX;	//width of data
	long m_nSizeY;	//height of data
	long m_nSizeZ;	//# of recordings in time dimension
	//unsigned char m_nColorTableSize;
	//RGBQUAD* m_pColorTable;

private:
	long m_nSizeXY;

};

#endif // !defined(AFX_ACTIVITYDENSITYDATA_H__C67ACF4F_B2AA_403D_97E3_57CA2129C9AD__INCLUDED_)
