#include <iostream>
#include <iomanip>
#include "testchip_tsi.h"

testchip_tsi_t::testchip_tsi_t(int argc, char** argv, bool can_have_loadmem, void *ch) : tsi_t(argc, argv)
{
  has_loadmem = false;
  std::vector<std::string> args(argv + 1, argv + argc);
  for (auto& arg : args) {
    if (arg.find("+loadmem=") == 0)
      has_loadmem = can_have_loadmem;
  }
  if (has_loadmem) {
    dram = (mm_t *)ch;
  }
}


void testchip_tsi_t::write_chunk(addr_t taddr, size_t nbytes, const void* src)
{
  //std::cout << __FUNCTION__ << ": " << std::hex << taddr << ", " << nbytes << std::endl;
  if (is_loadmem && is_ddr(taddr)) {
    load_mem_write(taddr, nbytes, src);
  } else {
    tsi_t::write_chunk(taddr, nbytes, src);
  }
}

void testchip_tsi_t::read_chunk(addr_t taddr, size_t nbytes, void* dst)
{
  //std::cout << __FUNCTION__ << ": " << std::hex << taddr << ", " << nbytes << std::endl;
  if (is_loadmem && is_ddr(taddr) && nbytes > 64) {
    load_mem_read(taddr, nbytes, dst);
  } else {
    tsi_t::read_chunk(taddr, nbytes, dst);
  }
}

void testchip_tsi_t::load_mem_write(addr_t taddr, size_t nbytes, const void* src)
{
  tsi_t::write_chunk(taddr, nbytes, src);
}

void testchip_tsi_t::load_mem_read(addr_t taddr, size_t nbytes, void* dst)
{
  uint8_t *result = static_cast<uint8_t *>(dst);
  size_t len = nbytes/sizeof(uint8_t);
  uint8_t *base = (uint8_t *)(dram->get_data());
  size_t size = dram->get_size();
  //dump_mem(taddr % size, nbytes, dram->get_data());
  for (size_t i = 0; i < len; i++) {
    size_t addr = (taddr + i) % size;
    result[i] = base[addr];
  }
}

void testchip_tsi_t::dump_mem(addr_t taddr, size_t nbytes, const void* src)
{
  const uint8_t *buf = static_cast<const uint8_t *>(src);
  //std::cout << "Address  ";
  //for (size_t i = 0; i < 16; i++) {
  //  std::cout << std::setfill('0') << std::setw(2) << std::hex << i;
  //}
  //std::cout << endl;
  for (size_t i = 0; i < nbytes; i++) {
    size_t ofs  = 15 - i % 16;
    size_t line = i / 16;
    size_t addr = taddr + line*16;
    if (i % 16 == 0) {
      std::cout << std::setfill('0') << std::setw(8) << std::hex << addr << ' ';
    }
    std::cout << std::setfill('0') << std::setw(2) << std::hex << unsigned(buf[addr + ofs]);
    if (i % 16 == 15) {
      std::cout << std::endl;
    } else if (i % 4 == 3) {
      std::cout << ' ';
    }
  }
}

