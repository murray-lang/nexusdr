#pragma once

#include <complex>
#include <vector>
//#include <valarray>

using sdrreal = float;
using vsdrreal = std::vector<sdrreal>;
using sdrcomplex = std::complex<sdrreal>;
using vsdrcomplex = std::vector<sdrcomplex>;
//using vsdrcomplex = std::valarray<sdrcomplex>;
using vsdrcomplex_iter = vsdrcomplex::iterator;

