/**************************************************************
 * @file queue.hpp
 * @brief A lock free queue implementation written in standard
 * c++11.
 * @version	1.0.0
 * @date 9. May 2023
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
 * Version:         v1.0.0
 **************************************************************/

/************************** INCLUDE ***************************/
#ifndef LOCKFREE_QUEUE_HPP
#define LOCKFREE_QUEUE_HPP

#include <atomic>
#include <cstddef>

#if __cplusplus >= 201703L
#include <optional>
#endif

namespace lockfree {

/*************************** TYPES ****************************/

template <typename T, size_t size> class Queue {
    /********************** PUBLIC METHODS ************************/
  public:
    Queue();

    bool Push(const T &element);

    bool Pop(T &element);

#if __cplusplus >= 201703L
    std::optional<T> PopOptional();
#endif

    /********************** PRIVATE MEMBERS ***********************/
    T _data[size]; /**< Data array */
#if LOCKFREE_CACHE_COHERENT
    alignas(LOCKFREE_CACHELINE_LENGTH) std::atomic_size_t _r; /**< Read index */
    alignas(LOCKFREE_CACHELINE_LENGTH) std::atomic_size_t _w; /**< Write index */
#else
    std::atomic_size_t _r; /**< Read index */
    std::atomic_size_t _w; /**< Write index */
#endif
};

/************************** INCLUDE ***************************/

/* Include the implementation */
#include "queue_impl.hpp"

} /* namespace lockfree */

#endif /* LOCKFREE_QUEUE_HPP */
