// MovingSideWorkTree.h: interface for the CMovingSideWorkTree class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOVINGSIDEWORKTREE_H__C8A4FD7C_C37D_40F5_B1A3_EFFB99162227__INCLUDED_)
#define AFX_MOVINGSIDEWORKTREE_H__C8A4FD7C_C37D_40F5_B1A3_EFFB99162227__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProcessorTreeCtrl.h"

class CMovingSideWorkTree : public CProcessorTreeCtrl  
{
public:
	CMovingSideWorkTree();
	virtual ~CMovingSideWorkTree();
public:
	virtual void LoadData( InputTerminal* _pInTerm, ProcessorDatabase* _pProcDB, int _nProcType = -1 );

};

#endif // !defined(AFX_MOVINGSIDEWORKTREE_H__C8A4FD7C_C37D_40F5_B1A3_EFFB99162227__INCLUDED_)
