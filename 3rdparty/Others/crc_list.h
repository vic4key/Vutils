#ifndef CRC_LIST_H
#define CRC_LIST_H

#include <iostream>
#include <vector>
#include <set>

#include "crc_t.h"

class AbstractProxy_CRC_t
{
    public:
        virtual ~AbstractProxy_CRC_t(){}

        //specification
        std::string  name;
        std::string  crc_name;
        std::string  impl_name;
        uint8_t      bits;
        uint64_t     poly;
        uint64_t     init;
        bool         ref_in;
        bool         ref_out;
        uint64_t     xor_out;
        uint64_t     check;

        // get param CRC
        virtual uint8_t  get_bits()     = 0;
        virtual uint64_t get_poly()     = 0;
        virtual uint64_t get_init()     = 0;
        virtual uint64_t get_xor_out()  = 0;
        virtual bool     get_ref_in()   = 0;
        virtual bool     get_ref_out()  = 0;

        virtual uint64_t get_crc_init() = 0;
        virtual uint64_t get_top_bit()  = 0;
        virtual uint64_t get_crc_mask() = 0;
        virtual uint64_t get_check()    = 0;

        // Calculate methods
        virtual uint64_t get_crc(const void* data, size_t len) = 0;
        virtual int      get_crc(uint64_t &crc, const char *file_name) = 0;
        virtual int      get_crc(uint64_t &crc, const char* file_name, void* buf, size_t size_buf) = 0;

        // Calculate for chunks of data
        virtual uint64_t get_raw_crc(const void* data, size_t len) = 0;
        virtual uint64_t get_raw_crc(const void* data, size_t len, uint64_t crc) = 0;
        virtual uint64_t get_end_crc(uint64_t raw_crc) = 0;
};

template <
  uint8_t Bits,
  CRC_TYPE Poly,
  CRC_TYPE Init,
  bool RefIn,
  bool RefOut,
  CRC_TYPE XorOut,
  uint64_t Check,
  template<uint8_t, CRC_TYPE, CRC_TYPE, bool, bool, CRC_TYPE> class Impl>
class Proxy_CRC_t : public AbstractProxy_CRC_t
{
    public:
        virtual ~Proxy_CRC_t(){}

        // get param CRC
        virtual uint8_t  get_bits()    { return _crc.get_bits();    }
        virtual uint64_t get_poly()    { return _crc.get_poly();    }
        virtual uint64_t get_init()    { return _crc.get_init();    }
        virtual uint64_t get_xor_out() { return _crc.get_xor_out(); }
        virtual bool     get_ref_in()  { return _crc.get_ref_in();  }
        virtual bool     get_ref_out() { return _crc.get_ref_out(); }

        virtual uint64_t get_crc_init() { return _crc.get_crc_init();}
        virtual uint64_t get_top_bit()  { return _crc.get_top_bit(); }
        virtual uint64_t get_crc_mask() { return _crc.get_crc_mask();}
        virtual uint64_t get_check()    { return _crc.get_check();   }

        // Calculate methods
        virtual uint64_t get_crc(const void* data, size_t len)
                                { return _crc.get_crc(data, len); }

        virtual int      get_crc(uint64_t &crc, const char *file_name)
                                { return _crc.get_crc((CRC_TYPE &)crc, file_name); }

        virtual int      get_crc(uint64_t &crc, const char* file_name, void* buf, size_t size_buf)
                                { return _crc.get_crc((CRC_TYPE &)crc, file_name, buf, size_buf); }

        //Calculate for chunks of data
        virtual uint64_t get_raw_crc(const void* data, size_t len)
                                    { return _crc.get_raw_crc(data, len); }

        virtual uint64_t get_raw_crc(const void* data, size_t len, uint64_t crc)
                                    { return _crc.get_raw_crc(data, len, (CRC_TYPE)crc); }

        virtual uint64_t get_end_crc(uint64_t raw_crc)
                                    { return _crc.get_end_crc((CRC_TYPE)raw_crc); }

    private:
        CRC_t<Bits, Poly, Init, RefIn, RefOut, XorOut, Impl>  _crc;
};

