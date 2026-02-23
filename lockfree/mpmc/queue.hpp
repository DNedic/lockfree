/**************************************************************
 * @file queue.hpp
 * @brief A queue implementation written in standard c++11
 * suitable for both low-end microcontrollers all the way
 * to HPC machines. Lock-free for all scenarios.
 **************************************************************/

/**************************************************************
 * Copyright (c) 2023-2025 Djordje Nedic
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
 * Version:         v2.0.10
 **************************************************************/

/************************** INCLUDE ***************************/
#ifndef LOCKFREE_MPMC_QUEUE_HPP
#define LOCKFREE_MPMC_QUEUE_HPP

#include <atomic>
#include <cstddef>
#include <type_traits>

#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#include <optional>
#endif

namespace lockfree {
namespace mpmc {
/*************************** TYPES ****************************/

template <typename T, size_t size> class Queue {
    static_assert(std::is_trivial<T>::value, "The type T must be trivial");
    static_assert(size > 2, "Buffer size must be bigger than 2");
    static_assert((size & (size - 1)) == 0, "Buffer size must be a power of 2");

    /********************** PUBLIC METHODS ************************/
  public:
    Queue();

    /**
     * @brief Adds an element into the queue.
     * @param[in] element
     * @retval Operation success
     */
    bool Push(const T &element);

    /**
     * @brief Removes an element from the queue.
     * @param[out] element
     * @retval Operation success
     */
    bool Pop(T &element);

#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
    /**
     * @brief Removes an element from the queue.
     * @retval Either the element or nothing
     */
    std::optional<T> PopOptional();
#endif

    /*********************** PRIVATE TYPES ************************/
  private:
    struct Slot {
        T val;
        /**
         * Counts all pushes and pops performed on this slot.
         * Parity encodes the current state, while the value encodes the
         * revolution:
         * Even (2*R) - equal pushes and pops (R each) - EMPTY,
         * ready for the R-th push.
         * Odd  (2*R+1) - one more push than pop - FULL, ready for the
         * R-th pop.
         */
        std::atomic_size_t access_count;

        Slot() : access_count(0U) {}
    };

    /********************** PRIVATE MEMBERS ***********************/
  private:
    /* Clips access_count / 2 to the same range as revolution_count,
     * keeping the our_turn check correct through counter wrap-around. */
    static constexpr size_t _revolution_mask = ~size_t(0) / size;

    Slot _data[size]; /**< Data array */
#if LOCKFREE_CACHE_COHERENT
    alignas(LOCKFREE_CACHELINE_LENGTH)
        std::atomic_size_t _r_count; /**< Read monotonic counter */
    alignas(LOCKFREE_CACHELINE_LENGTH)
        std::atomic_size_t _w_count; /**< Write monotonic counter */
#else
    std::atomic_size_t _r_count; /**< Read monotonic counter */
    std::atomic_size_t _w_count; /**< Write monotonic counter */
#endif
};

} /* namespace mpmc */
} /* namespace lockfree */

/************************** INCLUDE ***************************/

/* Include the implementation */
#include "queue_impl.hpp"

#endif /* LOCKFREE_MPMC_QUEUE_HPP */
