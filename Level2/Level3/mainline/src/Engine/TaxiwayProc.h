// TaxiwayProc.h: interface for the TaxiwayProc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TAXIWAYPROC_H__A816D34E_BBFE_4B33_AE85_7087C9C0030F__INCLUDED_)
#define AFX_TAXIWAYPROC_H__A816D34E_BBFE_4B33_AE85_7087C9C0030F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AirfieldProcessor.h"
#include "Main\RunwayTextures.h"
#include "../Common/Point.h"
using namespace std;

class TaxiwaySegment;
class TaxiwayTrafficGraph;

class TaxiwayProc : public AirfieldProcessor  
{
	
public:
	enum TaxiwayEnum {LEFTSIDE,RIGHTSIDE};
	TaxiwayProc();
	virtual ~TaxiwayProc();
	//processor type
	virtual int getProcessorType (void) const { return TaxiwayProcessor; }
	//processor name
	virtual const char *getProcessorName (void) const { return "TAXIWAY"; }
	//processor i/o
	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile) const;

	virtual void initServiceLocation (int pathCount, const Point *pointList);
	virtual void autoSnap(ProcessorList *procList);
	void SnapTo(Processor * proc);
	// Width
	void SetWidth(double width);
	DistanceUnit GetWidth(void){return m_dWidth;}
	// Marking
	void SetMarking(const string& _sMarking) { m_sMarking = _sMarking; };
	void SetMarking(LPCTSTR _psMarking) { m_sMarking =  _psMarking; };
	const string& GetMarking() const { return m_sMarking; };
	

	// Render
	//int GetBlockCount() const;										//returns the number of blocks
	//int GetBlockTextureID(int _idx) const;							//returns a texture id for the block
	//const vector<ARCVector2>& GetBlockGeometry(int _idx) const;		//returns a vector of 4 2d vertices
	//const vector<ARCVector2>& GetBlockTexCoords(int _idx) const;	//returns a vector of 4 2d tex coords

	void GenBlockFromPath(Path * path ,DistanceUnit width,std::vector<ARCPoint2>& outline);
	void initSegments();
	void CalculateSegment();

	struct TAXIWAYSEGMENT {		
		ARCVector2 ptdir;
		DistanceUnit length_right;
		DistanceUnit length_left;
		TAXIWAYSEGMENT(){length_right=length_left=1000.0;}
	};

	virtual Processor* CreateNewProc();
	virtual bool CopyOtherData(Processor* _pDestProc);

public:
	//void RecalcGeometry();
	
	void MoveSidePoint(DistanceUnit dx,DistanceUnit dy,UINT ceterPointID,TaxiwayEnum whichside);
	void MoveCenterPoint(DistanceUnit dx,DistanceUnit dy,UINT centerPointID);
	void removePoint(UINT ptID);
	void MoveMarkPosition(const Point & newPtPos);

	void SetMarkingPosition(UINT ptID, double relatePos);
	void getMarkingPosition(Point & startPos, double  & rotateAngle );

	//add new point after point(id) 
	void AddNewPoint(const Point& newPtPos );

	bool EditMode(){return m_bEditable;}
	void setEditMode(bool _b=true){m_bEditable=_b;}
	std::vector< TAXIWAYSEGMENT > m_vSegments;	
	
	enum DirectionType{ Bidirection=0, PositiveDirection, NegativeDirection};
	
	class TaxiwaySegment
	{
	public:
		TaxiwaySegment():emType(Bidirection){};
		virtual ~TaxiwaySegment(){}; 
		TaxiwaySegment(const TaxiwaySegment& rhs)
		{
			StartPoint = rhs.StartPoint;
			EndPoint = rhs.EndPoint;
			emType = rhs.emType;
		}

		Point StartPoint, EndPoint;
		DirectionType emType;
	};
	typedef std::vector<TaxiwaySegment> SegmentList;
	typedef std::vector<TaxiwaySegment>::iterator SegmentItr;

	
	SegmentList& GetSegmentList();
	SegmentList& GetSegmentListDirect(){	return m_segmentList; }
	TaxiwaySegment GetSegment(int idx){	ASSERT(idx>0&&idx<=(int)m_segmentList.size());	return m_segmentList[idx]; }
	TaxiwaySegment SetSegment(TaxiwaySegment seg, int idx){ ASSERT(idx>0&&idx<=(int)m_segmentList.size()); m_segmentList[idx] = seg; }
	void SetDirty(BOOL bDirty){	m_bDirty = bDirty; }
	
	void SetTrafficGraph(TaxiwayTrafficGraph* pTrafficGraph){	m_pTrafficGraph = pTrafficGraph; }

private:	
	string m_sMarking;
	DistanceUnit m_dWidth;

	int m_imarkPosAfterPtID;   //mark draw after the point(ID)
	double m_dmarkPosRelatePos; //relative position of the line(ID ,ID+1)
	
	bool m_bEditable;

	std::vector<Point> m_intersectionPointList;
	std::vector<DirectionType> m_directionList;
	SegmentList m_segmentList;
	TaxiwayTrafficGraph* m_pTrafficGraph;
	BOOL m_bDirty;
};


#endif // !defined(AFX_TAXIWAYPROC_H__A816D34E_BBFE_4B33_AE85_7087C9C0030F__INCLUDED_)
