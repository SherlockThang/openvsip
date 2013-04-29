//
// Copyright (c) 2005, 2006, 2008 by CodeSourcery
// Copyright (c) 2013 Stefan Seefeld
// All rights reserved.
//
// This file is part of OpenVSIP. It is made available under the
// license contained in the accompanying LICENSE.GPL file.

#ifndef ILLEGALCASE
#  define ILLEGALCASE 0
#endif

/***********************************************************************
  Included Files
***********************************************************************/

#include <iostream>
#include <cassert>

#include <vsip/initfin.hpp>
#include <vsip/support.hpp>
#include <vsip/vector.hpp>
#include <vsip_csl/test.hpp>

using namespace std;
using namespace vsip;
using namespace vsip_csl;


template <typename T,
	  typename Block>
inline
std::ostream&
operator<<(
  std::ostream&		       out,
  vsip::const_Vector<T, Block> vec)
  VSIP_NOTHROW
{
  for (vsip::index_type i=0; i<vec.size(); ++i)
    out << "  " << i << ": " << vec.get(i) << "\n";
  return out;
}

/***********************************************************************
  Definitions - Utility Functions
***********************************************************************/

// check_length -- check the reported length of a vector against
//                 expected length

template <typename T,
	  typename Block>
void
check_length(const_Vector<T, Block> vec, length_type len)
{
  test_assert(vec.length() == len);
  test_assert(vec.size() == len);
  test_assert(vec.size(0) == len);
}



// fill_vector -- fill a vector with a sequence of values.
//
// Values are generated with a slope of K.

template <typename T,
	  typename Block>
void
fill_vector(Vector<T, Block> vec, int k)
{
  for (index_type i=0; i<vec.size(0); ++i)
    vec.put(i, T(k*i+1));
}



// fill_block -- fill a block with sequence of values.
//
// Values are generated with a slope of K.

template <typename Block>
void
fill_block(Block& blk, int k)
{
  typedef typename Block::value_type T;

  for (index_type i=0; i<blk.size(1, 0); ++i)
    blk.put(i, T(k*i+1));
}



// test_vector -- test values in vector against sequence.
//
// Asserts that vector values match those generated by a call to
// fill_vector or fill_block with the same k value.

template <typename T,
	  typename Block>
void
test_vector(const_Vector<T, Block> vec, int k)
{
  for (index_type i=0; i<vec.size(0); ++i)
    test_assert(equal(vec.get(i), T(k*i+1)));
}



// check_vector -- check values in vector against sequence.
//
// Checks that vector values match those generated by a call to
// fill_vector or fill_block with the same k value.  Rather than
// triggering test_assertion failure, check_vector returns a boolean
// pass/fail that can be used to cause an test_assertion failure in
// the caller.

template <typename T,
	  typename Block>
bool
check_vector(const_Vector<T, Block> vec, int k)
{
  for (index_type i=0; i<vec.size(0); ++i)
    if (!equal(vec.get(i), T(k*i+1)))
      return false;
  return true;
}



// check_not_alias -- check that two views are not aliased.
//
// Changes made to one should not effect the other.

template <template <typename, typename> class View1,
	  template <typename, typename> class View2,
	  typename T1,
	  typename T2,
	  typename Block1,
	  typename Block2>
void
check_not_alias(
  View1<T1, Block1>& vec1,
  View2<T2, Block2>& vec2)
{
  fill_block(vec1.block(), 2);
  fill_block(vec2.block(), 3);

  // Make sure that updates to vec2 do not affect vec1.
  test_assert(check_vector(vec1, 2));

  // And visa-versa.
  fill_block(vec1.block(), 4);
  test_assert(check_vector(vec2, 3));
}



// check_alias -- check that two views are aliased.
//
// Changes made to one should effect the other.

template <template <typename, typename> class View1,
	  template <typename, typename> class View2,
	  typename T1,
	  typename T2,
	  typename Block1,
	  typename Block2>
void
check_alias(
  View1<T1, Block1>& vec1,
  View2<T2, Block2>& vec2)
{
  fill_block(vec1.block(), 2);
  test_assert(check_vector(vec1, 2));
  test_assert(check_vector(vec2, 2));

  fill_block(vec2.block(), 3);
  test_assert(check_vector(vec1, 3));
  test_assert(check_vector(vec2, 3));
}



/***********************************************************************
  Definitions - Vector Test Cases.
***********************************************************************/

// -------------------------------------------------------------------- //
// test cases for get/put.

// tc_get -- test a vector's get() member function.

template <typename                            T,
	  template <typename, typename> class View>
void
tc_get(length_type len)
{
  View<T, Dense<1, T> > vec(len);

  check_length(vec, len);
  fill_block(vec.block(), 2);
  test_vector(vec, 2);
}



