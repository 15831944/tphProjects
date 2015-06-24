// PaxFlowTreeNode.h: interface for the CPaxFlowTreeNode class.
#pragma once
#include <CommonData/procid.h>

class CPaxFlowTreeNode;
typedef std::vector<CPaxFlowTreeNode*> CHILDNODE;

class CPaxFlowTreeNode  
{
public:
	CPaxFlowTreeNode();
	virtual ~CPaxFlowTreeNode();
private:
	ProcessorID m_procId;
	int m_iDistPercent;
	CPaxFlowTreeNode* m_pParentNode;
	CHILDNODE m_vChildNodes;
	bool m_bReadOnly;

public:
	void SetProcessorID ( const ProcessorID& _procID){ m_procId= _procID ; };
	const ProcessorID& GetProcessorID () const { return m_procId ; };
	CString GetProcessorIDStr() const ;

	void SetDistributionPercent( int _iPercent ){ m_iDistPercent = _iPercent;};
	int GetDistributionPercent() const { return m_iDistPercent;};

	CPaxFlowTreeNode* GetParentNode() { return m_pParentNode;};
	void SetParentNode( CPaxFlowTreeNode* _pParentNode ){ m_pParentNode = _pParentNode;};

	int GetChildCount() const { return m_vChildNodes.size();};
	CPaxFlowTreeNode* GetChild( int _iIdx );

	int GetChildIndex( CPaxFlowTreeNode* _pChildNode ) const;
	void SetChild (int _iIdx ,CPaxFlowTreeNode* _pChildNode );

	void AddChild( CPaxFlowTreeNode* _pChild );
	void DeleteChild( int _iIdx , bool _bDeleteSubtree = false );
	void EraseChild( int _iIdx );
	void EraseAllChild();

	bool IsReadOnly() const { return m_bReadOnly; };
	void SetReadOnly( bool _bReadOnly ){ m_bReadOnly = _bReadOnly; };
	void EvenPercent();
	

};
