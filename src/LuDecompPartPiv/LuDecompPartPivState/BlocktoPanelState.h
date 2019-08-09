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
// Created by gck2 on 7/24/19.
//

#ifndef MATRIXLIB_BLOCKTOPANELSTATE_H
#define MATRIXLIB_BLOCKTOPANELSTATE_H

#include <hedgehog.h>
#include "data/MatrixBlockData.h"
template <class Type>
        
/// Used if blocks are used for mat mul
class BlocktoPanelState : public AbstractState<MatrixBlockData<Type, 's'>, MatrixBlockData<Type, 's'>> {
private:
    std::vector<std::shared_ptr<MatrixBlockData<Type, 's'>>> blockContainer_;
    std::vector<int> updatedBlocksCounter_;
    std::vector<int> numBlocksNeeded_;
    std::vector<int> minRowIdx_;
    size_t numBlocksRows_ = 0;
    size_t numBlocksCols_ = 0;

public:
    BlocktoPanelState(size_t numBlocksRows, size_t numBlocksCols) : numBlocksRows_(numBlocksRows),
                                                                    numBlocksCols_(numBlocksCols) {
        numBlocksRows_ = numBlocksRows;
        numBlocksCols_ = numBlocksCols;

        blockContainer_.resize(numBlocksCols * numBlocksCols, nullptr);

        updatedBlocksCounter_.resize(numBlocksCols, 0);
        numBlocksNeeded_.resize(numBlocksCols, numBlocksCols - 1);
        minRowIdx_.resize(numBlocksCols, numBlocksCols);
    }

    virtual ~BlocktoPanelState() {
        blockContainer_.clear();
        updatedBlocksCounter_.clear();
        numBlocksNeeded_.clear();
        minRowIdx_.clear();
    }

    void execute(std::shared_ptr<MatrixBlockData<Type, 's'>> ptr) override {
        size_t row = ptr->getRowIdx();
        size_t col = ptr->getColIdx();


        if (minRowIdx_[col] > row){
            minRowIdx_[col] = row;
        }
        blockContainer_[row * numBlocksCols_ + col] = ptr;

        updatedBlocksCounter_[col] ++;

        if (updatedBlocksCounter_[col] == numBlocksNeeded_[col]){
            this->push(std::make_shared<MatrixBlockData<Type, 's'>>(
                    minRowIdx_[col], col,
                    ptr->getBlockSizeHeight(),
                    ptr->getBlockSizeWidth(),
                    ptr->getLeadingDimension(),
                    ptr->getFullMatrixData(),
                    ptr->getBlockData()));

            /// Reset counters
            minRowIdx_[col] = numBlocksCols_;
            numBlocksNeeded_[col] --;
            updatedBlocksCounter_[col] = 0;

        }



    }
};
#endif //MATRIXLIB_BLOCKTOPANELSTATE_H
