#pragma once

#include "TreeDataSet.h"
#include "AcTypeAlias.h"

class CACTypesManager ;
class SuperTypeRelationTree : public CTreeDataSet<CAcTypeAlias*>
{
public:
	SuperTypeRelationTree(void): CTreeDataSet<CAcTypeAlias*>(){};
	~SuperTypeRelationTree(void){};
};
class CCompareFunction
{
public:
	BOOL operator()(SuperTypeRelationTree::PTREE_NODE _treenode,CString _AliasNmae)
	{
		if(_treenode == NULL || _treenode->m_NodeData == NULL)
			return FALSE ;
		return _treenode->m_NodeData->CheckAliasName(_AliasNmae) ;
	}
};

class SuperTypeRelationData
{
public:
	typedef SuperTypeRelationTree TY_RElATION_TREE ;
	typedef TY_RElATION_TREE::PTREE_NODE P_TREE_NODE ;
	typedef SuperTypeRelationTree::TREE_NODE TREE_NODE ;

	SuperTypeRelationData(CACTypesManager* _Manger):m_AcManager(_Manger) {} ;
	~SuperTypeRelationData() 
	{	
	for (int i = 0 ; i < (int)m_DeletNode.size() ;i++)
	{
		m_RelationForest.DeleteTreenode(m_DeletNode[i]) ;
	}
	m_DeletNode.clear();
	} ;
protected:
	TY_RElATION_TREE  m_RelationForest ;
	std::vector<P_TREE_NODE> m_DeletNode ;
public:
	//_ParentNode == NULL , add after the root node 
	P_TREE_NODE AddTreeNode(P_TREE_NODE _ParentNode ,CAcTypeAlias*  _alias) ;
	void RemoveTreeNode(P_TREE_NODE _Node) ;
	P_TREE_NODE GetRootData() { return m_RelationForest.GetRootNode() ;} ;
	CAcTypeAlias* FindAcAliasNode(CString& _Name) ;
public:
	CACType* FindActypeBySuperTypeRelation(CString& _actype) ;
    BOOL CheckTheAliasExist(CString& _aliasName,CAcTypeAlias** _PAlias = NULL) ;
    //check the alias name is valid ,if invalid ,return the error message
	BOOL CheckTheAliasNameValid(CAcTypeAlias* _AcTypeAlias , CString& _aliasName , CString& _errorMsg) ;
protected:
	void ReadData(int nodeID , P_TREE_NODE _ParentNode) ;
	void WriteData(int ParentID , P_TREE_NODE _Node) ;
		void DeleteDataFromDB() ;
public:
	void ReadDataFromDB() ;
	void WriteDataToDB() ;
protected:
	CACTypesManager* m_AcManager ;
public:
	void SetAcTypeManger(CACTypesManager* _acManager) { m_AcManager = _acManager ;};
public:
	// if _bigName contain _small or _bigname and _small is alias ,return true 
	//else return FALSE 
	BOOL Fit(CString _smallName ,CString _bigName) ;
};
