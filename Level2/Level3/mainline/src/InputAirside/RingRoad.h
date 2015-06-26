#pragma once
#include "../Common/Path2008.h"
#include "..\Database\ADODatabase.h"
#include "InputAirsideAPI.h"

class CAirsideExportFile;
class CAirsideImportFile;

class INPUTAIRSIDE_API RingRoad
{
public:
	RingRoad(void);
	~RingRoad();

public:
	CPath2008 GetPath() const;
	void SetPath(CPath2008 path);

	CPath2008 GetSmoothPath() const;

	int GetOffset(){ return m_nOffset; }
	void SetOffset(int nOffset){ m_nOffset = nOffset; }

	void ResetPath();

	void InitServiceLocation (int pathCount, const CPoint2008 *pointList);

	void ReadData(int nFltDetailsID);
	void SaveData(int nFltDetailsID);
	void UpdateData(int nFltDetailsID);
	void DeleteData();

	void ConvertPathIntoLocalCoordinate(const CPoint2008& pt, const double& dDegree);

	void ExportData(CAirsideExportFile& exportFile, int nFltTypeDetailsItemID);
	void ImportData(CAirsideImportFile& importFile, int nFltTypeDetailsItemID);

protected:
	int			m_nID;
	int			m_nPathID;
	CPath2008		m_path;
	int         m_nOffset;
};


