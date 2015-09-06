#pragma once
#include <vector>
class CMyFolderItem
{
public:
    CMyFolderItem();
    ~CMyFolderItem(){}
    void ListDir(CString strDir);
    void ListAllSiblingFilesWithSameExt(CString strFullFilePath);
    BOOL HasLastSiblingFile(const CString& strCurFile);
    CString GetLastSiblingFile(const CString& strCurFile);
    BOOL HasNextSiblingFile(const CString& strCurFile);
    CString GetNextSiblingFile(const CString& strCurFile);
    void Clear();
public:
    CString m_strCurDir;
    std::vector<CString> m_subDirs;
    std::vector<CString> m_fileNames;
};

class CMyFolderWalker
{
public:
    CMyFolderWalker();
    ~CMyFolderWalker();
    void ListDir(CString strDir);
    BOOL IsDir(CString strPath);
    BOOL IsFile(CString strPath);
    void CreateDirTree(int& iErr, CString strPath);
    void DeleteAll(int& iErr, CString dirName); // delete all sub directories and file in dirName.

public:
    std::vector<CMyFolderItem> m_vFileItemList;
};
