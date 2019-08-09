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

#include <mutex>
std::mutex mutex;

#define PRINT(msg) { std::unique_lock<std::mutex> lk(mutex); std::cout << msg << std::flush; }

#include <cblas.h>
#include <lapacke.h>
#include <hedgehog.h>
#include <unistd.h>
#include <numeric>

#include"LuDecompNoPiv/LuDecompState/GaussElimState.h"
#include"LuDecompNoPiv/LuDecompTasks/GaussElimTask.h"
#include"LuDecompNoPiv/LuDecompTasks/TRSMTask.h"
#include"LuDecompNoPiv/LuDecompTasks/MatMulTask.h"
#include"LuDecompNoPiv/LuDecompState/TRSMState.h"
#include"LuDecompNoPiv/LuDecompState/MatMulState.h"
#include"LuDecompNoPiv/LuDecompState/OutputState.h"
#include"LuDecompNoPiv/LuStateManagers/GaussElimStateManager.h"
#include"LuDecompNoPiv/LuStateManagers/TRSMStateManager.h"
#include"State/InputState.h"
#include"BlockDecomposition/BlockDecompositionTask.h"
#include "data/MatrixBlockData.h"

static unsigned int g_seed = 1;

inline int fastrand() {
    g_seed = (214013 * g_seed + 2531011);
    return (g_seed >> 16) & 0x7FFF;
}

