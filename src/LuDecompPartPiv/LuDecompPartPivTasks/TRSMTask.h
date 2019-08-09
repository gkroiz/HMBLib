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

#ifndef MATRIXLIB_TRSMTASK_H
#define MATRIXLIB_TRSMTASK_H

#include <hedgehog.h>
#include "data/MatrixBlockData.h"
#include "LuDecompPartPiv/LuDecompPartPivData/TRSMData.h"
template <class Type>
class TRSMTask : public AbstractTask<MatrixBlockData<Type, 'f'>, TRSMData<Type>> {
public:
    TRSMTask(const std::string_view &name, size_t numberThreads) : AbstractTask<MatrixBlockData<Type, 'f'>, TRSMData<Type>>(name, numberThreads) {}

    void execute(std::shared_ptr<TRSMData<Type>> ptr) override {
        auto permutedBlockData = ptr->getPermutedData()->getPermutedBlock();
        auto standardBlocKData = ptr->getStandardBlock();

        double *blockA = permutedBlockData->getBlockData();
        double *blockB = standardBlocKData->getBlockData();

        size_t ldp = permutedBlockData->getLeadingDimension();
        size_t lds = standardBlocKData->getLeadingDimension();

        double *startingLocationA = &(blockA[permutedBlockData->getRowIdx() * permutedBlockData->getBlockSizeWidth() * permutedBlockData->getLeadingDimension()]);
        double *startingLocationB = &(blockB[standardBlocKData->getRowIdx() * standardBlocKData->getBlockSizeWidth() * standardBlocKData->getLeadingDimension()]);

        cblas_dtrsm(CblasRowMajor, CblasLeft, CblasLower, CblasNoTrans, CblasUnit, permutedBlockData->getBlockSizeWidth(), standardBlocKData->getBlockSizeWidth(), 1.0, startingLocationA, ldp, startingLocationB, lds);

        this->addResult(std::make_shared<MatrixBlockData<Type, 'f'>>(standardBlocKData));
    }

    std::shared_ptr<AbstractTask<MatrixBlockData<Type, 'f'>, TRSMData<Type>>> copy() override {
        return std::make_shared<TRSMTask>(this->name(), this->numberThreads());
    }

};
#endif //MATRIXLIB_TRSMTASK_H
