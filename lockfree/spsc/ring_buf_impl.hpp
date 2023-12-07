/**************************************************************
 * @file ring_buf_impl.hpp
 * @brief A ring buffer implementation written in standard
 * c++11 suitable for both low-end microcontrollers all the way
 * to HPC machines. Lock-free for single consumer single
 * producer scenarios.
 * @version	2.0.5
 * @date 7. December 2023
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
 * Version:         v2.0.5
 **************************************************************/

namespace lockfree {
namespace spsc {
/********************** PUBLIC METHODS ************************/

template <typename T, size_t size>
RingBuf<T, size>::RingBuf() : _r(0U), _w(0U) {}

template <typename T, size_t size>
bool RingBuf<T, size>::Write(const T *data, const size_t cnt) {
    /* Preload variables with adequate memory ordering */
    size_t w = _w.load(std::memory_order_relaxed);
    const size_t r = _r.load(std::memory_order_acquire);

    if (CalcFree(w, r) < cnt) {
        return false;
    }

    /* Check if the write wraps */
    if (w + cnt <= size) {
        /* Copy in the linear region */
        memcpy(&_data[w], &data[0], cnt * sizeof(T));
        /* Correct the write index */
        w += cnt;
        if (w == size) {
            w = 0U;
        }
    } else {
        /* Copy in the linear region */
        const size_t linear_free = size - w;
        memcpy(&_data[w], &data[0], linear_free * sizeof(T));
        /* Copy remaining to the beginning of the buffer */
        const size_t remaining = cnt - linear_free;
        memcpy(&_data[0], &data[linear_free], remaining * sizeof(T));
        /* Correct the write index */
        w = remaining;
    }

    /* Store the write index with adequate ordering */
    _w.store(w, std::memory_order_release);

    return true;
}

template <typename T, size_t size>
bool RingBuf<T, size>::Read(T *data, const size_t cnt) {
    /* Preload variables with adequate memory ordering */
    size_t r = _r.load(std::memory_order_relaxed);
    const size_t w = _w.load(std::memory_order_acquire);

    if (CalcAvailable(w, r) < cnt) {
        return false;
    }

    /* Check if the read wraps */
    if (r + cnt <= size) {
        /* Copy in the linear region */
        memcpy(&data[0], &_data[r], cnt * sizeof(T));
        /* Correct the read index */
        r += cnt;
        if (r == size) {
            r = 0U;
        }
    } else {
        /* Copy in the linear region */
        const size_t linear_available = size - r;
        memcpy(&data[0], &_data[r], linear_available * sizeof(T));
        /* Copy remaining from the beginning of the buffer */
        const size_t remaining = cnt - linear_available;
        memcpy(&data[linear_available], &_data[0], remaining * sizeof(T));
        /* Correct the read index */
        r = remaining;
    }

    /* Store the write index with adequate ordering */
    _r.store(r, std::memory_order_release);

    return true;
}

template <typename T, size_t size>
bool RingBuf<T, size>::Peek(T *data, const size_t cnt) const {
    /* Preload variables with adequate memory ordering */
    const size_t r = _r.load(std::memory_order_relaxed);
    const size_t w = _w.load(std::memory_order_acquire);

    if (CalcAvailable(w, r) < cnt) {
        return false;
    }

    /* Check if the read wraps */
    if (r + cnt <= size) {
        /* Copy in the linear region */
        memcpy(&data[0], &_data[r], cnt * sizeof(T));
    } else {
        /* Copy in the linear region */
        const size_t linear_available = size - r;
        memcpy(&data[0], &_data[r], linear_available * sizeof(T));
        /* Copy remaining from the beginning of the buffer */
        const size_t remaining = cnt - linear_available;
        memcpy(&data[linear_available], &_data[0], remaining * sizeof(T));
    }

    return true;
}

template <typename T, size_t size>
bool RingBuf<T, size>::Skip(const size_t cnt) {
    /* Preload variables with adequate memory ordering */
    size_t r = _r.load(std::memory_order_relaxed);
    const size_t w = _w.load(std::memory_order_acquire);

    if (CalcAvailable(w, r) < cnt) {
        return false;
    }

    r += cnt;
    /* Wrap the index if necessary */
    if (r >= size) {
        r -= size;
    }

    /* Store the write index with adequate ordering */
    _r.store(r, std::memory_order_release);

    return true;
}

template <typename T, size_t size> size_t RingBuf<T, size>::GetFree() const {
    const size_t w = _w.load(std::memory_order_relaxed);
    const size_t r = _r.load(std::memory_order_acquire);

    return CalcFree(w, r);
}

template <typename T, size_t size>
size_t RingBuf<T, size>::GetAvailable() const {
    const size_t r = _r.load(std::memory_order_relaxed);
    const size_t w = _w.load(std::memory_order_acquire);

    return CalcAvailable(w, r);
}

/********************** std::array API ************************/

template <typename T, size_t size>
template <size_t arr_size>
bool RingBuf<T, size>::Write(const std::array<T, arr_size> &data) {
    return Write(data.begin(), arr_size);
}

template <typename T, size_t size>
template <size_t arr_size>
bool RingBuf<T, size>::Read(std::array<T, arr_size> &data) {
    return Read(data.begin(), arr_size);
}

template <typename T, size_t size>
template <size_t arr_size>
bool RingBuf<T, size>::Peek(std::array<T, arr_size> &data) const {
    return Peek(data.begin(), arr_size);
}

/********************** std::span API *************************/
#if __cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
template <typename T, size_t size>
bool RingBuf<T, size>::Write(std::span<const T> data) {
    return Write(data.data(), data.size());
}

template <typename T, size_t size>
bool RingBuf<T, size>::Read(std::span<T> data) {
    return Read(data.data(), data.size());
}

template <typename T, size_t size>
bool RingBuf<T, size>::Peek(std::span<T> data) const {
    return Peek(data.data(), data.size());
}
#endif

/********************* PRIVATE METHODS ************************/

template <typename T, size_t size>
size_t RingBuf<T, size>::CalcFree(const size_t w, const size_t r) {
    if (r > w) {
        return (r - w) - 1U;
    } else {
        return (size - (w - r)) - 1U;
    }
}

template <typename T, size_t size>
size_t RingBuf<T, size>::CalcAvailable(const size_t w, const size_t r) {
    if (w >= r) {
        return w - r;
    } else {
        return size - (r - w);
    }
}

} /* namespace spsc */
} /* namespace lockfree */
