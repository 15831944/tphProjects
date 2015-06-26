#pragma once
enum SceneNodeType
{
	SceneNode_None = -1,

	SceneNode_RefParent = 0, // hold no data, reference to parent node when query
	SceneNode_useGUID, // hold a GUIDQueryData data type, query data object and 3d object by this CGuid
	SceneNode_LayoutObject,//scene layout object node
	SceneNode_Other,//other node, use self specified data
	// typically a model object
	SceneNode_Pax, // hold pax id
	SceneNode_Flight, // hold flight id
	SceneNode_Vehicle, // hold vehicle id

	SceneNode_Count,
};