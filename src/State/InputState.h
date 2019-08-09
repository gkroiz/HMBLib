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
// Created by gck2 on 7/2/19.
//

#ifndef MATRIXLIB_INPUTSTATE_H
#define MATRIXLIB_INPUTSTATE_H

#include <hedgehog.h>
#include "../data/MatrixBlockData.h"
template <class Type>
class InputState : public AbstractState<MatrixBlockData<Type, 's'>, MatrixBlockData<Type, 's'>> {
public:
    void execute(std::shared_ptr<MatrixBlockData<Type, 's'>> ptr) override {

//        std::cout << "Row: " << ptr->getRowIdx() << ", Col: " << ptr->getColIdx() << std::endl;
//        std::cout << "Matrix from Input State: " << std::endl;
//        std::cout << "PTR full data: " << *(ptr->getFullMatrixData()) << ", PTR block data: " << *(ptr->getBlockData()) << std::endl;
//        for (size_t i = 0; i < ptr->getLeadingDimension(); ++i ) {
//            for (size_t j = 0; j < ptr->getLeadingDimension(); ++j){
//                std::cout <<  ptr->getFullMatrixData()[(i * ptr->getLeadingDimension() + j)] << " ";
//            }
//            std::cout << std::endl;
//        }
//        std::cout << std::endl;

        this->push(ptr);
    }
};
#endif //MATRIXLIB_INPUTSTATE_H
