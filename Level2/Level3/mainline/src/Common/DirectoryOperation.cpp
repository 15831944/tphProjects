#include "StdAfx.h"
#include "DirectoryOperation.h"
void myCopyDirectory(CString source, CString target)  
{  
    CreateDirectory(target,NULL); //创建目标文件夹  
    //AfxMessageBox("创建文件夹"+target);  
    CFileFind finder;  
    CString path;  
    path.Format(_T("%s/*.*"),source);  
    //AfxMessageBox(path);  
    BOOL bWorking =finder.FindFile(path);  
    while(bWorking)
    {  
        bWorking = finder.FindNextFile();  
        //AfxMessageBox(finder.GetFileName());  
        if(finder.IsDirectory() && !finder.IsDots())//是文件夹 而且 名称不含 . 或 ..  
        { 
            //递归创建文件夹+"/"+finder.GetFileName()
            myCopyDirectory(finder.GetFilePath(),target+"/"+finder.GetFileName());   
        }  
        else//是文件 则直接复制 
        {  
            //AfxMessageBox("复制文件"+finder.GetFilePath());//+finder.GetFileName()  
            CopyFile(finder.GetFilePath(),target+"/"+finder.GetFileName(),FALSE);  
        }  
    }  
}

void myDeleteDirectory(CString directory_path)
{   
    CFileFind finder;
    CString path;
    path.Format(_T("%s/*.*"),directory_path);
    BOOL bWorking = finder.FindFile(path);
    while(bWorking)
    {
        bWorking = finder.FindNextFile();
        if(finder.IsDirectory() && !finder.IsDots())//处理文件夹
        {
            myDeleteDirectory(finder.GetFilePath()); //递归删除文件夹
            RemoveDirectory(finder.GetFilePath());
        }
        else//处理文件
        {
            DeleteFile(finder.GetFilePath());
        }
    }
    RemoveDirectory(directory_path);
}