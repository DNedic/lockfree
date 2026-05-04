/**************************************************************
 * @file ring_buf.hpp
 * @brief A ring buffer implementation written in standard
 * c++11 suitable for both low-end microcontrollers all the way
 * to HPC machines. Lock-free for single consumer single
 * producer scenarios.
 **************************************************************/

/**************************************************************
 * Copyright (c) 2023-2026 Djordje Nedic
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, subject to the
 * following conditions:
 *
 * The above copyright notice and this permission notice shall
 * be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of lockfree
 *
 * Author:          Djordje Nedic <nedic.djordje2@gmail.com>
 * Version:         v3.0.1
 **************************************************************/

/************************** INCLUDE ***************************/
#ifndef LOCKFREE_RING_BUF_HPP
#define LOCKFREE_RING_BUF_HPP

#if !defined(LOCKFREE_CACHE_COHERENT) || !defined(LOCKFREE_CACHELINE_LENGTH)
#error                                                                         \
    "lockfree requires LOCKFREE_CACHE_COHERENT and LOCKFREE_CACHELINE_LENGTH to be defined; use the CMake build or define them manually"
#endif

#include <array>
#include <atomic>
#include <cstddef>
#include <type_traits>

#if __cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
#include <span>
#endif

namespace lockfree {
namespace spsc {
/*************************** TYPES ****************************/

template <typename T, size_t size> class RingBuf {
    static_assert(std::is_trivial<T>::value, "The type T must be trivial");
    static_assert(size > 2, "Buffer size must be bigger than 2");

    /********************** PUBLIC METHODS ************************/
  public:
    RingBuf();


    /**
     * @brief Clears the ring buffer.
     */
    void Clear();


    /**
     * @brief Writes data to the ring buffer.
     * Should only be called from the producer thread.
     * @param[in] data Pointer to the data to write
     * @param[in] cnt Number of elements to write
     * @retval Write success
     */
    bool Write(const T *data, size_t cnt);

    /**
     * @brief Writes data to the ring buffer.
     * Should only be called from the producer thread.
     * @param[in] data Data array to write
     * @retval Write success
     */
    template <size_t arr_size> bool Write(const std::array<T, arr_size> &data);

#if __cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
    /**
     * @brief Writes data to the ring buffer.
     * Should only be called from the producer thread.
     * @param[in] data Span of data to write
     * @retval Write success
     */
    bool Write(std::span<const T> data);
#endif

    /**
     * @brief Reads data from the ring buffer.
     * Should only be called from the consumer thread.
     * @param[out] data Pointer to the space to read the data to
     * @param[in] cnt Number of elements to read
     * @retval Read success
     */
    bool Read(T *data, size_t cnt);

    /**
     * @brief Reads data from the ring buffer.
     * Should only be called from the consumer thread.
     * @param[out] data Array to write the read to
     * @retval Read success
     */
    template <size_t arr_size> bool Read(std::array<T, arr_size> &data);

#if __cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
    /**
     * @brief Reads data from the ring buffer.
     * Should only be called from the consumer thread.
     * @param[out] data Span to read to
     * @retval Read success
     */
    bool Read(std::span<T> data);
#endif

    /**
     * @brief Reads data from the ring buffer without consuming it, meant to
     * be used in conjunction with Skip().
     * The combination is most useful when we want to keep the data in the
     * buffer after some operation using the data fails, or uses only some of
     * it. Should only be called from the consumer thread.
     * @param[out] data Pointer to the space to read the data to
     * @param[in] cnt Number of elements to read
     * @retval Peek success
     */
    bool Peek(T *data, size_t cnt) const;

    /**
     * @brief Reads data from the ring buffer without consuming it, meant to
     * be used in conjunction with Skip().
     * The combination is most useful when we want to keep the data in the
     * buffer after some operation using the data fails, or uses only some of
     * it. Should only be called from the consumer thread.
     * @param[out] data Array to write the read to
     * @retval Peek success
     */
    template <size_t arr_size> bool Peek(std::array<T, arr_size> &data) const;

    /**
     * @brief Reads data from the ring buffer without consuming it, meant to
     * be used in conjunction with Skip().
     * The combination is most useful when we want to keep the data in the
     * buffer after some operation using the data fails, or uses only some of
     * it. Should only be called from the consumer thread.
     * @param[out] data Span to read to
     * @retval Peek success
     */
#if __cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
    bool Peek(std::span<T> data) const;
#endif

    /**
     * @brief Makes the ring buffer skip the oldest data, meant to be used
     * in conjunction with Peek().
     * The combination is most useful when we want to keep the data in the
     * buffer after some operation using the data fails, or uses only some of
     * it. Should only be called from the consumer thread.
     * @param[in] cnt Number of elements to skip
     * @retval Skip success
     */
    bool Skip(size_t cnt);

    /**
     * @brief Gets the number of free slots in the ring buffer.
     * Just like Write(), this should be called only from the producer thread.
     * @retval Number of free slots
     */
    size_t GetFree() const;

    /**
     * @brief Gets the number of available elements in the ring buffer.
     * Just like Read(), this should be called only from the consumer thread.
     * @retval Number of available elements
     */
    size_t GetAvailable() const;

    /********************* PRIVATE METHODS ************************/
  private:
    static size_t CalcFree(const size_t w, const size_t r);
    static size_t CalcAvailable(const size_t w, const size_t r);

    /********************** PRIVATE MEMBERS ***********************/
  private:
    T _data[size]; /**< Data array */
#if LOCKFREE_CACHE_COHERENT
    alignas(LOCKFREE_CACHELINE_LENGTH) std::atomic_size_t _r; /**< Read index */
    alignas(
        LOCKFREE_CACHELINE_LENGTH) std::atomic_size_t _w; /**< Write index */
#else
    std::atomic_size_t _r; /**< Read index */
    std::atomic_size_t _w; /**< Write index */
#endif
};

} /* namespace spsc */
} /* namespace lockfree */

/************************** INCLUDE ***************************/

/* Include the implementation */
#include "ring_buf_impl.hpp"

#endif /* LOCKFREE_RING_BUF_HPP */
