/**
 * @file   math.cpp
 * @author Vic P.
 * @brief  Implementation for Math
 */

#include "Vutils.h"

#include <algorithm>

namespace vu
{

bool vuapi is_flag_on(ulongptr flags, ulongptr flag)
{
  return ((flags & flag) == flag);
}

intptr vuapi gcd(ulongptr count, ...) // Greatest Common Divisor -> BCNN
{
  va_list args;
  std::vector<intptr> array;
  intptr result = 0;

  array.clear();

  va_start(args, count);

  for (ulongptr i = 0; i < count; i++)
  {
    array.push_back(va_arg(args, intptr));
  }

  va_end(args);

  intptr min = *std::min_element(array.begin(), array.end());

  intptr pro = 1;
  for (auto i: array)
  {
    pro *= i;
  }

  if (pro == 0)
  {
    return 0;
  }

  ulongptr j;
  for (intptr i = min; i <= pro; i++)
  {
    for (j = 0; j < count; j++)
    {
      if (i % array[j] != 0)
      {
        break;
      }
    }
    if (j == count)
    {
      result = i;
      break;
    }
  }

  return result;
}

intptr vuapi lcm(ulongptr count, ...) // Least Common Multiples -> UCLN = | a1 * a2 * ... * an | / GCD(a1, a2, ..., an)
{
  va_list args;
  std::vector<intptr> array;
  intptr result = 0;

  array.clear();

  va_start(args, count);

  for (ulongptr i = 0; i < count; i++)
  {
    array.push_back(va_arg(args, intptr));
  }

  va_end(args);

  intptr min = *std::min_element(array.begin(), array.end());

  ulongptr j;
  for (intptr i = min - 1; i > 1; i--)
  {
    for (j = 0; j < count; j++)
    {
      if (array[j] % i != 0)
      {
        break;
      }
    }
    if (j == count)
    {
      result = i;
      break;
    }
  }

  return result;
}

} // namespace vu