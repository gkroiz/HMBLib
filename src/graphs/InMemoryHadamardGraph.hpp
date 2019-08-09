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

#ifndef MATRIXLIB_HADAMARDGRAPH_HPP
#define MATRIXLIB_HADAMARDGRAPH_HPP

#include <hedgehog.h>
#include <cmath>

#include "../Hadamard/HadamardState/HadamardState.hpp"
#include "../Hadamard/HadamardTasks/InMemoryHadamardProductTask.hpp"
template <class Type, char OutputId = 'c', Order Ord = Order::Row>
class InMemoryHadamardGraph : public Graph<MatrixData_Old<Type, OutputId, Ord>, MatrixData_Old<Type, 'a', Ord>, MatrixData_Old<Type, 'b', Ord>> {
 public:
  InMemoryHadamardGraph(std::shared_ptr<Type> resultMatrix, int resultMatrixLeadingDimension, int matrixWidth, int matrixHeight, int blockSize, int numThreadsHad = 1) :
  Graph<MatrixData_Old<Type, OutputId, Ord>, MatrixData_Old<Type, 'a', Ord>, MatrixData_Old<Type, 'b', Ord>>("HadamardProduct"),
  _resultMatrix(resultMatrix), _matrixWidth(matrixWidth), _matrixHeight(matrixHeight), _blockSize(blockSize), _resultMatrixLeadingDimension(resultMatrixLeadingDimension)
  {
    _numRows = static_cast<size_t>(ceil(static_cast<double>(_matrixHeight) / _blockSize));
    _numCols = static_cast<size_t>(ceil(static_cast<double>(_matrixWidth) / _blockSize));


    _hadState = std::make_shared<HadamardState<Type, Ord>>(_numCols, _numRows);
    _hadTask = std::make_shared<InMemoryHadamardProductTask<Type, OutputId, Ord>>(_resultMatrix, resultMatrixLeadingDimension, numThreadsHad);


    this->_hadStateManager = std::make_shared<DefaultStateManager<HadamardData<Type, Ord>, MatrixData_Old<Type, 'a', Ord>, MatrixData_Old<Type, 'b', Ord>>>(_hadState);

    this->input(this->_hadStateManager);
    this->addEdge(this->_hadStateManager, _hadTask);
    this->output(_hadTask);
  }



 private:

  std::shared_ptr<DefaultStateManager<HadamardData<Type, Ord>, MatrixData_Old<Type, 'a', Ord>, MatrixData_Old<Type, 'b', Ord>>> _hadStateManager;
  std::shared_ptr<HadamardState<Type, Ord>> _hadState;
  std::shared_ptr<InMemoryHadamardProductTask<Type, OutputId, Ord>> _hadTask;


  std::shared_ptr<Type> _resultMatrix;
  int _matrixWidth;
  int _matrixHeight;
  int _blockSize;

  int _resultMatrixLeadingDimension;

  size_t _numRows;
  size_t _numCols;


};


#endif //MATRIXLIB_HADAMARDGRAPH_HPP
