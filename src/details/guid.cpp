/**
 * @file   guid.cpp
 * @author Vic P.
 * @brief  Implementation for Globally/Universally Unique Identifier
 */

#include "Vutils.h"

#ifdef VU_GUID_ENABLED
#include <rpc.h>
#if defined(_MSC_VER) || defined(__BCPLUSPLUS__)
#pragma comment(lib, "rpcrt4.lib")
#endif
#endif // VU_GUID_ENABLED

namespace vu
{

#ifdef VU_GUID_ENABLED

#define NONE

#ifdef __MINGW32__
typedef unsigned char __RPC_FAR * RPC_CSTR;
#if defined(RPC_USE_NATIVE_WCHAR) && defined(_NATIVE_WCHAR_T_DEFINED)
typedef wchar_t __RPC_FAR * RPC_WSTR;
typedef const wchar_t * RPC_CWSTR;
#else
typedef unsigned short __RPC_FAR * RPC_WSTR;
typedef const unsigned short * RPC_CWSTR;
#endif
#endif

const sGUID& sGUID::operator=(const sGUID &right) const
{
  memcpy((void*)this, (const void*)&right, sizeof(*this));
  return *this;
}

bool sGUID::operator == (const sGUID &right) const
{
  return (memcmp(this, &right, sizeof(*this)) == 0);
}

bool sGUID::operator != (const sGUID &right) const
{
  return !(*this == right);
}

#define CAST_GUID(_object, _const) reinterpret_cast<_const ::UUID *> ( _object )

GUIDX::GUIDX(bool create)
{
  ZeroMemory(&m_guid, sizeof(m_guid));
  if (create)
  {
    this->create();
  }
}

GUIDX::~GUIDX() {}

bool GUIDX::create()
{
  return ((m_status = UuidCreate(CAST_GUID(&m_guid, NONE))) == RPC_S_OK);
}

const sGUID& GUIDX::GUID() const
{
  return m_guid;
}

void GUIDX::GUID(const sGUID& guid)
{
  m_guid = guid;
}

const GUIDX& GUIDX::operator = (const GUIDX &right)
{
  m_guid = right.m_guid;
  return *this;
}

bool GUIDX::operator == (const GUIDX &right) const
{
  return m_guid == right.m_guid;
}

bool GUIDX::operator != (const GUIDX &right) const
{
  return m_guid != right.m_guid;
}

void GUIDA::parse(const std::string& guid)
{
  m_guid = GUIDA::to_guid(guid);
}

std::string GUIDA::as_string() const
{
  return GUIDA::to_string(m_guid);
}

const std::string GUIDA::to_string(const sGUID& guid)
{
  std::string result("");

  RPC_CSTR rpcUUID;
  #if defined(__MINGW32__)
  auto pUUID = (::UUID*)(&guid);
  if (UuidToStringA(pUUID, &rpcUUID) != RPC_S_OK)
  #else
  if (UuidToStringA(CAST_GUID(&guid, const), &rpcUUID) != RPC_S_OK)
  #endif
  {
    return result;
  }

  result = LPCSTR(rpcUUID);

  RpcStringFreeA(&rpcUUID);

  return result;
}

const sGUID GUIDA::to_guid(const std::string& guid)
{
  sGUID result = {0};

  auto rpcUUID = RPC_CSTR(guid.c_str());
  UuidFromStringA(rpcUUID, CAST_GUID(&result, NONE));

  return result;
}

void GUIDW::parse(const std::wstring& guid)
{
  m_guid = GUIDW::to_guid(guid);
}

std::wstring GUIDW::as_string() const
{
  return GUIDW::to_string(m_guid);
}

const std::wstring GUIDW::to_string(const sGUID& guid)
{
  std::wstring result(L"");

  RPC_WSTR rpcUUID;
  #if defined(__MINGW32__)
  auto pUUID = (::UUID*)(&guid);
  if (UuidToStringW(pUUID, &rpcUUID) != RPC_S_OK)
  #else
  if (UuidToStringW(CAST_GUID(&guid, const), &rpcUUID) != RPC_S_OK)
  #endif
  {
    return result;
  }

  result = LPCWSTR(rpcUUID);

  RpcStringFreeW(&rpcUUID);

  return result;
}

const sGUID GUIDW::to_guid(const std::wstring& guid)
{
  sGUID result = {0};

  auto rpcUUID = RPC_WSTR(guid.c_str());
  UuidFromStringW(rpcUUID, CAST_GUID(&result, NONE));

  return result;
}

#endif // VU_GUID_ENABLED

} // namespace vu