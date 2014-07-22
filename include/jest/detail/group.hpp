#pragma once

#include <vector>
#include <string>
#include <functional>
#include <utility>

#include "test.hpp"

namespace jest
{
  namespace detail
  {
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
}
