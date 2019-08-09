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
// Created by gck2 on 6/25/19.
//

#ifndef MATRIXLIB_MATRIXBLOCKDATA_H
#define MATRIXLIB_MATRIXBLOCKDATA_H

#include <ostream>
#include <memory>

enum class Order {Row, Column};

template <class Type, char Id = '0', Order Ord = Order::Row>
class MatrixBlockData {
private:
    size_t rowIdx_;
    size_t colIdx_;
    size_t blockSizeHeight_;
    size_t blockSizeWidth_;
    size_t leadingDimension_;
    Type *fullMatrixData_;
    Type *blockData_;
public:
    MatrixBlockData(size_t rowIdx, size_t colIdx, size_t blockSizeHeight, size_t blockSizeWidth,
                    size_t leadingDimension, Type *fullMatrixData, Type *blockData) : rowIdx_(rowIdx),
                                                                                           colIdx_(colIdx),
                                                                                           blockSizeHeight_(
                                                                                                   blockSizeHeight),
                                                                                           blockSizeWidth_(
                                                                                                   blockSizeWidth),
                                                                                           leadingDimension_(
                                                                                                   leadingDimension),
                                                                                           fullMatrixData_(
                                                                                                   fullMatrixData),
                                                                                           blockData_(blockData) {}

    template <char OldId>
    explicit MatrixBlockData(MatrixBlockData<Type, OldId> &o)
    {
        this->rowIdx_ = o.rowIdx_;
        this->colIdx_ = o.colIdx_;
        this->blockSizeHeight_ = o.blockSizeHeight_;
        this->blockSizeWidth_ = o.blockSizeWidth_;
        this->leadingDimension_ = o.leadingDimension_;
        this->fullMatrixData_ = o.fullMatrixData_;
        this->blockData_ = o.blockData_;
    }

    template <char OldId>
    explicit MatrixBlockData(std::shared_ptr<MatrixBlockData<Type, OldId>> &o) {
        this->rowIdx_ = o->getRowIdx();
        this->colIdx_ = o->getColIdx();
        this->blockSizeHeight_ = o->getBlockSizeHeight();
        this->blockSizeWidth_ = o->getBlockSizeWidth();
        this->leadingDimension_ = o->getLeadingDimension();
        this->fullMatrixData_ = o->getFullMatrixData();
        this->blockData_ = o->getBlockData();
    }

    size_t getRowIdx() const {
        return rowIdx_;
    }

    size_t getColIdx() const {
        return colIdx_;
    }

    size_t getBlockSizeHeight() const {
        return blockSizeHeight_;
    }

    size_t getBlockSizeWidth() const {
        return blockSizeWidth_;
    }

    size_t getLeadingDimension() const {
        return leadingDimension_;
    }

    Type *getFullMatrixData() const {
        if (Ord == Order::Row)
            return fullMatrixData_;
//            return &(fullMatrixData_[this->getRowIdx() * this->getLeadingDimension() + this->getColIdx()]);
        else
           return fullMatrixData_;
//            return &(fullMatrixData_[this->getColIdx() * this->getLeadingDimension() + this->getRowIdx()]);
    }

    Type *getBlockData() const {
        return blockData_;
    }
};
#endif //MATRIXLIB_MATRIXBLOCKDATA_H
