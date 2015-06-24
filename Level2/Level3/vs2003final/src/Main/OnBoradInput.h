#pragma once

bool is_file_exsit(const char* path);
const char* force_cur_dir();

class OnBoradACInfo
{
public:
	std::string m_3d_model_path;

	void save(CString path);
	void load(CString path);
};

class CTermPlanDoc;
class OnBoradInput
{
	std::map<std::string, OnBoradACInfo*> m_acinfo_map;
	CTermPlanDoc* m_host_doc;
	std::string m_cur_acinfo_name;
public:
	OnBoradInput(CTermPlanDoc* doc);
	~OnBoradInput();
	OnBoradACInfo* add_acinfo(const std::string& name);
	OnBoradACInfo* get_acinfo(const std::string& name);
	OnBoradACInfo* set_cur_info_name(const std::string& name);
	const std::string& get_cur_info_name();
	OnBoradACInfo* get_cur_info();
	void destory_acinfo(const std::string& name);
	void destory();

	bool load_actype_info_from_database(const char* name);
	void save_cur_actype_info_to_database();
};