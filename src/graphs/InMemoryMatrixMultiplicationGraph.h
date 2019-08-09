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

#ifndef MATRIXLIB_INMEMORYMATRIXMULTIPLICATIONGRAPH_H
#define MATRIXLIB_INMEMORYMATRIXMULTIPLICATIONGRAPH_H

#include <cblas.h>
#include <hedgehog.h>
#include "../MatMul/MatMulState/MulBlockState.h"
#include "../MatMul/MatMulTasks/AccumulateTask.h"
#include "../MatMul/MatMulState/AccumulateState.h"
#include "../MatMul/MatMulTasks/AccumulateTask.h"
#include "../MatMul/MatMulState/SendState.h"
#include "../MatMul/MatMulTasks/MulBlockTask.h"
#include "../MatMul/MatMulManager/AccumlateStateManager.h"
#include "../MatMul/MatMulTasks/AddCTask.h"
#include "../MatMul/MatMulState/AddCState.h"
#include "../MatMul/MatMulData/MatrixMulData.hpp"

///When using InMemoryMatrixMultiplicationGraph, the inner dimensions must be identical.

template <class Type, char OutputId, Order Ord = Order::Row>
class InMemoryMatrixMultiplicationGraph : public Graph<MatrixBlockData<Type, OutputId, Ord>, MatrixBlockData<Type, 'a', Ord>, MatrixBlockData<Type, 'b', Ord>> {
public:
    InMemoryMatrixMultiplicationGraph(Type* resultMatrix, size_t matrixAHeight, size_t matrixAWidth,
                                      size_t matrixBHeight, size_t matrixBWidth, size_t blockHeight, size_t blockWidth,
                                      size_t numThreads)
            : Graph<MatrixBlockData<Type, OutputId, Ord>, MatrixBlockData<Type, 'a', Ord>, MatrixBlockData<Type, 'b', Ord>>(),
              resultMatrix_(resultMatrix), matrixAHeight_(matrixAHeight),
              matrixAWidth_(matrixAWidth), matrixBHeight_(matrixBHeight),
              matrixBWidth_(matrixBWidth), blockHeight_(blockHeight),
              blockWidth_(blockWidth), numThreads_(numThreads) {


        size_t numBlocksARows = ceil(Type(matrixAHeight) / Type(blockHeight));
        size_t numBlocksACols = ceil(Type(matrixAWidth) / Type(blockWidth));

        size_t numBlocksBRows = ceil(Type(matrixBHeight) / Type(blockHeight));
        size_t numBlocksBCols = ceil(Type(matrixBWidth) / Type(blockWidth));

        openblas_set_num_threads(1);

        size_t lda = matrixAWidth;
        size_t ldb = matrixBWidth;


        mulBlockTask_ = std::make_shared<MulBlockTask<Type, OutputId>>("MulBlockTask", numThreads);
        accumulateTask_ = std::make_shared<AccumulateTask<Type, OutputId>>("AccumulateTask", numThreads);
        addCTask_ = std::make_shared<AddCTask<Type, OutputId>>("AddCTask", numThreads);

        mulBlockState_ = std::make_shared<MulBlockState<Type>>(numBlocksARows, numBlocksACols, numBlocksBRows, numBlocksBCols);
        accumulateState_ = std::make_shared<AccumulateState<Type, OutputId>>(numBlocksARows, numBlocksBCols, numBlocksACols);
        sendState_ = std::make_shared<SendState<Type, OutputId>>(numBlocksARows, numBlocksBCols, numBlocksACols);
        addCState_ = std::make_shared<AddCState<Type, OutputId>>(resultMatrix, numBlocksARows, numBlocksBCols, matrixAHeight, matrixBWidth, blockHeight, blockWidth, ldb);

        mulBlockStateManager_ = std::make_shared<DefaultStateManager<MatrixMulData<Type>, MatrixBlockData<Type, 'a'>, MatrixBlockData<Type, 'b'>>>("MulBlockStateManager", mulBlockState_);

        accumulateStateManager_ = std::make_shared<AccumulateStateManager<OutputId, AccumMatData<Type, OutputId>, MatrixBlockData<Type, OutputId>>>(accumulateState_);

        sendStateManager_ = std::make_shared<DefaultStateManager<MatrixBlockData<Type, OutputId>, MatrixBlockData<Type, OutputId>>>("SendStateManager", sendState_);

        addCStateManager_ = std::make_shared<DefaultStateManager<AccumMatData<Type, OutputId>, MatrixBlockData<Type, OutputId>>>("AddCStateManager", addCState_);


       this->input(mulBlockStateManager_);
       this->addEdge(mulBlockStateManager_, mulBlockTask_);
       this->addEdge(mulBlockTask_, accumulateStateManager_);
       this->addEdge(accumulateStateManager_, accumulateTask_);
       this->addEdge(accumulateTask_, accumulateStateManager_);
       this->addEdge(accumulateTask_, sendStateManager_);
       this->addEdge(mulBlockTask_, sendStateManager_);
       this->addEdge(sendStateManager_, addCStateManager_);
       this->addEdge(addCStateManager_, addCTask_);
       this->output(addCTask_);
}
private:
    Type* resultMatrix_;

    size_t matrixAHeight_ = 0;
    size_t matrixAWidth_ = 0;
    size_t matrixBHeight_ = 0;
    size_t matrixBWidth_ = 0;

    size_t blockHeight_ = 0;
    size_t blockWidth_ = 0;
    size_t numThreads_ = 0;

    std::shared_ptr<MulBlockTask<Type, OutputId>> mulBlockTask_;
    std::shared_ptr<AccumulateTask<Type, OutputId>> accumulateTask_;
    std::shared_ptr<AddCTask<Type, OutputId>> addCTask_;

    std::shared_ptr<MulBlockState<Type>> mulBlockState_;
    std::shared_ptr<AccumulateState<Type, OutputId>> accumulateState_;
    std::shared_ptr<SendState<Type, OutputId>> sendState_;
    std::shared_ptr<AddCState<Type, OutputId>> addCState_;

    std::shared_ptr<DefaultStateManager<MatrixMulData<Type>, MatrixBlockData<Type, 'a'>, MatrixBlockData<Type, 'b'>>> mulBlockStateManager_;
    std::shared_ptr<AccumulateStateManager<OutputId, AccumMatData<Type, OutputId>, MatrixBlockData<Type, OutputId>>> accumulateStateManager_;
    std::shared_ptr<DefaultStateManager<MatrixBlockData<Type, OutputId>, MatrixBlockData<Type, OutputId>>> sendStateManager_;
    std::shared_ptr<DefaultStateManager<AccumMatData<Type, OutputId>, MatrixBlockData<Type, OutputId>>> addCStateManager_;
};


#endif //MATRIXLIB_INMEMORYMATRIXMULTIPLICATIONGRAPH_H
