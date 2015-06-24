// RunwayProc.cpp: implementation of the RunwayProc class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "RunwayProc.h"
#include "TaxiwayProc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RunwayProc::RunwayProc() :
	m_vBlocks(0)
{
	m_dWidth=0.0;
	m_dThreshold1=0.0;
	m_dThreshold2=0.0;
}

RunwayProc::~RunwayProc()
{

}
void RunwayProc::autoSnap(ProcessorList *procList){
	//snap all taxiways
	int nProcCount=procList->getProcessorCount();
	for(int i=0;i<nProcCount;++i)
	{
		Processor *pProc=procList->getProcessor(i);
		
		if(pProc->getProcessorType()==TaxiwayProcessor)
		{
			((TaxiwayProc *)pProc)->autoSnap(procList);
		}
	}
		
}
int RunwayProc::readSpecialField(ArctermFile& procFile)
{
	//width
	procFile.getFloat( m_dWidth );
	m_dWidth *= SCALE_FACTOR;
	//threshold 1
	procFile.getFloat( m_dThreshold1 );
	//threshold 2
	procFile.getFloat( m_dThreshold2 );

	char sBuf[256]="";
	//marking 1
	procFile.getField( sBuf,256 );
	m_sMarking1 = sBuf;
	//marking 2
	procFile.getField( sBuf, 256 );
	m_sMarking2 = sBuf;

	return TRUE;
}
int RunwayProc::writeSpecialField(ArctermFile& procFile) const
{
	//width
	procFile.writeFloat( (float)( m_dWidth/SCALE_FACTOR ) );
	//threshold 1
	procFile.writeFloat( (float) m_dThreshold1 );
	//threshold 2
	procFile.writeFloat( (float) m_dThreshold2 );
	//marking 1
	procFile.writeField( m_sMarking1.c_str() );
	//marking 2
	procFile.writeField( m_sMarking2.c_str() );

	return TRUE;
}

void RunwayProc::initServiceLocation (int pathCount, const Point *pointList)
{
    if (pathCount != 2)
        throw new StringError ("Processor must have 2 service m_location");

    m_location.init (pathCount, pointList);
}


void InsertSimpleBlock(vector<RunwayTextures::BlockType>& _vBlocks, ARCVector2& _corner1, ARCVector2& _corner2, const ARCVector2& _dir, double _dBlockLength, int _textureId)
{
	int idx = _vBlocks.size();
	_vBlocks.resize(idx+1);
	_vBlocks[idx].texture_id = _textureId;
	_vBlocks[idx].vertex_coords.resize(4);
	_vBlocks[idx].texture_coords.resize(4);
	_vBlocks[idx].vertex_coords[0] = _corner1;
	_vBlocks[idx].vertex_coords[3] = _corner2;
	_vBlocks[idx].vertex_coords[1] = _corner1 + _dBlockLength * _dir;
	_vBlocks[idx].vertex_coords[2] = _corner2 + _dBlockLength * _dir;
	_vBlocks[idx].texture_coords[0] = ARCVector2(1.0,0.0);
	_vBlocks[idx].texture_coords[3] = ARCVector2(0.0,0.0);
	_vBlocks[idx].texture_coords[1] = ARCVector2(1.0,1.0);
	_vBlocks[idx].texture_coords[2] = ARCVector2(0.0,1.0);
	_corner1 = _vBlocks[idx].vertex_coords[1];
	_corner2 = _vBlocks[idx].vertex_coords[2];
}

