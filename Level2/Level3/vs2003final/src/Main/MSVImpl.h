// MSVImpl.h: interface for the CMSVImpl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSVIMPL_H__A3B18E9E_CEAA_4792_BC19_BA41BF9427F9__INCLUDED_)
#define AFX_MSVIMPL_H__A3B18E9E_CEAA_4792_BC19_BA41BF9427F9__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTermPlanDoc;
class CTVNode;

class CMSVImpl  
{
public:
	CMSVImpl();
	virtual ~CMSVImpl();

	void AddItem(CTVNode* pNode, CTVNode* pAfterNode);
	void SetTermPlanDoc(CTermPlanDoc* pDoc);
	CTermPlanDoc* m_pTermPlanDoc;
	
};

#endif // !defined(AFX_MSVIMPL_H__A3B18E9E_CEAA_4792_BC19_BA41BF9427F9__INCLUDED