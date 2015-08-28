#include "StdAfx.h"
#include "MyFolderWalker.h"

const int xxxxx8 = 192844; // 非法的路径名称，无法创建。

CMyFolderWalker::CMyFolderWalker()
{
}

CMyFolderWalker::~CMyFolderWalker()
{
}

void CMyFolderWalker::ListDir(CString strDir)
{
    CMyFileItem myFileItem;
    myFileItem.ListDir(strDir);
    for(size_t i=0; i<myFileItem.m_subDirs.size(); i++)
    {
        ListDir(myFileItem.m_subDirs[i]);
    }
    m_vFileItemList.push_back(myFileItem);
    return;
}

// delete all file in folder 'dirName'.
void CMyFolderWalker::DeleteAll(int& iErr, CString dirName)
{
    CFileFind tempFind;
    CString foundFileName;
    CString tempFileFind=dirName+_T("*.*");
    BOOL IsFinded=(BOOL)tempFind.FindFile(tempFileFind);
    while(IsFinded)
    {
        IsFinded=(BOOL)tempFind.FindNextFile();
        if(!tempFind.IsDots())
        {
            foundFileName=tempFind.GetFileName();
            CString tempFileName=dirName+_T("\\")+foundFileName;
            DeleteFile(tempFileName); // 删除文件
        }
    }
    tempFind.Close();
    iErr = 0;
    return;
}

BOOL CMyFolderWalker::IsDir(CString strPath)
{
    return GetFileAttributes(strPath) == FILE_ATTRIBUTE_DIRECTORY;
}

BOOL CMyFolderWalker::IsFile(CString strPath)
{
    return GetFileAttributes(strPath) == FILE_ATTRIBUTE_DIRECTORY;
}

void CMyFolderWalker::CreateDirTree(int& iErr, CString strPath)
{
    if(::CreateDirectory(strPath, NULL) == 0)
    {
        CString strFatherPath = strPath.Left(strPath.Find("\\"));
        CreateDirTree(iErr, strFatherPath);
        ::CreateDirectory(strPath, NULL);
    }
}

/************************************************************************/

CMyFileItem::CMyFileItem()
{
}

void CMyFileItem::ListDir(CString strDir)
{
    m_strCurDir = strDir;
    CFileFind Finder;
    if(strDir.Right(1) != "\\")
        strDir += "\\";
    strDir += "*.*";
    BOOL bHasNext = Finder.FindFile(strDir);
    while(bHasNext)
    {
        bHasNext = Finder.FindNextFile();
        if(Finder.IsDirectory() && !Finder.IsDots())
        {
            CString strSubDir = Finder.GetFilePath();
            m_subDirs.push_back(strSubDir);
        }
        else if(!Finder.IsDirectory() && !Finder.IsDots())
        {
            CString strFileName = Finder.GetFileName();
            m_fileNames.push_back(strFileName);
        }
    }
    Finder.Close();
    return;
}
