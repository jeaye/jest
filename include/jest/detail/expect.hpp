#pragma once

#include <sstream>
#include <string>
#include <iostream>
#include <stdexcept>

namespace jest
{
  namespace detail
  {
    struct no_exception
    { };

    /* TODO: suffix for types? 'f' for float, 'i' for int */
    template <typename T>
    void render_component(std::stringstream &ss, T const &t)
    { ss << t << ", "; }
    template <>
    void render_component<std::string>(std::stringstream &ss,
                                       std::string const &s)
    { ss << "\"" << s << "\", "; }
    template <size_t N>
    void render_component(std::stringstream &ss, char const (&s)[N])
    { ss << "\"" << s << "\", "; }

    template <typename... Args>
    void fail(std::string const &msg, Args const &... args)
    {
      std::stringstream ss;
      ss << std::boolalpha;
      int const _[]{ (render_component(ss, args), 0)... };
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

    template <typename... Ts>
    struct filter_exception;
    template <>
    struct filter_exception<>
    {
      void operator ()() const
      { throw std::runtime_error{ "unexpected exception" }; }
    };
    template <typename T, typename... Ts>
    struct filter_exception<T, Ts...>
    {
      void operator ()() const
      try
      { throw; }
      catch(T const &)
      { }
      catch(...)
      { filter_exception<Ts...>{}(); }
    };
  }

  template <typename... Ts>
  void expect_exception(std::function<void ()> const &f)
  try
  {
    f();
    throw detail::no_exception{};
  }
  catch(detail::no_exception const &)
  { throw std::runtime_error{ "expected exception, none was thrown" }; }
  catch(...)
  { detail::filter_exception<Ts...>{}(); }

  template <typename... Args>
  void expect_equal(Args const &... args)
  { detail::expect_equal_impl(0, args...); }
  template <typename... Args>
  void expect(bool const b)
  {
    if(!b)
    { detail::fail("unexpected", b); }
  }
  void fail(std::string const &msg)
  { detail::fail("explicit fail", msg); }
  void fail()
  { fail(""); }
}
