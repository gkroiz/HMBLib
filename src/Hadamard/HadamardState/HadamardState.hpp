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

#ifndef MATRIXLIB_HADAMARDSTATE_HPP
#define MATRIXLIB_HADAMARDSTATE_HPP

#include <hedgehog.h>
#include "../../data/HadamardData.hpp"

template <class Type, Order Ord = Order::Row>
class HadamardState : public AbstractState<HadamardData<Type, Ord>, MatrixData_Old<Type, 'a', Ord>, MatrixData_Old<Type, 'b', Ord>> {

 public:
  HadamardState(size_t numRows, size_t numCols) : _numRows(numRows), _numCols(numCols){
    _matrixAs.resize(numRows * numCols, nullptr);
    _matrixBs.resize(numRows * numCols, nullptr);
  }

  void execute(std::shared_ptr<MatrixData_Old<Type, 'a', Ord>> ptr) override {
    _matrixAs[ptr->getRowIdx() * _numCols + ptr->getColIdx()] = ptr;

    if (_matrixBs[ptr->getRowIdx() * _numCols + ptr->getColIdx()] != nullptr)
    {
      this->push(std::make_shared<HadamardData<Type, Ord>>(ptr, _matrixBs[ptr->getRowIdx() * _numCols + ptr->getColIdx()]));
    }
  }

  void execute(std::shared_ptr<MatrixData_Old<Type, 'b', Ord>> ptr) override {
    _matrixBs[ptr->getRowIdx() * _numCols + ptr->getColIdx()] = ptr;

    if (_matrixAs[ptr->getRowIdx() * _numCols + ptr->getColIdx()] != nullptr)
    {
      this->push(std::make_shared<HadamardData<Type, Ord>>(_matrixAs[ptr->getRowIdx() * _numCols + ptr->getColIdx()], ptr));
    }
  }

 private:
  std::vector<std::shared_ptr<MatrixData_Old<Type, 'a', Ord>>> _matrixAs;
  std::vector<std::shared_ptr<MatrixData_Old<Type, 'b', Ord>>> _matrixBs;

  size_t _numRows;
  size_t _numCols;

};



#endif //MATRIXLIB_HADAMARDSTATE_HPP
