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
	//���ñ���ͷ
	virtual void SetPackageHeader(PACKAGEHEADER *pHeader) = 0;
	//���ù��ܺ�
	virtual void SetFuncNo(int nFuncNo) = 0;
	//����һ����¼(��)
	virtual int AppendBlankRecord(ERROR_INFO &errInfo) = 0;
	//�����ֶε�ֵ(char *����)
	virtual bool SetFieldValue(int index,char *fieldname,char *value,ERROR_INFO &errInfo) = 0;
	//�����ֶε�ֵ(string����)
	virtual bool SetFieldValue(int index,char *fieldname,string &value,ERROR_INFO &errInfo) = 0;
	//�����ֶε�ֵ(int����)
	virtual bool SetFieldValue(int index,char *fieldname,int value,ERROR_INFO &errInfo) = 0;
	//�����ֶε�ֵ(double����)
	virtual bool SetFieldValue(int index,char *fieldname,double value,ERROR_INFO &errInfo) = 0;
	//����strvector���͵�����
	virtual bool SetFieldValue(int index,char *fieldname,vector<string> &value,ERROR_INFO &errInfo) = 0;
	//����intvector���͵�����
	virtual bool SetFieldValue(int index,char *fieldname,vector<int> &value,ERROR_INFO &errInfo) = 0;
	//����doublevector���͵�����
	virtual bool SetFieldValue(int index,char *fieldname,vector<double> &value,ERROR_INFO &errInfo) = 0;

////////////////////////////////////////////////////////////////////////
	//����ֶ�����
	virtual int GetFieldCount() = 0;
	//��ü�¼��
	virtual int GetRecordCount() = 0;
	//��ð�ͷ
	virtual PACKAGEHEADER *GetPackageHeader() = 0;
	//����ַ������͵�����
	virtual bool GetFieldValue(int recordindex,char *fieldname,string &RetStr,ERROR_INFO &errInfo) = 0;
	//���int���͵�����
	virtual bool GetFieldValue(int recordindex,char *fieldname,int &Ret,ERROR_INFO &errInfo) = 0;
	//���double���͵�����
	virtual bool GetFieldValue(int recordindex,char *fieldname,double &Ret,ERROR_INFO &errInfo) = 0;
	//���strvector���͵�ֵ
	virtual bool GetFieldValue(int recordindex,char *fieldname,vector <string> &RetStr,ERROR_INFO &errInfo) = 0;
	//���intvector���͵�ֵ
	virtual bool GetFieldValue(int recordindex,char *fieldname,vector <int> &RetStr,ERROR_INFO &errInfo) = 0;
	//���doublevector���͵�ֵ
	virtual bool GetFieldValue(int recordindex,char *fieldname,vector <double> &RetStr,ERROR_INFO &errInfo) = 0;

///////////////////////////////////////////////////////////////////////////////
	//����ֶ���Ϣ
	virtual vector<FIELDINFO> *GetFieldInfo() = 0;

};
#endif
