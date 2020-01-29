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

CGUIDX::CGUIDX(bool create)
{
  ZeroMemory(&m_GUID, sizeof(m_GUID));
  if (create)
  {
    Create();
  }
}

CGUIDX::~CGUIDX() {}

bool CGUIDX::Create()
{
  return ((m_Status = UuidCreate(CAST_GUID(&m_GUID, NONE))) == RPC_S_OK);
}

const sGUID& CGUIDX::GUID() const
{
  return m_GUID;
}

void CGUIDX::GUID(const sGUID& guid)
{
  m_GUID = guid;
}

const CGUIDX& CGUIDX::operator = (const CGUIDX &right)
{
  m_GUID = right.m_GUID;
  return *this;
}

bool CGUIDX::operator == (const CGUIDX &right) const
{
  return m_GUID == right.m_GUID;
}

bool CGUIDX::operator != (const CGUIDX &right) const
{
  return m_GUID != right.m_GUID;
}

void CGUIDA::Parse(const std::string& guid)
{
  m_GUID = CGUIDA::ToGUID(guid);
}

std::string CGUIDA::AsString() const
{
  return CGUIDA::ToString(m_GUID);
}

const std::string CGUIDA::ToString(const sGUID& guid)
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

const sGUID CGUIDA::ToGUID(const std::string& guid)
{
  sGUID result = {0};

  auto rpcUUID = RPC_CSTR(guid.c_str());
  UuidFromStringA(rpcUUID, CAST_GUID(&result, NONE));

  return result;
}

void CGUIDW::Parse(const std::wstring& guid)
{
  m_GUID = CGUIDW::ToGUID(guid);
}

std::wstring CGUIDW::AsString() const
{
  return CGUIDW::ToString(m_GUID);
}

const std::wstring CGUIDW::ToString(const sGUID& guid)
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

const sGUID CGUIDW::ToGUID(const std::wstring& guid)
{
  sGUID result = {0};

  auto rpcUUID = RPC_WSTR(guid.c_str());
  UuidFromStringW(rpcUUID, CAST_GUID(&result, NONE));

  return result;
}

#endif // VU_GUID_ENABLED

} // namespace vu