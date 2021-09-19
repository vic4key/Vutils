#include "sha.h"
#include "sha3_impl.h"

#include <cstring>

namespace sha_3_224
{

void sha3(const void* data, size_t len, char* hash)
{
  sha3_state ctx = { 0 };
  sha3_init(&ctx, SHA3_224_DIGEST_SIZE);
  sha3_update(&ctx, (uint8_t*)data, uint32_t(len));
  sha3_final(&ctx, (uint8_t*)hash);
}

} // sha_3_224

namespace sha_3_256
{

void sha3(const void* data, size_t len, char* hash)
{
  sha3_state ctx = { 0 };
  sha3_init(&ctx, SHA3_256_DIGEST_SIZE);
  sha3_update(&ctx, (uint8_t*)data, uint32_t(len));
  sha3_final(&ctx, (uint8_t*)hash);
}

} // sha_3_256

namespace sha_3_384
{

void sha3(const void* data, size_t len, char* hash)
{
  sha3_state ctx = { 0 };
  sha3_init(&ctx, SHA3_384_DIGEST_SIZE);
  sha3_update(&ctx, (uint8_t*)data, uint32_t(len));
  sha3_final(&ctx, (uint8_t*)hash);
}

} // sha_3_384

namespace sha_3_512
{

void sha3(const void* data, size_t len, char* hash)
{
  sha3_state ctx = { 0 };
  sha3_init(&ctx, SHA3_512_DIGEST_SIZE);
  sha3_update(&ctx, (uint8_t*)data, uint32_t(len));
  sha3_final(&ctx, (uint8_t*)hash);
}

} // sha_3_512