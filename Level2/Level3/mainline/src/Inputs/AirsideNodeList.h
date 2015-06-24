#ifndef __AIRSIDENODE_H
#define __AIRSIDENODE_H
#include "Engine\Process.h"
#include"common\point.h"
#include "engine\proclist.h"

class CAirsideNode
{
public:
	CAirsideNode(CString &s, Point &p)
	{
		m_sLable  =  s;
		m_point = p;
	} 
	LPCTSTR GetLable()	{	return (LPCTSTR)m_sLable;	}
	Point& GetPoint()	{	return m_point;	}
	void SetPoint(Point &p)		{	m_point = p;	}
	void SetLable(CString &s)	{	m_sLable  = s;	}

private:
	Point m_point;
	CString m_sLable;
};

class CAirsideNodeList
{
public:
	CAirsideNodeList();
	void GenerateNodeList(ProcessorList *);
	std::vector<CAirsideNode>& GetNodeList(ProcessorList* );
	void SetDirty( bool bDirty = true)			{	m_bDirty = bDirty;	}

private:
	void GenerateIntersections(ProcessorList *);

	//void RunwayInterRunway(Processor* pProc1, Processor* pProc2);
	void RunwayInterTaxiway(Processor* pProc1, Processor* pProc2, bool bExchange = false);
	void TaxiwayInterTaxiway(Processor* pProc1, Processor* pProc2);
	void StandInterTaxiway(Processor* pProc1, Processor* pProc2, bool bExchange = false);

	bool m_bDirty;
	std::vector<CAirsideNode> m_vNodeList;
};
#endif