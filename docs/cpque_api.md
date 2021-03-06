# Container [cpque](../stc/cpque.h): Priority Queue

This describes the API of the queue type **cpque**. Implemented as a heap.

## Declaration

```c
#define using_cpque(X, ctype, heap_variant)
```
The macro `using_cpque()` must be instantiated in the global scope.
**cpque** uses normally a **cvec** type as underlying implementation, specified as `ctype`.
The `heap_variant` must be given as `<` or `>`, specifying *max-heap* or *min-heap* for the priority queue.
Note that the function `{ctype}_value_compare(x, y)` defined by the underlying vector type is used to
compare values (priorities). `X` is a type tag name and will affect the names of all cpque types and methods.
Declaring `using_cpque(my, cvec_my, >);`, `X` should be replaced by `my` in the following documentation.

## Types

| Type name            | Type definition                       | Used to represent...    |
|:---------------------|:--------------------------------------|:------------------------|
| `cpque_X`            | `struct {cpque_X_value_t* data; ...}` | The cpque type          |
| `cpque_X_value_t`    | Depends on underlying container type  | The cpque element type  |
| `cpque_X_input_t`    |                   "                   | cpque input type        |
| `cpque_X_rawvalue_t` |                   "                   | cpque raw value type    |

## Header file

All cpque definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/cpque.h"
```

## Methods

```c
cpque_X                 cpque_X_init(void);
cpque_X                 cpque_X_clone(cpque_X pq);
void                    cpque_X_make_heap(cpque_X* self);
void                    cpque_X_del(cpque_X* self);

size_t                  cpque_X_size(cpque_X pq);
bool                    cpque_X_empty(cpque_X pq);
const
cpque_X_value_t*        cpque_X_top(const cpque_X* self);

void                    cpque_X_push_n(cpque_X *self, const cpque_X_input_t arr[], size_t size);
void                    cpque_X_emplace(cpque_X* self, cpque_X_rawvalue_t raw);
void                    cpque_X_push(cpque_X* self, cpque_X_value_t value);
void                    cpque_X_pop(cpque_X* self);
void                    cpque_X_erase_at(cpque_X* self, size_t idx);
```

## Example
```c
#include <stdio.h>
#include "stc/cpque.h"
#include "stc/crand.h"

using_cvec(i, int64_t);
using_cpque(i, cvec_i, >); // adaptor type, '>' = min-heap

int main()
{
    size_t N = 10000000;
    crand_t rng = crand_init(1234);
    crand_uniform_t dist = crand_uniform_init(0, N * 10);

    cpque_i heap = cpque_i_init();
    // Push ten million random numbers to priority queue, plus some negative ones.
    c_forrange (N)
        cpque_i_push(&heap, crand_uniform(&rng, &dist));
    c_push_items(&heap, cpque_i, {-231, -32, -873, -4, -343});

    // Extract and display the fifty smallest.
    c_forrange (50) {
        printf("%zd ", *cpque_i_top(&heap));
        cpque_i_pop(&heap);
    }
    cpque_i_del(&heap);
}
```
Output:
```
 -873 -343 -231 -32 -4 3 5 6 18 23 31 54 68 87 99 105 107 125 128 147 150 155 167 178 181 188 213 216 272 284 287 302 306 311 313 326 329 331 344 348 363 367 374 385 396 399 401 407 412 477
```