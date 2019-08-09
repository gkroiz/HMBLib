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
// Created by gck2 on 7/5/19.
//

#ifndef MATRIXLIB_BLOCKDECOMPOSITIONTASK_H
#define MATRIXLIB_BLOCKDECOMPOSITIONTASK_H

#include <hedgehog.h>
#include "../data/MatrixData.h"
#include "../data/MatrixBlockData.h"

template <class Type, char Id = '0', Order Ord = Order::Row>

class BlockDecompositionTask : public AbstractTask<MatrixData<Type>, MatrixBlockData<Type>> {
private:
    size_t matrixHeight_ = 0;
    size_t matrixWidth_ = 0;
    size_t blockSize_ = 0;
    size_t numBlocksRows_ = 0;
    size_t numBlocksCols_ = 0;
    size_t ld_ = 0;
    Type *matrixData_ = 0;
public:
//    BlockDecompositionTask(size_t matrixHeight, size_t matrixWidth, size_t blockSize, size_t numBlocksRows,
//                       size_t numBlocksCols, size_t ld, Type *matrixData, const std::string_view &name, size_t numberThreads)  : matrixHeight_(matrixHeight),
//                                                                            matrixWidth_(matrixWidth),
//                                                                            blockSize_(blockSize),
//                                                                            numBlocksARows_(numBlocksRows),
//                                                                            numBlocksBCols_(numBlocksCols), ld_(ld),
//                                                                            matrixData_(matrixData), AbstractTask<MatrixData<Type>, MatrixBlockData<Type>>(name, numberThreads) {}

    BlockDecompositionTask(const std::string_view &name, size_t numberThreads) : AbstractTask<MatrixData<Type>, MatrixBlockData<Type>>(name, numberThreads) {}


        void execute(std::shared_ptr<MatrixData<Type, 'a'>> ptr) override {
        for (size_t col = 0; col < numBlocksRows_; ++col) {
            for (size_t row = 0; row < numBlocksCols_; ++row) {
                double *startLocationA = &(matrixData_[row * blockSize_ * ld_ + col * blockSize_]);
                //            std::cout << "StartingLocationA: " << *(startLocationA) << std::endl;
                size_t blockAHeight = blockSize_;
                size_t blockAWidth = blockSize_;
                if (row == numBlocksRows_ - 1 && matrixHeight_ % blockSize_ != 0)
                    blockAHeight = matrixHeight_ % blockSize_;
                if (col == numBlocksCols_ - 1 && matrixWidth_ % blockSize_ != 0)
                    blockAWidth = matrixWidth_ % blockSize_;

                this->addResult(std::make_shared<MatrixBlockData<Type, Id, Ord>>(row, col, blockAHeight, blockAWidth, ld_,
                                                                        matrixData_, startLocationA));
            }
        }
    }

    void execute(std::shared_ptr<MatrixData<Type, 'b'>> ptr) override {
        for (size_t col = 0; col < numBlocksRows_; ++col) {
            for (size_t row = 0; row < numBlocksCols_; ++row) {
                double *startLocationA = &(matrixData_[row * blockSize_ * ld_ + col * blockSize_]);
                //            std::cout << "StartingLocationA: " << *(startLocationA) << std::endl;
                size_t blockAHeight = blockSize_;
                size_t blockAWidth = blockSize_;
                if (row == numBlocksRows_ - 1 && matrixHeight_ % blockSize_ != 0)
                    blockAHeight = matrixHeight_ % blockSize_;
                if (col == numBlocksCols_ - 1 && matrixWidth_ % blockSize_ != 0)
                    blockAWidth = matrixWidth_ % blockSize_;

                this->addResult(std::make_shared<MatrixBlockData<Type, Id, Ord>>(row, col, blockAHeight, blockAWidth, ld_,
                                                                                matrixData_, startLocationA));
            }
        }
    }

    void execute(std::shared_ptr<MatrixData<Type, 'c'>> ptr) override {
        for (size_t col = 0; col < numBlocksRows_; ++col) {
            for (size_t row = 0; row < numBlocksCols_; ++row) {
                double *startLocationA = &(matrixData_[row * blockSize_ * ld_ + col * blockSize_]);
                //            std::cout << "StartingLocationA: " << *(startLocationA) << std::endl;
                size_t blockAHeight = blockSize_;
                size_t blockAWidth = blockSize_;
                if (row == numBlocksRows_ - 1 && matrixHeight_ % blockSize_ != 0)
                    blockAHeight = matrixHeight_ % blockSize_;
                if (col == numBlocksCols_ - 1 && matrixWidth_ % blockSize_ != 0)
                    blockAWidth = matrixWidth_ % blockSize_;

                this->addResult(std::make_shared<MatrixBlockData<Type, Id, Ord>>(row, col, blockAHeight, blockAWidth, ld_,
                                                                                matrixData_, startLocationA));
            }
        }
    }
    
    std::shared_ptr<AbstractTask<MatrixData<Type>, MatrixBlockData<Type>>> copy() {
            return std::make_shared<BlockDecompositionTask>(this->name(), this->numberThreads());
    }

};
#endif //MATRIXLIB_BLOCKDECOMPOSITIONTASK_H
