#pragma once

// CDlg3DObjectProperty dialog
#include <Renderer/RenderEngine/3DNodeObject.h>
#include <Renderer/RenderEngine/3DNodeObjectListener.h>

class CDlg3DObjectProperty : public CDialog, public C3DNodeObjectListener
{
	DECLARE_DYNAMIC(CDlg3DObjectProperty)

public:
	class UpdateListener
	{
	public:
		virtual void On3DObjectPropertyUpdate() = 0;
	};

public:
	CDlg3DObjectProperty(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlg3DObjectProperty();

	void SetListener(UpdateListener* val) { m_pListener = val; }

// Dialog Data
// 	enum { IDD = IDD_DIALOG_3D_OBJECT_PROP };

	void Load3DObject(C3DNodeObject _3dObject);
	void CommitChanges();
	void Reload3DObject();
	void Update3DObjectView();

	virtual void On3DNodeObjectFired(C3DNodeObject selNodeObj);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK();

	afx_msg void OnApply();

	void SetContent(int nControlID, double dValue, LPCTSTR strUnit = NULL);
	void ClearContent( int nControlID );
	double RetrieveContent(int nControlID);

	DECLARE_MESSAGE_MAP()
private:
	C3DNodeObject m_3dObject;
	UpdateListener* m_pListener; // Invalidate m_p3DWnd when committing user changes

};
