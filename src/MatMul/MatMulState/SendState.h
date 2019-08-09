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

#ifndef MATRIXMULTIPLICATION_SENDSTATE_H
#define MATRIXMULTIPLICATION_SENDSTATE_H

#include <hedgehog.h>
#include <data/MatrixBlockData.h>

template <class Type, char OutputId>
class SendState : public AbstractState<MatrixBlockData<Type, OutputId>, MatrixBlockData<Type, OutputId>> {
    std::vector<std::shared_ptr<MatrixBlockData<Type, OutputId>>> matrixBlockContainer;
    std::vector<int> currentCount_;
    int finalCount_;
    size_t numBlocksRows_;
    size_t numBlocksCols_;
    size_t blockLeadingDimension;

public:
    SendState(size_t numBlocksRows, size_t numBlocksCols, size_t blockLeadingDimension) : numBlocksRows_(numBlocksRows), numBlocksCols_(numBlocksCols), blockLeadingDimension(blockLeadingDimension) {
        matrixBlockContainer.resize(numBlocksRows * numBlocksCols, nullptr);
        currentCount_.resize(numBlocksRows * numBlocksCols, 0);
        finalCount_ = 2 * blockLeadingDimension - 1;
    }

    virtual ~SendState() {
        matrixBlockContainer.clear();
        currentCount_.clear();
    }

    void execute(std::shared_ptr<MatrixBlockData<Type, OutputId>> data) {

        size_t row = data->getRowIdx();
        size_t col = data->getColIdx();
        currentCount_[row * numBlocksCols_ + col] ++;
        if (currentCount_[row * numBlocksCols_ + col] == finalCount_) {
            this->push(data);

        }


    }

};
#endif //MATRIXMULTIPLICATION_SENDSTATE_H
