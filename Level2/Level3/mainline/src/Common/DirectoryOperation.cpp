#include "StdAfx.h"
#include "DirectoryOperation.h"
void myCopyDirectory(CString source, CString target)  
{  
    CreateDirectory(target,NULL); //����Ŀ���ļ���  
    //AfxMessageBox("�����ļ���"+target);  
    CFileFind finder;  
    CString path;  
    path.Format(_T("%s/*.*"),source);  
    //AfxMessageBox(path);  
    BOOL bWorking =finder.FindFile(path);  
    while(bWorking)
    {  
        bWorking = finder.FindNextFile();  
        //AfxMessageBox(finder.GetFileName());  
        if(finder.IsDirectory() && !finder.IsDots())//���ļ��� ���� ���Ʋ��� . �� ..  
        { 
            //�ݹ鴴���ļ���+"/"+finder.GetFileName()
            myCopyDirectory(finder.GetFilePath(),target+"/"+finder.GetFileName());   
        }  
        else//���ļ� ��ֱ�Ӹ��� 
        {  
            //AfxMessageBox("�����ļ�"+finder.GetFilePath());//+finder.GetFileName()  
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
        if(finder.IsDirectory() && !finder.IsDots())//�����ļ���
        {
            myDeleteDirectory(finder.GetFilePath()); //�ݹ�ɾ���ļ���
            RemoveDirectory(finder.GetFilePath());
        }
        else//�����ļ�
        {
            DeleteFile(finder.GetFilePath());
        }
    }
    RemoveDirectory(directory_path);
}