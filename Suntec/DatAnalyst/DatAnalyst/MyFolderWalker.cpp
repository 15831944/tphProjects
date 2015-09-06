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
    CMyFolderItem myFileItem;
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
            DeleteFile(tempFileName);
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

CMyFolderItem::CMyFolderItem()
{
}

void CMyFolderItem::Clear()
{
    m_strCurDir = _T("");
    m_subDirs.clear();
    m_fileNames.clear();
}

void CMyFolderItem::ListDir(CString strDir)
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

// list all sibling into 'm_fileNames' files with the same extension.
// for instance:
// your input file name(strCurFile) is: c:\\dats\\1.dat
// I will parse the input file name and get current directory: "c:\\dats",
// and the file extension ".dat".
// now I will list ".dat" files only.
void CMyFolderItem::ListAllSiblingFilesWithSameExt(CString strCurFile)
{
    Clear();
    strCurFile.ReleaseBuffer();
    m_strCurDir = strCurFile.Left(strCurFile.ReverseFind('\\'));
    CString strExt;
    int nPos = strCurFile.ReverseFind('.');
    if(nPos != -1)
        strExt = strCurFile.Right(strCurFile.GetLength() - strCurFile.ReverseFind('.'));
    CFileFind Finder;
    CString strTemp;
    strTemp.Format(_T("%s\\*%s"), m_strCurDir, strExt);

    BOOL bHasNext = Finder.FindFile(strTemp);
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
            m_fileNames.push_back(Finder.GetFileName());
        }
    }
    Finder.Close();
    return;
}

BOOL CMyFolderItem::HasLastSiblingFile(const CString& strCurFile)
{
    CString strFileName = strCurFile.Right(strCurFile.GetLength()-1-strCurFile.ReverseFind('\\'));
    for(std::vector<CString>::iterator itor = m_fileNames.begin()+1;
        itor != m_fileNames.end();
        itor++)
    {
        if(*itor == strFileName)
        {
            return TRUE;
        }
    }
    return FALSE;
}

CString CMyFolderItem::GetLastSiblingFile(const CString& strCurFile)
{
    CString strFileName = strCurFile.Right(strCurFile.GetLength()-1-strCurFile.ReverseFind('\\'));
    for(std::vector<CString>::iterator itor = m_fileNames.begin()+1;
        itor != m_fileNames.end();
        itor++)
    {
        if(*itor == strFileName)
        {
            return m_strCurDir + _T("\\") + *(itor-1);
        }
    }
    return _T("");
}

BOOL CMyFolderItem::HasNextSiblingFile(const CString& strCurFile)
{
    CString strFileName = strCurFile.Right(strCurFile.GetLength()-1-strCurFile.ReverseFind('\\'));
    for(std::vector<CString>::iterator itor = m_fileNames.begin();
        itor != m_fileNames.end()-1;
        itor++)
    {
        if(*itor == strFileName)
        {
            return TRUE;
        }
    }
    return FALSE;
}

CString CMyFolderItem::GetNextSiblingFile(const CString& strCurFile)
{
    CString strFileName = strCurFile.Right(strCurFile.GetLength()-1-strCurFile.ReverseFind('\\'));
    for(std::vector<CString>::iterator itor = m_fileNames.begin();
        itor != m_fileNames.end()-1;
        itor++)
    {
        if(*itor == strFileName)
        {
            return m_strCurDir + _T("\\") + *(itor+1);
        }
    }
    return _T("");
}