int InsertMarkingsBlocks(vector<RunwayTextures::BlockType>& _vBlocks, ARCVector2& _corner1, ARCVector2& _corner2, const ARCVector2& _dir, double _dBlockLength, const string& _marking)
{
	if(_marking.empty())return 0;
	const char* szMarking = _marking.c_str();
	const char* p = szMarking;
	int nRwyDir = atoi(szMarking);
	while(isdigit(*p)) {
		++p;
	}
	bool bHasLetter = (p != NULL && (*p)!='\0');

	int nBlockLevelsAdded = 0;

	int idx = _vBlocks.size();

	if(bHasLetter) {
		int nTextureId = -1;
		switch(*p) {
		case 'L':
			nTextureId = RunwayTextures::markingL;
			break;
		case 'R':
			nTextureId = RunwayTextures::markingR;
			break;
		case 'C':
			nTextureId = RunwayTextures::markingC;
			break;
		default:
			
			break;
		}
		//add letter block
		_vBlocks.resize(idx+1);
		_vBlocks[idx].texture_id = nTextureId;
		_vBlocks[idx].vertex_coords.resize(4);
		_vBlocks[idx].texture_coords.resize(4);
		_vBlocks[idx].vertex_coords[0] = _corner1;
		_vBlocks[idx].vertex_coords[3] = _corner2;
		_vBlocks[idx].vertex_coords[1] = _corner1 + _dBlockLength * _dir;
		_vBlocks[idx].vertex_coords[2] = _corner2 + _dBlockLength * _dir;
		_vBlocks[idx].texture_coords[3] = ARCVector2(0.0,0.0);
		_vBlocks[idx].texture_coords[0] = ARCVector2(1.0,0.0);
		_vBlocks[idx].texture_coords[2] = ARCVector2(0.0,1.0);
		_vBlocks[idx].texture_coords[1] = ARCVector2(1.0,1.0);
		_corner1 = _vBlocks[idx].vertex_coords[1];
		_corner2 = _vBlocks[idx].vertex_coords[2];
		idx++;

		nBlockLevelsAdded++;
	}
	if(nRwyDir<=0)return nBlockLevelsAdded;
	if(nRwyDir<10) {
		//add single digit block
		_vBlocks.resize(idx+1);
		_vBlocks[idx].texture_id = RunwayTextures::marking1C + (nRwyDir - 1);
		_vBlocks[idx].vertex_coords.resize(4);
		_vBlocks[idx].texture_coords.resize(4);
		_vBlocks[idx].vertex_coords[0] = _corner1;
		_vBlocks[idx].vertex_coords[3] = _corner2;
		_vBlocks[idx].vertex_coords[1] = _corner1 + _dBlockLength * _dir;
		_vBlocks[idx].vertex_coords[2] = _corner2 + _dBlockLength * _dir;
		_vBlocks[idx].texture_coords[3] = ARCVector2(0.0,0.0);
		_vBlocks[idx].texture_coords[0] = ARCVector2(1.0,0.0);
		_vBlocks[idx].texture_coords[2] = ARCVector2(0.0,1.0);
		_vBlocks[idx].texture_coords[1] = ARCVector2(1.0,1.0);
		_corner1 = _vBlocks[idx].vertex_coords[1];
		_corner2 = _vBlocks[idx].vertex_coords[2];
		idx++;

		nBlockLevelsAdded++;
	}
	else {
		//add double digit blocks
		ARCVector2 vRwyCenter = (_corner1 + _corner2)/2;

		int nRwyDirDigit1 = *szMarking - '0';
		int nRwyDirDigit2 = *(szMarking+1) - '0';

		_vBlocks.resize(idx+1);
		_vBlocks[idx].texture_id = RunwayTextures::marking1L + (nRwyDirDigit1 - 1);
		_vBlocks[idx].vertex_coords.resize(4);
		_vBlocks[idx].texture_coords.resize(4);
		_vBlocks[idx].vertex_coords[0] = vRwyCenter;
		_vBlocks[idx].vertex_coords[3] = _corner2;
		_vBlocks[idx].vertex_coords[1] = vRwyCenter + _dBlockLength * _dir;
		_vBlocks[idx].vertex_coords[2] = _corner2 + _dBlockLength * _dir;
		_vBlocks[idx].texture_coords[3] = ARCVector2(0.0,0.0);
		_vBlocks[idx].texture_coords[0] = ARCVector2(1.0,0.0);
		_vBlocks[idx].texture_coords[2] = ARCVector2(0.0,1.0);
		_vBlocks[idx].texture_coords[1] = ARCVector2(1.0,1.0);
		idx++;

		_vBlocks.resize(idx+1);
		_vBlocks[idx].texture_id = RunwayTextures::marking0R + nRwyDirDigit2;
		_vBlocks[idx].vertex_coords.resize(4);
		_vBlocks[idx].texture_coords.resize(4);
		_vBlocks[idx].vertex_coords[0] = _corner1;
		_vBlocks[idx].vertex_coords[3] = vRwyCenter;
		_vBlocks[idx].vertex_coords[1] = _corner1 + _dBlockLength * _dir;
		_vBlocks[idx].vertex_coords[2] = vRwyCenter + _dBlockLength * _dir;
		_vBlocks[idx].texture_coords[3] = ARCVector2(0.0,0.0);
		_vBlocks[idx].texture_coords[0] = ARCVector2(1.0,0.0);
		_vBlocks[idx].texture_coords[2] = ARCVector2(0.0,1.0);
		_vBlocks[idx].texture_coords[1] = ARCVector2(1.0,1.0);

		_corner1 = _corner1 + _dBlockLength * _dir;
		_corner2 = _corner2 + _dBlockLength * _dir;

		nBlockLevelsAdded++;
	}

	return nBlockLevelsAdded;
}


