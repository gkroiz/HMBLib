// NIST-developed software is provided by NIST as a public service. You may use, copy and distribute copies of the
// software in any medium, provided that you keep intact this entire notice. You may improve, modify and create
// derivative works of the software or any portion of the software, and you may copy and distribute such modifications
// or works. Modified works should carry a notice stating that you changed the software and should note the date and
// nature of any such change. Please explicitly acknowledge the National Institute of Standards and Technology as the
// source of the software. NIST-developed software is expressly provided "AS IS." NIST MAKES NO WARRANTY OF ANY KIND,
// EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
// WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE
// CORRECTED. NIST DOES NOT WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE SOFTWARE OR THE RESULTS
// THEREOF, INCLUDING BUT NOT LIMITED TO THE CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE. You
// are solely responsible for determining the appropriateness of using and distributing the software and you assume
// all risks associated with its use, including but not limited to the risks and costs of program errors, compliance
// with applicable laws, damage to or loss of data, programs or equipment, and the unavailability or interruption of 
// operation. This software is not intended to be used in any situation where a failure could cause risk of injury or
// damage to property. The software developed by NIST employees is not subject to copyright protection within the
// United States.
//
// Created by tjb3 on 4/29/19.
//

#ifndef MATRIXLIB_MATMUL_HPP
#define MATRIXLIB_MATMUL_HPP


#include <cassert>
#include <memory>
#include <iostream>
#include "../MatMul/MatMulData/MatrixMulData.hpp"

void computeMatMulDouble(int M, int N, int K, double alpha, double *A, int LDA, double *B, int LDB, double beta, double *C, int LDC, bool colMajor);
void computeMatMulFloat(int M, int N, int K, float alpha, float *A, int LDA, float *B, int LDB, float beta, float *C, int LDC, bool colMajor);


template <class Type>
void computeMatMulWrapper(std::shared_ptr<MatrixMulData<Type>> matrices, Type alpha, Type beta, bool colMajor) {
  auto matrixA = matrices->getMatrixA();
  auto matrixB = matrices->getMatrixB();
  auto matrixC = matrices->getMatrixC();

  int M = matrixA->getMatrixHeight();
  int N = matrixB->getMatrixWidth();
  int K = matrixA->getMatrixWidth();

  assert(matrixA->getMatrixWidth() == matrixB->getMatrixHeight());

  if (std::is_same<Type, double>::value)
  {
    computeMatMulDouble(M, N, K, alpha, (double *)matrixA->getMatrixData(), matrixA->getLeadingDim(),
                  (double *)matrixB->getMatrixData(), matrixB->getLeadingDim(),
                  beta,
                  (double *)matrixC->getMatrixData(), matrixC->getLeadingDim(),
                  colMajor);
  } else if (std::is_same<Type, float>::value)
  {
    computeMatMulFloat(M, N, K, alpha, (float *)matrixA->getMatrixData(), matrixA->getLeadingDim(),
                       (float *)matrixB->getMatrixData(), matrixB->getLeadingDim(),
                        beta,
                       (float *)matrixC->getMatrixData(), matrixC->getLeadingDim(),
                        colMajor);
  } else
  {
    std::cerr<< "Matrix type for matrix multiplication must be either float or double" << std::endl;
    exit(42);
  }




}


#endif //MATRIXLIB_MATMUL_HPP
