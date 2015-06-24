// ActivityDensityData.cpp: implementation of the CActivityDensityData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "../common/UnitsManager.h"
#include "ActivityDensityData.h"
#include "engine\Terminal.h"
#include "common\ProgressBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CActivityDensityData::CActivityDensityData()
{
	m_pData = NULL;
	//m_pColorTable = NULL;
	m_nSizeX = m_nSizeY = m_nSizeZ = m_nSizeXY = 0;
	//m_nColorTableSize = 0;
}

CActivityDensityData::~CActivityDensityData()
{
	if(m_pData) {
		delete [] m_pData;
		m_pData = NULL;
	}
}

BOOL CActivityDensityData::BuildData(AnimationData& animData, Terminal* pTerminal, CActivityDensityParams& adParams)
{
	ARCVector2 subdivs = adParams.GetAOISubdivs();
	m_nSizeX = static_cast<int>(subdivs[VX]);
	m_nSizeY = static_cast<int>(subdivs[VY]);
	m_nSizeZ = (animData.nAnimEndTime - animData.nAnimStartTime)/6000;
	m_nSizeXY = m_nSizeX*m_nSizeY;

	CProgressBar bar( "Calculating Activity Densities...", 100, m_nSizeZ, TRUE );

	m_pData = new float[m_nSizeX*m_nSizeY*m_nSizeZ];

	long nTimeStep = (animData.nAnimEndTime - animData.nAnimStartTime) / m_nSizeZ;
	const ARCVector2& v1 = adParams.GetAOISize();
	const ARCVector2& v2 = adParams.GetAOISubdivs();
	double dSpaceStepX = v1[VX] / v2[VX];
	double dSpaceStepY = v1[VY] / v2[VY];
	const ARCVector2 vOrigin(adParams.GetAOICenter() - 0.5*v1); //center - 1/2*size;
	double dArea = dSpaceStepX*dSpaceStepY/10000; // in sq. m

	unsigned int* pBins = new unsigned int[m_nSizeXY];

	for(long z=0; z<m_nSizeZ; z++) {
		long nTime = animData.nAnimStartTime + z*nTimeStep;
		//memset(m_pData+z*m_nSizeXY,0,sizeof(float)*m_nSizeXY);
		memset(pBins,0,m_nSizeXY*sizeof(unsigned int));


		long nPaxCount = pTerminal->paxLog->getCount();

       	CProgressBar bar1( "deal one pax...", 100, nPaxCount, TRUE ,1);
		
		for(long ii=0; ii<nPaxCount; ii++) {

			MobLogEntry element;
			pTerminal->paxLog->getItem(element,ii);
			const MobDescStruct& pds = element.GetMobDesc();
		
			if(pds.startTime <= nTime && pds.endTime >= nTime) {

				element.SetOutputTerminal(pTerminal);
				element.SetEventLog(pTerminal->m_pMobEventLog);

			//	long nEventCount = element.getCount();

			//******************************************************   modify by matt 20040429
			
			/*
				long nextIdx = -1;
													for(long jj=0; jj<nEventCount; jj++) {
														MobEventStruct pes = element.getEvent(jj);
														long attime = pes.time;
														if(attime >= nTime) {
															nextIdx = jj;
															break;
														}
									
										}*/
			
				MobEventStruct pesB;   
				MobEventStruct pesA ;  
				
				
				bool bRet = element.getEventsAtTime(nTime,pesA,pesB);
			
			//	if(nextIdx > 0) {
				if(bRet) {
					//MobEventStruct pesB = element.getEvent(nextIdx);
				   // MobEventStruct pesA = element.getEvent(nextIdx-1);

					//3)interpolate position based on the two PaxEventStructs and the time
					int w = pesB.time - pesA.time;
					int d = pesB.time - nTime;
					double r = ((double) d) / w;
					float xpos = static_cast<float>((1-r)*pesB.x + r*pesA.x);
					float ypos = static_cast<float>((1-r)*pesB.y + r*pesA.y);
					int zpos = (int) pesB.z/100;

					//if(zpos == adParams.GetAOIFloor()) {
					// modified by kevin
					// do not know why?
					// mybe the initialize floor number is error...
					// should be confirmed.
					if(zpos == adParams.GetAOIFloor()) {

						//add pax to proper bin
						xpos -= static_cast<float>(vOrigin[VX]);
						ypos -= static_cast<float>(vOrigin[VY]);
						int xcoord = static_cast<int>(xpos/dSpaceStepX);
						int ycoord = static_cast<int>(ypos/dSpaceStepY);
						int nBinIdx = xcoord+ycoord*m_nSizeX;
						if(nBinIdx < 0 || nBinIdx >= m_nSizeXY)
							continue;
						ASSERT(nBinIdx >= 0 && nBinIdx < m_nSizeXY);
						//m_pData[nBinIdx + z*m_nSizeXY] += dAreaInv;
						pBins[nBinIdx]++;
						//// TRACE("bin #(%d,%d) = %d\n",xcoord,ycoord,pBins[nBinIdx]);
					}
				}
			}
			bar1.StepIt();
		}
		bar1.SetPos(0);

		for(ii=0; ii<m_nSizeXY; ii++) {
			if(pBins[ii] != 0)
				m_pData[ii + z*m_nSizeXY] = static_cast<float>(pBins[ii]/dArea);
			else
				m_pData[ii + z*m_nSizeXY] = 0;
		}
		bar.StepIt();
	}

	delete [] pBins;
	bar.SetPos(100);

	return TRUE;
}

/*
unsigned char CActivityDensityData::GetColorIdx(RGBQUAD rgbquad)
{
	for(int i=0; i<m_nColorTableSize; i++) {
		if (m_pColorTable[i].rgbBlue == rgbquad.rgbBlue &&
			m_pColorTable[i].rgbGreen == rgbquad.rgbGreen &&
			m_pColorTable[i].rgbRed == rgbquad.rgbRed)
			return i;
	}
	return 0;
}
*/