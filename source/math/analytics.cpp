#include "analytics.h"

namespace smns::anltcs {

// auto f = [](auto x) { return UR ANALYTICAL FUNCTION THERE; };
real f_prime_complex(const std::function<complex(complex)>& f, real x) {
  real dx = std::sqrt(std::numeric_limits<real>::epsilon());
  std::complex x_dx{x, dx};
  auto res = f(x_dx);
  return std::imag(res) / dx;
}

real newton_solver_complex(
    const std::function<complex(complex)>& f,
    const std::function<complex(complex)>& g, const real guess,
    const real precision,
    const real iteration_max) {
  auto h = [&](auto x) { return f(x) - g(x); };
  real x = guess;

  for (uint32_t it = 0; it < iteration_max; ++it) {
    real fx = std::real(h(x));
    real dfx = f_prime_complex(h, x);

    if (std::abs(dfx) < precision) break;

    real step = fx / dfx;
    x -= step;

    if (std::abs(step) < precision) break;
  }

  return x;
}
real newton_solver_complex(const std::function<complex(complex)>& f,
                           const real guess, const real precision,
                           const real iteration_max) {
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
}  // namespace anltcs