#define ADD_CRC_TO_LIST_IMPL(CRC_Name, IMPL_Name, Bits, Poly, Init, RefIn, RefOut, XorOut, Check, Impl)                  \
{                                                                                                                        \
    AbstractProxy_CRC_t *ptr = static_cast<AbstractProxy_CRC_t *>                                                        \
                               ( new Proxy_CRC_t<Bits, Poly, Init, RefIn, RefOut, XorOut, Check, Impl>() );              \
    if( !ptr )                                                                                                           \
    {                                                                                                                    \
        std::cerr << "Can't get MEM for Proxy_CRC_t<" << Bits << "," << Poly << "," << Init << "," << RefIn << ","       \
                                                      << RefOut << "," << XorOut << "," << Check << ">";                 \
        exit(-1);                                                                                                        \
    }                                                                                                                    \
    ptr->name      = CRC_Name IMPL_Name;                                                                               \
    ptr->crc_name  = CRC_Name;                                                                                           \
    ptr->impl_name = IMPL_Name;                                                                                          \
    ptr->bits      = Bits;                                                                                               \
    ptr->poly      = Poly;                                                                                               \
    ptr->init      = Init;                                                                                               \
    ptr->ref_in    = RefIn;                                                                                              \
    ptr->ref_out   = RefOut;                                                                                             \
    ptr->xor_out   = XorOut;                                                                                             \
    ptr->check     = Check;                                                                                              \
                                                                                                                         \
    CRC_List.push_back(ptr);                                                                                             \
}                                                                                                                        \

#define ADD_CRC_TO_LIST(CRC_Name, Bits, Poly, Init, RefIn, RefOut, XorOut, Check)                             \
    ADD_CRC_TO_LIST_IMPL(CRC_Name, "(Table8)", Bits, Poly, Init, RefIn, RefOut, XorOut, Check, CRCImplTable8) \
    ADD_CRC_TO_LIST_IMPL(CRC_Name, "(Table4)", Bits, Poly, Init, RefIn, RefOut, XorOut, Check, CRCImplTable4) \
    ADD_CRC_TO_LIST_IMPL(CRC_Name, "(Bits)"  , Bits, Poly, Init, RefIn, RefOut, XorOut, Check, CRCImplBits)   \