int main() {

    int n = 10;

    double *times = new double[n];
    double *flops = new double[n];

    for (int i = 0; i < n; i++) {
        openblas_set_num_threads(1);
        size_t numThreads = 28;
        size_t matrixHeight = 32768;
        size_t matrixWidth = 32768;

        size_t ld = matrixWidth;

        size_t blockHeight = matrixHeight;
        size_t blockWidth = 1024;


        size_t numBlocksRows = ceil(double(matrixHeight) / double(blockHeight));
        size_t numBlocksCols = ceil(double(matrixWidth) / double(blockWidth));

        size_t bld = numBlocksCols;

        double *matrix = new double[matrixHeight * matrixWidth];

        /// Comparision matrix to check for accuracy of computation
//        double *compareMat = new double[matrixHeight * matrixWidth];


        for (size_t i = 0; i < matrixHeight; i++) {
        for (size_t j = 0; j < matrixWidth; j++) {
            matrix[i * matrixHeight + j] = i * matrixHeight + j + 1;

            /// Sets values of comparision matrix
//            compareMat[i * matrixHeight + j] = matrix[i * matrixHeight + j];
        }
    }
    for (size_t i = 0; i < matrixHeight; i++) {
        for (size_t j = 0; j < matrixWidth; j++) {
            if (i == j){
                matrix[i * matrixHeight + j] = matrixHeight * matrixHeight + 1;

                /// Sets values for comparision matrix
//                compareMat[i * matrixHeight + j] = matrix[i * matrixHeight + j];

            }
        }
    }


        auto gaussElimTask = std::make_shared<GaussElimTask<double>>("GaussElimTask", numThreads);
        auto trsmTask = std::make_shared<TRSMTask<double>>("TRSMTask", numThreads);
        auto matMulTask = std::make_shared<MatMulTask<double>>("MatMulTask", numThreads);

        auto inputState = std::make_shared<InputState<double>>();
        auto gaussElimState = std::make_shared<GaussElimState<double>>(numBlocksRows, numBlocksCols);
        auto trsmState = std::make_shared<TRSMState<double>>(numBlocksRows, numBlocksCols);

        auto matMulState = std::make_shared<MatMulState<double>>();
        auto outputState = std::make_shared<OutputState<double>>(numBlocksRows, numBlocksCols);

        auto inputStateManager = std::make_shared<DefaultStateManager<MatrixBlockData<double, 's'>, MatrixBlockData<double, 's'>>>(
                "InputStateManager", inputState);

        auto gaussElimStateManager = std::make_shared<GaussElimStateManager<MatrixBlockData<double, 'g'>, MatrixBlockData<double, 's'>>>(
                gaussElimState);

        auto trsmStateManager = std::make_shared<TRSMStateManager<TRSMData<double>, MatrixBlockData<double, 's'>, MatrixBlockData<double, 'g'>>>(
                trsmState);

        auto matMulStateManager = std::make_shared<DefaultStateManager<TRSMData<double>, TRSMData<double>>>(
                "MatMulStateManager", matMulState);

        auto outputStateManager = std::make_shared<DefaultStateManager<MatrixBlockData<double, 's'>, MatrixBlockData<double, 's'>, TRSMData<double>, MatrixBlockData<double, 'g'>>>(
                "OutputStateManager", outputState);

        auto taskGraph = std::make_shared<Graph<MatrixBlockData<double, 's'>, MatrixBlockData<double, 's'>>>();

        taskGraph->input(inputStateManager);
        taskGraph->addEdge(inputStateManager, gaussElimStateManager);
        taskGraph->addEdge(inputStateManager, trsmStateManager);
        taskGraph->addEdge(gaussElimStateManager, gaussElimTask);
        taskGraph->addEdge(gaussElimTask, outputStateManager);
        taskGraph->addEdge(gaussElimTask, trsmStateManager);
        taskGraph->addEdge(trsmStateManager, trsmTask);
        taskGraph->addEdge(trsmTask, matMulStateManager);
        taskGraph->addEdge(trsmTask, outputStateManager);
        taskGraph->addEdge(matMulStateManager, matMulTask);
        taskGraph->addEdge(matMulTask, outputStateManager);
        taskGraph->addEdge(matMulTask, gaussElimStateManager);
        taskGraph->addEdge(matMulTask, trsmStateManager);
        taskGraph->output(outputStateManager);

        auto start = std::chrono::high_resolution_clock::now();


        taskGraph->executeGraph();

        /// Sends in panels
        for (size_t col = 0; col < numBlocksCols; ++col) {
            double *startLocation = &(matrix[col * blockWidth]);
            taskGraph->pushData(
                    std::make_shared<MatrixBlockData<double, 's'>>(0, col, blockHeight, blockWidth, ld, matrix,
                                                                   startLocation));
        }

        taskGraph->finishPushingData();
        taskGraph->waitForTermination();

        auto end = std::chrono::high_resolution_clock::now();

        /// To create sepate matrix to compare with /////////////////////////////////////////////////////////////
//        lapack_int *iPiv = new lapack_int[matrixHeight];
//        LAPACKE_dgetrf(LAPACK_ROW_MAJOR, matrixHeight, matrixHeight, compareMat, ld, iPiv);
//
//        std::cout << "Compare Mat: " << std::endl;
//        for (size_t i = 0; i < matrixHeight; ++i) {
//            for (size_t j = 0; j < matrixWidth; ++j) {
//                std::cout << compareMat[(i * matrixWidth + j)] << " ";
//            }
//            std::cout << std::endl;
//        }
//        std::cout << std::endl;
//
//        std::cout << "Compare Piv Vector: " << std::endl;
//        for (int j = 0; j < matrixHeight; j++){
//            std::cout << iPiv[j] << std::endl;
//        }
//        std::cout << std::endl;
//
//        std::cout << "Right v Wrong: " << std::endl;
//        for (size_t i = 0; i < matrixHeight; ++i) {
//            for (size_t j = 0; j < matrixWidth; ++j) {
                /// Margin of error can be changed
//                if (abs(compareMat[i * matrixWidth + j] - matrix[i * matrixWidth + j]) > 0.1) {
//                    std::cout << "1 ";
//                }
//                else{
//                    std::cout << "0 ";
//                }
//            }
//            std::cout << std::endl;
//        }
//        std::cout << std::endl;
//
//        for (size_t i = 0; i < matrixHeight; ++i) {
//            for (size_t j = 0; j < matrixWidth; ++j) {
//                if (abs(compareMat[i * matrixWidth + j] - matrix[i * matrixWidth + j]) > 0.1) {
//                    std::cout << "DIFFERENCE | Row: " << i << ", Col: " << j << std::endl;
//                    std::cout << compareMat[(i * matrixWidth + j)] << " ";
//                    std::cout << matrix[(i * matrixWidth + j)] << " ";
//                    std::cout << std::endl;
//                }
//            }
//        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////

        double timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        double numGflops = (2.0 * matrixHeight * matrixHeight * matrixWidth) * 1.0e-9;
        double gflops = numGflops / (timeMs / 1000.0);

        taskGraph->createDotFile("LUDecompositionNoPiv.dot", ColorScheme::EXECUTION, StructureOptions::NONE);

        times[i] = (timeMs / 1000.0);
        flops[i] = gflops;
        std::cout << "Time: " << times[i] << std::endl;

        delete[] matrix;
        /// Delete if using comparision matrix
//        delete[] iPiv;
//        delete[] compareMat;
    }

    std::cout << std::endl;

    double timeSum = 0.0, flopSum = 0.0, timeMean, flopMean, timeVariance = 0.0, flopVariance = 0.0, timeStdDeviation, flopStdDeviation;

    for(int i = 0; i < n; ++i) {
        timeSum += times[i];
        flopSum += flops[i];

    }
    timeMean = timeSum/n;
    flopMean = flopSum/n;

    for(int i = 0; i < n; ++i) {
        timeVariance += pow(times[i] - timeMean, 2);
        flopVariance += pow(flops[i] - flopMean, 2);

    }
    timeVariance=timeVariance/n;
    flopVariance=flopVariance/n;
    timeStdDeviation = sqrt(timeVariance);
    flopStdDeviation = sqrt(flopVariance);

    std::cout<<"Times: | ";

    for(int i = 0; i < n; ++i) {
        std::cout << times[i] << "s | ";
    }

    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "Mean computation time: " << timeMean << "s" << std::endl;
    std::cout << "Standard Deviation: " << timeStdDeviation << "s" << std::endl;

    std::cout << std::endl;
    std::cout << "Mean GFlops: " << flopMean << "GFlops" << std::endl;
    std::cout << "Standard Deviation: " << flopStdDeviation << "GFlops" << std::endl;


    delete[] times;
    delete[] flops;
}