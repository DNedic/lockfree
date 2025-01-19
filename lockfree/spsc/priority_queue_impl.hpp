/**************************************************************
 * @file priority_queue_impl.hpp
 * @brief A priority queue implementation written in standard
 * c++11 suitable for both low-end microcontrollers all the way
 * to HPC machines. Lock-free for single consumer single
 * producer scenarios.
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

#include <cassert>

namespace lockfree {
namespace spsc {
/********************** PUBLIC METHODS ************************/

template <typename T, size_t size, size_t priority_count>
bool PriorityQueue<T, size, priority_count>::Push(const T &element,
                                                  const size_t priority) {
    assert(priority < priority_count);

    return _subqueue[priority].Push(element);
}

template <typename T, size_t size, size_t priority_count>
bool PriorityQueue<T, size, priority_count>::Pop(T &element) {

    for (size_t priority = priority_count; priority-- > 0;) {
        if (_subqueue[priority].Pop(element)) {
            return true;
        }
    }

    /* Could find no elements at all */
    return false;
}

/********************* std::optional API **********************/
#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
template <typename T, size_t size, size_t priority_count>
std::optional<T> PriorityQueue<T, size, priority_count>::PopOptional() {
    T element;
    bool result = Pop(element);

    if (result) {
        return element;
    } else {
        return {};
    }
}
#endif

} /* namespace spsc */
} /* namespace lockfree */
