#include "StdAfx.h"
#include ".\contourtree.h"

#include "assert.h"
#include "common\projectmanager.h"
#include "common\exeption.h"
#include "common\UndoManager.h"


CContourTree::CContourTree(void)
	:DataSet( AirsideContourTreeFile, 2.5f )
{
	m_vContourNodeVec.clear();

	CContourNode *ptNode = new CContourNode();
	ptNode->m_KeyVal = CONTOURROOTNODENAME;
	m_vContourNodeVec.push_back(ptNode);

	m_ContourRoot = ptNode;
	
	m_ContourVec.clear();
}

CContourTree::~CContourTree(void)
{	
	for( unsigned int i = 0; i < m_vContourNodeVec.size(); i++ )
	{
		delete m_vContourNodeVec[i];
		m_vContourNodeVec[i] = NULL;
	}

	m_vContourNodeVec.clear();
	
	m_ContourVec.clear();
}

bool CContourTree::Rename(CString oldName ,CString newName)
{
	CContourNode * pNode = FindNode(oldName);
	if (pNode)
	{
		pNode->m_KeyVal = newName;
		return true;
	}
	return false;
}
bool CContourTree::AddChildNode(CString curnode, CString childnode)
{
	CContourNode* pnode = FindNode(curnode);
	if(!pnode)
		return false;
	if(FindNode(childnode))
		return false;
	
	CContourNode* pAddNode = new CContourNode();
	if( NULL == pnode->m_pFirstChild )
		pnode->m_pFirstChild = pAddNode;
	else
	{
		CContourNode* ptempNode = pnode->m_pFirstChild;
		CContourNode* pBakNode;
		while(ptempNode)
		{
			pBakNode = ptempNode;
			ptempNode = ptempNode->m_pNext;
		}
		pBakNode->m_pNext = pAddNode;
	}

	pAddNode->m_pParent = pnode;
	pAddNode->m_KeyVal = childnode;
	pnode->m_ChildCount ++;
	m_vContourNodeVec.push_back(pAddNode);

	return true;
}

bool CContourTree::InsertNode(CString curnode, CString instnode)
{
	if( CONTOURROOTNODENAME == curnode)
		return false; // root node can't add node.
	if(FindNode(instnode))
		return false;

	CContourNode* pnode = FindNode(curnode);
	if(!pnode)
		return false;
	
	CContourNode* pAddNode = new CContourNode();
	
	assert(pnode->m_pParent);

	if( pnode->m_pParent->m_pFirstChild == pnode)
	{
		pnode->m_pParent->m_pFirstChild = pAddNode;
	}

	pAddNode->m_pNext = pnode;	
	pAddNode->m_pParent = pnode->m_pParent;
	pAddNode->m_KeyVal = instnode;
	pnode->m_pParent->m_ChildCount++;
	m_vContourNodeVec.push_back(pAddNode);

	return true;
}


bool CContourTree::DeleteAnyNode(std::vector<CString>& nodevec, CString delnode)
{	

	CContourNode* pnode = FindNode(delnode);
	if(!pnode)
		return false;

	CContourNode* ptempnode = NULL;
	if (pnode->m_pFirstChild != NULL)
	{
		if( pnode->m_pFirstChild->m_pNext)
		{
			ptempnode =  pnode->m_pFirstChild->m_pNext;
			m_ContourVec.push_back(ptempnode);
		}

		DeleteAnyNode(nodevec, pnode->m_pFirstChild->m_KeyVal) ;


		if( m_ContourVec.size() > 0 )
		{
			ptempnode = m_ContourVec[m_ContourVec.size()-1];
			m_ContourVec.resize( m_ContourVec.size() - 1);
		}
		else
			ptempnode = NULL;

		while( ptempnode != NULL)
		{
			if( ptempnode->m_pNext )
				m_ContourVec.push_back(ptempnode->m_pNext);

			DeleteAnyNode(nodevec, ptempnode->m_KeyVal);
//			ptempnode = ptempnode->m_pNext;

			if( m_ContourVec.size() > 0 )
			{
				ptempnode = m_ContourVec[m_ContourVec.size()-1];
				m_ContourVec.resize( m_ContourVec.size() - 1);
			}
			else
				ptempnode = NULL;

		}
	}

	if(DeleteNode(delnode))
	{
		nodevec.push_back(delnode);
		return true;
	}

	return false;
}

bool CContourTree::DeleteNode(CString delnode)
{// only can delete leaf node.
	if( CONTOURROOTNODENAME == delnode)
		return false; // root node can't add node.

	CContourNode* pnode = FindNode(delnode);
	if(!pnode)
		return false;
	if(0 != pnode->m_ChildCount)
		return false;
	pnode->m_pParent->m_ChildCount--;

	if( 0 != pnode->m_pParent->m_ChildCount )
	{
		if( pnode == pnode->m_pParent->m_pFirstChild )
		{
			pnode->m_pParent->m_pFirstChild = pnode->m_pNext;
		}
		else
		{
			CContourNode* ptempNode = pnode->m_pParent->m_pFirstChild;
			CContourNode* pBakNode;
			while(ptempNode!=pnode)
			{
				pBakNode = ptempNode;
				ptempNode = ptempNode->m_pNext;
			}
			pBakNode->m_pNext = pnode->m_pNext;			
		}
	}
	else
	{
		pnode->m_pParent->m_pFirstChild = NULL;
	}

	for( unsigned int i = 0; i < m_vContourNodeVec.size(); i++)
	{
		if( m_vContourNodeVec[i]->m_KeyVal == delnode )
		{
			m_vContourNodeVec.erase(m_vContourNodeVec.begin()+i);
			break;
		}
	}
	delete pnode;

	return true;
}

