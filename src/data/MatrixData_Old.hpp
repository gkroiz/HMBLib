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
// Created by tjb3 on 4/29/19.
//

#ifndef MATRIXLIB_MATRIXDATA_OLD_HPP
#define MATRIXLIB_MATRIXDATA_OLD_HPP

#include <ostream>
#include <memory>

enum class Order  {Row, Column};

template <class Type, char Id = '0', Order Ord = Order::Row>
class MatrixData_Old {

public:
    MatrixData_Old(size_t rowIdx,
               size_t colIdx,
               Type *matrixData,
               size_t height,
               size_t width,
               size_t leadingDim,
               size_t blockSize
    ) :
            rowIdx_(rowIdx), colIdx_(colIdx),
            fullMatrixData_(matrixData),
            height_(height), width_(width),
            leadingDim_(leadingDim),
            blockSize_(blockSize)
    {}


    template <char OldId>
    explicit MatrixData_Old(MatrixData_Old<Type, OldId> &o)
    {
        this->rowIdx_ = o.rowIdx_;
        this->colIdx_ = o.colIdx_;
        this->fullMatrixData_ = o.fullMatrixData_;
        this->height_ = o.height_;
        this->width_ = o.width_;
        this->leadingDim_ = o.leadingDim_;
        this->blockSize_ = o.blockSize_;
    }

    template <char OldId>
    explicit MatrixData_Old(std::shared_ptr<MatrixData_Old<Type, OldId>> &o)
    {
        this->rowIdx_ = o->getRowIdx();
        this->colIdx_ = o->getColIdx();
        this->fullMatrixData_ = o->getFullMatrixPtr();
        this->height_ = o->getMatrixHeight();
        this->width_ = o->getMatrixWidth();
        this->leadingDim_ = o->getLeadingDim();
        this->blockSize_ = o->getBlockSize();
    }

    size_t getRowIdx() const {
        return rowIdx_;
    }

    size_t getColIdx() const {
        return colIdx_;
    }

    Type *getMatrixData() const {
        if (Ord == Order::Row)
            return &(fullMatrixData_[this->getRowIdx() * blockSize_ * this->getLeadingDim() + this->getColIdx() * blockSize_]);
        else
            return &(fullMatrixData_[this->getColIdx() * blockSize_ * this->getLeadingDim() + this->getRowIdx() * blockSize_]);
    }

    Type *getFullMatrixPtr() const {
        return fullMatrixData_;
    }

    size_t getMatrixWidth() const {
        return width_;
    }

    size_t getMatrixHeight() const {
        return height_;
    }

    size_t getLeadingDim() const {
        return leadingDim_;
    }

    size_t getBlockSize() const {
        return blockSize_;
    }

//    friend std::ostream &operator<<(std::ostream &os, const MatrixData_Old &data) {
//        os << "Matrix(" << Id << ")" << "; (" << data.rowIdx_ << ", " << data.colIdx_
//           << ") w: " << data.width_ << ",  h:" << data.height_;
//        return os;
//    }

private:

    size_t rowIdx_;
    size_t colIdx_;

    size_t blockSize_;

//    std::shared_ptr<Type> fullMatrixData_;
    Type *fullMatrixData_;
    size_t height_;
    size_t width_;
    size_t leadingDim_;

    //std::string _name;

};

#endif //MATRIXLIB_MATRIXDATA_OLD_HPP

