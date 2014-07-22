#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <functional>
#include <type_traits>
#include <memory>
#include <experimental/optional>

namespace jest
{
  namespace detail
  { struct default_test {}; }

  template <typename T, size_t N>
  void test()
  { throw detail::default_test{}; }

  namespace detail
  {
    using failure = std::string;
    using optional_failure = std::experimental::optional<std::experimental::optional<failure>>;

    struct tally_results
    { size_t const total, failed; };

    void log_failure(size_t const n, std::string const &msg)
    { std::cout << "  test " << n << " failure: " << msg << std::endl; }
    void log_success(size_t const n)
    { std::cout << "  test " << n << " success" << std::endl; }

    struct group_concept
    {
      virtual ~group_concept() = default;
      virtual tally_results run() const = 0;
    };

    class registrar
    {
      public:
        using vec_t = std::vector<std::reference_wrapper<group_concept const>>;
        using const_iterator = vec_t::const_iterator;

        static registrar& get()
        {
          static registrar r;
          return r;
        }

        const_iterator begin() const
        { return groups_.begin(); }
        const_iterator end() const
        { return groups_.end(); }

        void add(group_concept const &g)
        { groups_.emplace_back(std::cref(g)); }
        
      private:
        vec_t groups_;
    };

    template <typename Group, size_t TN>
    detail::optional_failure test_impl()
    {
      try
      {
        test<Group, TN>();
        return {{}};
      }
      catch(std::exception const &e)
      { return {{ e.what() }}; }
      catch(detail::default_test)
      { return {}; }
      catch(...)
      { return {{ "unknown exception thrown" }}; }
    }

    template <typename Group, size_t... Ns>
    tally_results run_impl(std::string const &name, std::integer_sequence<size_t, Ns...>)
    {
      std::cout << "running group '" + name << "'" << std::endl;
      detail::optional_failure const results[]{ test_impl<Group, Ns>()... };
      size_t failed{};
      for(size_t i{}; i < sizeof...(Ns); ++i)
      {
        if(results[i])
        {
          if(results[i].value())
          {
            log_failure(i, results[i].value().value());
            ++failed;
          }
          else
          { log_success(i); }
        }
      }
      std::cout << "finished group '" + name << "'\n" << std::endl;
      return { sizeof...(Ns), failed };
    }

    template <typename... Args>
    void fail(std::string const &msg, Args const &... args)
    {
      std::stringstream ss;
      ss << std::boolalpha;
      int const _[]{ (ss << args << ", ", 0)... }; /* TODO: quotes around strings */
      (void)_;
      throw std::runtime_error{ "failed '" + msg + "' (" + ss.str().substr(0, ss.str().size() - 2) + ")" };
    }

    template <typename T1, typename T2>
    void compare(T1 const &t1, T2 const &t2)
    {
      if(t1 != t2)
      { fail("not equal", t1, t2); }
    }
    template <typename T, typename... Args>
    void expect_equal_impl(size_t const n,
                           T const &t, Args const &... args)
    {
      int const _[]
      { (compare(t, args), 0)... };
      (void)_;
      if(n != sizeof...(Args))
      { expect_equal_impl(n + 1, args..., t); }
    }
  }

  template <typename... Args>
  void expect_equal(Args const &... args)
  { detail::expect_equal_impl(0, args...); }
  template <typename... Args>
  void expect(bool const b)
  {
    if(!b)
    { detail::fail("unexpected", b); }
  }

  template <typename Data, size_t N = 50>
  class group : public Data, public detail::group_concept
  {
    public:
      group() = delete;
      group(std::string name)
        : name_{ std::move(name) }
      { detail::registrar::get().add(*this); }

      detail::tally_results run() const override
      { return detail::run_impl<group>(name_, std::make_index_sequence<N>()); }

    private:
      std::string name_;
  };

  struct worker
  {
    worker() = default;

    void operator ()() const
    {
      auto &registrar(detail::registrar::get());
      size_t failed{};
      for(auto const &group : registrar)
      {
        auto const tally(group.get().run());
        failed += tally.failed;
      }
      if(failed)
      { std::cout << failed << " test(s) failed" << std::endl; }
      else
      { std::cout << "all tests passed" << std::endl; }
    }
  };
}

struct ex_1 { };
using ex_1_group = jest::group<ex_1>;
ex_1_group ex_1_g{ "example" };

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
}

int main()
{
  jest::worker const j;
  j();
}
