/**
 * @file   crypt.cpp
 * @author Vic P.
 * @brief  Implementation for Cryptography
 */

#include "Vutils.h"
#include "defs.h"

#include VU_3RD_INCL(Others/base64.h)
#include VU_3RD_INCL(Others/md5.h)
#include VU_3RD_INCL(Others/md5.h)

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4333)
#endif // _MSC_VER

#include VU_3RD_INCL(Others/crc_t.h)

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

namespace vu
{

/**
 * Base 64 Encode/Decode
 */

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

/**
 * MD5
 */

std::string crypt_md5_string_A(const std::string& text)
{
  return md5(text);
}

std::wstring crypt_md5_string_W(const std::wstring& text)
{
  // const auto result = md5(text.data(), 2*text.length());
  auto result = to_string_A(text);
  result = crypt_md5_string_A(result);
  return to_string_W(result);
}

std::string crypt_md5_buffer_A(const std::vector<byte>& data)
{
  return md5(data.data(), data.size());
}

std::wstring crypt_md5_buffer_W(const std::vector<byte>& data)
{
  const auto result = crypt_md5_buffer_A(data);
  return to_string_W(result);
}

std::string crypt_md5_file_A(const std::string& file_path)
{
  if (!is_file_exists_A(file_path))
  {
    return "";
  }

  return md5file(file_path.c_str());
}

std::wstring crypt_md5_file_W(const std::wstring& file_path)
{
  if (!is_file_exists_W(file_path))
  {
    return L"";
  }

  FILE* file = nullptr;
  _wfopen_s(&file, file_path.c_str(), L"rb");
  if (file == nullptr)
  {
    return L"";
  }

  const auto result = md5file(file);

  fclose(file);

  return to_string_W(result);
}

/**
 * CRC
 */

uint8 crypt_crc8(const std::vector<byte>& data)
{
  CRC_t<8, 0x07, 0x00, false, false, 0x00> crc;
  return crc.get_crc(data.data(), data.size());
}

uint16 crypt_crc16(const std::vector<byte>& data)
{
  CRC_t<16, 0x8005, 0x0000, true, true, 0x0000> crc;
  return crc.get_crc(data.data(), data.size());
}

uint32 crypt_crc32(const std::vector<byte>& data)
{
  CRC_t<32, 0x04C11DB7, 0xFFFFFFFF, true, true, 0xFFFFFFFF> crc;
  return crc.get_crc(data.data(), data.size());
}

uint64 crypt_crc64(const std::vector<byte>& data)
{
  CRC_t<64, 0x42F0E1EBA9EA3693, 0x0000000000000000, false, false, 0x0000000000000000> crc;
  return crc.get_crc(data.data(), data.size());
}

} // vu