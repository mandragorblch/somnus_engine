#pragma once
#include <functional>
#include "math/defs.h"

namespace smns::anltcs {
  using real = ::smns::defs::real;
  using complex = ::smns::defs::complex;

real f_prime_complex(const std::function<complex(complex)>& f, real x);

//f(x) = g(x)
real newton_solver_complex(
    const std::function<complex(complex)>& f,
    const std::function<complex(complex)>& g, const real guess,
    const real precision = std::numeric_limits<real>::epsilon(),
    const real iteration_max = 100);

//f(x) = 0
real newton_solver_complex(
    const std::function<complex(complex)>& f, const real guess,
    const real precision = std::numeric_limits<real>::epsilon(),
    const real iteration_max = 100);
}  // namespace anltcs