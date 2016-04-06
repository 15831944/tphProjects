#ifndef __TRANSSTRUCT__H  
#define __TRANSSTRUCT__H  

using namespace std;
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
//----------------------------------------------------------
//���ܺ�
#define CANCELEVENT            -1  //ȡ������
#define EVENT_RSP              -2  //�ļ�����Ӧ��

#define REQTYPE                 0//��������
#define RSPTYPE                 1//Ӧ������
#define DATASENDTYPE            2//���ݷ�������

#define MSG_PROGRESS            1 //������Ϣ
#define MSG_SUCESS              9999//�ɹ���Ϣ

//----------------------------------------------------------
#define FIRSTFLAG   1

#define STRING_TYPE 0  //�ַ����ַ�������
#define INT_TYPE    1 //int����
#define DOUBLE_TYPE 2 //double����
#define STRING_LIST_TYPE 3 //�ַ�������
#define INT_LIST_TYPE 4 //��������
#define DOUBLE_LIST_TYPE 5 //double����


//�����ַ���
void CopyString(const char *source,char *dest,int destsize);
//д��־
int WriteLog(bool bErrorLog,const char *format,...);

typedef struct _ERROR_INFO
{
    int nErrID;
    char strErrMsg[200];
}ERROR_INFO;

typedef struct _NTY_STATUS_INFO
{
    int nMsgType;       //Ӧ�����ͣ�=EVENT_RSP������Ӧ����Ϣ�� =MSG_PROGRESS�Ǵ��������Ϣ
    int nEventHandle;
    int nMsgID;
    char strMsgInfo[300];
}NTY_STATUS_INFO;

typedef struct __FieldDefine
{
    char FieldName[200];//�ֶ���
    int FieldType;//�ֶ�����
    char Comment[100];//ע��
}FIELDDEF;

typedef struct __FieldDefine
{
    char FieldName[200];        //�ֶ���
    int FieldType;              //�ֶ�����
    char Comment[100];          //ע��
}FIELDDEF;

//����ͷ
typedef struct __PackageHeader
{
    int nFuncType;          //�������ͣ�0������1��Ӧ��
    int nFuncNo;            //���幦�ܺ�
    int nPackageIndex;      //�����
    int nTotalPackageNum;   //������
    int nTotalRecordNum;    //���еļ�¼��
    int nEventHandle;       //����ľ����ȫ��Ψһ
}PACKAGEHEADER;

typedef struct _FIELDINFO
{
    FIELDDEF FieldDefine;   //���ݽṹ
    int SubIndex;           //�����͵����
};

typedef struct _DXPhandle
{
    char buf[28];
}USER_HANDLE;

#endif