/**
 * (C) Copyright Projet SECRET, INRIA, Rocquencourt
 * (C) Bhaskar Biswas and Nicolas Sendrier
 * (C) 2014 Jack Lloyd
 *
 * Botan is released under the Simplified BSD License (see license.txt)
 *
 */

#include <botan/mceliece.h>
#include <cmath>

namespace Botan {

namespace {

double binomial(size_t n, size_t k)
   {
   double x = 1;

   for(size_t i = 0; i != k; ++i)
      {
      x *= n - i;
      x /= k -i;
      }

   return x;
   }

double log_binomial(size_t n, size_t k)
   {
   double x = 0;

   for(size_t i = 0; i != k; ++i)
      {
      x += std::log(n - i);
      x -= std::log(k - i);
      }

   return x / std::log(2);
   }

double nb_iter(size_t n, size_t k, size_t w, size_t p, size_t l)
   {
   double x = 2 * log_binomial(k / 2, p);
   x += log_binomial(n - k - l, w - 2 * p);
   x = log_binomial(n, w) - x;
   return x;
   }

double cout_iter(size_t n, size_t k, size_t p, size_t l)
   {
   // x <- binomial(k/2,p)
   double x = binomial(k / 2, p);
   // i <- log[2](binomial(k/2,p))
   size_t i = (size_t) (std::log(x) / std::log(2)); // normalement i < 2^31
   // res <- 2*p*(n-k-l)*binomial(k/2,p)^2/2^l
   double res = 2 * p * (n - k - l) * ldexp(x * x, -l);
   // x <- binomial(k/2,p)*2*(2*l+log[2](binomial(k/2,p)))
   x *= 2 * (2 * l + i);
   // res <- k*(n-k)/2 +
   // binomial(k/2,p)*2*(2*l+log[2](binomial(k/2,p))) +
   // 2*p*(n-k-l)*binomial(k/2,p)^2/2^l
   res += x + k * ((n - k) / 2.0);

   return std::log(res) / std::log(2);
   }

double cout_total(size_t n, size_t k, size_t w, size_t p, size_t l)
   {
   return nb_iter(n, k, w, p, l) + cout_iter(n, k, p, l);
   }

double best_wf(size_t n, size_t k, size_t w, size_t p)
   {
   if(p >= k / 2)
      return -1;

   // On part de l = u, en faisant croitre l.
   // On s'arrète dés que le work factor croit.
   // Puis on explore les valeurs <u, mais en tenant de la convexite'

   double min = cout_total(n, k, w, p, 0);
   for(size_t l = 1; l < n - k; ++l)
      {
      double lwf = cout_total(n, k, w, p, l);
      if(lwf < min)
         {
         min = lwf;
         }
      else
         break;
      }

   return min;
   }

}

size_t mceliece_work_factor(size_t n, size_t k, size_t t)
   {
   double min = cout_total(n, k, t, 0, 0); // correspond a p=1
   for(size_t p = 0; p != t / 2; ++p)
      {
      double lwf = best_wf(n, k + 1, t, p);
      if(lwf < 0)
         break;

      min = std::min(min, lwf);
      }

   return min;
   }

}
