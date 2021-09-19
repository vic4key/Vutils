#ifndef SHA_H
#define SHA_H

#include <cstdlib>

// SHA-1

namespace sha_1
{
  void sha1(const void* data, size_t len, char* hash);
} // sha1

// SHA-2

namespace sha_2_224
{
  void sha2(const void* data, size_t len, char* hash);
} // sha_2_224

namespace sha_2_256
{
  void sha2(const void* data, size_t len, char* hash);
} // sha_2_256

namespace sha_2_384
{
  void sha2(const void* data, size_t len, char* hash);
} // sha_2_384

namespace sha_2_512
{
  void sha2(const void* data, size_t len, char* hash);
} // sha_2_512

// SHA-3

namespace sha_3_224
{
  void sha3(const void* data, size_t len, char* hash);
} // sha_3_224

namespace sha_3_256
{
  void sha3(const void* data, size_t len, char* hash);
} // sha_3_256

namespace sha_3_384
{
  void sha3(const void* data, size_t len, char* hash);
} // sha_3_384

namespace sha_3_512
{
  void sha3(const void* data, size_t len, char* hash);
} // sha_3_512

#endif // SHA_H