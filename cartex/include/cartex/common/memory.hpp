/*
 * GridTools
 *
 * Copyright (c) 2014-2021, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#pragma once

#include <memory>
#ifdef __CUDACC__
#include <string>
#include <stdexcept>
#ifdef NDEBUG
#define CARTEX_CHECK_CUDA_RESULT(x) x;
#else
#define CARTEX_CHECK_CUDA_RESULT(x)                                                                \
    if (x != cudaSuccess)                                                                          \
        throw std::runtime_error("CUDA Call failed " + std::string(#x) + " (" +                    \
                                 std::string(cudaGetErrorString(x)) + ") in " +                    \
                                 std::string(__FILE__) + ":" + std::to_string(__LINE__));
#endif
#endif

namespace cartex
{

template<typename T>
struct memory
{
    unsigned int         m_size;
    std::unique_ptr<T[]> m_host_memory;
#ifdef __CUDACC__
    struct cuda_deleter
    {
        // no delete since this messes up the rma stuff
        // when doing 2 tests in a row!!!
        //void operator()(T* ptr) const { cudaFree(ptr); }
        void operator()(T*) const
        { /* do nothing */
        }
    };
    std::unique_ptr<T[], cuda_deleter> m_device_memory;
#endif

    memory(unsigned int size_, const T& value)
    : m_size{size_}
    , m_host_memory{new T[m_size]}
    {
        for (unsigned int i = 0; i < m_size; ++i) m_host_memory[i] = value;
#ifdef __CUDACC__
        void* ptr;
        CARTEX_CHECK_CUDA_RESULT(cudaMalloc(&ptr, m_size * sizeof(T)))
        m_device_memory.reset((T*)ptr);
        clone_to_device();
#endif
    }

    memory(const memory&) = delete;
    memory(memory&&) = default;

    T* data() const { return m_host_memory.get(); }
    T* host_data() const { return m_host_memory.get(); }
#ifdef __CUDACC__
    T* device_data() const { return m_device_memory.get(); }
    T* hd_data() const { return m_device_memory.get(); }
#else
    T* hd_data() const { return m_host_memory.get(); }
#endif

    unsigned int size() const { return m_size; }

    const T& operator[](unsigned int i) const { return m_host_memory[i]; }
    T&       operator[](unsigned int i) { return m_host_memory[i]; }

    T* begin() { return m_host_memory.get(); }
    T* end() { return m_host_memory.get() + m_size; }

    const T* begin() const { return m_host_memory.get(); }
    const T* end() const { return m_host_memory.get() + m_size; }

#ifdef __CUDACC__
    void clone_to_device()
    {
        CARTEX_CHECK_CUDA_RESULT(cudaMemcpy(
            m_device_memory.get(), m_host_memory.get(), m_size * sizeof(T), cudaMemcpyHostToDevice))
    }
    void clone_to_host()
    {
        CARTEX_CHECK_CUDA_RESULT(cudaMemcpy(
            m_host_memory.get(), m_device_memory.get(), m_size * sizeof(T), cudaMemcpyDeviceToHost))
    }
#endif
};

} // namespace cartex