void RunwayProc::GenerateBlocks()
{
	return;

	int c = m_location.getCount();
	assert(c >= 2);
	Point p0 = m_location.getPoint(0);
	Point p1 = m_location.getPoint(1);

	//get perpedicular vector
	ARCVector2 vPerp( p1.getY()-p0.getY() , -(p1.getX()-p0.getX()));
	vPerp.Normalize();
	vPerp*=(m_dWidth/2.0);

	//get runway direction vector
	ARCVector2 vRwy(p1.getX()-p0.getX(), p1.getY()-p0.getY());
	double dRwyLength = vRwy.Magnitude()/100.0; //in meters
	vRwy.Normalize();
	vRwy *= 100.0; // vRwy is 1 meter in magnitude

	//clear blocks
	m_vBlocks.clear();
	m_vBlocks.reserve(10);
	DistanceUnit dBlockLength = 0.0;
	DistanceUnit dLengthRemaining = dRwyLength;

	ARCVector2 vRwyCorner1, vRwyCorner2, vRwyCenter;
	

	// End of the runway at point p0
	vRwyCorner1 = ARCVector2(p0.getX()+vPerp[VX], p0.getY()+vPerp[VY]);
	vRwyCorner2 = ARCVector2(p0.getX()-vPerp[VX], p0.getY()-vPerp[VY]);
	vRwyCenter = ARCVector2(p0.getX(), p0.getY());
	
	//block is threshold 1 (50 meters)
	/*double dthreshold1  = GetThreshold1()/SCALE_FACTOR;
	InsertSimpleBlock(m_vBlocks, vRwyCorner1, vRwyCorner2, vRwy, dthreshold1, RunwayTextures::threshold2);
	dLengthRemaining -= dthreshold1;*/
	

	InsertSimpleBlock(m_vBlocks,vRwyCorner1,vRwyCorner2,vRwy,50.0,RunwayTextures::threshold);
	dLengthRemaining -= 50;
	
	int nBlockLevelsAdded = InsertMarkingsBlocks(m_vBlocks, vRwyCorner1, vRwyCorner2, vRwy, 30.0, m_sMarking1);
	dLengthRemaining -= (30.0 * nBlockLevelsAdded);
	
	ARCVector2 vRwyCorner1a = vRwyCorner1;
	ARCVector2 vRwyCorner2a = vRwyCorner2;

	// End of the runway at point p1
	vRwyCorner1 = ARCVector2(p1.getX()-vPerp[VX], p1.getY()-vPerp[VY]);
	vRwyCorner2 = ARCVector2(p1.getX()+vPerp[VX], p1.getY()+vPerp[VY]);
	vRwyCenter = ARCVector2(p1.getX(), p1.getY());
	vRwy = -vRwy;

	//next block is threshold 2 (50 meters)
	/*double dthreshold2 = GetThreshold2()/SCALE_FACTOR;
	InsertSimpleBlock(m_vBlocks, vRwyCorner1, vRwyCorner2, vRwy, dthreshold2, RunwayTextures::threshold2);
	dLengthRemaining -= dthreshold2;*/

	InsertSimpleBlock(m_vBlocks,vRwyCorner1,vRwyCorner2,vRwy,50.0,RunwayTextures::threshold);
	dLengthRemaining -= 50;

	//next n = [1..3] blocks are for designation 2
	
	nBlockLevelsAdded = InsertMarkingsBlocks(m_vBlocks, vRwyCorner1, vRwyCorner2, vRwy, 30.0, m_sMarking2);
	dLengthRemaining -= (30.0 * nBlockLevelsAdded);
	
	//last block is centerline
	
	dBlockLength = dLengthRemaining;
	
	
	int idx = m_vBlocks.size();
	m_vBlocks.resize(idx+1);
	m_vBlocks[idx].texture_id = RunwayTextures::centerline;
	m_vBlocks[idx].vertex_coords.resize(4);
	m_vBlocks[idx].texture_coords.resize(4);
	m_vBlocks[idx].vertex_coords[0] = vRwyCorner1;
	m_vBlocks[idx].vertex_coords[3] = vRwyCorner2;
	m_vBlocks[idx].vertex_coords[1] = vRwyCorner2a;
	m_vBlocks[idx].vertex_coords[2] = vRwyCorner1a;
	m_vBlocks[idx].texture_coords[0] = ARCVector2(0.0,0.0);
	m_vBlocks[idx].texture_coords[3] = ARCVector2(1.0,0.0);
	m_vBlocks[idx].texture_coords[1] = ARCVector2(0.0,dBlockLength/50.0);
	m_vBlocks[idx].texture_coords[2] = ARCVector2(1.0,dBlockLength/50.0);
   
}

int RunwayProc::GetBlockCount() const
{
	return m_vBlocks.size();
}

const vector<ARCVector2>& RunwayProc::GetBlockGeometry(int _idx) const
{
	return m_vBlocks.at(_idx).vertex_coords;
}

const vector<ARCVector2>& RunwayProc::GetBlockTexCoords(int _idx) const
{
	return m_vBlocks.at(_idx).texture_coords;
}

int RunwayProc::GetBlockTextureID(int _idx) const
{
	return m_vBlocks.at(_idx).texture_id;
}

Processor* RunwayProc::CreateNewProc()
{
	Processor* pProc = new RunwayProc;
	return pProc;
}

bool RunwayProc::CopyOtherData(Processor* _pDestProc)
{
	RunwayProc *pProc = (RunwayProc *)_pDestProc;

	pProc->SetWidth(GetWidth());
	pProc->SetThreshold1(GetThreshold1());
	pProc->SetThreshold2(GetThreshold2());
	pProc->SetMarking1(GetMarking1());
	pProc->SetMarking2(GetMarking2());

	return true;
}