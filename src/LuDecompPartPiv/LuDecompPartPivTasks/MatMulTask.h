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

#ifndef MATRIXLIB_MATMULTASK_H
#define MATRIXLIB_MATMULTASK_H

#include <hedgehog.h>
#include <data/MatrixBlockData.h>
#include "LuDecompPartPiv/LuDecompPartPivData/UpdateData.h"
#include <cblas.h>

template <class Type>
class MatMulTask : public AbstractTask<MatrixBlockData<Type, 's'>, UpdateData<Type>> {
public:
    MatMulTask(const std::string_view &name, size_t numberThreads) : AbstractTask<MatrixBlockData<Type, 's'>, UpdateData<Type>>(name, numberThreads) {}

    void execute(std::shared_ptr<UpdateData<Type>> ptr) override {
        auto permutedBlockData = ptr->getPermutedData()->getPermutedBlock();
        auto factoredBlockData = ptr->getFactoredBlock();
        size_t pRow = ptr->getPermutedRow();
        size_t pCol = permutedBlockData->getColIdx();
        size_t fRow = factoredBlockData->getRowIdx();
        size_t fCol = factoredBlockData->getColIdx();

        auto updateBlockData = std::make_shared<MatrixBlockData<Type, 's'>>(
                pRow,
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

        double *permutedStartLocation = &(permutedBlock[pRow * blockPWidth * ldp]);
        double *factoredStartLocation = &(factoredBlock[fRow * blockFWidth * ldf]);
        double *updateStartLocation = &(updateBlock[updateBlockData->getRowIdx() * updateBlockData->getBlockSizeWidth() * ldu]);


        double alpha = -1.0;
        double beta = 1.0;

        /// Change values dependent on inputs as panels or blocks (for 4th value in gemm)
            /// blockPWidth for blocks
            /// blockPHeight for panels

        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, blockPWidth, blockFWidth, blockPWidth, alpha, permutedStartLocation, ldp, factoredStartLocation,
                    ldf, beta, updateStartLocation, ldu);
        
        this->addResult(updateBlockData);

    }

    std::shared_ptr<AbstractTask<MatrixBlockData<Type, 's'>, UpdateData<Type>>> copy() override {
        return std::make_shared<MatMulTask>(this->name(), this->numberThreads());
    }
};
#endif //MATRIXLIB_MATMULTASK_H
