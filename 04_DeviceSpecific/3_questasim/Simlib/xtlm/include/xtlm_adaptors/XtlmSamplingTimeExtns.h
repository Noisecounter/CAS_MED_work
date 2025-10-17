// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
//#ifndef _NPPFlitType_h_
//#define _NPPFlitType_h_

//#include "tlm.h"
//#include "xtlm_generic_payload/xtlm_extension.h"
//class XtlmSamplingTimeExtns: public xtlm::xtlm_extension<XtlmSamplingTimeExtns> 
//{
//    public: 
//        XtlmSamplingTimeExtns() {
//            flitHdr.reset();
//        }
//
//	void reset(){
//            flitHdr.reset();
//	}
//
//        xtlm::xtlm_extension_base* clone() const { 
//            return new FlitHdrExtension(*this); 
//        }
//
//        void copy_from(xtlm::xtlm_extension_base const & that ) { 
//            *this = static_cast<const FlitHdrExtension&>(that); 
//        }
//}

#ifndef _XTLMSAMPLINGTIMEEXTNS_
#define _XTLMSAMPLINGTIMEEXTNS_

#include "xtlm_generic_payload/xtlm_extension.h"
#include "utils/xtlm_cmnhdr.h"

namespace xsc
{

    namespace extension
    {

        class XtlmSamplingTimeExtns: public xtlm::xtlm_extension<XtlmSamplingTimeExtns> {
            public:
                XtlmSamplingTimeExtns():sampling_time(sc_time(0,SC_PS)) {
                }
                xtlm::xtlm_extension_base* clone() const {
                    // Must override pure virtual clone method
                    XtlmSamplingTimeExtns* t = new XtlmSamplingTimeExtns();
                    t->set_sampling_time(sampling_time);
                    return t;
                }
                // Must override pure virtual copy_from method
                void copy_from(xtlm::xtlm_extension_base const &ext) {
                    sampling_time     = static_cast<XtlmSamplingTimeExtns const &>(ext).sampling_time;
                }

                sc_core::sc_time get_sampling_time() {
                    return sampling_time;
                }
                void set_sampling_time(sc_core::sc_time val) {
                    sampling_time = val;
                }
            private:
                sc_core::sc_time sampling_time;
        };

    }
}
#endif
