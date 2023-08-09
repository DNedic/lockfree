/**************************************************************
 * @file lockfree.hpp
 * @brief A collection of lock free data structures written in
 * standard c++11 suitable for all systems, from low-end
 * microcontrollers to HPC machines.
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

#ifndef LOCKFREE_HPP
#define LOCKFREE_HPP

/************************** DEFINE ****************************/

#ifndef LOCKFREE_CACHE_COHERENT
#define LOCKFREE_CACHE_COHERENT true
#endif

#ifndef LOCKFREE_CACHELINE_LENGTH
#define LOCKFREE_CACHELINE_LENGTH 64U
#endif

/************************** INCLUDE ***************************/

#include "spsc/bipartite_buf.hpp"
#include "spsc/priority_queue.hpp"
#include "spsc/queue.hpp"
#include "spsc/ring_buf.hpp"

#include "mpmc/priority_queue.hpp"
#include "mpmc/queue.hpp"

#endif /* LOCKFREE_HPP */
