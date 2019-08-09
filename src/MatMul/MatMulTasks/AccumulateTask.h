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

#ifndef MATRIXMULTIPLICATION_ACCUMULATETASK_H
#define MATRIXMULTIPLICATION_ACCUMULATETASK_H

#include <hedgehog.h>
#include <data/MatrixBlockData.h>
#include <data/AccumMatData.h>
template <class Type, char OutputId>
class AccumulateTask : public AbstractTask <MatrixBlockData<Type, OutputId>, AccumMatData<Type, OutputId>> {
public:
    AccumulateTask(const std::string_view &name, size_t numberThreads) : AbstractTask<MatrixBlockData<Type, OutputId>, AccumMatData<Type, OutputId>>(name, numberThreads) {}
    void execute(std::shared_ptr<AccumMatData<Type, OutputId>> data) override {
        auto matrixC1Data = data->getMatrixC1();
        auto matrixC2Data = data->getMatrixC2();

        double *matrixC1 =  matrixC1Data->getBlockData();
        double *matrixC2 = matrixC2Data->getBlockData();

        size_t heightC1 = matrixC1Data->getBlockSizeHeight();
        size_t widthC2 = matrixC2Data->getBlockSizeWidth();

        /// Adds each value within the two blocks together
        for (size_t i = 0; i < heightC1 * widthC2; ++i){
           matrixC1[i] = matrixC1[i] + matrixC2[i];
        }

        delete[] matrixC2;
        this->addResult(matrixC1Data);
    }

    std::shared_ptr<AbstractTask<MatrixBlockData<Type, OutputId>, AccumMatData<Type, OutputId>>> copy() {
        return std::make_shared<AccumulateTask>(this->name(), this->numberThreads());
    }

};
#endif //MATRIXMULTIPLICATION_ACCUMULATETASK_H
