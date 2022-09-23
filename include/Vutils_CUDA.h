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
#include <algorithm>

#include <cuda_runtime.h>

#ifdef max
#define vu_cuda_max max
#define vu_cuda_min min
#else  // std::max/min
#define vu_cuda_max max
#define vu_cuda_min min
#endif // max/min

namespace vu
{

#ifndef VUTILS_H
#include "inline/defs.inl"
#include "inline/types.inl"
#include "inline/spechrs.inl"
#endif // VUTILS_H

/**
 * For both Host and Device
 */

static __host__ __device__ void convert_index_to_position_2d(
  dim3& position, int index, int width, int height = NULL)
{
  position.x = index % width;
  position.y = index / width;
  position.z = 1;
}

static __host__ __device__ int convert_position_to_index_2d(
  const dim3& position, int width, int height = NULL)
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
__host__ std::pair<dim3, dim3> calculate_execution_configuration_3d(int width, int height, int depth, Fn fn)
{
  int min_grid_size = 0;
  int num_threads_per_block = 0;
  cudaOccupancyMaxPotentialBlockSize(&min_grid_size, &num_threads_per_block, static_cast<void*>(fn));

  cudaDeviceProp prop = { 0 };
  cudaGetDeviceProperties(&prop, host::device_id());

  num_threads_per_block = static_cast<int>(sqrt(num_threads_per_block));
  num_threads_per_block = VU_ALIGN_UP(num_threads_per_block, prop.warpSize);

  int num_elements = width * height * depth;
  int num_blocks_per_grid = (num_elements + num_threads_per_block - 1) / num_threads_per_block;

  // dim3 block_size(num_threads_per_block, num_threads_per_block, 1);
  // dim3 grid_size(width / num_threads_per_block + 1, height / num_threads_per_block + 1, depth);

  dim3 block_size(num_threads_per_block);
  dim3 grid_size(num_blocks_per_grid);

  return { grid_size, block_size };
}

template <typename Fn>
__host__ std::pair<dim3, dim3> calculate_execution_configuration_2d(int width, int height, Fn fn)
{
  return calculate_execution_configuration_3d(width, height, 1, fn);
}

template <typename Fn>
__host__ std::pair<int, int> calculate_execution_configuration_1d(int num_elements, Fn fn)
{
  int grid_size = 0;
  int block_size = 0;
  int min_grid_size = 0;
  cudaOccupancyMaxPotentialBlockSize(&min_grid_size, &block_size, static_cast<void*>(fn));
  grid_size = (num_elements + block_size - 1) / block_size;
  grid_size = vu_cuda_max(min_grid_size, grid_size);
  return { grid_size, block_size };
}

template <typename Fn>
__host__ std::pair<int, int> calculate_execution_configuration_1d(int width, int height, Fn fn)
{
  return calculate_execution_configuration_1d(width * height, fn);
}

} // namespace host

/**
 * For Device
 */

namespace device
{

#ifdef __CUDACC__

__device__ void current_element_position_1d(dim3& position)
{
  position.x = blockIdx.x * blockDim.x + threadIdx.x;
  position.y = 1;
  position.z = 1;
}

__device__ int current_element_index_1d()
{
  return blockIdx.x * blockDim.x + threadIdx.x;
}

__device__ void current_element_position_2d(dim3& position)
{
  position.x = blockIdx.x * blockDim.x + threadIdx.x;
  position.y = blockIdx.y * blockDim.y + threadIdx.y;
  position.z = 1;
}

__device__ int current_element_index_2d(int width, int height = NULL)
{
  return (blockIdx.y * blockDim.y + threadIdx.y) * width + (blockIdx.x * blockDim.x + threadIdx.x);
}

__device__ int current_element_index_3d(int width, int height, int depth)
{
  // unsigned int x = blockDim.x * blockIdx.x + threadIdx.x;
  // unsigned int y = blockDim.y * blockIdx.y + threadIdx.y;
  // unsigned int z = blockDim.z * blockIdx.z + threadIdx.z;
  // return x + (width * y) + (width * height * z);
  return\
    (blockDim.x * blockIdx.x + threadIdx.x) +
    (width * (blockDim.y * blockIdx.y + threadIdx.y)) +
    (width * height * (blockDim.z * blockIdx.z + threadIdx.z));
}

#endif // __CUDACC__

} // namespace device

} // namespace vu

#endif // VUTILS_CUDA_H