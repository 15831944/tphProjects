#ifndef __TRANSSTRUCT__H  
#define __TRANSSTRUCT__H  

using namespace std;
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
//----------------------------------------------------------
//功能号
#define CANCELEVENT            -1  //取消事务
#define EVENT_RSP              -2  //文件传输应答

#define REQTYPE                 0//请求类型
#define RSPTYPE                 1//应答类型
#define DATASENDTYPE            2//数据发送类型

#define MSG_PROGRESS            1 //进度信息
#define MSG_SUCESS              9999//成功信息

//----------------------------------------------------------
#define FIRSTFLAG   1

#define STRING_TYPE 0  //字符和字符串类型
#define INT_TYPE    1 //int类型
#define DOUBLE_TYPE 2 //double类型
#define STRING_LIST_TYPE 3 //字符串数组
#define INT_LIST_TYPE 4 //整数数组
#define DOUBLE_LIST_TYPE 5 //double数组


//拷贝字符串
void CopyString(const char *source,char *dest,int destsize);
//写日志
int WriteLog(bool bErrorLog,const char *format,...);

typedef struct _ERROR_INFO
{
    int nErrID;
    char strErrMsg[200];
}ERROR_INFO;

typedef struct _NTY_STATUS_INFO
{
    int nMsgType;       //应答类型，=EVENT_RSP是事务应答信息； =MSG_PROGRESS是传输进度信息
    int nEventHandle;
    int nMsgID;
    char strMsgInfo[300];
}NTY_STATUS_INFO;

typedef struct __FieldDefine
{
    char FieldName[200];//字段名
    int FieldType;//字段类型
    char Comment[100];//注释
}FIELDDEF;

typedef struct __FieldDefine
{
    char FieldName[200];        //字段名
    int FieldType;              //字段类型
    char Comment[100];          //注释
}FIELDDEF;

//报文头
typedef struct __PackageHeader
{
    int nFuncType;          //功能类型，0是请求，1是应答
    int nFuncNo;            //具体功能号
    int nPackageIndex;      //包序号
    int nTotalPackageNum;   //包总数
    int nTotalRecordNum;    //所有的记录数
    int nEventHandle;       //事务的句柄，全局唯一
}PACKAGEHEADER;

typedef struct _FIELDINFO
{
    FIELDDEF FieldDefine;   //数据结构
    int SubIndex;           //子类型的序号
};

typedef struct _DXPhandle
{
    char buf[28];
}USER_HANDLE;

#endif