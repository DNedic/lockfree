/**************************************************************
 * @file bipartite_buf.hpp
 * @brief A bipartite buffer implementation written in
 * standard c++11 suitable for all systems, from low-end
 * microcontrollers to HPC machines.
 * Lock-free for single consumer single producer scenarios.
 * @version	2.0.4
 * @date 10. August 2023
 * @author Djordje Nedic
 **************************************************************/

/**************************************************************
 * Copyright (c) 2023 Djordje Nedic
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
 * Version:         v2.0.4
 **************************************************************/

/************************** INCLUDE ***************************/
#ifndef LOCKFREE_BIPARTITE_BUF_HPP
#define LOCKFREE_BIPARTITE_BUF_HPP

#include <atomic>
#include <cstddef>
#include <type_traits>

#if __cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
#include <span>
#endif

namespace lockfree {
namespace spsc {
/*************************** TYPES ****************************/

template <typename T, size_t size> class BipartiteBuf {
    static_assert(std::is_trivial<T>::value, "The type T must be trivial");
    static_assert(size > 2, "Buffer size must be bigger than 2");

    /********************** PUBLIC METHODS ************************/
  public:
    BipartiteBuf();

    /**
     * @brief Acquires a linear region in the bipartite buffer for writing
     * Should only be called from the producer thread.
     * @param[in] Free linear space in the buffer required
     * @retval Pointer to the beginning of the linear space
     */
    T *WriteAcquire(size_t free_required);

#if __cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
    /**
     * @brief Acquires a linear region in the bipartite buffer for writing
     * Should only be called from the producer thread.
     * @param[in] Free linear space in the buffer required
     * @retval Span of the linear space
     */
    std::span<T> WriteAcquireSpan(size_t free_required);
#endif

    /**
     * @brief Releases the bipartite buffer after a write
     * Should only be called from the producer thread.
     * @param[in] Elements written to the linear space
     * @retval None
     */
    void WriteRelease(size_t written);

#if __cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
    /**
     * @brief Releases the bipartite buffer after a write
     * Should only be called from the producer thread.
     * @param[in] Span of the linear space
     * @retval None
     */
    void WriteRelease(std::span<T> written);
#endif

    /**
     * @brief Acquires a linear region in the bipartite buffer for reading.
     * Should only be called from the consumer thread.
     * @retval Pair containing the pointer to the beginning of the area and
     * elements available
     */
    std::pair<T *, size_t> ReadAcquire();

#if __cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
    /**
     * @brief Acquires a linear region in the bipartite buffer for reading
     * Should only be called from the consumer thread.
     * @retval Span of the linear space
     */
    std::span<T> ReadAcquireSpan();
#endif

    /**
     * @brief Releases the bipartite buffer after a read
     * Should only be called from the consumer thread.
     * @param[in] Elements read from the linear region
     * @retval None
     */
    void ReadRelease(size_t read);

#if __cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
    /**
     * @brief Releases the bipartite buffer after a read
     * Should only be called from the consumer thread.
     * @param[in] Span of the linear space
     * @retval None
     */
    void ReadRelease(std::span<T> read);
#endif

    /********************* PRIVATE METHODS ************************/
  private:
    static size_t CalcFree(const size_t w, const size_t r);

    /********************** PRIVATE MEMBERS ***********************/
  private:
    T _data[size]; /**< Data array */
#if LOCKFREE_CACHE_COHERENT
    alignas(LOCKFREE_CACHELINE_LENGTH) std::atomic_size_t _r; /**< Read index */
    alignas(
        LOCKFREE_CACHELINE_LENGTH) std::atomic_size_t _w; /**< Write index */
    alignas(LOCKFREE_CACHELINE_LENGTH)
        std::atomic_size_t _i; /**< Invalidated space index */
#else
    std::atomic_size_t _r; /**< Read index */
    std::atomic_size_t _w; /**< Write index */
    std::atomic_size_t _i; /**< Invalidated space index */
#endif
    bool _write_wrapped; /**< Write wrapped flag, used only in the producer */
    bool _read_wrapped;  /**< Read wrapped flag, used only in the consumer */
};

} /* namespace spsc */
} /* namespace lockfree */

/************************** INCLUDE ***************************/

/* Include the implementation */
#include "bipartite_buf_impl.hpp"

#endif /* LOCKFREE_BIPARTITE_BUF_HPP */
