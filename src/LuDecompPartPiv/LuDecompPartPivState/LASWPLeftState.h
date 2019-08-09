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
// Created by gck2 on 7/16/19.
//

#ifndef MATRIXLIB_LASWPLEFTSTATE_H
#define MATRIXLIB_LASWPLEFTSTATE_H

#include <hedgehog.h>
#include "LuDecompPartPiv/LuDecompPartPivData/LASWPData.h"
#include "LuDecompPartPiv/LuDecompPartPivData/IPivPermutedBlockData.h"
#include "data/MatrixBlockData.h"

template<class Type>
class LASWPLeftState
        : public AbstractState<IPivPermutedBlockData<Type>, PermutedBlockData<Type>, MatrixBlockData<Type, 's'>, MatrixBlockData<Type, 'l'>> {
private:
    std::vector<std::shared_ptr<MatrixBlockData<Type, 's'>>> permutedBlockContainer_;
    std::vector<std::shared_ptr<int>> permutedVectorContainer_;
    std::vector<int> permutedBlockTracker_;
    std::vector<int> updatedBlockCount_;
    std::vector<int> rowCount_;
    size_t numTotalUpdatedBlocks_;
    size_t finalCount_;
    size_t numBlocksRows_ = 0;
    size_t numBlocksCols_ = 0;
    int count = 0;


public:

    LASWPLeftState(size_t numBlocksRows, size_t numBlocksCols)
            : numBlocksRows_(numBlocksRows), numBlocksCols_(numBlocksCols) {
        numBlocksRows_ = numBlocksRows;
        numBlocksCols_ = numBlocksCols;
        permutedBlockContainer_.resize(numBlocksCols, nullptr);
        permutedVectorContainer_.resize(numBlocksCols, nullptr);

        /// Total number of blocks
        /// 0 : Not permuted fully
        /// 1 : Fully permuted
        permutedBlockTracker_.resize(numBlocksCols * numBlocksCols, 0);
        
        updatedBlockCount_.resize(numBlocksCols, -1);
        rowCount_.resize(numBlocksCols, -1);

        /// Total number of blocks under the diagonal
        finalCount_ = (numBlocksCols * numBlocksCols - numBlocksCols) / 2;
        numTotalUpdatedBlocks_ = 0;

    }

    virtual ~LASWPLeftState() {
        permutedBlockContainer_.clear();
        permutedBlockTracker_.clear();
        updatedBlockCount_.clear();
        rowCount_.clear();
        permutedBlockContainer_.clear();
    }

    /// count = -1 : block has not been recieved
    /// count = 0 : block is done row interchanges
    /// count = anything else : block is still subject to future row interchanges

    void execute(std::shared_ptr<MatrixBlockData<Type, 's'>> ptr) override {
        size_t row = ptr->getRowIdx();
        size_t col = ptr->getColIdx();

        /// row - 1 is to address the col before all the updated Cols
        /// X = permutedBlockCol
        /// P = ptr
        
        /// [ ][ ][ ][ ] 
        /// [X][ ][ ][P] 
        /// [ ][ ][ ][ ] 
        /// [ ][ ][ ][ ]


        size_t permutedBlockCol = row - 1;

        if (updatedBlockCount_[permutedBlockCol] == -1) {
            updatedBlockCount_[permutedBlockCol] = numBlocksCols_ - row;
        }

        if ((updatedBlockCount_[permutedBlockCol] != -1) && (updatedBlockCount_[permutedBlockCol] != 0)) {
            updatedBlockCount_[permutedBlockCol]--;
        }
        /// Row here is used as number of col
        for (size_t i = 0; i < row; i++) {
            /// Check if panel is stored for this col
            if (rowCount_[i] != numBlocksCols_) {
                if (permutedBlockContainer_[i] != nullptr) {
                    if (updatedBlockCount_[i] == 0) {
                        if (permutedVectorContainer_[rowCount_[i]] != nullptr) {
                            if (permutedBlockTracker_[rowCount_[i] * numBlocksCols_ + i] == 0) {
                                this->push(std::make_shared<IPivPermutedBlockData<Type>>(
                                        permutedBlockContainer_[i], permutedVectorContainer_[rowCount_[i]],
                                        rowCount_[i]));
                                numTotalUpdatedBlocks_++;
                                /// Mark the permutedBlockTracker
                                permutedBlockTracker_[rowCount_[i] * numBlocksCols_ + i]++;
                            }
                        }
                    }
                }
            }
        }

        /// Debug ////////////////////////////////////////////////////////////////////////////////
//        std::cout << numBlocksBCols_ * numBlocksBCols_  << std::endl;
//        std::cout << "What is correct: 0 = not sent, 1 = sent." << std::endl;
//        for (size_t i = 0; i < numBlocksBCols_; ++i) {
//            for (size_t j = 0; j < numBlocksBCols_; ++j) {
//                std::cout << permutedBlockTracker_[i * numBlocksBCols_ + j] << " ";
//            }
//            std::cout << std::endl;
//        }
//        std::cout << std::endl;

//        std::cout << "Count: " << numTotalUpdatedBlocks_ << " / " << finalCount_ << std::endl;

    }

    void execute(std::shared_ptr<PermutedBlockData<Type>> ptr) override {
        size_t row = ptr->getPermutedBlock()->getRowIdx();
        size_t col = ptr->getPermutedBlock()->getColIdx();

/// store ptr in container
        if (permutedBlockContainer_[col] == nullptr) {
            permutedBlockContainer_[col] = ptr->getPermutedBlock();
        }

/// Block (0,0) provides no use for piv vector
        if ((permutedVectorContainer_[col] == nullptr) && (col != 0)) {
            permutedVectorContainer_[col] = ptr->getPermutationVector();
        }
        
        if (rowCount_[col] == -1) {
            rowCount_[col] = row + 1;
        }

/// Sends all panels less than permutedBlockData
        for (size_t i = 0; i < col; i++) {
            if (updatedBlockCount_[i] == 0) {
                if (rowCount_[i] == row) {
                    if (permutedBlockTracker_[row * numBlocksCols_ + i] == 0) {
                        this->push(std::make_shared<IPivPermutedBlockData<Type>>(permutedBlockContainer_[i],
                                                                                   permutedVectorContainer_[col], row));
                        numTotalUpdatedBlocks_++;
                        /// Mark the permutedBlockTracker
                        permutedBlockTracker_[row * numBlocksCols_+ i]++;
                    }
                }
            }
        }
//        PRINT("COUNT: " + std::to_string(count) + " / 36 \n");

        /// Debug ////////////////////////////////////////////////////////////////////////////////
//        std::cout << numBlocksBCols_ * numBlocksBCols_  << std::endl;
//        std::cout << "What is correct: 0 = not sent, 1 = sent." << std::endl;
//        for (size_t i = 0; i < numBlocksBCols_; ++i) {
//            for (size_t j = 0; j < numBlocksBCols_; ++j) {
//                std::cout << permutedBlockTracker_[i * numBlocksBCols_ + j] << " ";
//            }
//            std::cout << std::endl;
//        }
//        std::cout << std::endl;

//        std::cout << "Count: " << numTotalUpdatedBlocks_ << " / " << finalCount_ << std::endl;

    }

    void execute(std::shared_ptr<MatrixBlockData<Type, 'l'>> ptr) override {
        size_t row = ptr->getRowIdx();
        size_t col = ptr->getColIdx();

        rowCount_[col]++;
        if (rowCount_[col] != numBlocksCols_) {
            if (updatedBlockCount_[col] == 0) {
                if (permutedVectorContainer_[rowCount_[col]] != nullptr) {
                    if (permutedBlockContainer_[col] != nullptr) {

                        this->push(std::make_shared<IPivPermutedBlockData<Type>>(permutedBlockContainer_[col],
                                                                                   permutedVectorContainer_[rowCount_[col]],
                                                                                   rowCount_[col])
                        );
                        
                        /// Update Counts
                        permutedBlockTracker_[rowCount_[col] * numBlocksCols_ + col]++;
                        numTotalUpdatedBlocks_++;

                    }
                }
            }
        }


    }


    bool finishAccumulating() {
        return finalCount_ == numTotalUpdatedBlocks_;
    }

};

#endif //MATRIXLIB_LASWPLEFTSTATE_H
