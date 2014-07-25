#include <jest/jest.hpp>

#include <algorithm>

struct output
{
  output()
  { std::cout.rdbuf(out.rdbuf()); }

  std::stringstream out;
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
