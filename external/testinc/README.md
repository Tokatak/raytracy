# testinc

One of the simplest header-only C unit test frameworks.

# Dependencies

- standard C library
- a GNU C macro extension: `__BASE_FILE__`
- a GCC C extension: nested functions

# Features

- separate process for each test case
- a single header file
- automatic test case registration
- no boilerplate
- provides a `TEST` and an `ASSERT` macro
- C-like simple syntax

# Usage

Include the `test.h` header file after all other includes, but before any code.

Define test case functions using `TEST` and `ASSERT` macros.

### Example

```c
#include "test.h"

TEST(my_test_case)
{
    ASSERT(0 == 0);
}
```

See also the [more detailed example](test_example.c).

Compile the test:

```
$ gcc -Wall test_example.c -o test_example
```

To print out all test cases, run the test executable without parameters:
```
$ ./test_example
this_will_pass
this_will_fail
this_will_pass_again
```

Run a single test case:
```
$ ./test_example this_will_pass
```
In this case, the zero exit status means that the test case passed.

Run all tests:
```
$ ./test_example --all
test_example.c:this_will_pass: PASS
test_example: test_example.c:23: this_will_fail: Assertion `0 == 1' failed.
test_example.c:this_will_fail: FAIL
test_example.c:this_will_pass_again: PASS
```
If all test cases passed, the exit status will be zero.
