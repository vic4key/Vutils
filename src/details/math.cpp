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

// https://en.wikipedia.org/wiki/Fast_inverse_square_root#Overview_of_the_code
float q_rsqrt(float number)
{
  long i;
  float x2, y;
  const float threehalfs = 1.5F;

  x2 = number * 0.5F;
  y = number;
  i = *(long*)&y;                       // evil floating point bit level hacking
  i = 0x5f3759df - (i >> 1);               // what the fuck?
  y = *(float*)&i;
  y = y * (threehalfs - (x2 * y * y));   // 1st iteration
  // y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

  return y;
}

float vuapi fast_sqrtf(const float number)
{
  return 1.F / q_rsqrt(number);
}

} // namespace vu