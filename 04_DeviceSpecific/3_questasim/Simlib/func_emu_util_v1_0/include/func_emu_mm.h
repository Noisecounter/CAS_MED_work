// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
#pragma once

#include "xtlm.h"
#include "xtlm_ap_ctrl.h"

namespace xsc {
namespace func {
namespace mm {

class base_access
{
  private:
    xtlm::xtlm_aximm_master_if* mm_if; 
  
  protected:
    void set_mm_if(xtlm::xtlm_aximm_master_if*& mm_if);
    void xtlm_rd_payload(char* data, size_t addr, size_t size);
    void xtlm_wr_payload(char* data, size_t addr, size_t size);

  public:
    virtual ~base_access() {}
    virtual void get_data(char* data, size_t addr, size_t size) = 0;
    virtual void set_data(char* data, size_t addr, size_t size) = 0;

};

class access_if 
{
  private:
    base_access *_access_if;

  public:
    access_if(base_access *_access_if = nullptr);
    ~access_if();

    void set_interface(base_access *_access_if);
    void get_data(char* data, size_t addr, size_t size) const;
    void set_data(char* data, size_t addr, size_t size) const;
};

class direct_access: public base_access
{
  public:
    direct_access(xtlm::xtlm_aximm_master_if*& mm_if);

    void get_data(char* data, size_t addr, size_t size);
    void set_data(char* data, size_t addr, size_t size);
};

class cached_access: public base_access 
{
  private:
    size_t _cache_size;
    size_t _valid_size;
    long long unsigned int _base_addr;
    bool _is_ready;
    char* _cache_data;
    bool* _visited;

    void _flush();

  public:
    cached_access(xtlm::xtlm_aximm_master_if* mm_if, size_t _cache_size);
    ~cached_access();

    void get_data(char* data, size_t addr, size_t size);
    void set_data(char* data, size_t addr, size_t size);

    // Experimental -- Use when transactions are in multiples of cache_size
    bool _enable_wr_cache=false;

};

} //! namespace mm
} //! namespace func
} //! namespace xsc

