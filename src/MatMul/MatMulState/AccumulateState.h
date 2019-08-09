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
// Created by gck2 on 6/17/19.
//

#ifndef MATRIXMULTIPLICATION_ACCUMULATESTATE_H
#define MATRIXMULTIPLICATION_ACCUMULATESTATE_H

#include <hedgehog.h>
#include <data/AccumMatData.h>

template<class Type, char OutputId>
class AccumulateState : public AbstractState<AccumMatData<Type, OutputId>, MatrixBlockData<Type, OutputId>> {

private:
    std::vector<std::shared_ptr<MatrixBlockData<Type, OutputId>>> matMulData_;
    size_t numBlocksACols_ = 0;
    size_t numBlocksBCols_ = 0;
    size_t numBlocksARows_ = 0;
    int finalCount_ = 0;
    int currentCount_ = 0;

public:
    AccumulateState(size_t numBlocksARows, size_t numBlocksBCols, size_t numBlocksACols) :
    numBlocksBCols_(numBlocksBCols),
    numBlocksARows_(numBlocksARows),
    numBlocksACols_(numBlocksACols){

        matMulData_.resize(numBlocksARows * numBlocksBCols, nullptr);
        finalCount_ = (numBlocksARows * numBlocksBCols * numBlocksACols) + (numBlocksARows * numBlocksBCols * (numBlocksACols - 1));
        currentCount_ = 0;
    }

    ~AccumulateState() {

        matMulData_.clear();
    }



    void execute(std::shared_ptr<MatrixBlockData<Type, OutputId>> givenMatrix) {

        size_t row = givenMatrix->getRowIdx();
        size_t col = givenMatrix->getColIdx();

        /// Checks to see if there are two subfinal blocks with same coordinates
        if (matMulData_[row * numBlocksBCols_ + col] != nullptr) {
            this->push(std::make_shared<AccumMatData<Type, OutputId>>(givenMatrix, matMulData_[row * numBlocksBCols_ + col]));
            matMulData_[row * numBlocksBCols_ + col] = nullptr;
        }
        else {
            /// Stores block
            matMulData_[row * numBlocksBCols_ + col] = givenMatrix;
            }
        currentCount_++;
    }

    bool finishAccumulating() {
        return currentCount_ == finalCount_;
    }

};
#endif //MATRIXMULTIPLICATION_ACCUMULATESTATE_H
