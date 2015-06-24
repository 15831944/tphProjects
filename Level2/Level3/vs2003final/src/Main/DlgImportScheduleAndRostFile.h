#pragma once

#include "MFCExControl/XListCtrl.h"
#include "MFCExControl/SortableHeaderCtrl.h"
#include "boost/tuple/tuple.hpp"
// CDlgImportScheduleAndRostFile dialog
typedef boost::tuple<CString,CString> FilePair;
class FolderParse
{
public:
	FolderParse(const CString& szPath);
	~FolderParse();

	bool IsEmpty()const;
	int GetCount()const;
	FilePair& GetFilePair(int nPair);

	void ParseFolder();
	bool operator ==(const FolderParse& folderParse)const;
	const CString GetFolderPath()const {return m_strFolderPath;}

protected:
	bool AddFileName(const CString& szFileName);
	bool fitRosterAndSetVaule(const CString& szFileName);
	bool fitScheduleAndSetVaule(const CString& szFileName);
	bool scheduleFile(const CString& szFileName)const;
	bool rosterFile(const CString& szFileName)const;
	bool IsNum(const LPCTSTR pszText)const;
protected:
	std::vector<FilePair> m_vPairFile;
	CString	m_strFolderPath;
	const static std::string m_sScheduleName;
	const static std::string m_sRosterName;
	const static int m_iNumLength;
};

class FolderParseList
{
public:
	FolderParseList();
	~FolderParseList();
public:
	void ParseFolder(const CString& szPath);

	void AddFolderParse(const FolderParse& folderParse);
	const FolderParse& GetFolderParse(int idx)const;
	int GetCount()const;
	bool IsEmpty()const;

protected:
	std::vector<FolderParse> m_vFolderList;
};

class CDlgImportScheduleAndRostFile : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgImportScheduleAndRostFile)

public:
	CDlgImportScheduleAndRostFile(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgImportScheduleAndRostFile();

// Dialog Data
	enum { IDD = IDD_DIALOG_IMPORTSCHEDULEANDROSTFILE };

public:
	CString GetScheduleFilePath()const;
	CString GetRosterFilePath()const;

	CString GetScheduleFileName()const;
	CString GetRosterFileName()const;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void FillListCtrl();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBtnOpen();
	afx_msg void OnColumnclickListFile(NMHDR* pNMHDR, LRESULT* pResult);
	LRESULT OnDBClickListCtrl(WPARAM wParam,LPARAM lParam);
	void OnOK();

	void FillListHeader();

protected:
	CXListCtrl			m_wndListCtrl;
	CSortableHeaderCtrl m_wndSortHeaderList;
	FolderParseList		m_folderParseList;

	CString				m_strFilePath;
	CString				m_strScheduleName;
	CString				m_strRosterName;
};
