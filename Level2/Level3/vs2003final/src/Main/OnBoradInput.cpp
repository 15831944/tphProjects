#include <stdafx.h>
#include "OnBoradInput.h"
#include "TermPlanDoc.h"
//#include "../on_borad_impl/view_cmd.h"
#include "../Database/ADODatabase.h"
//#include "../on_borad_impl\3dengine\help\tinyxml\tinyxml.h"
#include "../Common/CAutoSaveHandlerInterface.h"
bool is_file_exsit(const char* path)
{
	if(!path) return false;
	DWORD status = ::GetFileAttributes(path);
	if(status == 0xFFFFFFFF) return false;
	if(status & FILE_ATTRIBUTE_DIRECTORY) return false;
	return true;
}

const char* force_cur_dir()
{
	static char fullpath[256];
	GetModuleFileName(0, fullpath, sizeof(fullpath));
	for(int end=(int)strlen(fullpath)-1; end>=0; end -- )
	{
		if( fullpath[end] == '\\' || fullpath[end] == '/' ) break;
		fullpath[end] = 0;
	}
	SetCurrentDirectory(fullpath);
	return fullpath;
}

void OnBoradACInfo::save(CString path)
{
	if(CAutoSaveHandlerInterface::PAUTOSAVE_HANDLE)
		CAutoSaveHandlerInterface::PAUTOSAVE_HANDLE->DoAutoSave() ;
	force_cur_dir();
	TiXmlDocument* doc = new TiXmlDocument();
	TiXmlElement* root = new TiXmlElement("actypeinfo");
	doc->LinkEndChild(root);
	TiXmlElement* path_node = new TiXmlElement("model_path");
	path_node->SetAttribute("path", m_3d_model_path.c_str());
	root->LinkEndChild(path_node);
	doc->SaveFile(path.GetBuffer());
	delete doc;
}
void OnBoradACInfo::load(CString path)
{
	force_cur_dir();
	TiXmlDocument* doc = new TiXmlDocument();
	doc->LoadFile(path.GetBuffer());
	TiXmlElement* path_node = doc->RootElement()->FirstChildElement();
	m_3d_model_path = path_node->Attribute("path");
	delete doc;
}

OnBoradInput::OnBoradInput(CTermPlanDoc* doc)
{
	m_host_doc = doc;
}
OnBoradInput::~OnBoradInput()
{
	destory();
}
OnBoradACInfo* OnBoradInput::add_acinfo(const std::string& name)
{
	m_cur_acinfo_name = name;
	OnBoradACInfo* result = new OnBoradACInfo;
	m_acinfo_map[name] = result;
	return result;
}
void OnBoradInput::destory_acinfo(const std::string& name)
{
	if(m_cur_acinfo_name == name) m_cur_acinfo_name = "";
	std::map<std::string, OnBoradACInfo*>::iterator r = m_acinfo_map.find(name);
	if(r == m_acinfo_map.end()) return;
	delete r->second;
	m_acinfo_map.erase(r);
}
OnBoradACInfo* OnBoradInput::get_acinfo(const std::string& name)
{
	std::map<std::string, OnBoradACInfo*>::iterator r = m_acinfo_map.find(name);
	if(r == m_acinfo_map.end()) return 0;
	return r->second;
}
OnBoradACInfo* OnBoradInput::set_cur_info_name(const std::string& name)
{
	m_cur_acinfo_name = name;
	return get_acinfo(name);
}
const std::string& OnBoradInput::get_cur_info_name()
{
	return m_cur_acinfo_name;
}
OnBoradACInfo* OnBoradInput::get_cur_info()
{
	return get_acinfo(m_cur_acinfo_name);
}
void OnBoradInput::destory()
{
	m_cur_acinfo_name = "";
	std::map<std::string, OnBoradACInfo*>::iterator i(m_acinfo_map.begin()), e(m_acinfo_map.end());
	for(; i!=e; ++i) delete i->second;
	m_acinfo_map.clear();
}
/*
bool OnBoradInput::load_actype_info_from_database(const char* name)
{
	if(!name) return false;
	CString strSQL;
	strSQL.Format(_T("SELECT * FROM ONBORADLAYOUT WHERE (ACTYPE = '%s')"), name);
	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);
	while(!adoRecordset.IsEOF()) 
	{
		CString actype_name, model_path;
		adoRecordset.GetFieldValue("ACTYPE", actype_name);
		adoRecordset.GetFieldValue("MODELPATH", model_path);
		add_acinfo(actype_name.GetBuffer());
		set_cur_info_name(actype_name.GetBuffer());
		get_cur_info()->m_3d_model_path = model_path.GetBuffer();
		return true;
	}
	return false;
}
void OnBoradInput::save_cur_actype_info_to_database()
{
	OnBoradACInfo* cur_info = get_cur_info();
	if(!cur_info) return;
	CString strSQL;
	strSQL.Format(_T("SELECT * FROM ONBORADLAYOUT WHERE (ACTYPE = '%s')"), get_cur_info_name().c_str());
	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);
	bool has_record = false;
	while(!adoRecordset.IsEOF()) 
	{
		has_record = true;
		adoRecordset.MoveNextData();
	}
	if(has_record)
		strSQL.Format("UPDATE ONBORADLAYOUT SET MODELPATH ='%s' WHERE ACTYPE = '%s'", cur_info->m_3d_model_path.c_str(), get_cur_info_name().c_str() );
	else
		strSQL.Format("INSERT INTO ONBORADLAYOUT (ACTYPE, MODELPATH) VALUES ('%s', '%s')", get_cur_info_name().c_str(), cur_info->m_3d_model_path.c_str());
	CADODatabase::ExecuteSQLStatement(strSQL);
}
*/
bool OnBoradInput::load_actype_info_from_database(const char* name)
{
	if(!name) return false;
	force_cur_dir();
	std::string file_path = name + std::string(".xml");
	if(!is_file_exsit(file_path.c_str())) return false;
	TiXmlDocument* doc = new TiXmlDocument();
	doc->LoadFile(file_path.c_str());
	TiXmlElement* path_node = doc->RootElement()->FirstChildElement();
	std::string model_path = path_node->Attribute("path");
	add_acinfo(name);
	set_cur_info_name(name);
	get_cur_info()->m_3d_model_path = model_path;
	delete doc;
	return true;
}
void OnBoradInput::save_cur_actype_info_to_database()
{
	if(CAutoSaveHandlerInterface::PAUTOSAVE_HANDLE)
		CAutoSaveHandlerInterface::PAUTOSAVE_HANDLE->DoAutoSave() ;
	OnBoradACInfo* cur_info = get_cur_info();
	if(!cur_info) return;
	force_cur_dir();
	TiXmlDocument* doc = new TiXmlDocument();
	TiXmlElement* root = new TiXmlElement("actypeinfo");
	doc->LinkEndChild(root);
	TiXmlElement* path_node = new TiXmlElement("model_path");
	path_node->SetAttribute("path", cur_info->m_3d_model_path.c_str());
	root->LinkEndChild(path_node);
	std::string file_path = get_cur_info_name() + std::string(".xml");
	doc->SaveFile(file_path.c_str());
	delete doc;
}