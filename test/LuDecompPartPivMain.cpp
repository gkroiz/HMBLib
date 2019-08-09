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

//#include <mutex>
//std::mutex mutex;
//#define PRINT(msg) { std::unique_lock<std::mutex> lk(mutex); std::cout << msg << std::flush; }

#include <hedgehog.h>
#include <cblas.h>
#include <lapacke.h>
#include <unistd.h>
#include <numeric>
#include <LuDecompPartPiv/LuDecompPartPivState/TRSMState.h>
#include <LuDecompPartPiv/LuDecompPartPivState/MatMulState.h>
#include <LuDecompPartPiv/LuDecompPartPivState/BlocktoPanelState.h>
#include <LuDecompPartPiv/LuDecompPartPivStateManagers/LASWPLeftStateManager.h>
#include <LuDecompPartPiv/LuDecompPartPivStateManagers/LASWPRightStateManager.h>
#include <LuDecompPartPiv/LuDecompPartPivStateManagers/GETRFStateManager.h>
#include <LuDecompPartPiv/LuDecompPartPivState/OutputState.h>
#include <LuDecompPartPiv/LuDecompPartPivTasks/MatMulTask.h>
#include <LuDecompPartPiv/LuDecompPartPivTasks/TRSMTask.h>
#include <LuDecompPartPiv/LuDecompPartPivTasks/LASWPRightTask.h>
#include <LuDecompPartPiv/LuDecompPartPivTasks/LASWPLeftTask.h>
#include <LuDecompPartPiv/LuDecompPartPivState/LASWPRightState.h>
#include <LuDecompPartPiv/LuDecompPartPivState/LASWPLeftState.h>
#include <LuDecompPartPiv/LuDecompPartPivTasks/GETRFTask.h>
#include <LuDecompPartPiv/LuDecompPartPivState/GETRFState.h>
#include <State/InputState.h>

static unsigned int g_seed = 1;

inline int fastrand() {
    g_seed = (214013 * g_seed + 2531011);
    return (g_seed >> 16) & 0x7FFF;
}

