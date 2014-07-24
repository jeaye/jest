#include <jest/jest.hpp>

struct ex_1{ };
using ex_1_group = jest::group<ex_1>;
ex_1_group const ex_1_obj{ "example" };

namespace jest
{
  template <>
  void test<ex_1_group, 0>()
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
    expect_equal(i, f, f2);
  }
  template <>
  void test<ex_1_group, 2>()
  { fail("woops"); }
  template <>
  void test<ex_1_group, 3>()
  { fail(); }
  template <>
  void test<ex_1_group, 4>()
  { expect_equal(0, 0.0f, 0x00, 0000, 0b000); }
  template <>
  void test<ex_1_group, 28>()
  { expect_equal("string", "String"); }
}

int main()
{
  jest::worker const j{};
  return j();
}