std::vector<AbstractProxy_CRC_t *> get_crc_list()
{
    std::vector<AbstractProxy_CRC_t *> CRC_List;

    // CRC list from: http://reveng.sourceforge.net/crc-catalogue/all.htm

    // // CRC-3
    // ADD_CRC_TO_LIST("CRC-3/GSM",  3, 0x3, 0,   false, false, 0x7, 0x4);
    // ADD_CRC_TO_LIST("CRC-3/ROHC", 3, 0x3, 0x7, true,  true,  0x0, 0x6);

    // // CRC-4
    // ADD_CRC_TO_LIST("CRC-4/G-704",      4, 0x3, 0x0, true,  true,  0x0, 0x7);
    // ADD_CRC_TO_LIST("CRC-4/INTERLAKEN", 4, 0x3, 0xF, false, false, 0xF, 0xB);

    // // CRC-5
    // ADD_CRC_TO_LIST("CRC-5/EPC-C1G2", 5, 0x09, 0x09, false, false, 0x00, 0x00);
    // ADD_CRC_TO_LIST("CRC-5/G-704",    5, 0x15, 0x00, true,  true,  0x00, 0x07);
    // ADD_CRC_TO_LIST("CRC-5/USB",      5, 0x05, 0x1F, true,  true,  0x1F, 0x19);

    // // CRC-6
    // ADD_CRC_TO_LIST("CRC-6/CDMA2000-A", 6, 0x27, 0x3F, false, false, 0x0,  0x0D);
    // ADD_CRC_TO_LIST("CRC-6/CDMA2000-B", 6, 0x07, 0x3F, false, false, 0x0,  0x3B);
    // ADD_CRC_TO_LIST("CRC-6/DARC",       6, 0x19, 0x00, true,  true,  0x0,  0x26);
    // ADD_CRC_TO_LIST("CRC-6/G-704",      6, 0x03, 0x00, true,  true,  0x0,  0x06);
    // ADD_CRC_TO_LIST("CRC-6/GSM",        6, 0x2F, 0x00, false, false, 0x3F, 0x13);

    // // CRC-7
    // ADD_CRC_TO_LIST("CRC-7/MMC",  7, 0x9,  0x0,  false, false, 0x0, 0x75);
    // ADD_CRC_TO_LIST("CRC-7/ROHC", 7, 0x4F, 0x7F, true,  true,  0x0, 0x53);
    // ADD_CRC_TO_LIST("CRC-7/UMTS", 7, 0x45, 0x0,  false, false, 0x0, 0x61);

    // // CRC-8
    // ADD_CRC_TO_LIST("CRC-8/AUTOSAR",    8, 0x2F, 0xFF, false, false, 0xFF, 0xDF);
    // ADD_CRC_TO_LIST("CRC-8/BLUETOOTH",  8, 0xA7, 0x0,  true,  true,  0x0,  0x26);
    // ADD_CRC_TO_LIST("CRC-8/CDMA2000",   8, 0x9B, 0xFF, false, false, 0x0,  0xDA);
    // ADD_CRC_TO_LIST("CRC-8/DARC",       8, 0x39, 0x0,  true,  true,  0x0,  0x15);
    // ADD_CRC_TO_LIST("CRC-8/DVB-S2",     8, 0xD5, 0x0,  false, false, 0x0,  0xBC);
    // ADD_CRC_TO_LIST("CRC-8/GSM-A",      8, 0x1D, 0x0,  false, false, 0x0,  0x37);
    // ADD_CRC_TO_LIST("CRC-8/GSM-B",      8, 0x49, 0x0,  false, false, 0xFF, 0x94);
    // ADD_CRC_TO_LIST("CRC-8/I-432-1",    8, 0x7,  0x0,  false, false, 0x55, 0xA1);
    // ADD_CRC_TO_LIST("CRC-8/I-CODE",     8, 0x1D, 0xFD, false, false, 0x0,  0x7E);
    // ADD_CRC_TO_LIST("CRC-8/LTE",        8, 0x9B, 0x0,  false, false, 0x0,  0xEA);
    // ADD_CRC_TO_LIST("CRC-8/MAXIM-DOW",  8, 0x31, 0x0,  true,  true,  0x0,  0xA1);
    // ADD_CRC_TO_LIST("CRC-8/MIFARE-MAD", 8, 0x1D, 0xC7, false, false, 0x0,  0x99);
    // ADD_CRC_TO_LIST("CRC-8/NRSC-5",     8, 0x31, 0xFF, false, false, 0x0,  0xF7);
    // ADD_CRC_TO_LIST("CRC-8/OPENSAFETY", 8, 0x2F, 0x0,  false, false, 0x0,  0x3E);
    // ADD_CRC_TO_LIST("CRC-8/ROHC",       8, 0x7,  0xFF, true,  true,  0x0,  0xD0);
    // ADD_CRC_TO_LIST("CRC-8/SAE-J1850",  8, 0x1D, 0xFF, false, false, 0xFF, 0x4B);
    // ADD_CRC_TO_LIST("CRC-8/SMBUS",      8, 0x7,  0x0,  false, false, 0x0,  0xF4);
    // ADD_CRC_TO_LIST("CRC-8/TECH-3250",  8, 0x1D, 0xFF, true,  true,  0x0,  0x97);
    // ADD_CRC_TO_LIST("CRC-8/WCDMA",      8, 0x9B, 0x0,  true,  true,  0x0,  0x25);

    // // CRC-10
    // ADD_CRC_TO_LIST("CRC-10/ATM",      10, 0x233, 0x0,   false, false, 0x0,   0x199);
    // ADD_CRC_TO_LIST("CRC-10/CDMA2000", 10, 0x3D9, 0x3FF, false, false, 0x0,   0x233);
    // ADD_CRC_TO_LIST("CRC-10/GSM",      10, 0x175, 0x0,   false, false, 0x3FF, 0x12A);

    // // CRC-11
    // ADD_CRC_TO_LIST("CRC-11/FLEXRAY",  11, 0x385, 0x1A, false, false, 0x0, 0x5A3);
    // ADD_CRC_TO_LIST("CRC-11/UMTS",     11, 0x307, 0x0,  false, false, 0x0, 0x061);

    // // CRC-12
    // ADD_CRC_TO_LIST("CRC-12/CDMA2000", 12, 0xF13, 0xFFF, false, false, 0x0,   0xD4D);
    // ADD_CRC_TO_LIST("CRC-12/DECT",     12, 0x80F, 0x0,   false, false, 0x0,   0xF5B);
    // ADD_CRC_TO_LIST("CRC-12/GSM",      12, 0xD31, 0x0,   false, false, 0xFFF, 0xB34);
    // ADD_CRC_TO_LIST("CRC-12/UMTS",     12, 0x80F, 0x0,   false, true,  0x0,   0xDAF);
       
    // // CRC-13
    // ADD_CRC_TO_LIST("CRC-13/BBC", 13, 0x1CF5, 0x0, false, false, 0x0, 0x4FA);

    // // CRC-14
    // ADD_CRC_TO_LIST("CRC-14/DARC", 14, 0x805,  0x0, true,  true,  0x0,    0x82D);
    // ADD_CRC_TO_LIST("CRC-14/GSM",  14, 0x202D, 0x0, false, false, 0x3FFF, 0x30AE);

    // // CRC-15
    // ADD_CRC_TO_LIST("CRC-15/CAN",     15, 0x4599, 0x0, false, false, 0x0, 0x59E);
    // ADD_CRC_TO_LIST("CRC-15/MPT1327", 15, 0x6815, 0x0, false, false, 0x1, 0x2566);

    // // CRC-16
    // ADD_CRC_TO_LIST("CRC-16/ARC",              16, 0x8005, 0x0,    true,  true,  0x0,    0xBB3D);
    // ADD_CRC_TO_LIST("CRC-16/CDMA2000",         16, 0xC867, 0xFFFF, false, false, 0x0,    0x4C06);
    // ADD_CRC_TO_LIST("CRC-16/CMS",              16, 0x8005, 0xFFFF, false, false, 0x0,    0xAEE7);
    // ADD_CRC_TO_LIST("CRC-16/DDS-110",          16, 0x8005, 0x800D, false, false, 0x0,    0x9ECF);
    // ADD_CRC_TO_LIST("CRC-16/DECT-R",           16, 0x589,  0x0,    false, false, 0x1,    0x7E);
    // ADD_CRC_TO_LIST("CRC-16/DECT-X",           16, 0x589,  0x0,    false, false, 0x0,    0x7F);
    // ADD_CRC_TO_LIST("CRC-16/DNP",              16, 0x3D65, 0x0,    true,  true,  0xFFFF, 0xEA82);
    // ADD_CRC_TO_LIST("CRC-16/EN-13757",         16, 0x3D65, 0x0,    false, false, 0xFFFF, 0xC2B7);
    // ADD_CRC_TO_LIST("CRC-16/GENIBUS",          16, 0x1021, 0xFFFF, false, false, 0xFFFF, 0xD64E);
    // ADD_CRC_TO_LIST("CRC-16/GSM",              16, 0x1021, 0x0,    false, false, 0xFFFF, 0xCE3C);
    // ADD_CRC_TO_LIST("CRC-16/IBM-3740",         16, 0x1021, 0xFFFF, false, false, 0x0,    0x29B1);
    // ADD_CRC_TO_LIST("CRC-16/IBM-SDLC",         16, 0x1021, 0xFFFF, true,  true,  0xFFFF, 0x906E);
    // ADD_CRC_TO_LIST("CRC-16/ISO-IEC-14443-3-A",16, 0x1021, 0xC6C6, true,  true,  0x0,    0xBF05);
    // ADD_CRC_TO_LIST("CRC-16/KERMIT",           16, 0x1021, 0x0,    true,  true,  0x0,    0x2189);
    // ADD_CRC_TO_LIST("CRC-16/LJ1200",           16, 0x6F63, 0x0,    false, false, 0x0,    0xBDF4);
    // ADD_CRC_TO_LIST("CRC-16/MAXIM-DOW",        16, 0x8005, 0x0,    true,  true,  0xFFFF, 0x44C2);
    // ADD_CRC_TO_LIST("CRC-16/MCRF4XX",          16, 0x1021, 0xFFFF, true,  true,  0x0,    0x6F91);
    // ADD_CRC_TO_LIST("CRC-16/MODBUS",           16, 0x8005, 0xFFFF, true,  true,  0x0,    0x4B37);
    // ADD_CRC_TO_LIST("CRC-16/NRSC-5",           16, 0x080B, 0xFFFF, true,  true,  0x0,    0xA066);
    // ADD_CRC_TO_LIST("CRC-16/OPENSAFETY-A",     16, 0x5935, 0x0,    false, false, 0x0,    0x5D38);
    // ADD_CRC_TO_LIST("CRC-16/OPENSAFETY-B",     16, 0x755B, 0x0,    false, false, 0x0,    0x20FE);
    // ADD_CRC_TO_LIST("CRC-16/PROFIBUS",         16, 0x1DCF, 0xFFFF, false, false, 0xFFFF, 0xA819);
    // ADD_CRC_TO_LIST("CRC-16/RIELLO",           16, 0x1021, 0xB2AA, true,  true,  0x0,    0x63D0);
    // ADD_CRC_TO_LIST("CRC-16/SPI-FUJITSU",      16, 0x1021, 0x1D0F, false, false, 0x0,    0xE5CC);
    // ADD_CRC_TO_LIST("CRC-16/T10-DIF",          16, 0x8BB7, 0x0,    false, false, 0x0,    0xD0DB);
    // ADD_CRC_TO_LIST("CRC-16/TELEDISK",         16, 0xA097, 0x0,    false, false, 0x0,    0xFB3);
    // ADD_CRC_TO_LIST("CRC-16/TMS37157",         16, 0x1021, 0x89EC, true,  true,  0x0,    0x26B1);
    // ADD_CRC_TO_LIST("CRC-16/UMTS",             16, 0x8005, 0x0,    false, false, 0x0,    0xFEE8);
    // ADD_CRC_TO_LIST("CRC-16/USB",              16, 0x8005, 0xFFFF, true,  true,  0xFFFF, 0xB4C8);
    // ADD_CRC_TO_LIST("CRC-16/XMODEM",           16, 0x1021, 0x0,    false, false, 0x0,    0x31C3);

    // // CRC-17
    // ADD_CRC_TO_LIST("CRC-17/CAN-FD",     17, 0x1685B, 0x0, false, false, 0x0,  0x4F03);

    // // CRC-21
    // ADD_CRC_TO_LIST("CRC-21/CAN-FD",     21, 0x102899, 0x0,false, false, 0x0,  0xED841);

    // // CRC-24
    // ADD_CRC_TO_LIST("CRC-24/BLE",        24, 0x65B,    0x555555, true,  true,  0x0,      0xC25A56);
    // ADD_CRC_TO_LIST("CRC-24/FLEXRAY-A",  24, 0x5D6DCB, 0xFEDCBA, false, false, 0x0,      0x7979BD);
    // ADD_CRC_TO_LIST("CRC-24/FLEXRAY-B",  24, 0x5D6DCB, 0xABCDEF, false, false, 0x0,      0x1F23B8);
    // ADD_CRC_TO_LIST("CRC-24/INTERLAKEN", 24, 0x328B63, 0xFFFFFF, false, false, 0xFFFFFF, 0xB4F3E6);
    // ADD_CRC_TO_LIST("CRC-24/LTE-A",      24, 0x864CFB, 0x0,      false, false, 0x0,      0xCDE703);
    // ADD_CRC_TO_LIST("CRC-24/LTE-B",      24, 0x800063, 0x0,      false, false, 0x0,      0x23EF52);
    // ADD_CRC_TO_LIST("CRC-24/OPENPGP",    24, 0x864CFB, 0xB704CE, false, false, 0x0,      0x21CF02);
    // ADD_CRC_TO_LIST("CRC-24/OS-9",       24, 0x800063, 0xFFFFFF, false, false, 0xFFFFFF, 0x200FA5);

    // // CRC-30
    // ADD_CRC_TO_LIST("CRC-30/CDMA", 30, 0x2030B9C7, 0x3FFFFFFF, false, false, 0x3FFFFFFF, 0x4C34ABF);

    // // CRC-31
    // ADD_CRC_TO_LIST("CRC-31/PHILIPS", 31, 0x4C11DB7, 0x7FFFFFFF, false, false, 0x7FFFFFFF, 0xCE9E46C);

    // CRC-32
    ADD_CRC_TO_LIST("CRC-32/AIXM",       32, 0x814141AB, 0x0,        false, false, 0x0,        0x3010BF7F);
    ADD_CRC_TO_LIST("CRC-32/AUTOSAR",    32, 0xF4ACFB13, 0xFFFFFFFF, true,  true,  0xFFFFFFFF, 0x1697D06A);
    ADD_CRC_TO_LIST("CRC-32/BASE91-D",   32, 0xA833982B, 0xFFFFFFFF, true,  true,  0xFFFFFFFF, 0x87315576);
    ADD_CRC_TO_LIST("CRC-32/BZIP2",      32, 0x4C11DB7,  0xFFFFFFFF, false, false, 0xFFFFFFFF, 0xFC891918);
    ADD_CRC_TO_LIST("CRC-32/CD-ROM-EDC", 32, 0x8001801B, 0x0,        true,  true,  0x0,        0x6EC2EDC4);
    ADD_CRC_TO_LIST("CRC-32/CKSUM",      32, 0x4C11DB7,  0x0,        false, false, 0xFFFFFFFF, 0x765E7680);
    ADD_CRC_TO_LIST("CRC-32/ISCSI",      32, 0x1EDC6F41, 0xFFFFFFFF, true,  true,  0xFFFFFFFF, 0xE3069283);
    ADD_CRC_TO_LIST("CRC-32/ISO-HDLC",   32, 0x4C11DB7,  0xFFFFFFFF, true,  true,  0xFFFFFFFF, 0xCBF43926);
    ADD_CRC_TO_LIST("CRC-32/JAMCRC",     32, 0x4C11DB7,  0xFFFFFFFF, true,  true,  0x0,        0x340BC6D9);
    ADD_CRC_TO_LIST("CRC-32/MPEG-2",     32, 0x4C11DB7,  0xFFFFFFFF, false, false, 0x0,        0x376E6E7 );
    ADD_CRC_TO_LIST("CRC-32/XFER",       32, 0xAF,       0x0,        false, false, 0x0,        0xBD0BE338);

    // // CRC-40
    // ADD_CRC_TO_LIST("CRC-40/GSM", 40, 0x4820009, 0x0, false, false, 0xFFFFFFFFFF, 0xD4164FC646);

    // CRC-64
    ADD_CRC_TO_LIST("CRC-64/ECMA-182", 64, 0x42F0E1EBA9EA3693, 0x0,                false, false, 0x0,                0x6C40DF5F0B497347);
    ADD_CRC_TO_LIST("CRC-64/GO-ISO",   64, 0x1B,               0xFFFFFFFFFFFFFFFF, true,  true,  0xFFFFFFFFFFFFFFFF, 0xB90956C775A41001);
    ADD_CRC_TO_LIST("CRC-64/WE",       64, 0x42F0E1EBA9EA3693, 0xFFFFFFFFFFFFFFFF, false, false, 0xFFFFFFFFFFFFFFFF, 0x62EC59E3F1A4F00A);
    ADD_CRC_TO_LIST("CRC-64/XZ",       64, 0x42F0E1EBA9EA3693, 0xFFFFFFFFFFFFFFFF, true,  true,  0xFFFFFFFFFFFFFFFF, 0x995DC9BBDF1939FA);

    return CRC_List;
}

const std::vector<AbstractProxy_CRC_t *> CRC_List/*= get_crc_list()*/;

size_t get_cnt_impl()
{
     std::set<std::string> impl_names;

     for(size_t i = 0; i < CRC_List.size(); i++)
        impl_names.insert(CRC_List[i]->impl_name);

     return impl_names.size();
}

#endif // CRC_LIST_H
