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
// Created by gck2 on 7/10/19.
//

#ifndef MATRIXLIB_OUTPUTSTATE_H
#define MATRIXLIB_OUTPUTSTATE_H

#include <hedgehog.h>
#include "LuDecompPartPiv/LuDecompPartPivData/PermutedBlockData.h"
#include "data/MatrixBlockData.h"

template<class Type>
class OutputState
        : public AbstractState<PermutedBlockData<Type>, MatrixBlockData<Type, 's'>, MatrixBlockData<Type, 'l'>, MatrixBlockData<Type, 'f'>, PermutedBlockData<Type>> {
    std::vector<int> leftSideBlockCount_;
    size_t numBlocksRows_ = 0;
    size_t numBlocksCols_ = 0;
    size_t outputCount = 0;
public:
    OutputState(size_t numBlocksRows, size_t numBlocksCols) : numBlocksRows_(numBlocksRows),
                                                              numBlocksCols_(numBlocksCols) {
        numBlocksRows_ = numBlocksRows;
        numBlocksCols_ = numBlocksCols;
        leftSideBlockCount_.resize(numBlocksCols, 0);
    }

    virtual ~OutputState() {
        leftSideBlockCount_.clear();
    }

    void execute(std::shared_ptr<MatrixBlockData<Type, 's'>> ptr) override {

    }

    /// Blocks above the diagonal
    void execute(std::shared_ptr<MatrixBlockData<Type, 'f'>> ptr) override {
        size_t row = ptr->getRowIdx();
        size_t col = ptr->getColIdx();

        size_t blockWidth = ptr->getBlockSizeWidth();
        size_t ld = ptr->getLeadingDimension();
        if (row != col) {

            /// Change pointer to point to block
            auto blockPointer = &(ptr->getBlockData()[row * blockWidth * ld]);
            this->push(std::make_shared<PermutedBlockData<Type>>(
                    std::make_shared<MatrixBlockData<Type, 's'>>(row, col, blockWidth,
                                                                 blockWidth, ld, ptr->getFullMatrixData(),
                                                                 blockPointer),
                    nullptr));
            outputCount++;
        }
    }

    /// Blocks below the diagonal
    void execute(std::shared_ptr<MatrixBlockData<Type, 'l'>> ptr) override {
        size_t row = ptr->getRowIdx();
        size_t col = ptr->getRowIdx();
        size_t blockWidth = ptr->getBlockSizeWidth();
        size_t ld = ptr->getLeadingDimension();

        leftSideBlockCount_[col]++;

        /// Points to block based on count
        auto blockPointer = &(ptr->getBlockData()[(leftSideBlockCount_[col] + col) * blockWidth * ld]);
        this->push(std::make_shared<PermutedBlockData<Type>>(
                std::make_shared<MatrixBlockData<Type, 's'>>(leftSideBlockCount_[col] + col, col, blockWidth,
                                                             blockWidth, ld, ptr->getFullMatrixData(), blockPointer),
                nullptr));
        outputCount++;
    }

    ///Blocks on the diagonal
    void execute(std::shared_ptr<PermutedBlockData<Type>> ptr) override {
        auto permutedBlockData = ptr->getPermutedBlock();
        size_t row = permutedBlockData->getRowIdx();
        size_t col = permutedBlockData->getRowIdx();


        size_t blockWidth = permutedBlockData->getBlockSizeWidth();
        size_t bld = permutedBlockData->getLeadingDimension();

        /// Change pointer to point to block
        double *blockPointer = &(permutedBlockData->getBlockData()[row * blockWidth * bld]);
        this->push(std::make_shared<PermutedBlockData<Type>>(
                std::make_shared<MatrixBlockData<Type, 's'>>(row, col, blockWidth, blockWidth, bld,
                                                             permutedBlockData->getFullMatrixData(), blockPointer),
                ptr->getPermutationVector()));
        outputCount++;
    }
};

#endif //MATRIXLIB_OUTPUTSTATE_H
