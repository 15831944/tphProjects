#pragma once

/////////////////////////////////////////////////////////////////////////////
// CMathematicFrame frame

class CMathematicFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CMathematicFrame)
protected:
	CMathematicFrame();

public:
    virtual void OnUpdateFrameTitle(BOOL bAddToTitle);

protected:
	virtual ~CMathematicFrame();
	DECLARE_MESSAGE_MAP()
};