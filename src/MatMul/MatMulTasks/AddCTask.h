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
// Created by gck2 on 6/21/19.
//

#ifndef MATRIXLIB_ADDCTASK_H
#define MATRIXLIB_ADDCTASK_H

#include <data/MatrixBlockData.h>
#include <hedgehog.h>

template <class Type, char OutputId>
class AddCTask : public AbstractTask<MatrixBlockData<Type, OutputId>, AccumMatData<Type, OutputId>> {
public:
    AddCTask(const std::string_view &name, size_t numberThreads) : AbstractTask<MatrixBlockData<Type, OutputId>, AccumMatData<Type, OutputId>>(name, numberThreads) {
    }

    void execute(std::shared_ptr<AccumMatData<Type, OutputId>> data) override {
        auto finalMatrixData = data->getMatrixC1();
        auto resultC2Data = data->getMatrixC2();

        double *finalMatrixC =  finalMatrixData->getBlockData();
        double *matrixC2 = resultC2Data->getBlockData();

        size_t heightA = finalMatrixData->getBlockSizeHeight();
        size_t widthA = finalMatrixData->getBlockSizeWidth();
        size_t heightB = resultC2Data->getBlockSizeHeight();
        size_t widthB = resultC2Data->getBlockSizeWidth();

        size_t ldcFinal = finalMatrixData->getLeadingDimension();
        size_t ldc2 = resultC2Data->getLeadingDimension();

        /// Adds all values within blocks together
        for (size_t i = 0; i < heightB; ++i) {
            for (size_t j = 0; j < widthB; ++j) {
                finalMatrixC[i * ldcFinal + j] = finalMatrixC[i * ldcFinal + j] + matrixC2[i * ldc2 + j];
            }
        }

         this->addResult(finalMatrixData);

        delete[] matrixC2;
    }

    std::shared_ptr<AbstractTask<MatrixBlockData<Type, OutputId>, AccumMatData<Type, OutputId>>> copy() {
        return std::make_shared<AddCTask>(this->name(), this->numberThreads());
    }
};
#endif //MATRIXLIB_ADDCTASK_H
