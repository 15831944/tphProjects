#pragma once
#include "afxwin.h"
#include "resource.h"
#include "MFCExControl/ListCtrlEx.h"
#include "MFCExControl/XTResizeDialog.h"
#include "FltPaxDataDlg.h"
#include "afxeditbrowsectrl.h "

class CBrowserARCEdit : public CMFCEditBrowseCtrl
{
public:
	virtual void OnBrowse();
};

typedef std::map<ConstraintEntry*,CString> mapConstraintData;

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
	afx_msg void OnClickListContentItem(NMHDR* pNMHDR, LRESULT* pResult); 
	afx_msg void OnLvnItemchangedListcontrol(NMHDR *pNMHDR, LRESULT *pResult);
private:
	void SetImportDialogTitle();
	void InitComboxContent();
	void InitListCtrlHeader();
	void SetListCtrlContent();
	void InitRadioStatus();

	bool readIniFileData( const CString& _strTempExtractPath, CString& _strInputZip);
	bool unzipFiles( const CString& _strZip, const CString& _strPath );
	bool CreateTempZipFile( CString& strTempExtractPath,const CString& strFilePath );
	CString getTempPath( const CString& _strDefault = "c:\\"  );

	CString GetFileName();
	bool CheckFileFormat(ArctermFile& p_file);

	bool LoadDataFromFile(const CString& strFileName,bool bCheckFile);
	int GetExsitFltPaxData(ConstraintEntry* pEntry);
	void ClearData();
	void AddData(ConstraintEntry* pEntry);

	void ResetButtonStatus(BOOL bTrue);

	void ReadFlightConstrainDatabase(ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm);
	void ReadFlightConstrainWithSchedDatabase(ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm);
	void ReadMobileElemConstrainDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm);
private:
	CListCtrl m_wndListCtrl;
	CComboBox m_wndCombox;
	CBrowserARCEdit m_wndExportedProject;
	CBrowserARCEdit m_wndCSVFile;
	FLTPAXDATATTYPE m_emType;
	int m_iOperation;
	InputTerminal* m_pInterm;
	
	ConstraintDatabase* m_pContraintDB;
	ConstraintDatabase* m_pFltPaxDB;
	bool m_bHit;
	mapConstraintData m_mapConstraintData;
};

