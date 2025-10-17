// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689

#ifndef __AIE_HANDLE_SIM_CONFIG_H__
#define __AIE_HANDLE_SIM_CONFIG_H__

#include<stdio.h>
#include<iostream>
#include<fstream>
#include<string>
#include<map>
struct struct_sim_config
{
  std::string json_device_file_path;
  std::string aie_sim_sol_path;
  std::string used_tiles_file_path;
};

class ReadSimConfig
{
public:
	ReadSimConfig();
  ~ReadSimConfig();
  struct_sim_config aie_sim_config;
  std::map<unsigned int, std::string> sxx_plio_info_map;
  std::map<unsigned int, std::string> mxx_plio_info_map;
  int read_sim_config_file(std::string file_name);
  int read_scsim_config_file(std::string aie_work_dir); 
  void initialize_sim_config();
  std::map<unsigned int, std::string> get_sxx_plio_info_map();
  std::map<unsigned int, std::string> get_mxx_plio_info_map();
};


#endif //end of __AIE_HANDLE_SIM_CONFIG_H__
