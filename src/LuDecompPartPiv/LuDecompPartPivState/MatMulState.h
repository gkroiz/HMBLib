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

#ifndef MATRIXLIB_MATMULSTATE_H
#define MATRIXLIB_MATMULSTATE_H

#include <hedgehog.h>
#include "LuDecompPartPiv/LuDecompPartPivData/PermutedBlockData.h"
#include "LuDecompPartPiv/LuDecompPartPivData/UpdateData.h"


template <class Type>
class MatMulState : public AbstractState<UpdateData<Type>, PermutedBlockData<Type>, MatrixBlockData<Type, 'f'>> {
private:
    std::vector<std::shared_ptr<PermutedBlockData<Type>>> permutedBlockContainer_;
    std::vector<std::shared_ptr<MatrixBlockData<Type, 'f'>>> factoredBlockContainer_;
    size_t numBlocksRows_ = 0;
    size_t numBlocksCols_ = 0;
public:
    MatMulState(size_t numBlocksRows, size_t numBlocksCols) : numBlocksRows_(numBlocksRows),
                                                              numBlocksCols_(numBlocksCols) {
        numBlocksRows_ = numBlocksRows;
        numBlocksCols_ = numBlocksCols;
        factoredBlockContainer_.resize(numBlocksCols, nullptr);
        /// Only works for square matrices
        permutedBlockContainer_.resize(numBlocksCols * numBlocksCols, nullptr);

    }

    virtual ~MatMulState() {
        factoredBlockContainer_.clear();
        permutedBlockContainer_.clear();
    }


    void execute(std::shared_ptr<PermutedBlockData<Type>> ptr) override {
        size_t row = ptr->getPermutedBlock()->getRowIdx();
        size_t col = ptr->getPermutedBlock()->getColIdx();

//        PRINT("MatMulState P Block | Row: " + std::to_string(row) + ", Col: " + std::to_string(col) + "\n");

        /// Divides panel into blocks
        if(permutedBlockContainer_[col] == nullptr){
                permutedBlockContainer_[col] = ptr;
        }

        /// Checks every column of factored blocks
        for (size_t i = row; i < numBlocksCols_; i++){
            if(factoredBlockContainer_[i] != nullptr) {

                /// Send Panel
//                this->push(std::make_shared<UpdateData<Type>>(permutedBlockContainer_[col], factoredBlockContainer_[i], row + 1));

                /// Send all permuted blocks
                for (size_t j = row + 1; j < numBlocksCols_; j++) {
                    this->push(std::make_shared<UpdateData<double>>(permutedBlockContainer_[col],
                                                                    factoredBlockContainer_[i], j));
                }
                /// Prevents the block to be used more than once before re-update.
                factoredBlockContainer_[i] = nullptr;
            }
        }

    }


    void execute(std::shared_ptr<MatrixBlockData<Type, 'f'>> ptr) override {
        /// All ptrs should be updated correctly, no count should be needed

        size_t row = ptr->getRowIdx();
        size_t col = ptr->getColIdx();

//        PRINT("MatMulState F Block | Row: " + std::to_string(row) + ", Col: " + std::to_string(col) + "\n");


        if (factoredBlockContainer_[col] == nullptr) {
            factoredBlockContainer_[col] = ptr;

            /// Checks if there is permuted block in each row
            if (permutedBlockContainer_[row] != nullptr) {

                /// Send panel
//                this->push(std::make_shared<UpdateData<double>>(permutedBlockContainer_[row], factoredBlockContainer_[col], row + 1));

                /// Send Blocks
                for (size_t i = row + 1; i < numBlocksCols_; i++) {
                    this->push(std::make_shared<UpdateData<double>>(permutedBlockContainer_[row],
                                                                    factoredBlockContainer_[col], i));
                }
                /// Prevents the block to be used more than once before re-update.
               factoredBlockContainer_[col] = nullptr;
            }
        }
    }
};
#endif //MATRIXLIB_MATMULSTATE_H
