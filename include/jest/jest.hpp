#pragma once

#include <cstdint>
#include <iostream>

#include "detail/group.hpp"
#include "detail/expect.hpp"

namespace jest
{
  struct worker
  {
    worker() = default;

    bool operator ()() const
    {
      auto &registrar(detail::registrar::get());
      size_t total{}, failed{};
      for(auto const &group : registrar)
      {
        auto const tally(group.get().run());
        total += tally.total;
        failed += tally.failed;
      }
      if(failed)
      { std::cout << failed << "/" << total << " test(s) failed" << std::endl; }
      else
      { std::cout << "all " << total << "tests passed" << std::endl; }

      return failed;
    }
  };
}
