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

#ifndef MATRIXMULTIPLICATION_MULBLOCKSTATE_H
#define MATRIXMULTIPLICATION_MULBLOCKSTATE_H

#include <hedgehog.h>
#include <data/MatrixBlockData.h>
#include <MatMul/MatMulData/MatrixMulData.hpp>

template <class Type>
class MulBlockState : public AbstractState<MatrixMulData<Type>, MatrixBlockData<Type, 'a'>, MatrixBlockData<Type, 'b'>>{
private:
    std::vector<std::shared_ptr<MatrixBlockData<Type, 'a'>>>  aBlockContainer_;
    std::vector<std::shared_ptr<MatrixBlockData<Type, 'b'>>> bBlockContainer_;
    size_t numBlocksARows_ = 0;
    size_t numBlocksACols_ = 0;
    size_t numBlocksBRows_ = 0;
    size_t numBlocksBCols_ = 0;

public:
    MulBlockState(size_t numBlocksARows, size_t numBlocksACols, size_t numBlocksBRows, size_t numBlocksBCols)
            : numBlocksARows_(numBlocksARows), numBlocksACols_(numBlocksACols), numBlocksBRows_(numBlocksBRows),
              numBlocksBCols_(numBlocksBCols) {

        aBlockContainer_.resize(numBlocksARows * numBlocksACols, nullptr);
        bBlockContainer_.resize(numBlocksBRows * numBlocksBCols, nullptr);

        numBlocksARows_ = numBlocksARows;
        numBlocksACols_ = numBlocksACols;
        numBlocksBRows_ = numBlocksBRows;
        numBlocksBCols_ = numBlocksBCols;
    }

    ~MulBlockState() {
        aBlockContainer_.clear();
        bBlockContainer_.clear();
    }

    void execute(std::shared_ptr<MatrixBlockData<Type, 'a'>> ptr) {
        std::shared_ptr<MatrixBlockData<Type, 'b'>> matrixB = nullptr;

        size_t rowA = ptr->getRowIdx();
        size_t colA = ptr->getColIdx();

        size_t rowB = colA;

        /// Stores ptr
        aBlockContainer_[rowA * numBlocksACols_ + colA] = ptr;

        /// For the col
        for (size_t colB = 0; colB < numBlocksBCols_; ++colB) {
            /// Checks if there is a blockB stored
            if (bBlockContainer_[rowB * numBlocksBCols_ + colB] != nullptr){
                size_t colC= colB;
                this->push(std::make_shared<MatrixMulData<Type>>(ptr, bBlockContainer_[rowB * numBlocksBCols_ + colB], nullptr));

            }
        }
    }

    void execute(std::shared_ptr<MatrixBlockData<Type, 'b'>> ptr) {
        std::shared_ptr<MatrixBlockData<Type, 'a'>> matrixA = nullptr;

        size_t rowB = ptr->getRowIdx();
        size_t colB = ptr->getColIdx();
        size_t colA = rowB;

        /// Stores ptr
        bBlockContainer_[rowB * numBlocksBCols_ +colB] = ptr;

        /// For the row
        for (size_t rowA = 0; rowA < numBlocksARows_; ++rowA) {
            /// Checks if there is a blockA stored
            if (aBlockContainer_[rowA * numBlocksACols_ + colA] != nullptr){
                this->push(std::make_shared<MatrixMulData<Type>>(aBlockContainer_[rowA * numBlocksACols_ + colA], ptr, nullptr));
            }
        }
    }

};
#endif //MATRIXMULTIPLICATION_MULBLOCKSTATE_H
