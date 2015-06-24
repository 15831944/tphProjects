// Processor2.h: interface for the CProcessor2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSOR2_H__45421F3D_FD87_4CE8_9927_012440427A28__INCLUDED_)
#define AFX_PROCESSOR2_H__45421F3D_FD87_4CE8_9927_012440427A28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
#include "GL\gl.h"
#include "common\ARCVector.h"
#include "common\ARCColor.h"
#include "engine\process.h"
#include "engine\ProcGroupIdxTree.h"
#include ".\processorpathcontrolpoint.h"
#include <CommonData/ObjectDisplay.h>
#include "Common/ALTObjectID.h"
#include <CommonData/Processor2Base.h>
//
class CFloor2;
class CShape;
class Terminal;
class CTermPlanDoc;
class C3DView;
class ProcessorRenderer;
class CGuid;

class CProcessor2 : public CProcessor2Base
{
private:
	CProcessor2(const CProcessor2&); //disallow copy constructor
	CProcessor2& operator=(const CProcessor2&); //disallow assignment

public:
	//void* m_pGroup;
	
	//constructors - destructor
	CProcessor2();
	virtual ~CProcessor2();

	GLuint GetSelectName() const { return m_nSelectName; }
	void SetSelectName(GLuint nSelName) { m_nSelectName = nSelName; }

	
	virtual void DrawOGL(C3DView * _pView ,double dAlt, BOOL* pbDO, UINT nDrawMask);
	void DrawBB(double dAlt, BOOL bFirst = FALSE);
	virtual void DrawSelectOGL(CTermPlanDoc* pDoc, double* dAlt);

	Processor* GetProcessor()const { return m_pProcessor; }
	virtual ProcessorProp* GetProcessorProp() const { return GetProcessor();}
	void SetProcessor( Processor* _pProc );

	virtual void OffsetProperties(double dx, double dy);
	virtual void RotateProperties(double dr, const ARCVector3& point);
	void MirrorProperties(const std::vector<ARCVector3>& vLine);

	ProcessorRenderer * GetRenderer();

	virtual CProcessor2 *GetCopy();

	virtual bool CopyProcToDest(CTermPlanDoc* _pSrcDoc, CTermPlanDoc* _pDestDoc, CProcessor2* _pProc, const int _nFloor);

	bool GetProCtrlPointVisible(void);
	void SetProCtrlPointVisible(bool bVisible = false);
protected:
	GLuint m_nSelectName;						//selection info

	Processor* m_pProcessor;					//associated processor
	CProcessorControlPointManager m_pcpmControlPointManager;

	ProcessorRenderer * m_pRenderer;
	C3DView * m_pView;

protected: 
	BOOL   high_light_EntryPoint ;
public:
	void  EnableHighLightEntryPoint(BOOL val) { high_light_EntryPoint = val ;} ;
	BOOL  IsHighLightEntryPoint() { return high_light_EntryPoint ;} ;

};

class CNudefinePro2Node
{
public:
	CNudefinePro2Node();
	~CNudefinePro2Node();
public:
	void setNodeName(const CString nodeName){_nodeName = nodeName;}
	const CString& getNodeName(){return _nodeName;}
	void addNode(CNudefinePro2Node* pNode);
	CNudefinePro2Node* getNode(const CString idName);
	void setNode(const ALTObjectID& idName,int ndx);
	void removeNode(const ALTObjectID& idName,int ndx);
	int getNodeCount();
	CNudefinePro2Node* getItem(int ndx);
	void InsertMapNode(std::pair<CString,CNudefinePro2Node*>&_Item);
private:
	CString _nodeName;
	std::vector<CNudefinePro2Node*>vChild;
	std::map<CString,CNudefinePro2Node*>mapNode;
};

class CPROCESSOR2LIST:public std::vector<CProcessor2*>
{
public:
	CPROCESSOR2LIST();
	virtual ~CPROCESSOR2LIST();

	void DeleteItem(int nidx);

	CProcessor2* at(size_type i);
	const CProcessor2* at(size_type i)const;

	const size_t size()const;
	void clear();

	//for undefine shape(old version)
	CString GetUnquieName(const CString& strValue);
	//for underfine shape(current version)
	CString GetName(CString strValue);
	////////
	void InsertShapeName(const CString& strValue);
	void InsertUnDefinePro2(const std::pair<CString,CProcessor2*>&_Item);
	CProcessor2* GetProc2(const CString& shapeName);
	CProcessor2* GetProc2ByGuid(const CGuid& guid) const;
	CNudefinePro2Node* getNode(const CString nodeName);
	void setNode(const ALTObjectID& idName);
	void addNode(CNudefinePro2Node* pNode);
	int getNodeCount();
	CNudefinePro2Node* getItem(int ndx);
	void InsertMapNode(std::pair<CString,CNudefinePro2Node*>&_Item);
	void removePro2(CProcessor2* pPro2);
	bool IsInvalid(CString shapeName);
private:
	void removeNode(const ALTObjectID& idName);

private:
	std::map<CString,int> mapUnDefineObjIndex;
	std::map<CString,CProcessor2*> mapUnDefinePro2;
	std::vector<CNudefinePro2Node*>vChild;
	std::map<CString,CNudefinePro2Node*>mapNode;
	std::set<CString>mapShapeName;
};

#endif // !defined(AFX_PROCESSOR2_H__45421F3D_FD87_4CE8_9927_012440427A28__INCLUDED_)
