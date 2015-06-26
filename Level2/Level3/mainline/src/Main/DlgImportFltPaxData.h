#pragma once
#include "afxwin.h"
#include "resource.h"
#include "MFCExControl/ListCtrlEx.h"
#include "MFCExControl/XTResizeDialog.h"
#include "FltPaxDataDlg.h"
#include "afxeditbrowsectrl.h "


class CDlgImportFltPaxData : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgImportFltPaxData)
public:
	CDlgImportFltPaxData(FLTPAXDATATTYPE emType,InputTerminal* pInTerm,ConstraintDatabase* pFltPaxDB,CWnd* pParent);
	~CDlgImportFltPaxData(void);

	// Dialog Data
	//{{AFX_DATA(CDlgImportFltPaxData)
	enum { IDD = IDD_DIALOG_IMPORTFLTPAX};

	void SetOperation(int iType);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

	afx_msg void OnCbnSelchangeComboProject();
	afx_msg void OnUpdateExport();
	afx_msg void OnUpdateCSVFile();
	afx_msg void OnAppend();
	afx_msg void OnReplace();
	afx_msg void OnCheckAll();
	afx_msg void OnCheckLocalProject();
	afx_msg void OnCheckExportedProject();
	afx_msg void OnCheckCSVFile();

private:
	void SetImportDialogTitle();
	void InitComboxContent();
	void InitListCtrlHeader(ConstraintDatabase* pConDB);
	void SetListCtrlContent(ConstraintDatabase* pConDB);
	void InitRadioStatus();

	bool readIniFileData( const CString& _strTempExtractPath, CString& _strInputZip);
	bool unzipFiles( const CString& _strZip, const CString& _strPath );
	bool CreateTempZipFile( CString& strTempExtractPath,const CString& strFilePath );
	CString getTempPath( const CString& _strDefault = "c:\\"  );

	ConstraintDatabase* LoadDataFromFile(const CString& strFileName);
	int GetExsitFltPaxData(ConstraintEntry* pEntry);
	void ClearData();
	void AddData(ConstraintEntry* pEntry);
private:
	CListCtrl m_wndListCtrl;
	CComboBox m_wndCombox;
	CMFCEditBrowseCtrl m_wndExportedProject;
	CMFCEditBrowseCtrl m_wndCSVFile;
	FLTPAXDATATTYPE m_emType;
	int m_iOperation;
	InputTerminal* m_pInterm;
	DataSet* m_pDataSet;

	ConstraintDatabase* m_pFltPaxDB;
	ConstraintDatabase* m_pCSVDB;
};

