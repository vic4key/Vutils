/**
 * @file   crypt.cpp
 * @author Vic P.
 * @brief  Implementation for Cryptography
 */

#include "Vutils.h"
#include "defs.h"

#include VU_3RD_INCL(Others/base64.h)

namespace vu
{

uint b64_calc_encode_size(const std::vector<vu::byte>& data)
{
  return ((4 * uint(data.size()) / 3) + 3) & ~3;
}

uint b64_calc_decode_size(const std::string& text)
{
  // https://en.wikipedia.org/wiki/Base64#Decoding_Base64_with_padding

  const auto size = strlen(text.c_str());
  if (size < 3)
  {
    return 0;
  }

  int n_padding = 0;
  if (text[size - 1] == '=') n_padding++;
  if (text[size - 2] == '=') n_padding++;

  return uint(size * (3.F / 4.F)) - n_padding;
}

bool crypt_b64encode_A(const std::vector<vu::byte>& data, std::string& text)
{
  text.clear();

  if (data.empty())
  {
    return true;
  }

  const auto decoded_size = uint(data.size());
  const auto encoded_size = b64_calc_encode_size(data);
  text.resize(encoded_size); text.reserve(decoded_size + 1); // padding 1 null byte

  return base64_encode(data.data(), decoded_size, &text[0]) == encoded_size;
}

bool crypt_b64decode_A(const std::string& text, std::vector<vu::byte>& data)
{
  data.clear();

  if (text.empty())
  {
    return true;
  }

  const auto encoded_size = uint(strlen(text.c_str()));
  const auto decoded_size = b64_calc_decode_size(text);
  data.resize(decoded_size); data.reserve(decoded_size + 1); // padding 1 null byte

  return base64_decode(text.data(), encoded_size, &data[0]) == decoded_size;
}

bool crypt_b64encode_W(const std::vector<vu::byte>& data, std::wstring& text)
{
  std::string s;
  
  bool result = crypt_b64encode_A(data, s);
  if (result)
  {
    text.assign(s.cbegin(), s.cend());
  }
  
  return result;
}

bool crypt_b64decode_W(const std::wstring& text, std::vector<vu::byte>& data)
{
  const auto s = to_string_A(text);
  return crypt_b64decode_A(s, data);
}

} // vu