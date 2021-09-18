/******************************************************************************
 *  Copyright (c) 2015 Jamis Hoo
 *  Distributed under the MIT license 
 *  (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)
 *  
 *  Project: 
 *  Filename: sha2_256.cc 
 *  Version: 1.0
 *  Author: Jamis Hoo
 *  E-mail: hoojamis@gmail.com
 *  Date: May 11, 2015
 *  Time: 22:13:27
 *  Description: SHA-2 (256 bit)
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

namespace sha_2_256
{

void sha2_iteration(const uint8_t* data, uint32_t hi[]) {
    // rotate function
    auto right_rotate = [](uint32_t x, const size_t i)->uint32_t {
        return x >> i | x << (sizeof(uint32_t) * 8 - i);
    };

    constexpr uint32_t k[64] = { 
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    // extend 16 32-bit words to 80 32-bit words
    uint32_t word[80];
    for (size_t j = 0; j < 16; ++j)
        word[j] = data[4 * j + 0] << 24 | data[4 * j + 1] << 16 | 
                  data[4 * j + 2] <<  8 | data[4 * j + 3];

    for (size_t j = 16; j < 80; ++j) {
        uint32_t s0 = right_rotate(word[j - 15],  7) ^
                      right_rotate(word[j - 15], 18) ^
                      word[j - 15] >> 3;
        uint32_t s1 = right_rotate(word[j - 2], 17) ^
                      right_rotate(word[j - 2], 19) ^
                      word[j - 2] >> 10;
        word[j] = word[j - 16] + s0 + word[j - 7] + s1;
    }

    uint32_t a = hi[0], b = hi[1], c = hi[2], d = hi[3], 
             e = hi[4], f = hi[5], g = hi[6], h = hi[7];
            
    
    // main loop
    for (size_t i = 0; i < 64; ++i) {
        uint32_t S1 = right_rotate(e, 6) ^ 
                      right_rotate(e, 11) ^
                      right_rotate(e, 25);
        uint32_t ch = (e & f) ^ (~e & g);
        uint32_t tmp1 = h + S1 + ch + k[i] + word[i];
        uint32_t S0 = right_rotate(a, 2) ^
                      right_rotate(a, 13) ^ 
                      right_rotate(a, 22);
        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        uint32_t tmp2 = S0 + maj;

        h = g, g = f, f = e, e = d + tmp1, d = c, c = b, b = a, a = tmp1 + tmp2;
    }

    hi[0] += a, hi[1] += b, hi[2] += c, hi[3] += d, 
    hi[4] += e, hi[5] += f, hi[6] += g, hi[7] += h;
}

// len: in bytes
// hash: at least 32 bytes available
void sha2(const void* data, size_t len, char* hash) {
    uint8_t* data_ = (uint8_t*)data;
    constexpr size_t block_size = 64;

    uint32_t h[8] = { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };

    size_t ml = len * 8;

    for (size_t i = 0; i < len / block_size; ++i)
        sha2_iteration(data_ + i * block_size, h);

    uint8_t buffer[block_size];
    
    memcpy(buffer, data_ + len / block_size * block_size, len % block_size);
    len %= block_size;

    // append bit '1'
    buffer[len++] = 0x80;

    if (len % block_size == 0) {
        sha2_iteration(buffer, h);
        len = 0;
    }
    
    // append until the resulting message length (in bits) is congruent to 448 (mod 512)
    while (len % block_size != 56) {
        buffer[len++] = 0x00;
        if (len % block_size == 0) {
            sha2_iteration(buffer, h);
            len = 0;
        }
    }

    // append length
    buffer[len++] = ml >> 56, buffer[len++] = ml >> 48, 
    buffer[len++] = ml >> 40, buffer[len++] = ml >> 32,
    buffer[len++] = ml >> 24, buffer[len++] = ml >> 16,
    buffer[len++] = ml >>  8, buffer[len++] = ml;

    sha2_iteration(buffer, h);
    
    for (size_t i = 0; i < 32; i += 4)
        hash[i] = h[i / 4] >> 24, hash[i + 1] = h[i / 4] >> 16,
        hash[i + 2] = h[i / 4] >> 8, hash[i + 3] = h[i / 4];
}

} // sha_2_256

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

//int main(int argc, char** argv) {
//    uint8_t hash[32];
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
//    sha2(buffer.data(), buffer.length(), (char*)hash);
//
//    for (int i = 0; i < 32; ++i)
//        printf("%02x", int(hash[i]) & 0xff);
//    printf("\n");
//    
//}
