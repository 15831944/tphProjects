#pragma once
#include <functional>
#include <algorithm>
template <class TreeNodeDataType>
class CTreeDataSet
{
public:
	template <class TreeNodeDataType>
	class CTreeNode
	{
	public:
		typedef std::vector<CTreeNode*> TY_CHILD ;
	public:
        TreeNodeDataType m_NodeData ;
		CTreeNode* m_Parent ;
        TY_CHILD m_children ;
        int m_ID ;
		CTreeNode():m_NodeData(NULL),m_Parent(NULL),m_ID(-1){} ;
		~CTreeNode() { if(m_NodeData != NULL) delete m_NodeData ;} ;
	};
public:
	CTreeDataSet(void)
	{

	}
	virtual ~CTreeDataSet(void)
	{
		{
			ClearAllData() ;
		}

	}
	void ClearAllData()
	{
		for (int i = 0 ; i < (int)m_Root.m_children.size() ; i++)
		{
			DeleteTreenode(m_Root.m_children[i]) ;
		}
		m_Root.m_children.clear() ;
	}
public:
	     typedef CTreeNode<TreeNodeDataType> TREE_NODE ;
		typedef TREE_NODE* PTREE_NODE ;
protected:
	TREE_NODE m_Root ;
   
public:
	PTREE_NODE GetRootNode() { return &m_Root ;} ;
	PTREE_NODE AddChildNode(PTREE_NODE _ParentNode , TreeNodeDataType _data) 
	{
		if(_ParentNode != NULL)
		{
			PTREE_NODE treenode = new CTreeNode<TreeNodeDataType>() ;
			treenode->m_NodeData = _data ;
			treenode->m_Parent = _ParentNode ;
			_ParentNode->m_children.push_back(treenode) ;
			return treenode ;
		}else
			return NULL ;
	}
	void RemoveChildNode(PTREE_NODE _parentNode , PTREE_NODE _data) 
	{
		if(_parentNode == NULL)
			return ;
		TREE_NODE::TY_CHILD::iterator iter = std::find(_parentNode->m_children.begin() , _parentNode->m_children.end(),_data) ;
		if(iter != _parentNode->m_children.end())
		{
			_parentNode->m_children.erase(iter) ;
		}
	}
    void RemoveChildNode(PTREE_NODE _data) 
	{
		PTREE_NODE treeNode = FindData(_data,std::equal_to<PTREE_NODE>()) ; 
		if(treeNode != NULL)
			RemoveChildNode(treeNode->m_Parent,treeNode) ;
	}
   
	template <class CompareTy,class PFunType>
   PTREE_NODE FindData(CompareTy _ty1 , PFunType PFun) 
	{
         return FindData(&m_Root,_ty1,PFun) ;
	}

protected:
	template <class CompareTy,class PFunType>
	PTREE_NODE FindData(PTREE_NODE _treenode ,CompareTy _ty1 , PFunType PFun) 
	{
           if( (PFun)(_treenode,_ty1))
			   return _treenode ;
		   else
		   {
			   for (int i = 0 ; i < (int)_treenode->m_children.size() ;i++)
			   {
				   PTREE_NODE treenode = FindData(_treenode->m_children[i],_ty1,PFun) ;
                    if( treenode != NULL )
						return treenode ;
			   }
			   return NULL ;
		   }
	}
public:
	void DeleteTreenode(PTREE_NODE _treenode) 
	{
		if(_treenode == NULL)
			return ;
		for (int i = 0 ; i < (int)_treenode->m_children.size() ;i++)
		{
			DeleteTreenode(_treenode->m_children[i]) ;
		}
		delete _treenode ; 
	}
};





