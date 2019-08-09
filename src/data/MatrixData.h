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

#ifndef MATRIXLIB_MATRIXDATA_H
#define MATRIXLIB_MATRIXDATA_H

#include <ostream>
#include <memory>

//enum class Order  {Row, Column};
template <class Type, char Id = '0', Order Ord = Order::Row>
class MatrixData {
private:
    size_t matrixHeight_ = 0;
    size_t matrixWidth_ = 0;
    size_t blockSize_ = 0;
    size_t numBlocksRows_ = 0;
    size_t numBlocksCols_ = 0;
    size_t ld_ = 0;
    Type *matrixData_;

public:
    MatrixData(size_t matrixHeight, size_t matrixWidth, size_t blockSize, size_t numBlocksRows, size_t numBlocksCols,
               size_t ld, Type *matrixData) : matrixHeight_(matrixHeight), matrixWidth_(matrixWidth),
                                              blockSize_(blockSize), numBlocksRows_(numBlocksRows),
                                              numBlocksCols_(numBlocksCols), ld_(ld), matrixData_(matrixData) {}

    size_t getMatrixHeight() const {
        return matrixHeight_;
    }

    size_t getMatrixWidth() const {
        return matrixWidth_;
    }

    size_t getBlockSize() const {
        return blockSize_;
    }

    size_t getNumBlocksRows() const {
        return numBlocksRows_;
    }

    size_t getNumBlocksCols() const {
        return numBlocksCols_;
    }

    size_t getLd() const {
        return ld_;
    }

    Type *getMatrixData() const {
        return matrixData_;
    }

};
#endif //MATRIXLIB_MATRIXDATA_H
