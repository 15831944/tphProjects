#pragma once

class ALTOException;
class CDlgALTOException : public CDialog
{
	DECLARE_DYNAMIC(CDlgALTOException)

public:
	enum ALTOExceptionMode{
		DocOpenError = 0,
		SimRunningError
	};

public:
	CDlgALTOException(ALTOExceptionMode mode, ALTOException& _Exception, 
		CWnd* pParent = NULL);
	virtual ~CDlgALTOException();
	
private:
	ALTOExceptionMode			mMode;
	ALTOException				&mException;

	CFont						m_fontHeader;
	CFont						m_fontErrorCode;
	CStatic						m_wndErrorCode;
	CEdit						m_wndHeader;

public:
// Dialog Data
	enum { IDD = IDD_DIALOG_ALTOEXCEPTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	//afx_msg void OnBnClickedQuit();
};
