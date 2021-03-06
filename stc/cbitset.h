/* MIT License
 *
 * Copyright (c) 2020 Tyge Løvset, NORCE, www.norceresearch.no
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef CBITSET__H__
#define CBITSET__H__

/*
Similar to boost::dynamic_bitset / std::bitset

#include <stdio.h>
#include "cbitset.h"

int main() {
    cbitset_t bset = cbitset_with_size(23, true);
    cbitset_reset(&bset, 9);
    cbitset_resize(&bset, 43, false);
    printf("%4zu: ", bset.size); c_forrange (i, bset.size) printf("%d", cbitset_test(&bset, i));
    puts("");
    cbitset_set(&bset, 28);
    cbitset_resize(&bset, 77, true);
    cbitset_resize(&bset, 93, false);
    cbitset_resize(&bset, 102, true);
    cbitset_set_value(&bset, 99, false);
    printf("%4zu: ", bset.size); c_forrange (i, bset.size) printf("%d", cbitset_test(&bset, i));
    puts("");
    cbitset_del(&bset);
}
*/
#include <stdlib.h>
#include <string.h>
#include "ccommon.h"

typedef struct cbitset { uint64_t* _arr; size_t size; } cbitset_t;

STC_API cbitset_t    cbitset_with_size(size_t size, bool value);
STC_API cbitset_t    cbitset_from_str(const char* str);
STC_API char*        cbitset_to_str(cbitset_t set, char* str, size_t start, intptr_t stop);
STC_API cbitset_t    cbitset_clone(cbitset_t other);
STC_API void         cbitset_resize(cbitset_t* self, size_t size, bool value);
STC_API size_t       cbitset_count(cbitset_t set);
STC_API bool         cbitset_is_disjoint(cbitset_t set, cbitset_t other);
STC_API bool         cbitset_is_subset(cbitset_t set, cbitset_t other);
STC_API bool         cbitset_is_superset(cbitset_t set, cbitset_t other);

STC_INLINE size_t    cbitset_size(cbitset_t set) {return set.size;}
#define              cbitset_inits {NULL, 0}

STC_INLINE cbitset_t cbitset_init() {
    cbitset_t bs = cbitset_inits; return bs;
}
STC_INLINE void cbitset_del(cbitset_t* self) {
    c_free(self->_arr);
}

STC_DEF cbitset_t* cbitset_take(cbitset_t* self, cbitset_t other) {
    if (self->_arr != other._arr) cbitset_del(self), *self = other;
    return self;
}
STC_DEF cbitset_t* cbitset_assign(cbitset_t* self, cbitset_t other) {
    if (self->_arr != other._arr) cbitset_del(self), *self = cbitset_clone(other);
    return self;
}
STC_INLINE cbitset_t cbitset_move(cbitset_t* self) {
    cbitset_t tmp = *self; self->_arr = NULL, self->size = 0;
    return tmp;
}

STC_INLINE void cbitset_set(cbitset_t *self, size_t i) {
    self->_arr[i >> 6] |= 1ull << (i & 63);
}
STC_INLINE void cbitset_reset(cbitset_t *self, size_t i) {
    self->_arr[i >> 6] &= ~(1ull << (i & 63));
}
STC_INLINE void cbitset_set_value(cbitset_t *self, size_t i, bool value) {
    value ? cbitset_set(self, i) : cbitset_reset(self, i);
}
STC_INLINE void cbitset_flip(cbitset_t *self, size_t i) {
    self->_arr[i >> 6] ^= 1ull << (i & 63);
}
STC_INLINE bool cbitset_test(cbitset_t set, size_t i) {
    return (set._arr[i >> 6] & (1ull << (i & 63))) != 0;
}
STC_INLINE void cbitset_set_all(cbitset_t *self, bool value) {
    memset(self->_arr, value ? 0xff : 0x0, ((self->size + 63) >> 6) * 8);
}
STC_INLINE void cbitset_set_all_64(cbitset_t *self, uint64_t pattern) {
    size_t n = (self->size + 63) >> 6;
    for (size_t i=0; i<n; ++i) self->_arr[i] = pattern;
}
STC_INLINE void cbitset_flip_all(cbitset_t *self) {
    size_t n = (self->size + 63) >> 6;
    for (size_t i=0; i<n; ++i) self->_arr[i] ^= ~0ull;
}

/* Intersection */
STC_INLINE void cbitset_intersect_with(cbitset_t *self, cbitset_t other) {
    assert(self->size == other.size);
    size_t n = (self->size + 63) >> 6;
    for (size_t i=0; i<n; ++i) self->_arr[i] &= other._arr[i];
}
/* Union */
STC_INLINE void cbitset_union_with(cbitset_t *self, cbitset_t other) {
    assert(self->size == other.size);
    size_t n = (self->size + 63) >> 6;
    for (size_t i=0; i<n; ++i) self->_arr[i] |= other._arr[i];
}
/* Exclusive disjunction */
STC_INLINE void cbitset_xor_with(cbitset_t *self, cbitset_t other) {
    assert(self->size == other.size);
    size_t n = (self->size + 63) >> 6;
    for (size_t i=0; i<n; ++i) self->_arr[i] ^= other._arr[i];
}

