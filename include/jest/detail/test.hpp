#pragma once

#include <string>
#include <stdexcept>
#include <cstdint>
#include <utility>
#include <iostream>

#include "optional.hpp"

namespace jest
{
  namespace detail
  { struct default_test {}; }

  /* TODO: Access to test data? */
  template <typename T, size_t N>
  void test()
  { throw detail::default_test{}; }

  namespace detail
  {
    using failure = std::string;
    using optional_failure = optional<optional<failure>>;

    struct tally_results
    { size_t const total, failed; };

    void log_failure(size_t const n, std::string const &msg)
    { std::cerr << "  test " << n << " failure: " << msg << std::endl; }
    void log_success(size_t const n)
    { std::cerr << "  test " << n << " success" << std::endl; }

    template <typename Group, size_t TN>
    optional_failure test_impl(Group &g)
    {
      try
      {
        g.template test<TN>();
        return {{}};
      }
      catch(std::exception const &e)
      { return {{ e.what() }}; }
      catch(default_test)
      { return {}; }
      catch(...)
      { return {{ "unknown exception thrown" }}; }
    }

    template <typename Group, size_t... Ns>
    tally_results run_impl(Group &g, std::string const &name,
                           std::integer_sequence<size_t, Ns...>)
    {
      std::cerr << "running group '" + name << "'" << std::endl;
      optional_failure const results[]{ test_impl<Group, Ns>(g)... };
      size_t total{}, failed{};
      for(size_t i{}; i < sizeof...(Ns); ++i)
      {
        if(results[i])
        {
          ++total;
          if(results[i].value())
          {
            log_failure(i, results[i].value().value());
            ++failed;
          }
          else
          { log_success(i); }
        }
      }
      std::cerr << "finished group '" + name << "'\n" << std::endl;
      return { total, failed };
    }
  }
}
