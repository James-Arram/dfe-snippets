#ifndef BLAS_HPP_LTJDDIGU
#define BLAS_HPP_LTJDDIGU

#include <iostream>
#include <omp.h>
#include <string.h>

#include <dfesnippets/NumericUtils.hpp>

namespace dfesnippets {

  namespace blas {

    typedef double value_t;
    typedef std::vector<value_t> vec;

    class Matrix {
      long n;
      value_t* data;

      public:
      Matrix(long _n) : n(_n) {
        data = new value_t[n * n];
      }

      Matrix(const Matrix& other) {
        n = other.n;
        data = new value_t[n * n];
        memcpy(data, other.data, sizeof(value_t) * n * n);
      }

      ~Matrix() {
        delete [] data;
      }

      value_t& operator()(long i, long j) const {
        return data[i * n + j];
      }

      vec operator* (const vec& v) const {
        vec r(n, 0);
#pragma omp parallel for
        for (long i = 0; i < n; i++)
          for (long j = 0; j < n; j++) {
            r[i] += v[j] * data[i * n + j];
          }
        return r;
      }

      void print() const {
        for (int i = 0; i < n; i ++ ) {
          for (int j = 0 ; j < n; j++)
            std::cout << operator()(i, j) << " ";
          std::cout << std::endl;
        }
      }

      void init_random() {
#pragma omp parallel for
        for (long i = 0; i < n; i ++ )
          for (long j = 0 ; j < n; j++)
            data[i * n + j] =  j / 48 + 1;
      }

      void init() {
#pragma omp parallel for
        for (long i = 0; i < n; i ++ )
          for (long j = 0 ; j < n; j++)
            data[i * n + j] =  (i * j) % 48 + 1;
      }

      void init(int val) {
#pragma omp parallel for
        for (long i = 0; i < n; i ++ )
          for (long j = 0 ; j < n; j++)
            data[i * n + j] =  val;
      }

      void print_info() {
        std::cout << "Matrix "  << std::endl;
        std::cout << "  Dense (" << n << " x " << n << ")" << std::endl;
        std::cout << "  Size (GB) " << n * n  * 8 / (1024 * 1024 * 1024) << std::endl;
      }

      value_t* linear_access_pointer() {
        return data;
      }


      // converts matrix data to a strided access pattern, with given stride width (in elements)
      void convert_to_strided_access(int stride_width) {
        auto new_data = new value_t[n * n];
        if (n % stride_width != 0) {
          std::cerr << "Error! n must be a multiple of stride_width" << std::endl;
          return;
        }
        long nstrides = n / stride_width;
        long idx = 0;
        for (long i = 0; i < nstrides; i++)
          for (long row = 0; row < n; row++)
            for (long col = 0; col < stride_width; col++) {
              new_data[idx++] = data[row * n + i * stride_width + col] ;
            }
        delete [] data;
        data = new_data;
      }

      inline int size() const {
        return n;
      }

      void row_interchange(int i, int j) {
        for (int k = 0; k < n; k++) {
          value_t tmp = this->operator()(i, k);
          this->operator()(i, k) = this->operator()(j, k);
          this->operator()(j, k) = tmp;
        }
      }

    };

    class System {
      public:
        int n;
        Matrix a;
        std::vector<double> x;
        std::vector<double> b;

        System(int _n) : n(_n), a(_n), x(_n, 0), b(_n, 0) {
          a.init(0);
        }

        void init() {
          // start with identity matrix
          // fill the first row with multiples of main diagonal elements
          for (int i = 0; i < n; i++) {
            a(0, i) = i % 4;
            a(i, i) = 1;
          }

          fillMatrix();

          // generate solution
          for (int i = 0; i < n; i++)
            x[i] = i % 20;

          // generate rhs
          b = a * x;
        }

        bool checkSolution(std::vector<double> got) {
          bool good = true;
          for (size_t i = 0; i < x.size(); i++)
            if (!dfesnippets::numeric_utils::almost_equal(got[i], x[i], 1E-10)) {
              std::cerr << "Got " << got[i] << " exp " << x[i] << " @i= " << i << std::endl;
              good = false;
            }
          return good;
        }

        virtual void fillMatrix() {
          // apply some of the basic operations, adding a multiple of the first row to
          // all other rows
          for (int i = 1; i < n; i++) {
            for (int k = 0; k < n; k++) {
              a(i, k) += i * ((k % 4) + 1) * a(0, k);
            }
          }
        }
    };

    // A system which must be solved with (at least) partial pivoting
    class GEPPSystem : public System {

      public:
        GEPPSystem(long _n) : System(_n) {}

        virtual void fillMatrix() override {
          // for the system to require pivoting, at least one row must have zero
          // entries on the main diagonal: we don't fill the last row and just interchange
          // it with another row at the end
          for (int i = 1; i < n - 1; i++) {
            for (int k = 0; k < n; k++) {
              a(i, k) += i * ((k % 4) + 1) * a(0, k);
            }
          }

          if (n - 1 == n / 2) {
            std::cerr << "Error: can't build a nice GEPP System with n of size " << n << std::endl;
          }
          a.row_interchange(n - 1, n / 2);
        }
    };


  }
}

#endif /* end of include guard: BLAS_HPP_LTJDDIGU */
