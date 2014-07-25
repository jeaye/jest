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
      virtual tally_results run() = 0;
    };

    class registrar
    {
      public:
        using vec_t = std::vector<std::reference_wrapper<group_concept>>;
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

        void add(group_concept &g)
        { groups_.emplace_back(std::ref(g)); }

      private:
        vec_t groups_;
    };
  }

  template <typename Data, size_t N = 50>
  class group : public Data, public detail::group_concept
  {
    public:
      group() = delete;
      group(std::string &&name)
        : name_{ std::move(name) }
      { detail::registrar::get().add(*this); }

      detail::tally_results run() override
      { return detail::run_impl(*this, name_, std::make_index_sequence<N>()); }

      template <size_t TN>
      void test()
      { throw detail::default_test{}; }

    private:
      std::string name_;
  };
}
