#pragma once

//model of arc port
#include "ENV_MODE.h"
#include "IEnvModeEditContext.h"

class CGuid;
class CRenderFloorList;

class ILayoutObject;
class IARCportLayoutEditContext
{
public:
	IARCportLayoutEditContext(void);
	virtual ~IARCportLayoutEditContext(void);

	virtual IEnvModeEditContext* GetCurContext()const=0;
	virtual IEnvModeEditContext* GetContext(EnvironmentMode mode)const=0;

	ITerminalEditContext* GetTerminalContext();
	IAirsideEditContext* GetAirsideContext();
	ILandsideEditContext* GetLandsideContext();

	virtual void* OnQueryDataObject(const CGuid& guid, const GUID& class_guid)=0;
	virtual bool OnQueryDataObjectList(std::vector<void*>& vData, const GUID& cls_guid)=0;
	virtual InputAirside* OnQueryInputAirside()=0;
	virtual CRenderFloorList  OnQueryAirsideFloor(int nAirportID)=0;
	virtual bool OnQueryAirportDataObjectList(int airportID,std::vector<void*>& vData, const GUID& cls_guid )=0;


	virtual CString GetTexturePath()const=0;

	virtual bool IsShowText()const=0;

	virtual bool IsEditable()const=0;

};
