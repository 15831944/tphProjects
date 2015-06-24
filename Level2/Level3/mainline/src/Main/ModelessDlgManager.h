#pragma once

class CDlgMovie;
class CTermPlanDoc;
class C3DView;
class CRender3DView;
class CDlgWalkthrough;
class CDlgWalkthroughRender;
//put the modeless dialog here of the document
class CModelessDlgManager
{
public:
	CModelessDlgManager(CTermPlanDoc* pDoc);
	~CModelessDlgManager(void);

	
	void ClearDlgs();
	void ShowMovieDialog(CWnd *pParent);
	void ShowDialogWalkThrough(C3DView* pView);
	void DestroyDialogWalkThrough();
	void DestroyDialogWalkThroughRender();
	void ShowDialogWalkThroughRender(CRender3DView* pview);

	void OnSelectPax(UINT nPaxID, const CString& sDesc);

	CDlgWalkthrough* GetDlgWalkThrough(){ return m_pDlgWalkthrough; }
	CDlgWalkthroughRender* GetDlgWalkThroughRender(){ return m_pDlgWalkthroughRender; }
protected:
	CDlgMovie* m_pDlgMovie;
	CDlgWalkthrough* m_pDlgWalkthrough;
	CDlgWalkthroughRender* m_pDlgWalkthroughRender; //

	CTermPlanDoc* m_pDoc;
};
