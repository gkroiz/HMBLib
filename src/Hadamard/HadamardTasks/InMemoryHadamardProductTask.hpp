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
// Created by tjb3 on 5/16/19.
//

#ifndef MATRIXLIB_HADAMARDPRODUCTTASK_HPP
#define MATRIXLIB_HADAMARDPRODUCTTASK_HPP


#include <hedgehog.h>
#include "../../data/HadamardData.hpp"

template <class Type, char OutputId = 'c', Order Ord = Order::Row>
class InMemoryHadamardProductTask : public AbstractTask<MatrixData_Old<Type, OutputId, Ord>, HadamardData<Type, Ord>> {

 public:
  explicit InMemoryHadamardProductTask(std::shared_ptr<Type> resultMatrix, int leadingDimension, size_t numberThreads = 1) :
  AbstractTask<MatrixData_Old<Type, OutputId>, HadamardData<Type>>("HadamardProduct", numberThreads),
  _resultMatrix(resultMatrix), _leadingDimension(leadingDimension)
  { }


  std::shared_ptr<AbstractTask<MatrixData_Old<Type, OutputId, Ord>, HadamardData<Type, Ord>>> copy() override {
    return std::make_shared<InMemoryHadamardProductTask<Type, OutputId, Ord>>(_resultMatrix, _leadingDimension);
  }

  void execute(std::shared_ptr<HadamardData<Type, Ord>> ptr) override {
    auto matrixAData = ptr->getAMatrixData();
    auto matrixBData = ptr->getBMatrixData();

    assert(matrixAData->getMatrixHeight() == matrixBData->getMatrixHeight() && matrixAData->getMatrixWidth() == matrixBData->getMatrixWidth());

    auto aPtr = matrixAData->getMatrixData();
    auto bPtr = matrixBData->getMatrixData();

    auto resultMatrixData = std::make_shared<MatrixData_Old<Type, OutputId>>(matrixAData->getRowIdx(),
                                                                         matrixAData->getColIdx(),
                                                                         _resultMatrix,
                                                                         matrixAData->getMatrixWidth(),
                                                                         matrixAData->getMatrixHeight(),
                                                                         _leadingDimension);

    auto cPtr = resultMatrixData->getMatrixData();

  // TODO: Fix order for Ord type
    for(int i = 0; i < matrixAData->getMatrixHeight(); i++)
    {
      for (int j = 0; j < matrixAData->getMatrixWidth(); j++)
      {
        cPtr[i * resultMatrixData->getLeadingDim() + j] = aPtr[i * matrixAData->getLeadingDim() + j] * bPtr[i * matrixBData->getLeadingDim() + j];
      }
    }


    this->addResult(resultMatrixData);
  }

 private:
  std::shared_ptr<Type> _resultMatrix;
  int _leadingDimension;

};


#endif //MATRIXLIB_HADAMARDPRODUCTTASK_HPP
