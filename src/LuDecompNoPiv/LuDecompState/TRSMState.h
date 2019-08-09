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
// Created by gck2 on 6/28/19.
//

#ifndef MATRIXLIB_TRSMSTATE_H
#define MATRIXLIB_TRSMSTATE_H

#include <hedgehog.h>
#include "../LuDecompData/TRSMData.h"
#include "../../data/MatrixBlockData.h"

template <class Type>
class TRSMState : public AbstractState<TRSMData<Type>, MatrixBlockData<Type, 's'>, MatrixBlockData<Type, 'g'>> {
private:
    std::vector<std::shared_ptr<MatrixBlockData<Type, 'g'>>> permutedBlockContainer_;
    std::vector<std::shared_ptr<MatrixBlockData<Type, 's'>>> standardBlockContainer_;
    size_t currentCount_ = 0;
    size_t numBlocksRows_ = 0;
    size_t numBlocksCols_ = 0;
    size_t finalCount_ = 0;
public:
    TRSMState(size_t numBlocksRows, size_t numBlocksCols) : numBlocksRows_(numBlocksRows),
                                                            numBlocksCols_(numBlocksCols) {
        numBlocksRows_ = numBlocksRows;
        numBlocksCols_ = numBlocksCols;
        standardBlockContainer_.resize(numBlocksCols, nullptr);
        permutedBlockContainer_.resize(numBlocksCols, nullptr);

        /// Set counts
        currentCount_ = 0;

        /// Upper triange portion of the matrix (not including the diagonal, hence subtraction of number of rows).
        finalCount_ = (numBlocksCols * numBlocksCols - numBlocksCols) / 2;

    }

    virtual ~TRSMState() {
        standardBlockContainer_.clear();
        permutedBlockContainer_.clear();
    }

    void execute(std::shared_ptr<MatrixBlockData<Type, 's'>> ptr) override {
        size_t row = ptr->getRowIdx();
        size_t col = ptr->getColIdx();

        /// No need for blocks/panels on the diagonal
        if (row != col) {

            if (standardBlockContainer_[col] == nullptr) {
                standardBlockContainer_[col] = ptr;
            }

            /// Checks if there is permuted block
            if (permutedBlockContainer_[row] != nullptr) {
                this->push(std::make_shared<TRSMData<double>>(permutedBlockContainer_[row], standardBlockContainer_[col]));

                /// Update Count
                currentCount_++;

                /// Prevents the block to be used more than once before re-update.
                standardBlockContainer_[col] = nullptr;
            }
        }

    }

    void execute(std::shared_ptr<MatrixBlockData<Type, 'g'>> ptr) override {
        /// Permuted Block
        size_t row = ptr->getRowIdx();
        size_t col = ptr->getColIdx();

        if (permutedBlockContainer_[col] == nullptr){
            permutedBlockContainer_[col] = ptr;
        }

        /// lower bound for the for loop decreases as the matrix is further decomposed
        for (size_t i = row; i < numBlocksCols_; i++){

            /// Checks if there is a standardBlock for each col
            if(standardBlockContainer_[i] != nullptr){
                this->push(std::make_shared<TRSMData<double>>(permutedBlockContainer_[col], standardBlockContainer_[i]));

                /// Update Count
                currentCount_ ++;
            }

            /// Prevents the block to be used more than once before re-update.
            standardBlockContainer_[i] = nullptr;

        }
    }

    bool finishAccumulating(){
        return currentCount_ == finalCount_;

    }
};
#endif //MATRIXLIB_TRSMSTATE_H
