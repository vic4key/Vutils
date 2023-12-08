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

/**
 * Divides a set of items into a specified number of pieces.
 * @param[in] num_items  The number of items.
 * @param[in] num_pieces The number of pieces.
 * @param[in] fn The function that apply to each piece.
 *    piece.idx The piece index.
 *    piece.beg The index of the begin item in piece.
 *    piece.end The index of the end item in piece.
 *    piece.num The number of items in piece.
 * Eg. In these cases, the number of items of each piece as the following:
 *  (4, 1) => (4)
 *  (4, 2) => (2, 2)
 *  (4, 3) => (1, 1, 2)
 *  (4, 4) => (1, 1, 1, 1)
 *  (4, 5) => (1, 1, 1, 1, 0)
 */
void divide_items_into_pieces(const size_t num_items, const size_t num_pieces,
  std::function<void(const piece_t& piece)> fn)
{
  if (fn == nullptr)
  {
    return;
  }

  size_t num_items_per_piece = std::max(1, int(num_items / float(num_pieces) + 0.5F));

  piece_t piece;

  for (size_t idx = 0; idx < num_pieces; ++idx)
  {
    size_t beg = idx * num_items_per_piece;
    size_t end = idx == num_pieces - 1 ? num_items : (idx + 1) * num_items_per_piece;
    size_t num = end - beg;

    piece.idx = idx;
    piece.beg = idx < num_items ? beg : -1;
    piece.end = idx < num_items ? end - 1 : -1;
    piece.num = idx < num_items ? num : 0;

    fn(piece);
  }
}

/**
 * Divides a set of items into a specified number of items per piece.
 * @param[in] num_items  The number of items.
 * @param[in] num_pieces The number of item per piece.
 * @param[in] fn The function that apply to each piece.
 *    piece.idx The piece index.
 *    piece.beg The index of the begin item in piece.
 *    piece.end The index of the end item in piece.
 *    piece.num The number of items in piece.
 * @return The number of pieces.
 * Eg. The number of items of each piece as the following.
 *  (4, 1) => (1, 1, 1, 1)
 *  (4, 2) => (2, 2)
 *  (4, 3) => (3, 1)
 *  (4, 4) => (4)
 *  (4, 5) => (4)
 */
size_t divide_items_into_num_items_per_piece(const size_t num_items, const size_t num_items_per_piece,
  std::function<void(const piece_t& piece)> fn)
{
  if (fn == nullptr)
  {
    return 0;
  }

  piece_t piece;

  size_t result = 0;

  std::vector<size_t> temp;

  for (size_t j = 1; j <= num_items; j++)
  {
    size_t i = j - 1;

    temp.push_back(i);

    if (j % num_items_per_piece == 0 || i == num_items - 1)
    {
      piece.idx = result;
      piece.beg = temp.front();
      piece.end = temp.back();
      piece.num = temp.size();

      fn(piece);

      temp.clear();
      result += 1;
    }
  }

  return result;
}

} // namespace vu