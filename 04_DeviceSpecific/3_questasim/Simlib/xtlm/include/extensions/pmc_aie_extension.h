// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
#ifndef _PMC_AIE_EXTENSION_
#define _PMC_AIE_EXTENSION_

#include "xtlm_generic_payload/xtlm_extension.h"

namespace xsc {

namespace extension {

class pmc_aie_extension: public xtlm::xtlm_extension<pmc_aie_extension> {
public:
    pmc_aie_extension():pmc_aie_cdo_config(true) {
    }
    xtlm::xtlm_extension_base* clone() const {
		// Must override pure virtual clone method
		pmc_aie_extension* t = new pmc_aie_extension();
        t->set_pmc_aie_cdo_config(pmc_aie_cdo_config);
		return t;
	}
	// Must override pure virtual copy_from method
	void copy_from(xtlm::xtlm_extension_base const &ext) {
		pmc_aie_cdo_config     = static_cast<pmc_aie_extension const &>(ext).pmc_aie_cdo_config;
	}
    
    bool get_pmc_aie_cdo_config() {
        return pmc_aie_cdo_config;
    }
    void set_pmc_aie_cdo_config(const bool val) {
        pmc_aie_cdo_config = val;
    }
private:
    bool pmc_aie_cdo_config;
};
}
}
#endif

