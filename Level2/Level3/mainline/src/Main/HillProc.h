#ifndef __HILL_PROC
#define __HILL_PROC

#pragma once

#include "Common\ARCVector.h"
#include "engine\AirfieldProcessor.h"
#include "engine\ContourTree.h"
#include "../Engine/ContourProc.h"
#include "../main/TermPlanDoc.h"
#include <vector>

#define DATAIN  Path 


class HillProc{
	GLuint textureid;
	
	CContourNode * m_first;

	// CTremPlanDoc ( what is this?? )
	CTermPlanDoc * m_pDoc;
	
	GLuint m_displaylist;
	bool m_bDirtflag;
	
public:
	void GenAllTriBlocks(CContourNode * father);
	bool CalBlocks();
	void DrawProc();
	void setTextureid(int id){textureid=id;};
	void setContourFirst(CContourNode * first){m_first=first;}
	void setDoc(CTermPlanDoc * pDoc){	m_pDoc=pDoc;
	}
	/*void genDisplayList();*/
	HillProc(){m_pDoc=NULL;m_bDirtflag=true;m_displaylist=0;}
	HillProc (CContourNode * father){m_pDoc=NULL;m_first=father;}
private:
	void GenTriBlocks(DATAIN &first,double hightfir ,DATAIN &second,double hightsec);

	int getstartid(DATAIN &path);
	
	
};

#endif