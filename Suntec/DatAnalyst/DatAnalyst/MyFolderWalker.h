#pragma once
#include <vector>
class CMyFileItem
{
public:
    CMyFileItem();
    ~CMyFileItem(){}
    void ListDir(CString strDir);
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
    void DeleteAll(int& iErr, CString dirName); // 删除目录中全部文件

public:
    std::vector<CMyFileItem> m_vFileItemList;
};
