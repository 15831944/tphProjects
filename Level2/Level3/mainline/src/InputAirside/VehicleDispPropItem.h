// AircraftDispPropItem.h: interface for the CAircraftDispPropItem class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VEHICLEDISPPROPITEM_H__
#define __VEHICLEDISPPROPITEM_H__

#pragma once

#include "../Database/DBElement.h"
#include "../InputAirside/AirsideImportExportManager.h"
#include "../InputAirside/vehiclespecificationitem.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API CVehicleDispPropItem   : public DBElement
{
public:
	enum ENUM_LINE_TYPE { 
		ENUM_LINE_1PTSOLID=0, ENUM_LINE_2PTSOLID, ENUM_LINE_3PTSOLID,
		ENUM_LINE_1PTDASH, ENUM_LINE_2PTDASH, ENUM_LINE_3PTDASH,
		ENUM_LINE_1PTDOT, ENUM_LINE_2PTDOT, ENUM_LINE_3PTDOT
	};

	CVehicleDispPropItem(int nProjID);
	virtual ~CVehicleDispPropItem(void);

	int GetProjectID(void);
	void SetParentID(int nParentID);
	int GetParentID(void);
	const CVehicleSpecificationItem * GetVehicleType(void) const;
	BOOL IsVisible() const ;
	const std::pair<int, int>& GetShape(void) const; 
	COLORREF GetColor() const ;
	BOOL IsLeaveTrail() const ;
	CVehicleDispPropItem::ENUM_LINE_TYPE GetLineType() const ;
	double GetDensity()const ;
	void SetDensity(double density_);
	void SetVehicleType( const CVehicleSpecificationItem& _vehicleItem );
	void SetVisible( BOOL _isVisible );
	void SetShape( const std::pair<int, int>& _shape );
	void SetColor( COLORREF _colorRef );
	void SetLeaveTrail( BOOL _isLeaveTrail );
	void SetLineType( CVehicleDispPropItem::ENUM_LINE_TYPE _linetype );
	friend bool operator == (const CVehicleDispPropItem & itemVehicle1,const CVehicleDispPropItem& itemVehicle2);
protected:
	virtual void InitFromDBRecordset(CADORecordset& adoRecordset);
	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
	virtual void GetSelectSQL(int nID,CString& strSQL)const;
public:
	static void GetVehicleDispPropItemIDListByParentID(int nParentID,std::vector<int> & vrIDList);
public:
	virtual void ExportData(CAirsideExportFile& exportFile);
	virtual void ImportData(CAirsideImportFile& importFile);

protected:
	int m_nProjID;
	int m_nParentID;
	CVehicleSpecificationItem m_vehicleItem;
	BOOL m_bVisible;
	std::pair<int, int> m_shape;
	enum ENUM_LINE_TYPE m_linetype;
	COLORREF m_colorRef;
	BOOL m_bIsLeaveTrail;
	double m_dDensity;	
};

#endif //__VEHICLEDISPPROPITEM_H__
