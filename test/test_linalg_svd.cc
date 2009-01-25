// -*- mode: c++; fill-column: 80; c-basic-offset: 2; indent-tabs-mode: nil -*-
//
// Copyright 2008, Juan Jose Garcia-Ripoll
//

#include "loops.h"
#include <gtest/gtest.h>
#include <gtest/gtest-death-test.h>
#include <tensor/tensor.h>
#include <tensor/linalg.h>

namespace tensor_test {

  using namespace tensor;

  //////////////////////////////////////////////////////////////////////
  // SINGULAR VALUE DECOMPOSITIONS
  //

  template<typename elt_t>
  bool unitaryp(const Tensor<elt_t> &U)
  {
    Tensor<elt_t> Ut = adjoint(U);
    if (U.rows() <= U.columns()) {
      if (!approx_eq(mmult(U, Ut), Tensor<elt_t>::eye(U.rows())))
        return false;
    }
    if (U.columns() <= U.rows()) {
      if (!approx_eq(mmult(Ut, U), Tensor<elt_t>::eye(U.columns())))
        return false;
    }
    return true;
  }

  template<typename elt_t>
  void test_eye_svd(int n) {
    if (n == 0) {
#ifndef NDEBUG
      ASSERT_DEATH(linalg::svd(Tensor<elt_t>::eye(n,n)), ".*");
#endif
      return;
    }
    for (int m = 1; m < n; m++) {
      Tensor<elt_t> Imn = Tensor<elt_t>::eye(m,n);
      Tensor<elt_t> Imm = Tensor<elt_t>::eye(m,m);
      Tensor<elt_t> Inn = Tensor<elt_t>::eye(n,n);
      Tensor<elt_t> V1(std::min(m,n));
      V1.fill_with(number_one<elt_t>());
      Tensor<elt_t> U, V;
      RTensor s;
      
      s = linalg::svd(Imn, &U, &V, false);
      EXPECT_EQ(Imm, U);
      EXPECT_EQ(Inn, V);
      EXPECT_EQ(s, V1);
      s = linalg::svd(Imn, &U, &V, true);
      EXPECT_EQ(U, m<n? Imm : Imn);
      EXPECT_EQ(V, m<n? Imn : Inn);
      EXPECT_EQ(s, V1);
    }
  }

  template<typename elt_t>
  void test_random_svd(int n) {
    if (n == 0) {
#ifndef NDEBUG
      ASSERT_DEATH(linalg::svd(Tensor<elt_t>::eye(n,n)), ".*");
#endif
      return;
    }
    for (int times = 10; times; --times) {
      for (int m = 1; m < 2*n; ++m) {
        Tensor<elt_t> A(m,n);
        A.randomize();

        Tensor<elt_t> U, Vt;
        RTensor s = linalg::svd(A, &U, &Vt, false);
        EXPECT_TRUE(unitaryp(U));
        EXPECT_TRUE(unitaryp(Vt));
        EXPECT_EQ(abs(s), s);
        EXPECT_TRUE(approx_eq(A, mmult(U, mmult(diag(s, 0,m,n), Vt))));

        RTensor s2 = linalg::svd(A, &U, &Vt, true);
        EXPECT_EQ(s, s2);
        EXPECT_TRUE(unitaryp(U));
        EXPECT_TRUE(unitaryp(Vt));
        EXPECT_TRUE(approx_eq(A, mmult(U, mmult(diag(s), Vt))));
      }
    }
  }

  //////////////////////////////////////////////////////////////////////
  // REAL SPECIALIZATIONS
  //
  
  TEST(RMatrixTest, EyeSvdTest) {
    test_over_integers(0, 32, test_eye_svd<double>);
  }

  TEST(RMatrixTest, RandomSvdTest) {
    test_over_integers(0, 32, test_random_svd<double>);
  }

  //////////////////////////////////////////////////////////////////////
  // COMPLEX SPECIALIZATIONS
  //

  TEST(CMatrixTest, EyeSvdTest) {
    test_over_integers(0, 32, test_eye_svd<cdouble>);
  }

  TEST(CMatrixTest, RandomSvdTest) {
    test_over_integers(0, 32, test_random_svd<cdouble>);
  }

} // namespace linalg_test