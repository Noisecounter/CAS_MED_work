// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689

/*  (c) Copyright 2014 - 2019 Xilinx, Inc. All rights reserved.

 This file contains confidential and proprietary information
 of Xilinx, Inc. and is protected under U.S. and
 international copyright and other intellectual property
 laws.

 DISCLAIMER
 This disclaimer is not a license and does not grant any
 rights to the materials distributed herewith. Except as
 otherwise provided in a valid license issued to you by
 Xilinx, and to the maximum extent permitted by applicable
 law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND
 WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES
 AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
 BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
 INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
 (2) Xilinx shall not be liable (whether in contract or tort,
 including negligence, or under any other theory of
 liability) for any loss or damage of any kind or nature
 related to, arising under or in connection with these
 materials, including for any direct, or any indirect,
 special, incidental, or consequential loss or damage
 (including loss of data, profits, goodwill, or any type of
 loss or damage suffered as a result of any action brought
 by a third party) even if such damage or loss was
 reasonably foreseeable or Xilinx had been advised of the
 possibility of the same.

 CRITICAL APPLICATIONS
 Xilinx products are not designed or intended to be fail-
 safe, or for use in any application requiring fail-safe
 performance, such as life-support or safety devices or
 systems, Class III medical devices, nuclear facilities,
 applications related to the deployment of airbags, or any
 other applications that could lead to death, personal
 injury, or severe property or environmental damage
 (individually and collectively, "Critical
 Applications"). Customer assumes the sole risk and
 liability of any use of Xilinx products in Critical
 Applications, subject only to applicable laws and
 regulations governing limitations on product liability.

 THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
 PART OF THIS FILE AT ALL TIMES.                       */

#ifndef __ME_AXI_STREAM_H__
#define __ME_AXI_STREAM_H__

#include <tlm.h>
#include <vector>
#include <queue>

#define NO_STREAM_DATA (0x77777777)

class MEStreamData32 {
public:
	uint32_t data;
	bool tlast;

	MEStreamData32(uint32_t d = 0, bool l = 0) :
			data(d), tlast(l) {
	}

	friend std::ostream &operator <<(std::ostream &os,
			MEStreamData32 const &data) {
		os << std::showbase << std::hex << data.data;
		if (data.tlast)
			os << "[TLAST]";
		return os;
	}
};

class MEStreamData64 {
public:
	uint32_t data[2];
	bool tlast;
	bool tkeep;

	MEStreamData64(uint32_t d0 = 0, uint32_t d1 = 0, bool tl = false, bool tk =
			false) {
		data[0] = d0;
		data[1] = d1;
		tlast = tl;
		tkeep = tk;
	}

	friend std::ostream &operator <<(std::ostream &os,
			MEStreamData64 const &data) {
		os << std::showbase << std::hex << data.data[1] << ", " << data.data[0]
				<< " ";
		if (data.tlast)
			os << "[TLAST]";
		if (data.tkeep)
			os << "[TKEEP]";
		return os;
	}
};

class MEStreamData256 {
  public:
    uint32_t data[8];
    bool     tlast;
    uint8_t  tkeep; // TODO: a priori enconded in HW with 3 bits.

    MEStreamData256(uint32_t d0=0, uint32_t d1=0, uint32_t d2=0, uint32_t d3=0,
                    uint32_t d4=0, uint32_t d5=0, uint32_t d6=0, uint32_t d7=0,
                    bool tl=false, uint8_t tk=0)
     : tlast(tl)
     , tkeep(tk)
    {
      data[0] = d0;
      data[1] = d1;
      data[2] = d2;
      data[3] = d3;
      data[4] = d4;
      data[5] = d5;
      data[6] = d6;
      data[7] = d7;
    }

    void reset()
    {
      data[0] = NO_STREAM_DATA;
      data[1] = NO_STREAM_DATA;
      data[2] = NO_STREAM_DATA;
      data[3] = NO_STREAM_DATA;
      data[4] = NO_STREAM_DATA;
      data[5] = NO_STREAM_DATA;
      data[6] = NO_STREAM_DATA;
      data[7] = NO_STREAM_DATA;
      tlast = false;
      tkeep = 0;
    }
    
    friend std::ostream &operator << (std::ostream &os, MEStreamData256 const &data)
    {
      os << std::showbase << std::hex << data.data[7] << ", " << data.data[6] << ", " << data.data[5] << ", " << data.data[4] << ", "
                                      << data.data[3] << ", " << data.data[2] << ", " << data.data[1] << ", " << data.data[0] << " "; 
      if (data.tlast)
        os << "[TLAST] [TKEEP=" << (uint32_t) data.tkeep << "]";
      return os;
    }
};

class NoCStreamData128 {
public:
	uint32_t data[4];
	uint32_t tid;
	uint32_t destid;
	uint32_t tdest;
	uint32_t tkeep;
	bool tlast;

