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
#include VU_3RD_INCL(Others/sha.h)

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4333 4244)
#endif // _MSC_VER

#include VU_3RD_INCL(Others/crc_t.h)
#include VU_3RD_INCL(Others/crc_list.h)

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
  if (size < 2 || size % 4 != 0)
  {
    return 0;
  }

  uint result = BASE64_DECODE_OUT_SIZE(size);

  if (size >= 2)
  {
    if (text[size - 1] == '=') result -= 1;
    if (text[size - 2] == '=') result -= 1;
  }

  return result;
}

bool crypt_b64encode_A(const std::vector<vu::byte>& data, std::string& text)
{
  text.clear();

  if (data.empty())
  {
    return true;
  }

  const auto encoded_size = b64_calc_encode_size(data);
  if (encoded_size == 0)
  {
    return false;
  }

  const auto decoded_size = uint(data.size());
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

  const auto decoded_size = b64_calc_decode_size(text);
  if (decoded_size == 0)
  {
    return false;
  }

  const auto encoded_size = uint(strlen(text.c_str()));
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

std::string crypt_md5_text_A(const std::string& text)
{
  return md5(text);
}

std::wstring crypt_md5_text_W(const std::wstring& text)
{
  // const auto result = md5(text.data(), 2*text.length());
  auto result = to_string_A(text);
  result = crypt_md5_text_A(result);
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

uint64 crypt_crc_buffer(const std::vector<byte>& data,
  uint8_t bits, uint64 poly, uint64 init, bool ref_in, bool ref_out, uint64 xor_out, uint64 check)
{
  static std::vector<AbstractProxy_CRC_t*> g_crc_list;
  if (g_crc_list.empty())
  {
    g_crc_list = get_crc_list();
  }
  if (g_crc_list.empty())
  {
    throw "crc list empty";
  }

  uint64 result = 0;

  for (auto& ptr_crc : g_crc_list)
  {
    if (ptr_crc->bits    == bits &&
        ptr_crc->poly    == poly &&
        ptr_crc->init    == init &&
        ptr_crc->ref_in  == ref_in &&
        ptr_crc->ref_out == ref_out &&
        ptr_crc->xor_out == xor_out &&
        ptr_crc->check   == check)
    {
      result = ptr_crc->get_crc(data.data(), data.size());
      break;
    }
  }

  return result;
}

uint64 crypt_crc_buffer(const std::vector<byte>& data, const crypt_bits bits)
{
  switch (bits)
  {
  case crypt_bits::_8:
    {
      CRC_t<8, 0x07, 0x00, false, false, 0x00> crc;
      return crc.get_crc(data.data(), data.size());
    }
    break;

  case crypt_bits::_16:
    {
      CRC_t<16, 0x8005, 0x0000, true, true, 0x0000> crc;
      return crc.get_crc(data.data(), data.size());
    }
    break;

  case crypt_bits::_32:
    {
      CRC_t<32, 0x04C11DB7, 0xFFFFFFFF, true, true, 0xFFFFFFFF> crc;
      return crc.get_crc(data.data(), data.size());
    }
    break;

  case crypt_bits::_64:
    {
      CRC_t<64, 0x42F0E1EBA9EA3693, 0x0000000000000000, false, false, 0x0000000000000000> crc;
      return crc.get_crc(data.data(), data.size());
    }
    break;

  default:
    {
      throw "invalid crc bits";
    }
    break;
  }

  return 0;
}

uint64 crypt_crc_text_A(const std::string& text, const crypt_bits bits)
{
  std::vector<byte> data(text.cbegin(), text.cend());
  return crypt_crc_buffer(data, bits);
}

uint64 crypt_crc_text_W(const std::wstring& text, const crypt_bits bits)
{
  auto s = to_string_A(text);
  return crypt_crc_text_A(s, bits);
}

uint64 crypt_crc_file_A(const std::string& file_path, const crypt_bits bits)
{
  if (!is_file_exists_A(file_path))
  {
    return 0;
  }

  std::vector<vu::byte> data;
  vu::read_file_binary_A(file_path, data);

  return crypt_crc_buffer(data, bits);
}

uint64 crypt_crc_file_W(const std::wstring& file_path, const crypt_bits bits)
{
  auto s = to_string_A(file_path);
  return crypt_crc_file_A(s, bits);
}

/**
 * SHA
 */

std::string crypt_sha_text_A(const std::string& text, const sha_version version, const crypt_bits bits)
{
  std::vector<byte> data(text.cbegin(), text.cend());

  std::vector<byte> hash;
  crypt_sha_buffer(data, version, bits, hash);

  std::string result = to_hex_string_A(hash.data(), hash.size());
  return result;
}

std::wstring crypt_sha_text_W(const std::wstring& text, const sha_version version, const crypt_bits bits)
{
  const auto s = to_string_A(text);
  auto hash = crypt_sha_text_A(s, version, bits);
  return to_string_W(hash);
}

std::string crypt_sha_file_A(const std::string& file_path, const sha_version version, const crypt_bits bits)
{
  if (!is_file_exists_A(file_path))
  {
    return "";
  }

  std::vector<vu::byte> data;
  vu::read_file_binary_A(file_path, data);

  std::vector<byte> hash;
  crypt_sha_buffer(data, version, bits, hash);

  std::string result = to_hex_string_A(hash.data(), hash.size());
  return result;
}

std::wstring crypt_sha_file_W(const std::wstring& file_path, const sha_version version, const crypt_bits bits)
{
  const auto s = to_string_A(file_path);
  auto hash = crypt_sha_file_A(s, version, bits);
  return to_string_W(hash);
}

void crypt_sha_buffer(
  const std::vector<byte>& data,
  const sha_version version,
  const crypt_bits bits,
  std::vector<byte>& hash)
{
  bool valid_args = false;

  valid_args |= (version == sha_version::_1) &&
    (bits == crypt_bits::_160);

  valid_args |= (version == sha_version::_2 || version == sha_version::_3) &&
    (bits == crypt_bits::_224 || bits == crypt_bits::_384 || bits == crypt_bits::_256 || bits == crypt_bits::_512);

  if (!valid_args)
  {
    throw "invalid sha bits";
  }

  hash.resize(int(bits) / 8); // size in bytes
  std::fill(hash.begin(), hash.end(), 0x00);
  auto pstr = reinterpret_cast<char*>(&hash[0]);

  if (version == sha_version::_1)
  {
    sha_1::sha1(data.data(), data.size(), pstr);
  }
  else if (version == sha_version::_2)
  {
    if (bits == crypt_bits::_224)
    {
      sha_2_224::sha2(data.data(), data.size(), pstr);
    }
    else if (bits == crypt_bits::_256)
    {
      sha_2_256::sha2(data.data(), data.size(), pstr);
    }
    else if (bits == crypt_bits::_384)
    {
      sha_2_384::sha2(data.data(), data.size(), pstr);
    }
    else if (bits == crypt_bits::_512)
    {
      sha_2_512::sha2(data.data(), data.size(), pstr);
    }
  }
  else if (version == sha_version::_3)
  {
    if (bits == crypt_bits::_224)
    {
      sha_3_224::sha3(data.data(), data.size(), pstr);
    }
    else if (bits == crypt_bits::_256)
    {
      sha_3_256::sha3(data.data(), data.size(), pstr);
    }
    else if (bits == crypt_bits::_384)
    {
      sha_3_384::sha3(data.data(), data.size(), pstr);
    }
    else if (bits == crypt_bits::_512)
    {
      sha_3_512::sha3(data.data(), data.size(), pstr);
    }
  }
  else
  {
    throw "invalid sha version";
  }
}

} // vu