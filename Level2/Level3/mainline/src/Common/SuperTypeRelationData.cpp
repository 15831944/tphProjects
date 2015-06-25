#include "StdAfx.h"
#include ".\supertyperelationdata.h"
#include "../Database/ADODatabase.h"
#include "ACTypesManager.h"
void SuperTypeRelationData::ReadData(int nodeID , P_TREE_NODE _ParentNode)
{
	CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_SUPERSUB_RELATION_ACTYPE WHERE PARENT_ID = %d"),nodeID) ;
	CADORecordset dataset ;
	long count = 0 ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,dataset) ;
	}
	catch (CADOException& e)
	{
		e.ErrorMessage() ;
		return ;
	}
	SuperTypeRelationTree::PTREE_NODE  TreeNode = NULL ;
	CAcTypeAlias* Palias = NULL ;
	while(!dataset.IsEOF())
	{
		
		Palias = new CAcTypeAlias ;
		TreeNode = m_RelationForest.AddChildNode(_ParentNode,Palias) ;
		dataset.GetFieldValue(_T("ID"),TreeNode->m_ID) ;
		//read alias data 
		CAcTypeAlias::ReadDataFromDB(TreeNode->m_NodeData,TreeNode->m_ID) ;

		//read sub treenode
        ReadData(TreeNode->m_ID,TreeNode) ;
		dataset.MoveNextData() ;
	}
}
void SuperTypeRelationData::WriteData(int ParentID , P_TREE_NODE _Node)
{
	if(_Node == NULL)
		return ;
	//write node first ;
	CString SQL ;
	if(_Node->m_ID == -1 )
	{
		SQL.Format(_T("INSERT INTO TB_SUPERSUB_RELATION_ACTYPE (PARENT_ID) VALUES(%d)") , ParentID) ;
		int id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL) ;
		_Node->m_ID = id ;
	}
	else
	{
		SQL.Format(_T("UPDATE TB_SUPERSUB_RELATION_ACTYPE SET PARENT_ID = %d WHERE ID= %d") ,ParentID,_Node->m_ID) ;
		CADODatabase::ExecuteSQLStatement(SQL) ;
	}
	//write node alias 
	CAcTypeAlias::WriteDataFromDB(_Node->m_NodeData,_Node->m_ID) ;

	//write it's child node 
	for (int i = 0 ; i < (int)_Node->m_children.size() ;i++ )
	{
       WriteData(_Node->m_ID , _Node->m_children[i]) ;
	}
}
void SuperTypeRelationData::ReadDataFromDB()
{

	//clear all data 
	m_RelationForest.ClearAllData() ;
	//read root data 
	ReadData(-1,m_RelationForest.GetRootNode()) ;
  
}
SuperTypeRelationData::P_TREE_NODE SuperTypeRelationData::AddTreeNode(P_TREE_NODE _ParentNode ,CAcTypeAlias*  _alias) 
{
	if(_alias == NULL)
		return NULL;
   return  m_RelationForest.AddChildNode(_ParentNode,_alias) ;
}
CACType* SuperTypeRelationData::FindActypeBySuperTypeRelation(CString& _actype) 
{
   P_TREE_NODE PNode =  m_RelationForest.FindData(_actype,CCompareFunction()) ;
   if(PNode == NULL)
	   return NULL ;
   if(PNode->m_NodeData->GetActypeInstance() != NULL)
	   return PNode->m_NodeData->GetActypeInstance() ;
   else
   {
		CACType* PacType = m_AcManager->FilterActypeByStrings(*(PNode->m_NodeData->GetAliasNames())) ;
		if(PacType != NULL)
			return PacType ;
   }
  P_TREE_NODE PParentNode = PNode->m_Parent ;
	while(PParentNode)
	{
		if(PParentNode->m_NodeData->GetActypeInstance() != NULL)
			return PParentNode->m_NodeData->GetActypeInstance() ;
		else
		{
			CACType* PacType = m_AcManager->FilterActypeByStrings(*(PParentNode->m_NodeData->GetAliasNames())) ;
			if(PacType != NULL)
			{
                PParentNode->m_NodeData->SetActypeInstance(PacType) ;
				return PacType;
			}
		}
		PParentNode = PParentNode->m_Parent ;
	}
	return NULL ;
}
BOOL SuperTypeRelationData::Fit(CString _smallName ,CString _bigName)
{
	P_TREE_NODE PNode =  m_RelationForest.FindData(_smallName,CCompareFunction()) ;
	if(PNode == NULL)
		return FALSE ;
	if(PNode->m_NodeData->CheckAliasName(_bigName))
		return TRUE ;
	P_TREE_NODE PParentNode = PNode->m_Parent ;
	while(PParentNode)
	{
		if(PParentNode->m_NodeData->CheckAliasName(_bigName))
			return TRUE ;
		PParentNode = PParentNode->m_Parent ;
	}
	return FALSE ;
}
BOOL SuperTypeRelationData::CheckTheAliasExist(CString& _aliasName,CAcTypeAlias** _PAlias)
{
	P_TREE_NODE PNode =  m_RelationForest.FindData(_aliasName,CCompareFunction()) ;
	if(PNode == NULL)
		return FALSE ;
	else
	{
		*_PAlias = PNode->m_NodeData ;
		return TRUE ;
	}
}
void SuperTypeRelationData::RemoveTreeNode(P_TREE_NODE _Node)
{
    m_RelationForest.RemoveChildNode(_Node) ;
	m_DeletNode.push_back(_Node) ;
}
void SuperTypeRelationData::WriteDataToDB()
{
	P_TREE_NODE PNode = m_RelationForest.GetRootNode();
	for (int i = 0 ; i < (int)PNode->m_children.size() ;i++ )
	{
		WriteData(-1 , PNode->m_children[i]) ;
	}
	DeleteDataFromDB() ;

	//release the memory
	for (int i = 0 ; i < (int)m_DeletNode.size() ;i++)
	{
         m_RelationForest.DeleteTreenode(m_DeletNode[i]) ;
	}
    m_DeletNode.clear();
}
void SuperTypeRelationData::DeleteDataFromDB()
{
	CString SQL ;
	for (int i = 0 ; i < (int)m_DeletNode.size() ;i++)
	{
          SQL.Format(_T("DELETE * FROM TB_SUPERSUB_RELATION_ACTYPE WHERE ID = %d") ,m_DeletNode[i]->m_ID) ;
		  CADODatabase::ExecuteSQLStatement(SQL) ;
	}
}
BOOL SuperTypeRelationData::CheckTheAliasNameValid(CAcTypeAlias* _AcTypeAlias , CString& _aliasName , CString& _errorMsg) 
{
    CAcTypeAlias* AliasPoint = NULL ;
	if(CheckTheAliasExist(_aliasName,&AliasPoint) && AliasPoint != _AcTypeAlias)
	{
		_errorMsg.Format(_T("%s has existed!"),_aliasName) ;
		return FALSE ;
	}
	if(_AcTypeAlias == NULL)
		return FALSE ;
	CACType* PacType = m_AcManager->FilterActypeByStrings(*_AcTypeAlias->GetAliasNames()) ;
	if(_AcTypeAlias->GetActypeInstance() == NULL && PacType == NULL)
		return TRUE ;
	else
	{
		std::vector<CString> _ALiasName ;
		_ALiasName.push_back(_aliasName) ;
		CACType* _Actype = m_AcManager->FilterActypeByStrings(_ALiasName) ;
		if(_Actype == NULL)
			return TRUE ;
		else
		{
			if(_AcTypeAlias->GetActypeInstance() != NULL)
			{
				_errorMsg.Format(_T("%s conflict with %s,One AliasNameSet Can't attach two different Actype Instances !"),_Actype->GetIATACode(),_AcTypeAlias->GetActypeInstance()->GetIATACode()) ;
				return FALSE ;
			}else
			{
				if(PacType != NULL)
				{
					_errorMsg.Format(_T("%s conflict with %s,One AliasNameSet Can't attach two different Actype Instances !"),_Actype->GetIATACode(),PacType->GetIATACode()) ;
				   return FALSE ;
				}
			}
		}
	}
	return TRUE ;
}
CAcTypeAlias* SuperTypeRelationData::FindAcAliasNode(CString& _Name) 
{
      P_TREE_NODE PNode =  m_RelationForest.FindData(_Name,CCompareFunction()) ;
	  if(PNode == NULL)
		  return NULL;
	  else
          return PNode->m_NodeData ;
}