#pragma once
#include <Common/CADInfo.h>
#include <Common/pollygon.h>
#include <OpenDWGToolKit/CADFileLayer.h>
#include <common/Grid.h>

struct CVisibleRegion {
	CVisibleRegion(const CString& _sName);
	CString sName;
	Pollygon polygon;
	int m_ID ;
	void ReadPathFromDB() ;
	void WritePathToDB() ;
	static void WriteDataToDB(int _paeentID,std::vector<CVisibleRegion*>* RegionContainer,BOOL Isvisible) ;
	static void ReadDataFromDB(int _paeentID,std::vector<CVisibleRegion*>* RegionContainer,BOOL Isvisible) ;
	//std::vector<Pollygon> vTessPoly;


protected:
	static void GetDataSet(CADORecordset& _set , std::vector<CVisibleRegion*>* RegionContainer) ;
	static void WriteData(CVisibleRegion* _region,int _parentID,BOOL Isvisible) ;
};

class CAlignLine
{
public:
	ARCVector2& getBeginPoint() { return beg_point; }
	ARCVector2& getEndPoint()   { return end_point; }
	void  setBeginPoint(const ARCVector2& point) { beg_point = point; }
	void  setEndPoint  (const ARCVector2& point) { end_point = point; }

private:
	ARCVector2 beg_point;
	ARCVector2 end_point;
};



class CBaseFloor : public CCADFileContent
{
public:
	CBaseFloor();

	double RealAltitude() const { return m_dRealAltitude; }
	void RealAltitude(double dAlt) { m_dRealAltitude = dAlt; }
	double Thickness() const { return m_dThickness; }
	void Thickness(double dThickness) { m_dThickness = dThickness;m_bVRDirtyFlag=TRUE; }
	BOOL IsVisible() const { return m_bIsVisible; }
	void IsVisible(BOOL bVis) { m_bIsVisible = bVis; }
	BOOL IsActive() const { return m_bIsActive; }
	void IsActive(BOOL bAct) { m_bIsActive = bAct; }
	double Altitude() const { return m_dVisualAltitude; }
	void Altitude(double dAlt) { m_dVisualAltitude = dAlt; }


	CGrid* GetGrid() { return &m_Grid; }
	void SetGrid(const CGrid& grid) { m_Grid = grid; }
	BOOL IsGridVisible() const { return m_Grid.bVisibility; }
	void IsGridVisible(BOOL bVis) { m_Grid.bVisibility = bVis; }
	//double GridSpacing() { return m_Grid.dSpacing; }

	BOOL IsOpaque() const { return m_bIsOpaque; }
	void IsOpaque(BOOL b) { m_bIsOpaque = b; }

	BOOL IsShowMap() const { return m_bIsShowMap; }
	void IsShowMap(BOOL b) { m_bIsShowMap = b; }

	void InvalidateTexture() { m_bIsTexValid = FALSE; }

	BOOL UseVisibleRegions() const { return m_bUseVisibleRegions; }
	void UseVisibleRegions(BOOL bVal) { m_bUseVisibleRegions = bVal; }
	std::vector<CVisibleRegion*>* GetVisibleRegionVectorPtr() { return &m_vVisibleRegions; }
	std::vector<CVisibleRegion *> * GetInVisibleRegionVectorPtr() { return &m_vInVisibleRegions; }
	void DeleteAllVisibleRegions();
	void DeleteAllInVisibleRegions();
	void InvalidateVisibleRegions() { m_bVRDirtyFlag = TRUE; }

	const ARCVector2& GetFloorOffset() const { return m_vOffset; }
	void SetFloorOffset( const ARCVector2& _v2D ){ m_vOffset = _v2D; }

	ARCVector2 m_vOffset;  //floor offset	


	CCADInfo m_CadFile;		//CAD data (offset, scale, rotation, filename)
	double MapRotation() const { return m_CadFile.dRotation; }
	void MapRotation(double dRot) { m_CadFile.dRotation = dRot; m_bMapdirtyFlag=TRUE;}
	const ARCVector2& MapOffset() const { return m_CadFile.vOffset; }
	void MapOffset(const ARCVector2& vOffset) { m_CadFile.vOffset = vOffset; m_bMapdirtyFlag=TRUE;}
	double MapScale() const { return m_CadFile.dScale; }
	void MapScale(double dScale) { m_CadFile.dScale = dScale;m_bMapdirtyFlag=TRUE;  }
	const CString& MapFileName() const { return m_CadFile.sFileName; }
	const CString& MapPathName() const { return m_CadFile.sPathName; }
	void MapFileName(const CString& sFileName);
	void MapPathName(const CString& sPathName);

	virtual BOOL LoadCAD();

	//------------------------------------------------------------
	// Align markers
	void UseMarker(BOOL bVal) { m_bUseMarker = bVal; }
	BOOL UseMarker() const { return m_bUseMarker; }
	ARCVector2& GetMarkerLocation(ARCVector2& vLoc)  { vLoc = m_vMarkerLoc; return m_vMarkerLoc ;}
	ARCVector2& GetMarkerLocation() { return m_vMarkerLoc; }
	void SetMarkerLocation(const ARCVector2& vLoc) { m_vMarkerLoc = vLoc; }

	BOOL UseAlignLine() {return use_AlignLine ;}
	void UseAlignLine(BOOL res) { use_AlignLine = res ;}

	void SetAlignLine(const ARCVector2& beg_point , const ARCVector2& end_point ) { m_vAlignLine.setBeginPoint(beg_point);m_vAlignLine.setEndPoint(end_point) ; }
	void SetAlignLine(CAlignLine& line) {m_vAlignLine.setBeginPoint(line.getBeginPoint()) ; m_vAlignLine.setEndPoint(line.getEndPoint()) ;}
	CAlignLine& GetAlignLine()  { return m_vAlignLine; }
	//------------------------------------------------------------


	static bool CompareFloors(const CBaseFloor* _lhs, const CBaseFloor* _rhs)
	{
		return _lhs->Altitude() < _rhs->Altitude();
	}

public:
	BOOL IsExtentsSane() const; // sanity check on extents
	BOOL IsTextureValid()const{ return m_bIsTexValid ; }

	CGrid m_Grid;
	std::vector<CVisibleRegion*> m_vVisibleRegions;
	std::vector<CVisibleRegion*> m_vInVisibleRegions;
	BOOL m_bIsTexValid;		//the texture info for the floor is valid and up to date

	const CString& GetUID()const{ return m_sUUID; }
	void SetUID(const CString& sID){ m_sUUID = sID; }
protected:
	//options
	double m_dVisualAltitude;
	double m_dRealAltitude;
	double m_dThickness;	//floor thickness in arcterm units (cm)

	BOOL m_bIsActive;		//floor is the active floor
	BOOL m_bIsVisible;		//floor is visible
	BOOL m_bIsOpaque;       //thickness on off
	BOOL m_bIsShowMap;
	BOOL m_bUseVisibleRegions;
	BOOL m_bVRDirtyFlag;


	//deck
	BOOL m_bUseMarker;
	ARCVector2 m_vMarkerLoc;
	BOOL use_AlignLine;
	CAlignLine  m_vAlignLine;

	CString m_sUUID;
};

