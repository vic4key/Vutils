/******************************************************************************
 *  Copyright (c) 2015 Jamis Hoo
 *  Distributed under the MIT license 
 *  (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)
 *  
 *  Project: 
 *  Filename: sha3_512.cc 
 *  Version: 1.0
 *  Author: Jamis Hoo
 *  E-mail: hoojamis@gmail.com
 *  Date: May 20, 2015
 *  Time: 10:01:31
 *  Description: SHA-3 (512 bit)
 *****************************************************************************/

#include "sha.h"

#include <cstring>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <streambuf>

namespace sha_3_512
{

// rotate functions
inline uint64_t right_rotate(uint64_t x, const size_t i) {
    return x >>i | x << (sizeof(uint64_t) * 8 - i);
}

inline uint64_t left_rotate(uint64_t x, const size_t i) {
    return x << i | x >> (sizeof(uint64_t) * 8 - i);
}

inline void keccak_theta(uint64_t A[]) {
    uint64_t C[5], D[5];
    for (size_t i = 0; i < 5; ++i) 
        C[i] = A[i] ^ A[i + 5] ^ A[i + 10] ^ A[i + 15] ^ A[i + 20];

    D[0] = left_rotate(C[1], 1) ^ C[4];
    D[1] = left_rotate(C[2], 1) ^ C[0];
    D[2] = left_rotate(C[3], 1) ^ C[1];
    D[3] = left_rotate(C[4], 1) ^ C[2];
    D[4] = left_rotate(C[0], 1) ^ C[3];

    for (size_t i = 0; i < 5; ++i) {
        A[i]      ^= D[i];
        A[i + 5]  ^= D[i];
        A[i + 10] ^= D[i];
        A[i + 15] ^= D[i];
        A[i + 20] ^= D[i];
    }
}

inline void keccak_pi(uint64_t A[]) {
    uint64_t A1 = A[1];
    A[ 1] = A[ 6], A[ 6] = A[ 9], A[ 9] = A[22], A[22] = A[14],
    A[14] = A[20], A[20] = A[ 2], A[ 2] = A[12], A[12] = A[13],
    A[13] = A[19], A[19] = A[23], A[23] = A[15], A[15] = A[ 4],
    A[ 4] = A[24], A[24] = A[21], A[21] = A[ 8], A[ 8] = A[16],
    A[16] = A[ 5], A[ 5] = A[ 3], A[ 3] = A[18], A[18] = A[17],
    A[17] = A[11], A[11] = A[ 7], A[ 7] = A[10], A[10] = A1;
}

inline void keccak_chi(uint64_t A[]) {
    for (size_t i = 0; i < 25; i += 5) {
        uint64_t A0 = A[0 + i], A1 = A[1 + i];
        A[0 + i] ^= ~A1 & A[2 + i];
        A[1 + i] ^= ~A[2 + i] & A[3 + i];
        A[2 + i] ^= ~A[3 + i] & A[4 + i];
        A[3 + i] ^= ~A[4 + i] & A0;
        A[4 + i] ^= ~A0 & A1;
    }
}

inline void sha3_permutation(uint64_t state[]) {
    const uint64_t keccak_round_constants[24] = {
        0x0000000000000001, 0x0000000000008082, 0x800000000000808A,
        0x8000000080008000, 0x000000000000808B, 0x0000000080000001,
        0x8000000080008081, 0x8000000000008009, 0x000000000000008A, 
        0x0000000000000088, 0x0000000080008009, 0x000000008000000A,
        0x000000008000808B, 0x800000000000008B, 0x8000000000008089,
        0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
        0x000000000000800A, 0x800000008000000A, 0x8000000080008081,
        0x8000000000008080, 0x0000000080000001, 0x8000000080008008,
    };
    for (size_t round = 0; round < 24; round++) {
        keccak_theta(state);

        state[ 1] = left_rotate(state[ 1],  1);
        state[ 2] = left_rotate(state[ 2], 62);
        state[ 3] = left_rotate(state[ 3], 28);
        state[ 4] = left_rotate(state[ 4], 27);
        state[ 5] = left_rotate(state[ 5], 36);
        state[ 6] = left_rotate(state[ 6], 44);
        state[ 7] = left_rotate(state[ 7],  6);
        state[ 8] = left_rotate(state[ 8], 55);
        state[ 9] = left_rotate(state[ 9], 20);
        state[10] = left_rotate(state[10],  3);
        state[11] = left_rotate(state[11], 10);
        state[12] = left_rotate(state[12], 43);
        state[13] = left_rotate(state[13], 25);
        state[14] = left_rotate(state[14], 39);
        state[15] = left_rotate(state[15], 41);
        state[16] = left_rotate(state[16], 45);
        state[17] = left_rotate(state[17], 15);
        state[18] = left_rotate(state[18], 21);
        state[19] = left_rotate(state[19],  8);
        state[20] = left_rotate(state[20], 18);
        state[21] = left_rotate(state[21],  2);
        state[22] = left_rotate(state[22], 61);
        state[23] = left_rotate(state[23], 56);
        state[24] = left_rotate(state[24], 14);
        
        keccak_pi(state);
        keccak_chi(state);

        *state ^= keccak_round_constants[round];
    }
}

void sha3_iteration(const uint8_t* data, uint64_t h[]) {
    for (size_t i = 0; i <= 8; ++i)
        h[i] ^= uint64_t(data[8 * i + 0]) <<  0 | 
                uint64_t(data[8 * i + 1]) <<  8 |
                uint64_t(data[8 * i + 2]) << 16 | 
                uint64_t(data[8 * i + 3]) << 24 |
                uint64_t(data[8 * i + 4]) << 32 | 
                uint64_t(data[8 * i + 5]) << 40 |
                uint64_t(data[8 * i + 6]) << 48 | 
                uint64_t(data[8 * i + 7]) << 56;
    sha3_permutation(h);
}

// len: in bytes
// hash: at least 32 bytes available
void sha3(const void* data, size_t len, char* hash) {
    uint8_t* data_ = (uint8_t*)data;
    const size_t block_size = 72;

    uint64_t h[25] = { 0 };


    for (size_t i = 0; i < len / block_size; ++i) 
        sha3_iteration(data_ + i * block_size, h);

    uint8_t buffer[block_size] = { 0 };
    
    memcpy(buffer, data_ + len / block_size * block_size, len % block_size);
    len %= block_size;

    buffer[len] |= 0x06;
    buffer[block_size - 1] |= 0x80;

    sha3_iteration(buffer, h);

    
    memcpy(hash, h, 64);
}

} // sha_3_512

//int main(int argc, char** argv) {
//    uint8_t hash[64];
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
//    sha3(buffer.data(), buffer.length(), (char*)hash);
//
//    for (int i = 0; i < 64; ++i)
//        printf("%02x", int(hash[i]) & 0xff);
//    printf("\n");
//    
//}

