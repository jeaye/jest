#include <jest/jest.hpp>

struct ex_1{ };
using ex_1_group = jest::group<ex_1>;
ex_1_group const ex_1_obj{ "example" };

namespace jest
{
  template <> template <>
  void ex_1_group::test<0>()
  {
    int const i{};
    float const f{ 3.14 };
    expect(i == f);
  }
  template <> template <>
  void ex_1_group::test<1>()
  {
    int const i{};
    float const f{ 3.14 };
    float const f2{ f * 2.0f };
    expect_equal(i, f, f2);
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
  void ex_1_group::test<5>()
  { expect_almost_equal(3.1400000f, 3.1400009f); }
  template <> template <>
  void ex_1_group::test<28>()
  { expect_equal("string", "String"); }
  template <> template <>
  void ex_1_group::test<29>()
  {
    expect_exception<std::runtime_error>([]
    { throw std::runtime_error{""}; });
    expect_exception<std::runtime_error>([]
    { throw std::logic_error{""}; });
  }
  template <> template <>
  void ex_1_group::test<30>()
  { expect_exception<std::runtime_error>([]{ }); }
  template <> template <>
  void ex_1_group::test<31>()
  { expect_exception<std::runtime_error, float>([]{ throw 2.0f; }); }
}

int main()
{
  jest::worker const j{};
  return j();
}