	NoCStreamData128(uint32_t d0 = 0, uint32_t d1 = 0, uint32_t d2 = 0,
			uint32_t d3 = 0, bool tl = false) {
		data[0] = d0;
		data[1] = d1;
		data[2] = d2;
		data[3] = d3;
		tlast = tl;
		tid = 0;
		destid = 0;
		tdest = 0;
		tkeep = 0;
	}
};

class NoCStreamData512 {
  public:
    uint32_t data[16];
    unsigned long long addr[16];
    uint32_t tid;
    uint32_t destid;
    uint32_t tdest;
    uint32_t tkeep;
    bool tlast;

    NoCStreamData512(uint32_t d0=0, uint32_t d1=0, uint32_t d2=0, uint32_t d3=0,
                     uint32_t d4=0, uint32_t d5=0, uint32_t d6=0, uint32_t d7=0,
                     uint32_t d8=0, uint32_t d9=0, uint32_t d10=0, uint32_t d11=0,
                     uint32_t d12=0, uint32_t d13=0, uint32_t d14=0, uint32_t d15=0,
                     bool tl=false)
     : data{d0,d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11,d12,d13,d14,d15}
     , addr{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
     , tid(0)
     , destid(0)
     , tdest(0)
     , tkeep(0)
     , tlast(tl)
    {
    }

    friend std::ostream& operator<<(std::ostream& os, const NoCStreamData512& w) 
    {
      for(int i = 0; i < 16; i++)
      {
        os << w.data[i];
        if(i != 15)
        {
          os << ", ";
        }
      }
      if(w.tlast)
      {
        os << " TLAST";
      }
      return os;
    }
};

typedef tlm::tlm_fifo<MEStreamData32> MEStream32_fifo;
typedef tlm::tlm_fifo_put_if<MEStreamData32> MEStream32_put_if;
typedef sc_core::sc_port<MEStream32_put_if> MEStream32_put_port;
typedef sc_core::sc_export<MEStream32_put_if> MEStream32_put_export;
typedef tlm::tlm_fifo_get_if<MEStreamData32> MEStream32_get_if;
typedef sc_core::sc_port<MEStream32_get_if> MEStream32_get_port;
typedef sc_core::sc_export<MEStream32_get_if> MEStream32_get_export;

typedef tlm::tlm_fifo<MEStreamData64> MEStream64_fifo;
typedef tlm::tlm_fifo_put_if<MEStreamData64> MEStream64_put_if;
typedef sc_core::sc_port<MEStream64_put_if> MEStream64_put_port;
typedef sc_core::sc_export<MEStream64_put_if> MEStream64_put_export;
typedef tlm::tlm_fifo_get_if<MEStreamData64> MEStream64_get_if;
typedef sc_core::sc_port<MEStream64_get_if> MEStream64_get_port;
typedef sc_core::sc_export<MEStream64_get_if> MEStream64_get_export;

typedef tlm::tlm_fifo<MEStreamData256>          MEStream256_fifo;
typedef tlm::tlm_fifo_put_if<MEStreamData256>   MEStream256_put_if;
typedef sc_core::sc_port<MEStream256_put_if>    MEStream256_put_port;
typedef sc_core::sc_export<MEStream256_put_if>  MEStream256_put_export;
typedef tlm::tlm_fifo_get_if<MEStreamData256>   MEStream256_get_if;
typedef sc_core::sc_port<MEStream256_get_if>    MEStream256_get_port;
typedef sc_core::sc_export<MEStream256_get_if>  MEStream256_get_export;

typedef tlm::tlm_fifo<NoCStreamData128> NoCStream128_fifo;
typedef tlm::tlm_fifo_put_if<NoCStreamData128> NoCStream128_put_if;
typedef sc_core::sc_port<NoCStream128_put_if> NoCStream128_put_port;
typedef sc_core::sc_export<NoCStream128_put_if> NoCStream128_put_export;
typedef tlm::tlm_fifo_get_if<NoCStreamData128> NoCStream128_get_if;
typedef sc_core::sc_port<NoCStream128_get_if> NoCStream128_get_port;
typedef sc_core::sc_export<NoCStream128_get_if> NoCStream128_get_export;

typedef tlm::tlm_fifo<NoCStreamData512>         NoCStream512_fifo;
typedef tlm::tlm_fifo_put_if<NoCStreamData512>  NoCStream512_put_if;
typedef sc_core::sc_port<NoCStream512_put_if>   NoCStream512_put_port;
typedef sc_core::sc_export<NoCStream512_put_if> NoCStream512_put_export;
typedef tlm::tlm_fifo_get_if<NoCStreamData512>  NoCStream512_get_if;
typedef sc_core::sc_port<NoCStream512_get_if>   NoCStream512_get_port;
typedef sc_core::sc_export<NoCStream512_get_if> NoCStream512_get_export;

#endif //__ME_AXI_STREAM_H__


