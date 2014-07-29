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
  {
    using optional_failure = optional<optional<bool>>;

    struct default_test
    { };
    struct failed_test : std::runtime_error
    { using std::runtime_error::runtime_error; };

    struct tally_results
    { size_t const total, failed; };

    void log_failure(size_t const n, std::string const &msg)
    { std::cerr << "  test " << n << " failure: " << msg << std::endl; }
    void log_success(size_t const n)
    { std::cerr << "  test " << n << " success" << std::endl; }

    template <typename Group, size_t TN>
    optional_failure test_impl(Group &g)
    try
    {
      try
      {
        g.template test<TN>();
        log_success(TN);
        return {{ true }};
      }
      catch(std::exception const &e)
      { throw failed_test{ e.what() }; }
      catch(default_test)
      { return {}; }
      catch(...)
      { throw failed_test{ "unknown exception thrown" }; }
    }
    catch(failed_test const &e)
    {
      log_failure(TN, e.what());
      return {{ false }};
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
          if(!results[i].value().value())
          { ++failed; }
        }
      }
      std::cerr << "finished group '" + name << "'" << std::endl;
      return { total, failed };
    }
  }
}