template <typename T>
void
test_get_type(length_type len)
{
  tc_get<T,       Vector>(len);
  tc_get<T, const_Vector>(len);
}



void test_get(length_type len)
{
  test_get_type<scalar_f>(len);
  test_get_type<cscalar_f>(len);
  test_get_type<int>(len);
  // test_get_type<short>(len);
}



// -------------------------------------------------------------------- //
// test cases for get/put.

// tc_getput -- test a vector's get() and put() member functions.

template <typename                            T,
	  template <typename, typename> class View>
void
tc_getput(length_type len)
{
  View<T, Dense<1, T> > vec(len);

  check_length(vec, len);
  fill_vector(vec, 2);
  test_vector(vec, 2);
}



template <typename T>
void
test_getput_type(length_type len)
{
  tc_getput<T,       Vector>(len);
#if (ILLEGALCASE == 1)
  // const_Vector should not provide put()
  tc_getput<T, const_Vector>(len);
#endif
}



void test_getput(length_type len)
{
  test_getput_type<scalar_f>(len);
  test_getput_type<cscalar_f>(len);
  test_getput_type<int>(len);
  // test_getput_type<short>(len);
}



// -------------------------------------------------------------------- //
// test cases for copy constructor.

template <typename T1,
	  typename T2,
	  template <typename, typename> class View1,
	  template <typename, typename> class View2>
void
tc_copy_cons(length_type len, int k)
{
  View1<T1, Dense<1, T1> > vec1(len);

  fill_block(vec1.block(), k);

  View2<T2, Dense<1, T2> > vec2(vec1);

  test_vector(vec2, k);

  check_alias(vec1, vec2);
}



template <typename T1,
	  typename T2>
void
test_copy_cons_type(length_type len, int k)
{
  tc_copy_cons<T1, T2,       Vector,       Vector>(len, k);
  tc_copy_cons<T1, T2,       Vector, const_Vector>(len, k);
  tc_copy_cons<T1, T2, const_Vector, const_Vector>(len, k);

#if (ILLEGALCASE == 2)
  // It should be illegal to construct a Vector from a const_Vector.
  tc_copy_cons<T1, T2, const_Vector,       Vector>(len, k);
#endif
}



void
test_copy_cons(length_type len, int k)
{
  test_copy_cons_type<float, float>(len, k);
  // tc_copy_cons_set<  int,   int>(len, k);
}



// -------------------------------------------------------------------- //
// test cases for assignment

template <typename T1,
	  typename T2,
	  template <typename, typename> class View1,
	  template <typename, typename> class View2>
void
tc_assign(length_type len, int k)
{
  View1<T1, Dense<1, T1> > vec1(len);
  View2<T2, Dense<1, T2> > vec2(len);

  fill_block(vec1.block(), k);

  vec2 = vec1;

  test_assert(check_vector(vec2, k));

  check_not_alias(vec1, vec2);
}



template <typename T1,
	  typename T2>
void
test_assign_type(length_type len, int k)
{
  tc_assign<T1, T2, Vector,       Vector>(len, k);
  tc_assign<T1, T2, const_Vector, Vector>(len, k);

#if (ILLEGALCASE == 3)
  tc_assign<T1, T2, Vector,       const_Vector>(len, k);
#endif
#if (ILLEGALCASE == 4)
  tc_assign<T1, T2, const_Vector, const_Vector>(len, k);
#endif
}



void
test_assign(length_type len, int k)
{
  test_assign_type<float, float>(len, k);
}



// -------------------------------------------------------------------- //
// test cases for funcall

// tc_sum_const -- sum values in a const_Vector.

template <typename T,
	  typename Block>
T
tc_sum_const(const_Vector<T, Block> vec)
{
  T sumval = T();
  for (index_type i=0; i<vec.size(0); ++i)
    sumval += vec.get(i);
  return sumval;
}



// tc_sum -- sum values in a Vector.

template <typename T,
	  typename Block>
T
tc_sum(Vector<T, Block> vec)
{
  T sumval = T();
  for (index_type i=0; i<vec.size(0); ++i)
    sumval += vec.get(i);
  return sumval;
}



// tc_call_sum_const -- create View, pass it to tc_sum_const,
//                      check result.
template <typename T,
	  template <typename, typename> class View>
void
tc_call_sum_const(length_type len, int k)
{
  View<T, Dense<1, T> > vec1(len);

  fill_block(vec1.block(), k);
  T sum = tc_sum_const(vec1);

  test_assert(equal(sum, T(k*len*(len-1)/2+len)));
}



// tc_call_sum -- create View, pass it to tc_sum, check result.

template <typename T,
	  template <typename, typename> class View>
