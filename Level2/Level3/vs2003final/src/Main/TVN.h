// TVN.h: interface for the CTVN class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TVN_H__B5C634AB_BE0E_4068_8120_B4D6255F2D2F__INCLUDED_)
#define AFX_TVN_H__B5C634AB_BE0E_4068_8120_B4D6255F2D2F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TVNode.h" 

/*
class CFloorNode : public CTVNode
{
	DECLARE_DYNAMIC( CFloorNode )
public:
	CFloorNode(CFloor2* pFloor);
	virtual ~CFloorNode();

	CFloor2* GetFloor() { return m_pFloor; }

protected:
	CFloorNode();
	void SetDefaultName();

private:
	CFloor2* m_pFloor;
};
*/

class CAreaNode : public CTVNode
{
	DECLARE_DYNAMIC( CAreaNode )
public:
	CAreaNode(int idx);
	virtual ~CAreaNode();

	int GetIdx() { return m_nIdx; }
	void SetIdx(int _idx) { m_nIdx = _idx; }

protected:
	CAreaNode();

private:
	int m_nIdx;
};

class CPortalNode : public CTVNode
{
	DECLARE_DYNAMIC( CPortalNode )
public:
	CPortalNode(int idx);
	virtual ~CPortalNode();

	int GetIdx() { return m_nIdx; }
	void SetIdx(int _idx) { m_nIdx = _idx; }

protected:
	CPortalNode();

private:
	int m_nIdx;
};

/*
class CProcessorNode : public CTVNode
{
	DECLARE_DYNAMIC( CProcessorNode )
public:
	CProcessorNode(CProcessor2* pProc);
	virtual ~CProcessorNode();

	CProcessor2* GetProcessor() { return m_pProc; }

protected:
	CProcessorNode();

private:
	CProcessor2* m_pProc;
};
*/

#endif // !defined(AFX_TVN_H__B5C634AB_BE0E_4068_8120_B4D6255F2D2F__INCLUDED_)
