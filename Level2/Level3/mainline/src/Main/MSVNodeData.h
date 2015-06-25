#pragma once


namespace MSV
{
	enum enumNodeType
	{
		NodeType_Normal =0,
		NodeType_ObjectRoot,
		NodeType_ObjectGroup,
		NodeType_Object,
		NodeType_Dlg,
		NodeType_Airport,
		NodeType_Airspace,
		NodeType_Topography,
		NodeType_Level,
		NodeType_AircraftFacility,
		NodeType_ManeuverSurface,
		NodeType_VehicleFacility,
		NodeType_ControlDevice,
		NodeType_LevelRoot,
	};
	
	class CNodeDataAllocator;
	class CNodeData
	{
		friend class CNodeDataAllocator;
	public:
		CNodeData(enumNodeType nodetype,CNodeDataAllocator* pCreator);
		~CNodeData();
	protected:

	public:
		//node type
		enumNodeType nodeType;// the node type,enum 
		int nSubType; //mostly the enum dlg, or the object type
		//menu id
		UINT nIDResource;//the menu id

		DWORD dwData;//mostly,the object id
		CString strDesc;//description
		bool bHasExpand;//the Item is expand or not, working in loading object
		int nOtherData; //mostly, The airport id, airspace id, topography id

		CNodeDataAllocator* mpCreator;

	};

	class CNodeDataAllocator{
	public:
		virtual CNodeData* allocateNodeData(enumNodeType t);
		~CNodeDataAllocator();
		
		void remove(CNodeData* d);
	protected:
		std::vector<CNodeData*> vDatas;		

	};
}