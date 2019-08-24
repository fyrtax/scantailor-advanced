// Copyright (C) 2019  Joseph Artsimovich <joseph.artsimovich@gmail.com>, 4lex4 <4lex49@zoho.com>
// Use of this source code is governed by the GNU GPLv3 license that can be found in the LICENSE file.

#ifndef SCANTAILOR_MATH_HOMOGRAPHICTRANSFORM_H_
#define SCANTAILOR_MATH_HOMOGRAPHICTRANSFORM_H_

#include <cstddef>
#include "MatrixCalc.h"
#include "VecNT.h"

template <size_t N, typename T>
class HomographicTransform;

template <size_t N, typename T>
class HomographicTransformBase {
 public:
  typedef VecNT<N, T> Vec;
  typedef VecNT<(N + 1) * (N + 1), T> Mat;

  explicit HomographicTransformBase(const Mat& mat) : m_mat(mat) {}

  HomographicTransform<N, T> inv() const;

  Vec operator()(const Vec& from) const;

  const Mat& mat() const { return m_mat; }

 private:
  Mat m_mat;
};


template <size_t N, typename T>
class HomographicTransform : public HomographicTransformBase<N, T> {
 public:
  explicit HomographicTransform(const typename HomographicTransformBase<N, T>::Mat& mat)
      : HomographicTransformBase<N, T>(mat) {}
};


/** An optimized, both in terms of API and performance, 1D version. */
template <typename T>
class HomographicTransform<1, T> : public HomographicTransformBase<1, T> {
 public:
  explicit HomographicTransform(const typename HomographicTransformBase<1, T>::Mat& mat)
      : HomographicTransformBase<1, T>(mat) {}

  T operator()(T from) const;

  // Prevent it's shadowing by the above one.
  using HomographicTransformBase<1, T>::operator();
};


template <size_t N, typename T>
HomographicTransform<N, T> HomographicTransformBase<N, T>::inv() const {
  StaticMatrixCalc<T, 4 * (N + 1) * (N + 1), N + 1> mc;
  Mat invMat;
  mc(m_mat, static_cast<int>(N + 1), static_cast<int>(N + 1)).inv().write(invMat);

  return HomographicTransform<N, T>(invMat);
}

template <size_t N, typename T>
typename HomographicTransformBase<N, T>::Vec HomographicTransformBase<N, T>::operator()(const Vec& from) const {
  StaticMatrixCalc<T, N + 1, 1> mc;
  const VecNT<N + 1, T> hsrc(from, T(1));
  VecNT<N + 1, T> hdst;
  (mc(m_mat, static_cast<int>(N + 1), static_cast<int>(N + 1)) * mc(hsrc, static_cast<int>(N + 1), 1)).write(hdst);
  VecNT<N, T> res(&hdst[0]);
  res /= hdst[N];

  return res;
}

template <typename T>
T HomographicTransform<1, T>::operator()(T from) const {
  // Optimized version for 1D case.
  const T* m = this->mat().data();

  return (from * m[0] + m[2]) / (from * m[1] + m[3]);
}

#endif  // ifndef SCANTAILOR_MATH_HOMOGRAPHICTRANSFORM_H_
