#ifndef SHA_H
#define SHA_H

namespace sha_1
{
  void sha1(const void* data, size_t len, char* hash);
} // sha1

namespace sha_2_256
{
  void sha2(const void* data, size_t len, char* hash);
} // sha_2_256

namespace sha_2_512
{
  void sha2(const void* data, size_t len, char* hash);
} // sha_2_512

namespace sha_3_256
{
  void sha3(const void* data, size_t len, char* hash);
} // sha_3_256

namespace sha_3_512
{
  void sha3(const void* data, size_t len, char* hash);
} // sha_3_512

#endif // SHA_H