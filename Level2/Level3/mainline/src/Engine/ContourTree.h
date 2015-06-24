#pragma once
#include <vector>
#include "common\dataset.h"

#define CONTOURROOTNODENAME	"CONTOUR_ROOT_NODE"

class CContourNode
{
public:
	CContourNode(void) 
	{
		m_pFirstChild = NULL;
		m_pNext = NULL;
		m_pParent = NULL;
		m_KeyVal = "";
		m_ChildCount = 0;
	}
	
	~CContourNode(void) 
	{
		m_pFirstChild = NULL;
		m_pNext = NULL;
		m_pParent = NULL;
		m_KeyVal = "";
		m_ChildCount = 0;
	}

public:
	CContourNode* m_pFirstChild;
	CContourNode* m_pNext;
	CContourNode* m_pParent;

	CString m_KeyVal;
	int m_ChildCount;

};

class CContourTree : public DataSet 
{
public:
	CContourTree(void);
	~CContourTree(void);

public:
	virtual void initDefaultValues (void) {};

	virtual void clear (void) {}

	virtual void loadDataSet (const CString& _pProjPath);

	virtual void saveDataSet (const CString& _pProjPath, bool _bUndo) const;
	virtual void readData (ArctermFile& p_file) ;
	virtual void readObsoleteData ( ArctermFile& p_file )
	{ 
		readData(p_file); 
	}
	virtual void writeData (ArctermFile& p_file) const ;

	virtual const char *getTitle (void) const { return " ContourTree Dataset.";}
	virtual const char *getHeaders (void) const { return "KeyVal,KeyCount,KeyEnum"; }

private:
	std::vector<CContourNode*> m_vContourNodeVec;
	CContourNode* m_ContourRoot;

public:
	bool AddChildNode(CString curnode, CString childnode); // add a child node on curnode.
	bool InsertNode(CString curnode, CString instnode); // insert a node in front of curnode.
	bool DeleteNode(CString delnode);	// only can delete leaf node.	
	bool DeleteAnyNode(std::vector<CString>& nodevec, CString delnode);

	CContourNode* FindNode(CString nodeval);
	CContourNode* GetRootNode() const;
	bool Rename(CString oldName ,CString newName);


private:
	std::vector<CContourNode*> m_ContourVec;



};
