/**
 * @file   nameop.tpl
 * @author Vic P.
 * @brief  Template for Name Operator
 */

 /**
  * Name Operator
  */

#pragma once

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201402L) || (__cplusplus >= 201402L))

#include "3rdparty/NO/include/base/named_operator.hpp"
#include "3rdparty/NO/include/util/io_helpers.hpp"

namespace op_impl
{

struct in_t
{
  template <typename Container>
  bool operator ()(typename Container::value_type e, typename Container const& l) const
  {
    return std::find(std::begin(l), std::end(l), e) != std::end(l);
  }
};

} // op_impl

namespace std { namespace op {

static auto in = base::make_named_operator(op_impl::in_t());

}} // std::op

#endif