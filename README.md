jest (C++14 unit test framework)
---

jest is a sane and minimal (header only) C++ unit test framework that uses a templated approach to registering tests. Absolutely no macros are used or needed for test writing, and the whole API can be described in the following example:

```cpp
#include <jest/jest.hpp>

/* Step 1: Define a group type and object. */
struct ex_1{ };
using ex_1_group = jest::group<ex_1>;
ex_1_group const ex_1_obj{ "example" };

/* Step 2: Specialize for your group and all tests you'd like. */
namespace jest
{
  template <>
  void test<ex_1_group, 0>() /* Tests are numbered; the order does not matter. */
  {
    int const i{};
    float const f{ 3.14 };
    expect(i == f);
  }
  template <>
  void test<ex_1_group, 1>()
  {
    int const i{};
    float const f{ 3.14 };
    float const f2{ f * 2.0f };
    expect_equal(i, f, f2); /* Variadic; compares all permutations of pairs. */
  }
  template <>
  void test<ex_1_group, 2>()
  { fail("woops"); }
  template <>
  void test<ex_1_group, 3>()
  { fail(); }
  template <>
  void test<ex_1_group, 28>() /* Test numbers do not need to be sequential. */
  { expect_equal("string", "String"); }
  template <>
  void test<ex_1_group, 4>()
  { }
}

/* Step 3: Create a worker, which will run the tests. */
int main()
{ jest::worker const j; }
```
Possible output:
```
running group 'example'
  test 0 failure: 'unexpected' (false) 
  test 1 failure: 'not equal' (0, 3.14) 
  test 2 failure: 'explicit fail' ("woops") 
  test 3 failure: 'explicit fail' ("") 
  test 4 success
  test 28 failure: 'not equal' ("string", "String") 
finished group 'example'

5/6 test(s) failed
```

### Installation
Since jest is a header-only library, simply copy over the contents of `include` to your project, or, better yet, add jest as a submodule and introduce `jest/include` to your header search paths.
