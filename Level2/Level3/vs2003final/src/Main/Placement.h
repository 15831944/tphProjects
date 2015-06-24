// Placement.h: interface for the CPlacement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLACEMENT_H__F9560AB4_FFFA_4EA9_BC10_81520E5BC613__INCLUDED_)
#define AFX_PLACEMENT_H__F9560AB4_FFFA_4EA9_BC10_81520E5BC613__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common\dataset.h"
#include "engine\proclist.h"
#include "Processor2.h"
#include <CommonData/ObjectDisplay.h>
#include "PipeDisplay.h"

class CPortals;
class CAreas;
class WallShapeList;
class CStructureList;
class CPipeDataSet;

class CPlacement : public DataSet  
{
public:
	CPlacement();
	CPlacement(InputFiles fileType);
	virtual ~CPlacement();
	
	CPROCESSOR2LIST m_vUndefined;
	CPROCESSOR2LIST m_vDefined;



    virtual void clear();

	virtual void readData (ArctermFile& p_file);
	virtual void writeData (ArctermFile& p_file) const;

	virtual void readObsoleteData ( ArctermFile& p_file );
	void readData2_2( ArctermFile& p_file);
	void readData2_3( ArctermFile& p_file);

	virtual const char *getTitle (void) const { return "Placement Data"; }
    virtual const char *getHeaders (void) const{  return "Shape,locX,locY,locZ,scaleX,scaleY,scaleZ,rotation,floor,processor,displayproperties"; }

	virtual void BuildFromProcList(ProcessorList* _pProcList, CProcessor2::CProcDispProperties& _dispProp);
	void ReplaceProcssor( Processor* _pOldProc, Processor* _pNewProc );
	
	//refresh the associated processor from the name read from file
	void RefreshAssociateProcs(ProcessorList  * _pProcList);


	//group operations
	int GetGroupCount(){ return (int)m_vGroupList.size(); }
	void AddGroup(CObjectDisplayGroup* pObjGroup){ m_vGroupList.push_back(pObjGroup); }
	void RemoveGroup(CObjectDisplayGroup* pObjGroup);
	
	//pipe operation
	CPipeDisplay * GetPipeDislplay(CPipeDataSet* pPipeData, int index);
	void DeletePipe(CPipeDisplay* pPipeDisplay);
	
	CObjectDisplay* GetStructureDisplay( CStructureList* pStructureList,int nIndex);
	void DeleteStructureDisplay(CObjectDisplay *pObjectDisplay);

	CObjectDisplay* GetWallShapeDisplay(WallShapeList *pWallShapes, int nIndex);
	void DeleteWallShapeDisplay(CObjectDisplay *pObjectDisplay);

	CObjectDisplay* GetAreaDisplay(CAreas *pAreas, int nIndex);
	void DeleteAreaDisplay(CObjectDisplay *pObjectDisplay);

	CObjectDisplay* GetPortalDisplay(CPortals *pPortals, int nIndex);
	void DeletePortalDisplay(CObjectDisplay *pObjectDisplay);


protected:	
	void writeProc( CProcessor2* _pProc2, ArctermFile& p_file ) const;
	InputFiles m_fileType;


	CPipeDisplayList m_vPipeList;
	CObjectDisplayList m_vStructureList;
	CObjectDisplayList m_vWallShapeList;
	CObjectDisplayList m_vAreaList;
	CObjectDisplayList m_vPortalList;
	std::list<CObjectDisplayGroup*> m_vGroupList; //group of the pipes, areas, structures

};

#endif // !defined(AFX_PLACEMENT_H__F9560AB4_FFFA_4EA9_BC10_81520E5BC613__INCLUDED_)
