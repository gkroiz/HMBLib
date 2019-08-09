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

#ifndef MATRIXLIB_LASWPRIGHTSTATE_H
#define MATRIXLIB_LASWPRIGHTSTATE_H

#include <hedgehog.h>
#include "LuDecompPartPiv/LuDecompPartPivData/LASWPData.h"
#include "LuDecompPartPiv/LuDecompPartPivData/PermutedBlockData.h"

template<class Type>
class LASWPRightState : public AbstractState<LASWPData<Type>, PermutedBlockData<Type>, MatrixBlockData<Type, 's'>> {
private:
    std::vector<int> standardBlockCount_;
    std::vector<std::shared_ptr<PermutedBlockData<Type>>> permutedVectorContainer_;
    std::vector<std::shared_ptr<MatrixBlockData<Type, 's'>>> standardBlockContainer_;
    size_t updateCount_ = 0;
    size_t numBlocksRows_ = 0;
    size_t numBlocksCols_ = 0;
    size_t finalCount_ = 0;

public:
    LASWPRightState(size_t numBlocksRows, size_t numBlocksCols)
            : numBlocksRows_(numBlocksRows), numBlocksCols_(numBlocksCols) {
        numBlocksRows_ = numBlocksRows;
        numBlocksCols_ = numBlocksCols;
        standardBlockCount_.resize(numBlocksRows * numBlocksCols);
        permutedVectorContainer_.resize(numBlocksRows * numBlocksCols, nullptr);
        standardBlockContainer_.resize(numBlocksRows * numBlocksCols, nullptr);

        std::fill(standardBlockCount_.begin(), standardBlockCount_.end(), -1);
        finalCount_ = numBlocksCols - 1;
        updateCount_ = 0;
    }

    /// count = -1 : block has not been recieved
    /// count = 0 : block is done row interchanges
    /// count = anything else : block is still subject to future row interchanges

    void execute(std::shared_ptr<PermutedBlockData<Type>> ptr) override {
        size_t row = ptr->getPermutedBlock()->getRowIdx();
        size_t col = ptr->getPermutedBlock()->getColIdx();
        
        /// store ptr in container
        permutedVectorContainer_[col] = ptr;

        for (size_t i = row; i < numBlocksCols_; i++) {
            
            /// Check to see if block is in container
            if (standardBlockContainer_[i] != nullptr) {
                
                /// Check to see if block is ready to send
                if ((standardBlockCount_[i] != 0) &
                    (standardBlockCount_[i] != -1)) {
                    this->push(std::make_shared<LASWPData<Type>>(standardBlockContainer_[i],
                                                                 ptr));

                    /// Forces the updated block to return before the block can be sent through again.
                    standardBlockContainer_[i] = nullptr;

                    standardBlockCount_[i]--;

                    /// Check to see if block is finished all pivots, if so, count is incremented
                    if (standardBlockCount_[i] == 0) {
                        updateCount_++;
                    }
                }
            }
        }
    }

    void execute(std::shared_ptr<MatrixBlockData<Type, 's'>> ptr) override {
        size_t row = ptr->getRowIdx();
        size_t col = ptr->getColIdx();
        if (row != col) {
            if (standardBlockContainer_[col] == nullptr) {
                standardBlockContainer_[col] = ptr;
            }

            /// Count is based on the number pivot vectors needed to fully pivot the panel
            if (standardBlockCount_[col] == -1) {
                standardBlockCount_[col] = col;
            }

            
            if (standardBlockCount_[col] != 0) {
                
                /// Checks to see if permuted vector is stored
                if ((permutedVectorContainer_[row] != nullptr)) {
                
                    this->push(std::make_shared<LASWPData<Type>>(standardBlockContainer_[col],
                                                                 permutedVectorContainer_[row]));

                    /// Forces the updated block to return before the block can be sent through again.
                    standardBlockContainer_[col] = nullptr;

                    /// Count decrement
                    standardBlockCount_[col]--;

                    /// Check to see if block is finished all pivots, if so, count is incremented
                    if (standardBlockCount_[col] == 0) {
                        updateCount_++;
                    }
                }
            }
        }
    }

    bool finishAccumulating() {
        /// updateCount is updated everytime a block count reaches 0, final count is numBlocks
        return finalCount_ == updateCount_;
    }

};

#endif //MATRIXLIB_LASWPRIGHTSTATE_H
