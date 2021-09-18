/******************************************************************************
 *  Copyright (c) 2015 Jamis Hoo
 *  Distributed under the MIT license 
 *  (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)
 *  
 *  Project: 
 *  Filename: sha1.cc 
 *  Version: 1.0
 *  Author: Jamis Hoo
 *  E-mail: hoojamis@gmail.com
 *  Date: May 11, 2015
 *  Time: 15:47:06
 *  Description: SHA-1
 *****************************************************************************/

#include "sha.h"

#include <cstring>
#include <cinttypes>
#include <iostream>
#include <fstream>
#include <streambuf>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4267)
#endif // _MSC_VER

namespace sha_1
{

void sha1_iteration(const uint8_t* data, uint32_t h[]) {
    // rotate functions
    auto left_rotate = [](uint32_t x, const size_t i)->uint32_t { 
        return x << i | x >> (sizeof(uint32_t) * 8 - i); 
    };

    // extend 16 32-bit words to 80 32-bit words
    uint32_t word[80];
    for (size_t j = 0; j < 16; ++j)
        word[j] = data[4 * j + 0] << 24 | data[4 * j + 1] << 16 | 
                  data[4 * j + 2] <<  8 | data[4 * j + 3];

    for (size_t j = 16; j < 80; ++j) 
        word[j] = left_rotate(word[j - 3] ^ word[j - 8] ^ word[j - 14] ^ word[j - 16], 1);


    uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4];
    
    // main loop
    for (size_t j = 0; j < 80; ++j) {
        int f, k;
        if (j < 20) 
            f = (b & c) | (~b & d), k = 0x5A827999;
        else if (j < 40)
            f = b ^ c ^ d, k = 0x6ED9EBA1;
        else if (j < 60)
            f = (b & c) | (b & d) | (c & d), k = 0x8F1BBCDC;
        else 
            f = b ^ c ^ d, k = 0xCA62C1D6;
        
        uint32_t tmp = left_rotate(a, 5) + f + e + k + word[j];
        e = d, d = c, c = left_rotate(b, 30), b = a, a = tmp;
    }

    h[0] += a, h[1] += b, h[2] += c, h[3] += d, h[4] += e;
}

// len: in bytes
// hash: at least 20 bytes available
void sha1(const void* data, size_t len, char* hash) {
    uint8_t* data_ = (uint8_t*)data;
    constexpr size_t block_size = 64;

    uint32_t h[5] = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 };

    size_t ml = len * 8;

    for (size_t i = 0; i < len / block_size; ++i)
        sha1_iteration(data_ + i * block_size, h);

    uint8_t buffer[block_size];
    
    memcpy(buffer, data_ + len / block_size * block_size, len % block_size);
    len %= block_size;

    // append bit '1'
    buffer[len++] = 0x80;

    if (len % block_size == 0) {
        sha1_iteration(buffer, h);
        len = 0;
    }
    
    // append until the resulting message length (in bits) is congruent to 448 (mod 512)
    while (len % block_size != 56) {
        buffer[len++] = 0x00;
        if (len % block_size == 0) {
            sha1_iteration(buffer, h);
            len = 0;
        }
    }

    // append length
    buffer[len++] = ml >> 56, buffer[len++] = ml >> 48, 
    buffer[len++] = ml >> 40, buffer[len++] = ml >> 32,
    buffer[len++] = ml >> 24, buffer[len++] = ml >> 16,
    buffer[len++] = ml >>  8, buffer[len++] = ml;

    sha1_iteration(buffer, h);
    
    for (size_t i = 0; i < 20; i += 4)
        hash[i] = h[i / 4] >> 24, hash[i + 1] = h[i / 4] >> 16,
        hash[i + 2] = h[i / 4] >> 8, hash[i + 3] = h[i / 4];
}

} // sha_1

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

//int main(int argc, char** argv) {
//    uint8_t hash[20];
//
//    if (argc == 1) return 0;
//
//    std::ifstream fin(argv[1]);
//
//    fin.seekg(0, std::ios::end);
//    std::string buffer;
//    buffer.reserve(fin.tellg());
//    fin.seekg(0, std::ios::beg);
//
//    buffer.assign((std::istreambuf_iterator<char>(fin)),
//                   std::istreambuf_iterator<char>());
//
//    
//    sha1(buffer.data(), buffer.length(), (char*)hash);
//
//    for (int i = 0; i < 20; ++i)
//        printf("%02x", int(hash[i]) & 0xff);
//    printf("\n");
//    
//}
