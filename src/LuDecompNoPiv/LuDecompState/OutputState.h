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

#ifndef MATRIXLIB_OUTPUTSTATE_H
#define MATRIXLIB_OUTPUTSTATE_H

#include <api/state_manager/abstract_state.h>
#include "../../data/MatrixBlockData.h"

template <class Type>
class OutputState : public AbstractState<MatrixBlockData<Type, 's'>, MatrixBlockData<Type, 's'>, TRSMData<Type>, MatrixBlockData<Type, 'g'>> {
private:
    size_t numBlocksRows_;
    size_t numBlocksCols_;
public:
    OutputState(size_t numBlocksRows, size_t numBlocksCols) : numBlocksRows_(numBlocksRows),
                                                              numBlocksCols_(numBlocksCols) {}

    void execute(std::shared_ptr<MatrixBlockData<Type, 's'>> ptr) override {

    }

    /// READ ONLY OUTPUTS!
    void execute(std::shared_ptr<TRSMData<Type>> ptr) override {
        auto trsmBlockData = ptr->getStandardBlock();

        /// Set BlockPointer of the block
        auto blockPointer = &(trsmBlockData->getBlockData()[trsmBlockData->getRowIdx() * trsmBlockData->getBlockSizeWidth() * trsmBlockData->getLeadingDimension()]);

        /// Changes the Output Data into a block

        /// Send out Factored Block Data
        this->push(std::make_shared<MatrixBlockData<Type, 's'>>(
                trsmBlockData->getRowIdx(),
                trsmBlockData->getColIdx(),
                trsmBlockData->getBlockSizeWidth(),
                trsmBlockData->getBlockSizeWidth(),
                trsmBlockData->getLeadingDimension(),
                trsmBlockData->getFullMatrixData(),
                blockPointer));
    }

    /// READ ONLY OUTPUTS!
    void execute(std::shared_ptr<MatrixBlockData<Type, 'g'>> ptr) override {

        /// Changes the Output Data into a block
        /// Send out Gauss Elim Block Data


        /// Divides panels into blocks for num Blocks Cols
        for (size_t i = ptr->getRowIdx(); i < numBlocksCols_; i++) {

            /// Set pointer of the block
            double *blockPointer = &(ptr->getBlockData()[i * ptr->getBlockSizeWidth() * ptr->getLeadingDimension()]);

            this->push(std::make_shared<MatrixBlockData<Type, 's'>>(
                    i,
                    ptr->getColIdx(),
                    ptr->getBlockSizeWidth(),
                    ptr->getBlockSizeWidth(),
                    ptr->getLeadingDimension(),
                    ptr->getFullMatrixData(),
                    blockPointer));
        }
    }
};
#endif //MATRIXLIB_OUTPUTSTATE_H
