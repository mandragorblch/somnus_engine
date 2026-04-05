#pragma once
#include <functional>
#include "math/defs.h"
#include <concepts>

namespace smns::analytic {
  using real = ::smns::defs::real;
  using complex = ::smns::defs::complex;

template<typename func_t>
concept implicit_function =
requires(func_t f, real x, real y)
{
    { f(x, y) } -> std::convertible_to<real>;
};

template<typename func_t>
concept explicit_real_function =
requires(func_t f, real x)
{
    { f(x) } -> std::convertible_to<real>;
};

template<typename func_t>
concept explicit_complex_function =
requires(func_t f, complex x)
{
    { f(x) } -> std::convertible_to<complex>;
};

template<implicit_function func_t>
bool is_inside(const func_t& f, real x, real y) {
  return f(x, y) < 0;
}

template<explicit_real_function func_t>
real f_prime_real(const func_t& f, real x) { 
    real dx = std::sqrt(std::numeric_limits<real>::epsilon());
    return (f(x + dx) - f(x - dx)) / (2 * dx);
}

template<explicit_complex_function func_t>
real f_prime_complex(const func_t& f, real x) {
  real dx = std::sqrt(std::numeric_limits<real>::epsilon());
  std::complex x_idx{x, dx};
  auto res = f(x_idx);
  return std::imag(res) / dx;
}

//f(x) = g(x)
template<explicit_complex_function func_t>
real newton_solver_complex(
    const func_t& f,
    const func_t& g, const real guess,
    const real precision = std::sqrt(std::numeric_limits<real>::epsilon()),
    const size_t iteration_max = 100) {
  auto h = [&](auto x) { return f(x) - g(x); };
  real x = guess;

  for (size_t it = 0; it < iteration_max; ++it) {
    real fx = std::real(h(x));
    auto dfx = f_prime_complex(h, x);

    if (std::abs(dfx) < precision) break;

    real step = fx / dfx;
    x -= step;

    if (std::abs(step) < precision) break;
  }

  return x;
}

//f(x) = 0
template<explicit_complex_function func_t>
real newton_solver_complex(
    const func_t& f, const real guess,
    const real precision = std::sqrt(std::numeric_limits<real>::epsilon()),
    const real iteration_max = 100) {
   real x = guess;

  for (uint32_t it = 0; it < iteration_max; ++it) {
    real fx = std::real(f(x));
    real dfx = f_prime_complex(f, x);

    if (std::abs(dfx) < precision) break;

    real step = fx / dfx;
    x -= step;

    if (std::abs(step) < precision) break;
  }

  return x;
}
}  // namespace analytic