void
tc_call_sum(length_type len, int k)
{
  View<T, Dense<1, T> > vec1(len);

  fill_block(vec1.block(), k);
  T sum = tc_sum(vec1);

  test_assert(equal(sum, T(k*len*(len-1)/2+len)));
}



// test_call -- test that Vector and const_Vector can be passed
//              as arguments to functions.

void
test_call(length_type len, int k)
{
  tc_call_sum_const<float, const_Vector>(len, k);
  tc_call_sum_const<float,       Vector>(len, k);

#if (ILLEGALCASE == 5)
  // should not be able to pass a const_Vector to a routine
  // expecting a Vector.
  tc_call_sum<float, const_Vector>(len, k);
#endif
  tc_call_sum<float,       Vector>(len, k);
}



// -------------------------------------------------------------------- //
// test cases for returning a view from a function.

// Generate a view/block of length LEN, with element 0 initialized
// to VAL.

template <template <typename, typename> class View,
	  typename			      T,
	  typename			      Block>
View<T, Block>
return_view(length_type len, T val)
{
  Block* blk = new Block(Domain<1>(len));
  blk->put(0, val);
  View<T, Block> vec(*blk);
  blk->decrement_count();

  return vec;
}



// Assign a view from the value of a function returning a view.

template <typename T,
	  template <typename, typename> class View1,
	  template <typename, typename> class View2>
void
tc_assign_return(length_type len, T val)
{
  typedef Dense<1, T> block_type;
  View1<T, block_type> vec1(len, T());

  test_assert(vec1.get(0) != val || val == T());

  vec1 = return_view<View2, T, block_type>(len, val);

  test_assert(vec1.get(0) == val);
}



// Construct a view from the value of a function returning a view.

template <typename T,
	  template <typename, typename> class View1,
	  template <typename, typename> class View2>
void
tc_cons_return(length_type len, T val)
{
  typedef Dense<1, T> block_type;

  View1<T, block_type> vec1(return_view<View2, T, block_type>(len, val));

  test_assert(vec1.get(0) == val);
}



void
test_return()
{
  tc_assign_return<float,       Vector,       Vector>(10, 1.0);
  tc_assign_return<float,       Vector, const_Vector>(10, 1.0);

  // These cases are illegal: you cannot assign to a const_Vector
#if ILLEGALCASE == 6
  tc_assign_return<float, const_Vector, const_Vector>(10, 1.0);
#endif
#if ILLEGALCASE == 7
  tc_assign_return<float, const_Vector,       Vector>(10, 1.0);
#endif

  tc_cons_return<float,       Vector,       Vector>(10, 1.0);
  tc_cons_return<float,       Vector, const_Vector>(10, 1.0);
  tc_cons_return<float, const_Vector,       Vector>(10, 1.0);
  tc_cons_return<float, const_Vector, const_Vector>(10, 1.0);
}


template <typename T>
void
tc_subview(Domain<1> const& dom, Domain<1> const& sub)
{
  Vector<T> view(dom[0].length());

  fill_vector(view, 2);

  typename Vector<T>::subview_type        subv = view(sub);
  typename Vector<T>::const_subview_type csubv = view.get(sub);

  for (index_type i=0; i<sub.length(); ++i)
  {
    index_type parent_i = sub.impl_nth(i);

    test_assert(view.get(parent_i) ==  subv.get(i));
    test_assert(view.get(parent_i) == csubv.get(i));

    view.put(parent_i, view.get(parent_i) + T(1));

    test_assert(view.get(parent_i) ==  subv.get(i));
    test_assert(view.get(parent_i) == csubv.get(i));

    subv.put(i, subv.get(i) + T(1));

    test_assert(view.get(parent_i) ==  subv.get(i));
    test_assert(view.get(parent_i) == csubv.get(i));
  }
}



void
test_subview()
{
  tc_subview<float>(Domain<1>(10), Domain<1>(0, 1, 3));
  tc_subview<float>(Domain<1>(10), Domain<1>(5, 1, 3));
  tc_subview<float>(Domain<1>(10), Domain<1>(0, 2, 3));
  tc_subview<float>(Domain<1>(10), Domain<1>(5, 2, 3));

  tc_subview<complex<float> >(Domain<1>(10), Domain<1>(0, 1, 3));
  tc_subview<complex<float> >(Domain<1>(10), Domain<1>(5, 1, 3));
  tc_subview<complex<float> >(Domain<1>(10), Domain<1>(0, 2, 3));
  tc_subview<complex<float> >(Domain<1>(10), Domain<1>(5, 2, 3));

  tc_subview<int>(Domain<1>(10), Domain<1>(0, 1, 3));
  tc_subview<int>(Domain<1>(10), Domain<1>(5, 1, 3));
  tc_subview<int>(Domain<1>(10), Domain<1>(0, 2, 3));
  tc_subview<int>(Domain<1>(10), Domain<1>(5, 2, 3));
}

