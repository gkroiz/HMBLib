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
// Created by gck2 on 7/16/19.
//

#ifndef MATRIXLIB_IPIVPERMUTEDBLOCKDATA_H
#define MATRIXLIB_IPIVPERMUTEDBLOCKDATA_H

#include "data/MatrixBlockData.h"

template <class Type, char Id = '0', Order Ord = Order::Row>
class IPivPermutedBlockData {
private:
    std::shared_ptr<MatrixBlockData<Type, 's', Ord>> permutedBlock_;

    std::shared_ptr<int> permutationVector_;

    size_t ipivRowIdx_;
public:
    IPivPermutedBlockData(const std::shared_ptr<MatrixBlockData<Type, 's', Ord>> &permutedBlock,
                      const std::shared_ptr<int> &permutationVector, size_t ipivRowIdx) : permutedBlock_(permutedBlock),
                                                                                          permutationVector_(
                                                                                                  permutationVector),
                                                                                          ipivRowIdx_(ipivRowIdx) {}

    const std::shared_ptr<MatrixBlockData<Type, 's', Ord>> &getPermutedBlock() const {
        return permutedBlock_;
    }

    const std::shared_ptr<int> &getPermutationVector() const {
        return permutationVector_;
    }

    size_t getIpivRowIdx() const {
        return ipivRowIdx_;
    }

};
#endif //MATRIXLIB_IPIVPERMUTEDBLOCKDATA_H
