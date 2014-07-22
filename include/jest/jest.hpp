#pragma once

#include <cstdint>
#include <iostream>

#include "detail/group.hpp"
#include "detail/expect.hpp"

namespace jest
{
  struct worker
  {
    worker()
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