int main() {

    int n = 10;
//    openblas_set_num_threads(1);
    size_t numThreads = 8;
    size_t matrixHeight = 4096;
    size_t matrixWidth = 4096;

    size_t ld = matrixWidth;

    size_t blockHeight = matrixHeight;
    size_t blockWidth = 128;

    /// Only works for square matrices
    size_t numBlocksRows = ceil(double(matrixHeight) / double(blockHeight));
    size_t numBlocksCols = ceil(double(matrixWidth) / double(blockWidth));

    size_t bld = numBlocksCols;
    double *times = new double[n];
    double *flops = new double[n];
    double *firstBlock = new double[n];
    double *avgBlock = new double[n];

    std::cout << "BlockSize: " << blockWidth << "x" << blockWidth << std::endl;
    for (int i = 0; i < n; i++) {


        double *matrix = new double[matrixHeight * matrixWidth];

        /// To create a comparision matrix to test for accuracy
//        double *compareMat = new double[matrixHeight * matrixWidth];

        for (size_t i = 0.0; i < matrixHeight; i++) {
            for (size_t j = 0.0; j < matrixWidth; j++) {
                    matrix[i * matrixHeight + j] = fastrand();
                /// To set values for comparision matrix
//                compareMat[i * matrixHeight + j] = matrix[i * matrixHeight + j];
            }
        }

        auto getrfTask = std::make_shared<GETRFTask<double>>("GETRFTask", numThreads);
        auto lswapRightTask = std::make_shared<LASWPRightTask<double>>("LASWPRightTask", numThreads);

        auto lswapLeftTask = std::make_shared<LASWPLeftTask<double>>("LASWPLeftTask", numThreads);
        auto trsmTask = std::make_shared<TRSMTask<double>>("TRSMTask", numThreads);
        auto matMulTask = std::make_shared<MatMulTask<double>>("MatMulTask", numThreads);

        auto inputState = std::make_shared<InputState<double>>();
        auto getrfState = std::make_shared<GETRFState<double>>(numBlocksRows, numBlocksCols);
        auto lswapRightState = std::make_shared<LASWPRightState<double>>(numBlocksRows, numBlocksCols);
        auto lswapLeftState = std::make_shared<LASWPLeftState<double>>(numBlocksRows, numBlocksCols);
        auto trsmState = std::make_shared<TRSMState<double>>(numBlocksRows, numBlocksCols);
        auto matMulState = std::make_shared<MatMulState<double>>(numBlocksRows, numBlocksCols);
        auto blocktoPanelState = std::make_shared<BlocktoPanelState<double>>(numBlocksRows, numBlocksCols);
        auto outputState = std::make_shared<OutputState<double>>(numBlocksRows, numBlocksCols);


        auto inputStateManager = std::make_shared<DefaultStateManager<MatrixBlockData<double, 's'>, MatrixBlockData<double, 's'>>>(
                "InputStateManager", inputState);

        auto getrfStateManager = std::make_shared<GETRFStateManager<MatrixBlockData<double, 's'>, MatrixBlockData<double, 's'>>>(
                getrfState);

        auto lswapRightStateManager = std::make_shared<LASWPRightStateManager<LASWPData<double>, PermutedBlockData<double>, MatrixBlockData<double, 's'>>>(
                lswapRightState);

        auto lswapLeftStateManager = std::make_shared<LASWPLeftStateManager<IPivPermutedBlockData<double>, PermutedBlockData<double>, MatrixBlockData<double, 's'>, MatrixBlockData<double, 'l'>>>(
                lswapLeftState);

        auto trsmStateManager = std::make_shared<DefaultStateManager<TRSMData<double>, MatrixBlockData<double, 's'>, PermutedBlockData<double>>>(
                "TRSMStateManager", trsmState);

        auto matMulStateManager = std::make_shared<DefaultStateManager<UpdateData<double>, PermutedBlockData<double>, MatrixBlockData<double, 'f'>>>(
                "MatMulStateManager", matMulState);

        auto blocktoPanelStateManager = std::make_shared<DefaultStateManager<MatrixBlockData<double, 's'>, MatrixBlockData<double, 's'>>>(
                "BlocktoPanelStateManager", blocktoPanelState);

        auto outputStateManager = std::make_shared<DefaultStateManager<PermutedBlockData<double>, MatrixBlockData<double, 's'>, MatrixBlockData<double, 'l'>, MatrixBlockData<double, 'f'>, PermutedBlockData<double>>>(
                "OutputStateManager", outputState);

        auto taskGraph = std::make_shared<Graph<PermutedBlockData<double>, MatrixBlockData<double, 's'>>>();

        taskGraph->input(inputStateManager);
        taskGraph->addEdge(inputStateManager, getrfStateManager);
        taskGraph->addEdge(inputStateManager, lswapRightStateManager);
        taskGraph->addEdge(getrfStateManager, getrfTask);
        taskGraph->addEdge(getrfTask, lswapRightStateManager);
        taskGraph->addEdge(getrfTask, lswapLeftStateManager);
        taskGraph->addEdge(getrfTask, trsmStateManager);
        taskGraph->addEdge(getrfTask, matMulStateManager);
        taskGraph->addEdge(getrfTask, outputStateManager);
        taskGraph->addEdge(lswapLeftStateManager, lswapLeftTask);
        taskGraph->addEdge(lswapLeftTask, outputStateManager);
        taskGraph->addEdge(lswapLeftTask, lswapLeftStateManager);
        taskGraph->addEdge(lswapRightStateManager, lswapRightTask);
        taskGraph->addEdge(lswapRightTask, trsmStateManager);
        taskGraph->addEdge(trsmStateManager, trsmTask);
        taskGraph->addEdge(trsmTask, matMulStateManager);
        taskGraph->addEdge(trsmTask, outputStateManager);
        taskGraph->addEdge(matMulStateManager, matMulTask);

        /// Add in if you want panels for mat mul
//        taskGraph->addEdge(matMulTask, outputStateManager);
//        taskGraph->addEdge(matMulTask, outputStateManager);
//        taskGraph->addEdge(matMulTask, lswapRightStateManager);
//        taskGraph->addEdge(matMulTask, lswapLeftStateManager);
//        taskGraph->addEdge(matMulTask, getrfStateManager);


        /// Add in if you want to you blocks for mat mul
        taskGraph->addEdge(matMulTask, blocktoPanelStateManager);
        taskGraph->addEdge(blocktoPanelStateManager, outputStateManager);
        taskGraph->addEdge(blocktoPanelStateManager, lswapRightStateManager);
        taskGraph->addEdge(blocktoPanelStateManager, lswapLeftStateManager);
        taskGraph->addEdge(blocktoPanelStateManager, getrfStateManager);

        taskGraph->output(outputStateManager);



        auto start = std::chrono::high_resolution_clock::now();

        auto startFirstItem = std::chrono::high_resolution_clock::now();
        std::vector<double> getResultTimes;


        taskGraph->executeGraph();
        for (size_t col = 0; col < numBlocksCols; ++col) {
            double *startLocation = &(matrix[col * blockWidth]);
            taskGraph->pushData(
                    std::make_shared<MatrixBlockData<double, 's'>>(0, col, blockHeight, blockWidth, ld, matrix,
                                                                   startLocation));
        }

        taskGraph->finishPushingData();

        int count = 0;

        /// Full pivot vector
        lapack_int *matrixPVector = new lapack_int[matrixHeight];
        while (auto graphOutput = taskGraph->getBlockingResult()) {
            auto endFirstItem = std::chrono::high_resolution_clock::now();
            double timeFirst = std::chrono::duration_cast<std::chrono::milliseconds>(
                    endFirstItem - startFirstItem).count();
            getResultTimes.push_back(timeFirst);
            startFirstItem = std::chrono::high_resolution_clock::now();
            auto blockPVector = graphOutput->getPermutationVector().get();
            if (graphOutput->getPermutedBlock()->getRowIdx() == graphOutput->getPermutedBlock()->getColIdx()) {
                for (size_t i = 0; i < blockWidth; i++) {
                    matrixPVector[count + i] =
                            blockPVector[i] + graphOutput->getPermutedBlock()->getColIdx() * blockWidth;
                }
                count += blockWidth;
            }
        }

        taskGraph->waitForTermination();
        auto end = std::chrono::high_resolution_clock::now();

        taskGraph->createDotFile("LUDecompositionPartPiv.dot", ColorScheme::EXECUTION, StructureOptions::QUEUE);

        double timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        double numGflops = (2.0 / 3.0 * matrixWidth * matrixWidth * matrixWidth) * 1.0e-9;
        double gflops = numGflops / (timeMs / 1000.0);
        times[i] = (timeMs / 1000.0);

        flops[i] = gflops;

        firstBlock[i] = getResultTimes.at(0) / 1000.0;
//        std::cout << "Time for first element: " << firstBlock[i] << std::endl;

        double average = std::accumulate(getResultTimes.begin(), getResultTimes.end(), 0.0) / getResultTimes.size();

        avgBlock[i] = average / 1000.0;
        std::cout << "Average time per element: " << average / 1000.0 << std::endl;

        std::cout << "Time " << i << ": " << times[i] << std::endl;

        /// MatrixHeight is a filler for the same value
//        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, matrixHeight, matrixHeight, matrixHeight, 1.0,
//                    lowerMatrix, matrixHeight, upperMatrix,
//                    matrixHeight, 0.0, LxUMatrix, matrixHeight);

        /// Test for accuracy from LU Decomposition ///////////////////////////////////////////////////

        //    double *compareMat = new double[matrixHeight * matrixWidth];

//        LAPACKE_dgetf2(LAPACK_ROW_MAJOR, matrixHeight, matrixHeight, compareMat, ld, matrixPVector);

//        double *correctness = new double[matrixHeight * matrixWidth];
//        for (int i = 0; i < matrixHeight * matrixWidth; i++) {

//            auto diff = fabs(compareMat[i] - matrix[i]);
//            correctness[i] = 0;

        /// Determine epsilon ////////////////////////////////////////
//            if (diff > std::numeric_limits<double>::epsilon()) {
        /// Or ///////////////////////////////////////////////////////
//            if (diff > 0.1) {
        //////////////////////////////////////////////////////////////
//                correctness[i] = 1;
//            }
//        }

//        std::cout << "What is correct: 0 = correct, 1 = incorrect." << std::endl;
//        for (size_t i = 0; i < matrixHeight; ++i) {
//            for (size_t j = 0; j < matrixWidth; ++j) {
//                std::cout << correctness[i * matrixWidth + j] << " ";
//            }
//            std::cout << std::endl;
//        }
//        std::cout << std::endl;
//
//        delete[] correctness;

        //////////////////////////////////////////////////////////////////////////////////////////////////

        delete[] matrix;
        delete[] matrixPVector;
    }
    std::cout << std::endl;

    double timeSum = 0.0, flopSum = 0.0, firstBSum = 0.0, avgBSum = 0.0;
    double timeMean, flopMean, firstBMean, avgBMean;
    double timeVariance = 0.0, flopVariance = 0.0, firstBVariance = 0.0, avgBVariance = 0.0;
    double timeStdDeviation, flopStdDeviation, firstBStdDev, avgBStdDev;

    for (int i = 0; i < n; ++i) {
        timeSum += times[i];
        flopSum += flops[i];
        firstBSum += firstBlock[i];
        avgBSum += avgBlock[i];

    }
    timeMean = timeSum / n;
    flopMean = flopSum / n;
    firstBMean = firstBSum / n;
    avgBMean = avgBSum / n;

    for (int i = 0; i < n; ++i) {
        timeVariance += pow(times[i] - timeMean, 2);
        flopVariance += pow(flops[i] - flopMean, 2);
        firstBVariance += pow(firstBlock[i] - firstBMean, 2);
        avgBVariance += pow(avgBlock[i] - avgBMean, 2);

    }
    timeVariance = timeVariance / n;
    flopVariance = flopVariance / n;
    firstBVariance = firstBVariance / n;
    avgBVariance = avgBVariance / n;
    timeStdDeviation = sqrt(timeVariance);
    flopStdDeviation = sqrt(flopVariance);
    firstBStdDev = sqrt(firstBVariance);
    avgBStdDev = sqrt(avgBVariance);

    std::ofstream LuDecompStats;
    LuDecompStats.open ("LuDecompStats" + std::to_string(blockWidth) + "x" + std::to_string(blockWidth) + ".txt");
    LuDecompStats << "Times: | " << std::endl;

    for (int i = 0; i < n; ++i) {
        LuDecompStats << times[i] << std::endl;
    }
    LuDecompStats << std::endl;


    LuDecompStats << "First Block Times: | " << std::endl;

    for (int i = 0; i < n; ++i) {
        LuDecompStats << firstBlock[i] << std::endl;
    }
    LuDecompStats << std::endl;


    LuDecompStats << "Average Block Times: | " << std::endl;

    for (int i = 0; i < n; ++i) {
        LuDecompStats << avgBlock[i] << std::endl;
    }

    LuDecompStats << std::endl;
    LuDecompStats << std::endl;
    LuDecompStats << "Mean computation time: " << timeMean << std::endl;
    LuDecompStats << "Standard Deviation: " << timeStdDeviation << std::endl;

    LuDecompStats << std::endl;
    LuDecompStats << "Mean GFlops: " << flopMean  << std::endl;
    LuDecompStats << "Standard Deviation: " << flopStdDeviation  << std::endl;

    LuDecompStats << std::endl;
    LuDecompStats << "Mean First Block Time: " << firstBMean  << std::endl;
    LuDecompStats << "Standard Deviation: " << firstBStdDev  << std::endl;

    LuDecompStats << std::endl;
    LuDecompStats << "Mean Avg Block Time: " << avgBMean << std::endl;
    LuDecompStats << "Standard Deviation: " << avgBStdDev << std::endl;
    std::cout << "This Works" << std::endl;
    LuDecompStats.close();

    delete[] times;
    delete[] flops;
    delete[] firstBlock;
    delete[] avgBlock;

    ///If testing for accuracy, compareMat needs to be deleted
//    delete[] compareMat;


};