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

#ifndef MATRIXLIB_UPDATEDATA_H
#define MATRIXLIB_UPDATEDATA_H

#include "data/MatrixBlockData.h"

template <class Type, char Id = '0', Order Ord = Order::Row>
class UpdateData {
public:
    UpdateData(const std::shared_ptr<PermutedBlockData<Type>> &permutedData,
               const std::shared_ptr<MatrixBlockData<Type, 'f', Ord>> &factoredBlock, size_t permutedRow)
            : permutedData_(permutedData), factoredBlock_(factoredBlock), permutedRow_(permutedRow) {}

    const std::shared_ptr<PermutedBlockData<Type>> &getPermutedData() const {
        return permutedData_;
    }

    const std::shared_ptr<MatrixBlockData<Type, 'f', Ord>> &getFactoredBlock() const {
        return factoredBlock_;
    }

    size_t getPermutedRow() const {
        return permutedRow_;
    }

private:
    std::shared_ptr<PermutedBlockData<Type>> permutedData_;
    std::shared_ptr<MatrixBlockData<Type, 'f', Ord>> factoredBlock_;

    /// Used to determine row with panels or blocks
    size_t permutedRow_;
};
#endif //MATRIXLIB_UPDATEDATA_H