STC_INLINE cbitset_t cbitset_intersect(cbitset_t s1, cbitset_t s2) {
    cbitset_t set = cbitset_clone(s1);
    cbitset_intersect_with(&set, s2); return set;
}
STC_INLINE cbitset_t cbitset_union(cbitset_t s1, cbitset_t s2) {
    cbitset_t set = cbitset_clone(s1);
    cbitset_union_with(&set, s2); return set;
}
STC_INLINE cbitset_t cbitset_xor(cbitset_t s1, cbitset_t s2) {
    cbitset_t set = cbitset_clone(s1);
    cbitset_xor_with(&set, s2); return set;
}
STC_INLINE cbitset_t cbitset_not(cbitset_t s1) {
    cbitset_t set = cbitset_clone(s1);
    cbitset_flip_all(&set); return set;
}

typedef struct {
    cbitset_t *_bs;
    size_t ref;
} cbitset_iter_t;

STC_INLINE cbitset_iter_t
cbitset_begin(cbitset_t* self) {
    cbitset_iter_t it = {self, 0}; return it;
}
STC_INLINE cbitset_iter_t
cbitset_end(cbitset_t* self) {
    cbitset_iter_t it = {self, self->size}; return it;
}
STC_INLINE void
cbitset_next(cbitset_iter_t* it) {++it->ref;}

STC_INLINE bool cbitset_itval(cbitset_iter_t it) {
    return cbitset_test(*it._bs, it.ref);
}

#if defined(__GNUC__) || defined(__clang__)
    STC_INLINE uint64_t cpopcount64(uint64_t x) {return __builtin_popcountll(x);}
#elif defined(_MSC_VER) && defined(_WIN64)
    #include <intrin.h>
    STC_INLINE uint64_t cpopcount64(uint64_t x) {return __popcnt64(x);}
#else
    STC_INLINE uint64_t cpopcount64(uint64_t x) { /* http://en.wikipedia.org/wiki/Hamming_weight */
        x -= (x >> 1) & 0x5555555555555555;
        x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333);
        x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0f;
        return (x * 0x0101010101010101) >> 56;
    }
#endif

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)

STC_DEF void cbitset_resize(cbitset_t* self, size_t size, bool value) {
    size_t new_n = (size + 63) >> 6, osize = self->size, old_n = (osize + 63) >> 6;
    self->_arr = (uint64_t *) c_realloc(self->_arr, new_n * 8);
    self->size = size;
    if (new_n >= old_n) {
        memset(self->_arr + old_n, value ? 0xff : 0x0, (new_n - old_n) * 8);
        if (old_n > 0) {
            uint64_t m = (1ull << (osize & 63)) - 1; /* mask */
            value ? (self->_arr[old_n - 1] |= ~m) : (self->_arr[old_n - 1] &= m);
        }
    }
}

STC_DEF cbitset_t cbitset_with_size(size_t size, bool value) {
    cbitset_t set = {(uint64_t *) c_malloc(((size + 63) >> 6) * 8), size};
    cbitset_set_all(&set, value);
    return set;
}
STC_DEF cbitset_t cbitset_from_str(const char* str) {
    const char* p = str; while (*p) ++p;
    cbitset_t set = cbitset_with_size(p - str, false);
    for (size_t i=0; i<set.size; ++i) if (str[i] == '1') cbitset_set(&set, i);
    return set;
}
STC_DEF char* cbitset_to_str(cbitset_t set, char* out, size_t start, intptr_t stop) {
    size_t end = stop < 0 ? set.size : stop;
    for (size_t i=start; i<end; ++i) out[i] = cbitset_test(set, i) ? '1' : '0';
    out[end] = '\0'; return out;
}
STC_DEF cbitset_t cbitset_clone(cbitset_t other) {
    size_t bytes = ((other.size + 63) >> 6) * 8;
    cbitset_t set = {(uint64_t *) memcpy(c_malloc(bytes), other._arr, bytes), other.size};
    return set;
}
STC_DEF size_t cbitset_count(cbitset_t s) {
    size_t count = 0, n = ((s.size + 63) >> 6) - 1;
    if (s.size > 0) {
        for (size_t i=0; i<n; ++i) count += cpopcount64(s._arr[i]);
        count += cpopcount64(s._arr[n] & ((1ull << (s.size & 63)) - 1));
    }
    return count;
}

#define _cbitset_SETOP(OPR) \
    assert(s.size == other.size); \
    if (s.size == 0) return false; /* ? */ \
    size_t n = ((s.size + 63) >> 6) - 1; \
    for (size_t i=0; i<n; ++i) \
        if ((s._arr[i] OPR other._arr[i]) != s._arr[i]) \
            return false; \
    uint64_t m = (1ull << (s.size & 63)) - 1, last = s._arr[n] & m; \
    return (last OPR (other._arr[n] & m)) == last

STC_DEF bool cbitset_is_disjoint(cbitset_t s, cbitset_t other) { _cbitset_SETOP(^); }
STC_DEF bool cbitset_is_subset(cbitset_t s, cbitset_t other) { _cbitset_SETOP(|); }
STC_DEF bool cbitset_is_superset(cbitset_t s, cbitset_t other) { _cbitset_SETOP(&); }

#endif
#endif
