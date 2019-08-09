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
// Created by gck2 on 7/9/19.
//

#ifndef MATRIXLIB_INMEMORYLUDECOMPOSITIONGRAPH_H
#define MATRIXLIB_INMEMORYLUDECOMPOSITIONGRAPH_H

#include <cblas.h>
#include <hedgehog.h>
#include "../LuDecompNoPiv/LuDecompState/GaussElimState.h"
#include "../LuDecompNoPiv/LuDecompTasks/GaussElimTask.h"
#include "../LuDecompNoPiv/LuDecompTasks/TRSMTask.h"
#include "../LuDecompNoPiv/LuDecompTasks/MatMulTask.h"
#include "../LuDecompNoPiv/LuDecompState/TRSMState.h"
#include "../LuDecompNoPiv/LuDecompState/MatMulState.h"
#include "../LuDecompNoPiv/LuDecompState/OutputState.h"
#include "../LuDecompNoPiv/LuStateManagers/GaussElimStateManager.h"
#include "../LuDecompNoPiv/LuStateManagers/TRSMStateManager.h"
#include "../State/InputState.h"
#include "../BlockDecomposition/BlockDecompositionTask.h"


template <class Type, char OutputId = 's', Order Ord = Order::Row>

class InMemoryLUDecompositionGraph : public Graph<MatrixBlockData<Type, 's'>, MatrixBlockData<Type, 's'>>{
public:
    InMemoryLUDecompositionGraph(Type *matrix, size_t numThreads, size_t matrixHeight, size_t matrixWidth,
                                 size_t blockHeight, size_t blockWidth) : Graph<MatrixBlockData<Type, 's', Ord>, MatrixBlockData<Type, 's', Ord>>(), matrix_(matrix), numThreads_(numThreads),
                                                                          matrixHeight_(matrixHeight),
                                                                          matrixWidth_(matrixWidth),
                                                                          blockHeight_(blockHeight),
                                                                          blockWidth_(blockWidth) {

        openblas_set_num_threads(1);
        size_t ld = matrixWidth;

        size_t numBlocksRows = ceil(double(matrixHeight) / double(blockHeight));
        size_t numBlocksCols = ceil(double(matrixWidth) / double(blockWidth));

        size_t bld = numBlocksCols;

        gaussElimTask_ = std::make_shared<GaussElimTask<double>>("GaussElimTask", numThreads);
        trsmTask_ = std::make_shared<TRSMTask<double>>(matrixHeight, matrixWidth, "TRSMTask", numThreads);
        matMulTask_ = std::make_shared<MatMulTask<double>>("MatMulTask", numThreads);

        inputState_ = std::make_shared<InputState<double>>();
        gaussElimState_ = std::make_shared<GaussElimState<double>>(numBlocksRows, numBlocksCols, bld, blockHeight);
        trsmState_ = std::make_shared<TRSMState<double>>(numBlocksRows, numBlocksCols);
        matMulState_ = std::make_shared<MatMulState<double>>(numBlocksRows, numBlocksCols);
        outputState_ = std::make_shared<OutputState<double>>(numBlocksRows, numBlocksCols, bld);

        inputStateManager_ = std::make_shared<DefaultStateManager<MatrixBlockData<double, 's'>, MatrixBlockData<double, 's'>>>("InputStateManager", inputState_);
        gaussElimStateManager_ = std::make_shared<GaussElimStateManager<MatrixBlockData<double, 'g'>, MatrixBlockData<double, 's'>>>(gaussElimState_);
        trsmStateManager_ = std::make_shared<TRSMStateManager<TRSMData<double>, MatrixBlockData<double, 's'>, MatrixBlockData<double, 'g'>>>(trsmState_);
        matMulStateManager_ = std::make_shared<DefaultStateManager<UpdateData<double>, MatrixBlockData<double, 'f'>, MatrixBlockData<double, 's'>>>("MatMulStateManager", matMulState_);
        outputStateManager_  = std::make_shared<DefaultStateManager<MatrixBlockData<double, 's'>, MatrixBlockData<double, 's'>, MatrixBlockData<double, 'f'>, MatrixBlockData<double, 'g'>>>("OutputStateManager", outputState_);

        this->input(inputStateManager_);
        this->addEdge(inputStateManager_, gaussElimStateManager_);
        this->addEdge(inputStateManager_, trsmStateManager_);
        this->addEdge(inputStateManager_, matMulStateManager_);
        this->addEdge(gaussElimStateManager_, gaussElimTask_);
        this->addEdge(gaussElimTask_, outputStateManager_);
        this->addEdge(gaussElimTask_, trsmStateManager_);
        this->addEdge(trsmStateManager_, trsmTask_);
        this->addEdge(trsmTask_, matMulStateManager_);
        this->addEdge(trsmTask_, outputStateManager_);
        this->addEdge(matMulStateManager_, matMulTask_);
        this->addEdge(matMulTask_, outputStateManager_);
        this->addEdge(matMulTask_, gaussElimStateManager_);
        this->addEdge(matMulTask_, trsmStateManager_);
        this->output(outputStateManager_);
    }
private:
    Type* matrix_;

    size_t numThreads_ = 0;
    size_t matrixHeight_ = 0;
    size_t matrixWidth_ = 0;
    size_t blockHeight_ = 0;
    size_t blockWidth_ = 0;

    std::shared_ptr<GaussElimTask<double>> gaussElimTask_;
    std::shared_ptr<TRSMTask<double>> trsmTask_;
    std::shared_ptr<MatMulTask<double>> matMulTask_;

    std::shared_ptr<InputState<double>> inputState_;
    std::shared_ptr<GaussElimState<double>> gaussElimState_;
    std::shared_ptr<TRSMState<double>> trsmState_;
    std::shared_ptr<MatMulState<double>> matMulState_;
    std::shared_ptr<OutputState<double>> outputState_;

    std::shared_ptr<DefaultStateManager<MatrixBlockData<double, 's'>, MatrixBlockData<double, 's'>>> inputStateManager_;
    std::shared_ptr<GaussElimStateManager<MatrixBlockData<double, 'g'>, MatrixBlockData<double, 's'>>> gaussElimStateManager_;
    std::shared_ptr<TRSMStateManager<TRSMData<double>, MatrixBlockData<double, 's'>, MatrixBlockData<double, 'g'>>> trsmStateManager_;
    std::shared_ptr<DefaultStateManager<UpdateData<double>, MatrixBlockData<double, 'f'>, MatrixBlockData<double, 's'>>> matMulStateManager_;
    std::shared_ptr<DefaultStateManager<MatrixBlockData<double, 's'>, MatrixBlockData<double, 's'>, MatrixBlockData<double, 'f'>, MatrixBlockData<double, 'g'>>> outputStateManager_;
};


#endif //MATRIXLIB_INMEMORYLUDECOMPOSITIONGRAPH_H