void
test_complex()
{
  typedef Vector<std::complex<double> > CVector;
  CVector cv(2, 3.);
  CVector::realview_type rv = cv.real();
  CVector::imagview_type iv = cv.imag();
  CVector::const_realview_type crv = const_cast<CVector const&>(cv).real();
  CVector::const_imagview_type civ = const_cast<CVector const&>(cv).imag();
  cv.put(0, 5.);
  cv.put(1, 5.);
  test_assert(equal(10., tc_sum(rv)));
  test_assert(equal(0., tc_sum(iv)));
  test_assert(equal(10., tc_sum_const(crv)));
  test_assert(equal(0., tc_sum_const(civ)));
  rv.put(0, 0.);
  rv.put(1, 0.);
  iv.put(0, 5.);
  iv.put(1, 5.);
  test_assert(equal(0., tc_sum(rv)));
  test_assert(equal(10., tc_sum(iv)));
  test_assert(equal(0., tc_sum_const(crv)));
  test_assert(equal(10., tc_sum_const(civ)));
}

void
test_const_complex()
{
  typedef const_Vector<std::complex<double> > CVector;
  CVector cv(2, 5.);
  CVector::const_realview_type crv = cv.real();
  CVector::const_imagview_type civ = cv.imag();
  test_assert(equal(10., tc_sum_const(crv)));
  test_assert(equal(0., tc_sum_const(civ)));
}

#define VSIP_TEST_ELEMENTWISE_SCALAR(x, op, y) \
{                                              \
  Vector<int> v(1, x);                         \
  Vector<int> &v1 = (v op y);		       \
  int r = x;                                   \
  r op y;                                      \
  test_assert(&v1 == &v && equal(v1.get(0), r));    \
}

#define VSIP_TEST_ELEMENTWISE_VECTOR(x, op, y) \
{                                              \
  Vector<int> v(1, x);                         \
  Vector<int> w(1, y);                         \
  Vector<int> &v1 = (v op w);		       \
  int r = x;                                   \
  r op y;                                      \
  test_assert(&v1 == &v && equal(v1.get(0), r));    \
}

int
main(int argc, char** argv)
{
  vsip::vsipl init(argc, argv);

  test_get(10);
  test_getput(10);

  test_copy_cons(10, 3);
  test_assign(10, 3);

  test_call(10, 3);

  test_subview();

  test_complex();
  test_const_complex();

  VSIP_TEST_ELEMENTWISE_SCALAR(4, +=, 2)
  VSIP_TEST_ELEMENTWISE_SCALAR(4, -=, 2)
  VSIP_TEST_ELEMENTWISE_SCALAR(4, *=, 2)
  VSIP_TEST_ELEMENTWISE_SCALAR(4, /=, 2)
  VSIP_TEST_ELEMENTWISE_SCALAR(4, &=, 3)
  VSIP_TEST_ELEMENTWISE_SCALAR(4, &=, 2)
  VSIP_TEST_ELEMENTWISE_SCALAR(4, |=, 3)
  VSIP_TEST_ELEMENTWISE_SCALAR(4, |=, 2)
  VSIP_TEST_ELEMENTWISE_SCALAR(4, ^=, 3)
  VSIP_TEST_ELEMENTWISE_SCALAR(4, ^=, 2)

  VSIP_TEST_ELEMENTWISE_VECTOR(4, +=, 2)
  VSIP_TEST_ELEMENTWISE_VECTOR(4, -=, 2)
  VSIP_TEST_ELEMENTWISE_VECTOR(4, *=, 2)
  VSIP_TEST_ELEMENTWISE_VECTOR(4, /=, 2)
  VSIP_TEST_ELEMENTWISE_VECTOR(4, &=, 3)
  VSIP_TEST_ELEMENTWISE_VECTOR(4, &=, 2)
  VSIP_TEST_ELEMENTWISE_VECTOR(4, |=, 3)
  VSIP_TEST_ELEMENTWISE_VECTOR(4, |=, 2)
  VSIP_TEST_ELEMENTWISE_VECTOR(4, ^=, 3)
  VSIP_TEST_ELEMENTWISE_VECTOR(4, ^=, 2)

  // operator!
  {
    Vector<bool> v(1, true);
    Vector<bool>  w = !v;
    test_assert(w.get(0) == false);
  }
  // operator~
  {
    Vector<int> v(1, 3);
    Vector<int>  w = ~v;
    test_assert(w.get(0) == ~3);
  }
}
