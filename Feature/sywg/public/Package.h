#ifndef __PACKAGE_H
#define __PACKAGE_H
using namespace std;
#include "BusStruct.h"
#include <vector>

class CPackage
{
public:
	CPackage();
	~CPackage();

////////////////////////////////////////////////////////////////////////
	//设置报文头
	virtual void SetPackageHeader(PACKAGEHEADER *pHeader) = 0;
	//设置功能号
	virtual void SetFuncNo(int nFuncNo) = 0;
	//增加一条记录(空)
	virtual int AppendBlankRecord(ERROR_INFO &errInfo) = 0;
	//设置字段的值(char *类型)
	virtual bool SetFieldValue(int index,char *fieldname,char *value,ERROR_INFO &errInfo) = 0;
	//设置字段的值(string类型)
	virtual bool SetFieldValue(int index,char *fieldname,string &value,ERROR_INFO &errInfo) = 0;
	//设置字段的值(int类型)
	virtual bool SetFieldValue(int index,char *fieldname,int value,ERROR_INFO &errInfo) = 0;
	//设置字段的值(double类型)
	virtual bool SetFieldValue(int index,char *fieldname,double value,ERROR_INFO &errInfo) = 0;
	//设置strvector类型的数据
	virtual bool SetFieldValue(int index,char *fieldname,vector<string> &value,ERROR_INFO &errInfo) = 0;
	//设置intvector类型的数据
	virtual bool SetFieldValue(int index,char *fieldname,vector<int> &value,ERROR_INFO &errInfo) = 0;
	//设置doublevector类型的数据
	virtual bool SetFieldValue(int index,char *fieldname,vector<double> &value,ERROR_INFO &errInfo) = 0;

////////////////////////////////////////////////////////////////////////
	//获得字段数量
	virtual int GetFieldCount() = 0;
	//获得记录数
	virtual int GetRecordCount() = 0;
	//获得包头
	virtual PACKAGEHEADER *GetPackageHeader() = 0;
	//获得字符串类型的数据
	virtual bool GetFieldValue(int recordindex,char *fieldname,string &RetStr,ERROR_INFO &errInfo) = 0;
	//获得int类型的数据
	virtual bool GetFieldValue(int recordindex,char *fieldname,int &Ret,ERROR_INFO &errInfo) = 0;
	//获得double类型的数据
	virtual bool GetFieldValue(int recordindex,char *fieldname,double &Ret,ERROR_INFO &errInfo) = 0;
	//获得strvector类型的值
	virtual bool GetFieldValue(int recordindex,char *fieldname,vector <string> &RetStr,ERROR_INFO &errInfo) = 0;
	//获得intvector类型的值
	virtual bool GetFieldValue(int recordindex,char *fieldname,vector <int> &RetStr,ERROR_INFO &errInfo) = 0;
	//获得doublevector类型的值
	virtual bool GetFieldValue(int recordindex,char *fieldname,vector <double> &RetStr,ERROR_INFO &errInfo) = 0;

///////////////////////////////////////////////////////////////////////////////
	//获得字段信息
	virtual vector<FIELDINFO> *GetFieldInfo() = 0;

};
#endif
