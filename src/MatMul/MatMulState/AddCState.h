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
// Created by gck2 on 6/21/19.
//

#ifndef MATRIXLIB_ADDCSTATE_H
#define MATRIXLIB_ADDCSTATE_H

#include <hedgehog.h>
#include <data/AccumMatData.h>

template <class Type, char OutputId>
class AddCState : public AbstractState<AccumMatData<Type, OutputId>, MatrixBlockData<Type, OutputId>> {
private:
    std::vector<std::shared_ptr<MatrixBlockData<Type, OutputId>>> cBlocksContainer_;
    Type *matrixC_;
    size_t numBlocksWidthA_ = 0;
    size_t numBlocksCols_ = 0;
    size_t numBlocksRows_ = 0;
    size_t matrixHeight_ = 0;
    size_t matrixWidth_ = 0;
    size_t blockHeight_ = 0;
    size_t blockWidth_ = 0;
    size_t leadingDimension_ = 0;

public:

    AddCState(Type *matrixC,
              size_t numBlocksRows,
              size_t numBlocksCols,
              size_t matrixHeight,
              size_t matrixWidth,
              size_t blockHeight,
              size_t blockWidth,
              size_t leadingDimension)
            : matrixC_(matrixC),
              numBlocksCols_(numBlocksCols), numBlocksRows_(numBlocksRows), matrixHeight_(matrixHeight),
              matrixWidth_(matrixWidth), blockHeight_(blockHeight), blockWidth_(blockWidth), leadingDimension_(leadingDimension) {

        cBlocksContainer_.resize(numBlocksRows * numBlocksCols, nullptr);
        for (size_t row = 0; row < numBlocksRows; ++row) {
            for (size_t col = 0; col < numBlocksCols; ++col) {
                double *startLocationC = &(matrixC[row * leadingDimension * blockHeight + col * blockWidth]);
                size_t blockCHeight = blockHeight;
                size_t blockCWidth = blockWidth;

                /// Determines appropiate block size
                if (row == numBlocksRows - 1 && matrixHeight % blockHeight != 0)
                    blockCHeight = matrixHeight % blockHeight;
                if (col == numBlocksCols - 1 && matrixWidth % blockWidth != 0)
                    blockCWidth = matrixWidth % blockWidth;

                std::shared_ptr<MatrixBlockData<Type, OutputId>> matrixCData = std::make_shared<MatrixBlockData<Type, OutputId>>(
                        row,
                        col,
                        blockCHeight,
                        blockCWidth,
                        leadingDimension,
                        matrixC,
                        startLocationC);

                cBlocksContainer_[row * numBlocksCols +col] = matrixCData;
            }
        }
    }

    ~AddCState() {
        cBlocksContainer_.clear();
    }

    void execute(std::shared_ptr<MatrixBlockData<Type, OutputId>> givenMatrix) {
        size_t row = givenMatrix->getRowIdx();
        size_t col = givenMatrix->getColIdx();
        if (cBlocksContainer_[row * numBlocksCols_ +col] != nullptr) {
            this->push(std::make_shared<AccumMatData<Type, OutputId>>(cBlocksContainer_[row * numBlocksCols_ +col], givenMatrix));
            cBlocksContainer_[row * numBlocksCols_ +col] = nullptr;
        }
    }
};
#endif //MATRIXLIB_ADDCSTATE_H
