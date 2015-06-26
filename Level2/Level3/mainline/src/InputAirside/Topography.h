#pragma once
#include <vector>
#include "InputAirsideAPI.h"
#include "ALTObject.h"

class CAirsideExportFile;
class INPUTAIRSIDE_API Topography
{
public:
	Topography(void);
	virtual ~Topography(void);


	//if failed,throw exception CADOException
	static void GetSurfaceList(int nTopographyID, std::vector<int>& vSurfaceID);
	static void GetStructureList(int nTopographyID, std::vector<int>& vStructrIDs );


	//if failed,throw exception CADOException
	static void GetContourList(int nTopographyID, std::vector<int>& vContourID);
	static void GetRootContourList(int nTopographyID, std::vector<int>& vContourID);

	static void ExportObjects(CAirsideExportFile& exportFile);

	void LoadTopography(int nTopographyID);

	ALTObjectList* getAirStructureList();
	ALTObjectList* getSurfaceList();
	ALTObjectList* getContourList();
protected:
	void LoadStructure(int nTopographyID);
	void LoadSurface(int nTopographyID);
	void LoadContour(int nTopographyID);

protected:
	ALTObjectList m_lstStructure;
	ALTObjectList m_lstSurface;
	ALTObjectList m_lstContour;
};
