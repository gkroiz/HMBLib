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

#ifndef MATRIXLIB_GETRFSTATE_H
#define MATRIXLIB_GETRFSTATE_H

#include <hedgehog.h>
#include "data/MatrixBlockData.h"

template <class Type>
class GETRFState : public AbstractState<MatrixBlockData<Type, 's'>, MatrixBlockData<Type, 's'>> {
private:
    std::vector<int> getrfCount;
    size_t updateCount_ = 0;
    size_t numBlocksRows_ = 0;
    size_t numBlocksCols_ = 0;
    size_t finalCount_ = 0;
public:
    GETRFState(size_t numBlocksRows, size_t numBlocksCols) {
        numBlocksRows_ = numBlocksRows;
        numBlocksCols_ = numBlocksCols;
        getrfCount.resize(numBlocksCols, -1);
        
        /// number of panels = numBlocksCols
        finalCount_ = numBlocksCols;
    }

    virtual ~GETRFState() {
        getrfCount.clear();
    }
    /// count = -1 : panel has not been recieved
    /// count = 0 : panel is ready to send
    /// count <= -2 : panel has already sent
    /// count = anything else : panel is not ready to send

    void execute(std::shared_ptr<MatrixBlockData<Type, 's'>> ptr) override {
        
        /// Sets panel count based on col
        if (getrfCount[ptr->getColIdx()] == -1){
            getrfCount[ptr->getColIdx()] = ptr->getColIdx();
        }
        
        if (getrfCount[ptr->getColIdx()] == 0){
            size_t row = ptr->getRowIdx();
            size_t col = ptr->getColIdx();
            size_t blockHeight = ptr->getBlockSizeHeight();
            size_t blockWidth = ptr->getBlockSizeWidth();
            size_t ld = ptr->getLeadingDimension();

            /// change block height
            blockHeight -= row * blockWidth;

            this->push(std::make_shared<MatrixBlockData<double, 's'>>(row, col, blockHeight, blockWidth, ld, ptr->getFullMatrixData(), ptr->getBlockData()));
            
            /// Lowers count to show that the panel has been sent
            getrfCount[ptr->getColIdx()] -= 2;
            
            updateCount_ ++;
        }
        
        /// Decrements panel count
        else {
            getrfCount[ptr->getColIdx()] --;
        }


    }

    bool finishAccumulating() {
        /// updateCount increases evertime block is sent through, final count is numBlocksCols
        return finalCount_ == updateCount_;
    }
};
#endif //MATRIXLIB_GETRFSTATE_H
