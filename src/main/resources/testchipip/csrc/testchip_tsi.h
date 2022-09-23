#ifndef __TESTCHIP_TSI_H
#define __TESTCHIP_TSI_H

#include <fesvr/tsi.h>
#include <fesvr/htif.h>
#include "mm_dramsim2.h"

class testchip_tsi_t : public tsi_t
{
 public:
  testchip_tsi_t(int argc, char** argv, bool has_loadmem, void *ch);
  virtual ~testchip_tsi_t() {};

  void write_chunk(addr_t taddr, size_t nbytes, const void* src) override;
  void read_chunk(addr_t taddr, size_t nbytes, void* dst) override;
  void load_program() {
    switch_to_target();
    is_loadmem = has_loadmem;
    tsi_t::load_program();
    //is_loadmem = false;
  }

 protected:
  virtual void load_mem_write(addr_t taddr, size_t nbytes, const void* src);
  virtual void load_mem_read(addr_t taddr, size_t nbytes, void* dst);
  bool is_ddr(addr_t taddr) { return (taddr >= get_entry_point()); }
  bool is_address_preloaded(addr_t taddr, size_t len) {
    return has_loadmem && is_ddr(taddr);
  }
  void dump_mem(addr_t taddr, size_t nbytes, const void* src);
  bool has_loadmem;
  mm_t *dram;

 private:

  bool is_loadmem;
};
#endif
