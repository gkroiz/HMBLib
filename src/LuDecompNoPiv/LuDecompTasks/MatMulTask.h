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

#ifndef MATRIXLIB_MATMULTASK_H
#define MATRIXLIB_MATMULTASK_H

#include <hedgehog.h>
#include <cblas.h>
#include "../LuDecompData/UpdateData.h"

template <class Type>
class MatMulTask : public AbstractTask<MatrixBlockData<Type, 's'>, TRSMData<Type>> {
public:
    MatMulTask(const std::string_view &name, size_t numberThreads) : AbstractTask<MatrixBlockData<Type, 's'>, TRSMData<Type>>(name, numberThreads) {}

    void execute(std::shared_ptr<TRSMData<Type>> ptr) override {

        auto permutedBlockData = ptr->getPermutedBlock();
        auto factoredBlockData = ptr->getStandardBlock();

        size_t pRow = permutedBlockData->getRowIdx();
        size_t pCol = permutedBlockData->getColIdx();
        size_t fRow = factoredBlockData->getRowIdx();
        size_t fCol = factoredBlockData->getColIdx();


        /// Create new data to (matrix C) to output from this task
        auto updateBlockData = std::make_shared<MatrixBlockData<Type, 's'>>(
                pRow + 1,
                fCol,
                factoredBlockData->getBlockSizeHeight(),
                factoredBlockData->getBlockSizeWidth(),
                factoredBlockData->getLeadingDimension(),
                factoredBlockData->getFullMatrixData(),
                factoredBlockData->getBlockData());

        auto permutedBlock = permutedBlockData->getBlockData();
        auto factoredBlock = factoredBlockData->getBlockData();
        auto updateBlock = updateBlockData->getBlockData();

        size_t blockPHeight = permutedBlockData->getBlockSizeHeight();
        size_t blockPWidth = permutedBlockData->getBlockSizeWidth();

        /// Shrinks size of height based on row
        blockPHeight -= blockPWidth;

        size_t blockFWidth = factoredBlockData->getBlockSizeWidth();

        size_t ldp = permutedBlockData->getLeadingDimension();
        size_t ldf = factoredBlockData->getLeadingDimension();
        size_t ldu = updateBlockData->getLeadingDimension();


        /// Change pointers accordingly
        double *permutedStartLocation = &(permutedBlock[(pRow + 1) * blockPWidth * ldp]);
        double *factoredStartLocation = &(factoredBlock[fRow * blockFWidth * ldf]);
        double *updateStartLocation = &(updateBlock[updateBlockData->getRowIdx() * updateBlockData->getBlockSizeWidth() * ldu]);


        double alpha = -1.0;
        double beta = 1.0;

        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, blockPHeight, blockFWidth, blockPWidth, alpha, permutedStartLocation, ldp, factoredStartLocation,
                    ldf, beta, updateStartLocation, ldu);

        this->addResult(updateBlockData);

    }

    std::shared_ptr<AbstractTask<MatrixBlockData<Type, 's'>, TRSMData<Type>>> copy() {
        return std::make_shared<MatMulTask>(this->name(), this->numberThreads());
    }

};
#endif //MATRIXLIB_MATMULTASK_H