CContourNode* CContourTree::FindNode(CString nodeval)
{
	CContourNode* pret = NULL;
	for( unsigned int i = 0; i < m_vContourNodeVec.size(); i++)
	{
		if( m_vContourNodeVec[i]->m_KeyVal == nodeval )
		{
			pret = m_vContourNodeVec[i];
			break;
		}
	}

	return pret;
}

void CContourTree::saveDataSet (const CString& _pProjPath, bool _bUndo) const
{
	//if( _bUndo )
	//{
	//	CUndoManager* undoMan = CUndoManager::GetInStance( _pProjPath );
	//	if( !undoMan->AddNewUndoProject() )
	//		AfxMessageBox( "Can not create UNDO folder, UNDO did not proceeded", MB_OK|MB_ICONWARNING );

	//}
	if(CAutoSaveHandlerInterface::PAUTOSAVE_HANDLE)
		CAutoSaveHandlerInterface::PAUTOSAVE_HANDLE->DoAutoSave() ;
	char filename[_MAX_PATH];
	PROJMANAGER->getFileName (_pProjPath, fileType, filename);

	ArctermFile file;
	file.openFile (filename, WRITE, m_fVersion);

	file.writeField (getTitle());
	file.writeLine();

	file.writeField (getHeaders());
	file.writeLine();

	writeData (file);

	file.endFile();
}

void CContourTree::loadDataSet (const CString& _pProjPath)
{	
	char filename[_MAX_PATH];
	PROJMANAGER->getFileName (_pProjPath, fileType, filename);

	ArctermFile file;
	try 
	{
		file.openFile (filename, READ);
	}
	catch (FileOpenError *exception)
	{
		delete exception;
		initDefaultValues();
		saveDataSet(_pProjPath, false);
		return;
	}

	readData (file);
	file.closeIn();

}

CContourNode* CContourTree::GetRootNode() const
{
	if(0 == (int)m_vContourNodeVec.size())
		return NULL;

	return m_vContourNodeVec[0];
}

void CContourTree::writeData(ArctermFile& p_file) const
{
	int nCount = (int)m_vContourNodeVec.size();

	for( int i=0; i<nCount; i++ ) //for each node
	{// keyval, childcount, childenum....
		p_file.writeField( m_vContourNodeVec[i]->m_KeyVal );
		p_file.writeInt( m_vContourNodeVec[i]->m_ChildCount );
		CContourNode* ptNode = m_vContourNodeVec[i]->m_pFirstChild;
		for( int j = 0; j < m_vContourNodeVec[i]->m_ChildCount; j++)
		{
			assert(ptNode);
			p_file.writeField( ptNode->m_KeyVal );
			ptNode = ptNode->m_pNext;			
		}
		p_file.writeLine();
	}
}

void CContourTree::readData(ArctermFile& p_file)
{
	p_file.getLine();

	for( unsigned int i = 0; i < m_vContourNodeVec.size(); i++ )
	{
		delete m_vContourNodeVec[i];
		m_vContourNodeVec[i] = NULL;
	}
	m_vContourNodeVec.clear();

	while (!p_file.isBlank ())
	{
		char name[256] = {0};
		p_file.getField(name, 255);
		if(0 == name[0])
			continue;
		bool rootfind = true;
		CContourNode* ptroot = FindNode(name);
		if(NULL == ptroot)
		{
			ptroot = new CContourNode();
			ptroot->m_KeyVal = name;
			rootfind = false;
		}

		int childcount;
		p_file.getInteger(childcount);
		ptroot->m_ChildCount = childcount;

		int i = 0;
		CContourNode* ptPreNode = NULL, *ptNextNode;
		bool prenodefind = true;
		if( childcount > 0 )
		{
			p_file.getField(name, 255);
			ptPreNode = FindNode(name);
			if(NULL == ptPreNode)
			{
				ptPreNode = new CContourNode();
				ptPreNode->m_KeyVal = name;	
				prenodefind = false;
			}

			ptPreNode->m_pParent = ptroot;
			ptroot->m_pFirstChild = ptPreNode;
		}		
		if(!rootfind)
			m_vContourNodeVec.push_back(ptroot);
		if(!prenodefind)
			m_vContourNodeVec.push_back(ptPreNode);

		for( i = 1; i< childcount; i++)
		{
			p_file.getField(name, 255);
			assert(name[0]);

			bool nextnodefind = true;
			ptNextNode = FindNode(name);
			if(NULL == ptNextNode)
			{
				ptNextNode = new CContourNode();
				ptNextNode->m_KeyVal = name;	
				nextnodefind = false;
			}
			ptNextNode->m_pParent = ptroot;
			ptPreNode->m_pNext = ptNextNode;
			if(!nextnodefind)
				m_vContourNodeVec.push_back(ptNextNode);
			ptPreNode = ptNextNode;
		}

		p_file.getLine();
	}

	if( m_vContourNodeVec.size() > 0)
		m_ContourRoot = m_vContourNodeVec[0];

}



