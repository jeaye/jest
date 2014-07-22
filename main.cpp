#include <iostream>
#include <vector>
#include <string>
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

    void log_failure(size_t const n, std::string const &msg)
    { std::cout << "  test " << n << " failure: " << msg << std::endl; }
    void log_success(size_t const n)
    { std::cout << "  test " << n << " success" << std::endl; }

    struct group_concept
    {
      virtual ~group_concept() = default;
      virtual void run() const = 0;
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
    void run_impl(std::string const &name, std::integer_sequence<size_t, Ns...>)
    {
      std::cout << "running group '" + name << "'" << std::endl;
      detail::optional_failure const results[]{ test_impl<Group, Ns>()... };
      for(size_t i{}; i < sizeof...(Ns); ++i)
      {
        if(results[i])
        {
          if(results[i].value())
          { log_failure(i, results[i].value().value()); }
          else
          { log_success(i); }
        }
      }
      std::cout << "finished group '" + name << "'\n" << std::endl;
    }
  }

  template <typename Data, size_t N = 50>
  class group : public Data, public detail::group_concept
  {
    public:
      group() = delete;
      group(std::string name)
        : name_{ std::move(name) }
      { detail::registrar::get().add(*this); }

      void run() const override
      { detail::run_impl<group>(name_, std::make_integer_sequence<size_t, N>()); }

    private:
      std::string name_;
  };

  struct worker
  {
    worker() = default;

    void operator ()() const
    {
      auto &registrar(detail::registrar::get());
      for(auto const &group : registrar)
      { group.get().run(); }
    }
  };
}

struct ex_1 { };
using ex_1_group = jest::group<ex_1>;
ex_1_group ex_1_g{ "ctor" };
struct ex_2 { };
using ex_2_group = jest::group<ex_2>;
ex_2_group ex_2_g{ "shared" };

namespace jest
{
  template <>
  void test<ex_1_group, 0>()
  { }
  template <>
  void test<ex_1_group, 1>()
  { throw std::runtime_error{ "not good" }; }

  template <>
  void test<ex_2_group, 0>()
  { }
  template <>
  void test<ex_2_group, 1>()
  { }
}

int main()
{
  jest::worker const j;
  j();
}
