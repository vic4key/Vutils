/**********************************************************/
/*  Name:     Vic Utilities for CUDA aka Vutils CUDA      */
/*  Version:  1.0                                         */
/*  Platform: Windows 32-bit & 64-bit                     */
/*  Type:     C++ Library for CUDA                        */
/*  Author:   Vic P. aka vic4key                          */
/*  Mail:     vic4key[at]gmail.com                        */
/*  Website:  https://vic.onl/                            */
/**********************************************************/

#ifndef VUTILS_CUDA_H
#define VUTILS_CUDA_H

/* Vutils Version */

#define VU_CUDA_VERSION  0x01000000 // Version 01.00.0000

/* The Conditions of Vutils */

#if !defined(_WIN32) && !defined(_WIN64)
#error Vutils CUDA required Windows 32-bit/64-bit platform
#endif

#ifndef __cplusplus
#error Vutils CUDA required C++ compiler
#endif

#include <string>
#include <utility>

#include <cuda_runtime.h>

namespace vu
{

#ifndef VUTILS_H
#include "inline/types.inl"
#include "inline/spechrs.inl"
#endif // VUTILS_H

/**
 * For both Host and Device
 */

static __host__ __device__ dim3 convert_index_to_position_2d(int index, int width, int height = NULL)
{
  return dim3(index % width, index / width, 1);
}

static __host__ __device__ int convert_position_to_index_2d(const dim3& position, int width, int height = NULL)
{
  return position.y * width + position.x;
}

/**
 * For Host
 */

namespace host
{

static __host__ int device_count()
{
  int count = 0;
  cudaGetDeviceCount(&count);
  return count;
}

static __host__ int device_id()
{
  int id = -1;
  cudaGetDevice(&id);
  return id;
}

static __host__ std::string device_name(int id)
{
  static cudaDeviceProp prop = { 0 };
  cudaGetDeviceProperties(&prop, id);
  std::string name(prop.name);
  return name;
}

static __host__ void device_synchronize()
{
  cudaDeviceSynchronize();
}

static __host__ void device_reset()
{
  cudaDeviceReset();
}

template <typename Fn>
__host__ float calcuate_occupancy(int block_size, Fn fn)
{
  int max_active_blocks;
  cudaOccupancyMaxActiveBlocksPerMultiprocessor(
    &max_active_blocks, static_cast<void*>(fn), block_size, 0);

  int device;
  cudaDeviceProp props;
  cudaGetDevice(&device);
  cudaGetDeviceProperties(&props, device);

  float occupancy = (max_active_blocks * block_size / props.warpSize) /
    float(props.maxThreadsPerMultiProcessor /
      props.warpSize);

  return occupancy;
}

template <typename Fn>
__host__ std::pair<int, int> calculate_execution_configuration_1d(int num_elements, Fn fn)
{
  int grid_size = 0;
  int block_size = 0;
  int min_grid_size = 0;
  cudaOccupancyMaxPotentialBlockSize(&min_grid_size, &block_size, static_cast<void*>(fn));
  grid_size = (num_elements + block_size - 1) / block_size;
  grid_size = max(min_grid_size, grid_size);
  return { grid_size, block_size };
}

template <typename Fn>
__host__ std::pair<dim3, dim3> calculate_execution_configuration_2d(int width, int height, Fn fn)
{
  const int num_threads_per_block = 32;
  dim3 block_size(num_threads_per_block, num_threads_per_block, 1);
  dim3 grid_size(width / num_threads_per_block + 1, height / num_threads_per_block + 1, 1);
  return { grid_size, block_size };
}

} // namespace host

/**
 * For Device
 */

namespace device
{

#ifdef __CUDACC__

__device__ dim3 current_element_position_1d()
{
  return dim3(blockIdx.x * blockDim.x + threadIdx.x, 1, 1);
}

__device__ int current_element_index_1d()
{
  return blockIdx.x * blockDim.x + threadIdx.x;
}

__device__ dim3 current_element_position_2d()
{
  int x = blockIdx.x * blockDim.x + threadIdx.x;
  int y = blockIdx.y * blockDim.y + threadIdx.y;
  return dim3(x, y, 1);
}

__device__ int current_element_index_2d(int width, int height = NULL)
{
  auto position = current_element_position_2d();
  return convert_position_to_index_2d(position, width, height);
}

#endif // __CUDACC__

} // namespace device

} // namespace vu

#endif // VUTILS_CUDA_H