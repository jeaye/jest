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
  template <> template <>
  void ex_1_group::test<0>() /* Tests are numbered; the order does not matter. */
  {
    int const i{};
    float const f{ 3.14 };
    expect(i == f);
  }
  template <> template <> /* Double template bit here is required. */
  void ex_1_group::test<1>()
  {
    int const i{};
    float const f{ 3.14 };
    float const f2{ f * 2.0f };
    expect_equal(i, f, f2); /* Variadic; compares all permutations of pairs. */
  }
  template <> template <>
  void ex_1_group::test<2>()
  { fail("woops"); }
  template <> template <>
  void ex_1_group::test<3>()
  { fail(); }
  template <> template <>
  void ex_1_group::test<4>()
  { expect_equal(0, 0.0f, 0x00, 0000, 0b000); }
  template <> template <>
  void ex_1_group::test<28>() /* Test numbers do not need to be sequential. */
  { expect_equal("string", "String"); }
  template <> template <>
  void ex_1_group::test<29>()
  {
    expect_exception<std::runtime_error>([]
    { throw std::runtime_error{""}; });
  }
}

/* Step 3: Create a worker, which will run the tests. */
int main()
{
  jest::worker const j{};
  return j();
}
```
Possible output:
```
running group 'example'
  test 0 failure: failed 'unexpected' (false)
  test 1 failure: failed 'not equal' (0, 3.14)
  test 2 failure: failed 'explicit fail' ("woops")
  test 3 failure: failed 'explicit fail' ("")
  test 4 success
  test 28 failure: failed 'not equal' ("string", "String")
  test 29 success
finished group 'example'

5/7 test(s) failed

```

### What the hell is `template <> template <>`?
You're specializing a member function of `jest::group`, which is templated on your test type. It also inherits from your test type, giving direct access to your test type's member variables from within `jest::group::test`.

An example of where I use this group-specific data is for testing the output of certain functions to stdout. Add a `std::stringstream` to the test data, redirect `std::cout` to it, and now you can check its contents for each test. Example:
```cpp
#include <jest/jest.hpp>

#include <algorithm>

/* My test type is output, which has a stringstream. */
struct output
{
  output()
  { std::cout.rdbuf(out.rdbuf()); }

  std::stringstream out; /* This will be accessible in each test. */
};
using output_group = jest::group<output>;
output_group const output_obj{ "output" };

namespace jest
{
  namespace detail
  {
    void speak()
    { std::cout << "BARK"; }
    void yell(std::string s)
    {
      std::transform(std::begin(s), std::end(s), std::begin(s),
      [](char const c)
      { return std::toupper(c); });
      std::cout << s;
    }
  }

  template <> template <>
  void output_group::test<0>()
  {
    /* Here, I can access `out`, a member variable of my test type. */
    detail::speak();
    expect_equal(out.str(), "BARK");
    out.str("");
  }
  template <> template <>
  void output_group::test<1>()
  {
    detail::yell("testing is the best");
    expect_equal(out.str(), "TESTING IS THE BEST");
    out.str("");
  }
}

int main()
{
  jest::worker const j{};
  return j();
}
```
Possible output:
```
running group 'output'
  test 0 success
  test 1 success
finished group 'output'

all 2 tests passed
```

### Installation
Since jest is a header-only library, simply copy over the contents of `include` to your project, or, better yet, add jest as a submodule and introduce `jest/include` to your header search paths.  

Full installation can also be achieved by using `./configure && make install`. See the `configure` script for prefix options.  
