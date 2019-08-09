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

#ifndef MATRIXMULTIPLICATION_PARTIALANSWERTASK_H
#define MATRIXMULTIPLICATION_PARTIALANSWERTASK_H

#include <hedgehog.h>
#include <cblas.h>
#include <MatMul/MatMulData/MatrixMulData.hpp>
#include <data/MatrixBlockData.h>
//#include <mkl.h>
template <class Type, char OutputId>
class MulBlockTask : public AbstractTask<MatrixBlockData<Type, OutputId>, MatrixMulData<Type>> {
public:

    MulBlockTask(const std::string_view &name, size_t numberThreads) : AbstractTask<MatrixBlockData<Type, OutputId>, MatrixMulData<Type>>(name, numberThreads) {
    }

    void execute(std::shared_ptr<MatrixMulData<Type>> givenMatrices) override {

        auto resultAData = givenMatrices->getMatrixA();
        auto resultBData = givenMatrices->getMatrixB();

        size_t blockAHeight = resultAData->getBlockSizeHeight();
        size_t blockAWidth = resultAData->getBlockSizeWidth();
        size_t blockBHeight = resultBData->getBlockSizeHeight();
        size_t blockBWidth = resultBData->getBlockSizeWidth();

        size_t lda = resultAData->getLeadingDimension();
        size_t ldb = resultBData->getLeadingDimension();
        size_t ldc = resultBData->getBlockSizeWidth();

        double *matrixA = resultAData->getBlockData();
        double *matrixB = resultBData->getBlockData();
        double *matrixC = new double[blockAHeight * blockBWidth];

        double alpha = 1.0;
        double beta = 0.0;

        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, blockAHeight, blockBWidth, blockAWidth, alpha, matrixA, lda, matrixB,
                 ldb, beta, matrixC, ldc);


        this->addResult(std::make_shared<MatrixBlockData<Type, OutputId>>(resultAData->getRowIdx(), resultBData->getColIdx(), blockAHeight, blockBWidth, ldc, matrixC, matrixC));
    }

    std::shared_ptr<AbstractTask<MatrixBlockData<Type , OutputId>, MatrixMulData<Type>>> copy() {
        return std::make_shared<MulBlockTask>(this->name(), this->numberThreads());
    }
};
#endif //MATRIXMULTIPLICATION_PARTIALANSWERTASK_H
