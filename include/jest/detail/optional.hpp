#pragma once

/* Depending on boost is an ass, but std::optional
 * won't be around until next year or so... */

#include <utility>

namespace jest
{
  namespace detail
  {
    /* Super simple and not standard-compliant. */
    template <typename T>
    class optional
    {
      public:
        optional() = default;
        optional(T &&t)
          : valid_{ true }, data_{ std::forward<T>(t) }
        { }

        operator bool() const
        { return valid_; }

        T& value()
        { return data_; }
        T const& value() const
        { return data_; }
      
      private:
        bool valid_{};
        T data_{};
    };
  }
}
