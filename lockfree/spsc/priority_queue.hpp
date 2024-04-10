/**************************************************************
 * @file priority_queue.hpp
 * @brief A priority queue implementation written in standard
 * c++11 suitable for both low-end microcontrollers all the way
 * to HPC machines. Lock-free for single consumer single
 * producer scenarios.
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
 * Version:         v2.0.8
 **************************************************************/

/************************** INCLUDE ***************************/
#ifndef LOCKFREE_PRIORITY_QUEUE_HPP
#define LOCKFREE_PRIORITY_QUEUE_HPP

#include <atomic>
#include <cstddef>
#include <type_traits>

#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#include <optional>
#endif

#include "queue.hpp"

namespace lockfree {
namespace spsc {
/*************************** TYPES ****************************/

template <typename T, size_t size, size_t priority_count> class PriorityQueue {
    static_assert(std::is_trivial<T>::value, "The type T must be trivial");
    static_assert(size > 2, "Buffer size must be bigger than 2");

    /********************** PUBLIC METHODS ************************/
  public:
    /**
     * @brief Adds an element with a specified priority into the queue.
     * Should only be called from the producer thread.
     * @param[in] Element
     * @param[in] Element priority
     * @retval Operation success
     */
    bool Push(const T &element, size_t priority);

    /**
     * @brief Removes an element with the highest priority from the queue.
     * Should only be called from the consumer thread.
     * @param[out] Element
     * @retval Operation success
     */
    bool Pop(T &element);

#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
    /**
     * @brief Removes an element with the highest priority from the queue.
     * Should only be called from the consumer thread.
     * @retval Either the element or nothing if the queue is empty.
     */
    std::optional<T> PopOptional();
#endif

    /********************** PRIVATE MEMBERS ***********************/
  private:
    Queue<T, size> _subqueue[priority_count];
};

} /* namespace spsc */
} /* namespace lockfree */

/************************** INCLUDE ***************************/

/* Include the implementation */
#include "priority_queue_impl.hpp"

#endif /* LOCKFREE_PRIORITY_QUEUE_HPP */
