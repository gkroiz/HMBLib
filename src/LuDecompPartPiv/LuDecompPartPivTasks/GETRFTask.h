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

#ifndef MATRIXLIB_GETRFTASK_H
#define MATRIXLIB_GETRFTASK_H

#include <hedgehog.h>
#include <lapacke.h>
//#include <mkl.h>
#include "LuDecompPartPiv/LuDecompPartPivData/PermutedBlockData.h"

template <class Type>
class GETRFTask : public AbstractTask<PermutedBlockData<Type>, MatrixBlockData<Type, 's'>> {
public:
    GETRFTask(const std::string_view &name, size_t numberThreads) : AbstractTask<PermutedBlockData<Type>, MatrixBlockData<Type, 's'>>(name, numberThreads) {}

    void execute(std::shared_ptr<MatrixBlockData<Type, 's'>> ptr) override {
        ///only works for square matrices
        double *block = ptr->getBlockData();
        
        int blockHeight = ptr->getBlockSizeHeight();
        int blockWidth = ptr->getBlockSizeWidth();
        
        /// Permutation Vector
        auto ipiv = std::shared_ptr<lapack_int>(new lapack_int[blockWidth](), std::default_delete<lapack_int[]>());
        
        /// StartingLocation is not within the data structure
        double *blockStartingLocation = &(block[ptr->getRowIdx() * blockWidth * ptr->getLeadingDimension()]);
        
        LAPACKE_dgetrf(LAPACK_ROW_MAJOR, blockHeight, blockWidth, blockStartingLocation, ptr->getLeadingDimension(), ipiv.get());
        
        this->addResult(std::make_shared<PermutedBlockData<double>>(ptr, ipiv));
    }
    
    std::shared_ptr<AbstractTask<PermutedBlockData<Type>, MatrixBlockData<Type, 's'>>> copy() override {
        return std::make_shared<GETRFTask>(this->name(), this->numberThreads());
    }
};
#endif //MATRIXLIB_GETRFTASK_H
