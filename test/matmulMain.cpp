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
#include <iostream>
#include <hedgehog.h>
#include <numeric>
#include <MatMul/MatMulState/MulBlockState.h>
#include <MatMul/MatMulTasks/AccumulateTask.h>
#include <MatMul/MatMulState/AccumulateState.h>
#include <MatMul/MatMulState/SendState.h>
#include <MatMul/MatMulTasks/MulBlockTask.h>
#include <MatMul/MatMulManager/AccumlateStateManager.h>
#include <MatMul/MatMulTasks/AddCTask.h>
#include <MatMul/MatMulState/AddCState.h>
#include <data/MatrixBlockData.h>
#include <random>

static unsigned int g_seed = 1;

inline int fastrand() {
    g_seed = (214013 * g_seed + 2531011);
    return (g_seed >> 16) & 0x7FFF;
}



int main() {

    // Mersenne Twister Random Generator
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>(uint64_t)32)};
    std::mt19937_64 rng(ss);
    std::uniform_real_distribution<double> unif(0, 1);

    int n = 10;
    double *times = new double[n];
    double *flops = new double[n];
    double *firstBlock = new double[n];
    double *avgBlock = new double[n];

//    openblas_set_num_threads(1);

    size_t matrixAHeight = 8192;
    size_t matrixAWidth = 8192;
    size_t matrixBHeight = 8192;
    size_t matrixBWidth = 8192;

    size_t lda = matrixAWidth;
    size_t ldb = matrixBWidth;

    size_t numThreads = 8;
    size_t blockHeight = 512;
    size_t blockWidth = 512;

    std::cout << "BlockSize: " << blockHeight << "x" << blockWidth << std::endl;
    for (int i = 0; i < n; i++) {
        size_t numBlocksARows = ceil(double(matrixAHeight) / double(blockHeight));
        size_t numBlocksACols = ceil(double(matrixAWidth) / double(blockWidth));

        size_t numBlocksBRows = ceil(double(matrixBHeight) / double(blockHeight));
        size_t numBlocksBCols = ceil(double(matrixBWidth) / double(blockWidth));

        double *matrixA = new double[matrixAWidth * matrixAHeight];
        double *matrixB = new double[matrixBWidth * matrixBHeight];
        double *matrixC = new double[matrixBWidth * matrixAHeight];

        /// To check the result for accuracy
        double *checkMatrix = new double[matrixAHeight * matrixBWidth];


        for (size_t i = 0; i < matrixAHeight * matrixAWidth; i++) {
            matrixA[i] = ran;
//            matrixA[i] = (rand() % 10) + 1;

        }
        for (size_t i = 0; i < matrixBHeight * matrixBWidth; i++) {
            matrixB[i] = fastrand();
//            matrixB[i] = (rand() % 10) + 1;

        }
        for (size_t i = 0; i < matrixAHeight * matrixBWidth; i++) {
            matrixC[i] = fastrand();
//            matrixC[i] = (rand() % 10) + 1;
            /// To set values for checkMatrix before the matrix multiplication operation
            checkMatrix[i] = matrixC[i];
        }
        

        auto mulBlockTask = std::make_shared<MulBlockTask<double, 'c'>>("MulBlockTask", numThreads);
        auto accumulateTask = std::make_shared<AccumulateTask<double, 'c'>>("AccumulateTask", numThreads);
        auto addCTask = std::make_shared<AddCTask<double, 'c'>>("AddCTask", numThreads);

        auto mulBlockState = std::make_shared<MulBlockState<double>>(numBlocksARows, numBlocksACols, numBlocksBRows,
                                                                     numBlocksBCols);
        auto accumulateState = std::make_shared<AccumulateState<double, 'c'>>(numBlocksARows, numBlocksBCols,
                                                                              numBlocksACols);
        auto sendState = std::make_shared<SendState<double, 'c'>>(numBlocksARows, numBlocksBCols, numBlocksACols);
        auto addCState = std::make_shared<AddCState<double, 'c'>>(matrixC, numBlocksARows, numBlocksBCols,
                                                                  matrixAHeight, matrixBWidth, blockHeight,
                                                                  blockWidth, ldb);

        auto mulBlockStateManager = std::make_shared<DefaultStateManager<MatrixMulData<double>, MatrixBlockData<double, 'a'>, MatrixBlockData<double, 'b'>>>(
                "MulBlockStateManager", mulBlockState);

        auto accumulateStateManager = std::make_shared<AccumulateStateManager<'c', AccumMatData<double, 'c'>, MatrixBlockData<double, 'c'>>>(
                accumulateState);

        auto sendStateManager = std::make_shared<DefaultStateManager<MatrixBlockData<double, 'c'>, MatrixBlockData<double, 'c'>>>(
                "SendStateManager", sendState);

        auto addCStateManager = std::make_shared<DefaultStateManager<AccumMatData<double, 'c'>, MatrixBlockData<double, 'c'>>>(
                "AddCStateManager", addCState);

        auto taskGraph = std::make_shared<Graph<MatrixBlockData<double, 'c'>, MatrixBlockData<double, 'a'>, MatrixBlockData<double, 'b'>>>();

        taskGraph->input(mulBlockStateManager);
        taskGraph->addEdge(mulBlockStateManager, mulBlockTask);
        taskGraph->addEdge(mulBlockTask, accumulateStateManager);
        taskGraph->addEdge(accumulateStateManager, accumulateTask);
        taskGraph->addEdge(accumulateTask, accumulateStateManager);
        taskGraph->addEdge(accumulateTask, sendStateManager);
        taskGraph->addEdge(mulBlockTask, sendStateManager);
        taskGraph->addEdge(sendStateManager, addCStateManager);
        taskGraph->addEdge(addCStateManager, addCTask);
        taskGraph->output(addCTask);


        auto start = std::chrono::high_resolution_clock::now();
        auto startFirstItem = std::chrono::high_resolution_clock::now();
        std::vector<double> getResultTimes;

        taskGraph->executeGraph();

        /// Decomposes matrix A into blocks and sends them into graph

        for (size_t row = 0; row < numBlocksARows; ++row) {
            for (size_t col = 0; col < numBlocksACols; ++col) {

                double *startLocationA = &(matrixA[row * blockHeight * lda + col * blockWidth]);
                size_t blockAHeight = blockHeight;
                size_t blockAWidth = blockWidth;

                /// Determines block size
                if (row == numBlocksARows - 1 && matrixAHeight % blockHeight != 0)
                    blockAHeight = matrixAHeight % blockHeight;
                if (col == numBlocksACols - 1 && matrixAWidth % blockWidth != 0)
                    blockAWidth = matrixAWidth % blockWidth;
                taskGraph->pushData(std::make_shared<MatrixBlockData<double, 'a'>>(
                        row,
                        col,
                        blockAHeight,
                        blockAWidth,
                        lda,
                        matrixA,
                        startLocationA));
            }
        }

        /// Decomposes matrix B into blocks and sends them into graph
        for (size_t col = 0; col < numBlocksBCols; ++col) {
            for (size_t row = 0; row < numBlocksBRows; ++row) {
                double *startLocationB = &(matrixB[row * blockHeight * ldb + col * blockWidth]);

                size_t blockBHeight = blockHeight;
                size_t blockBWidth = blockWidth;

                /// Determines block size
                if (row == numBlocksBRows - 1 && matrixBHeight % blockHeight != 0)
                    blockBHeight = matrixBHeight % blockHeight;
                if (col == numBlocksBCols - 1 && matrixBWidth % blockWidth != 0)
                    blockBWidth = matrixBWidth % blockWidth;
                taskGraph->pushData(std::make_shared<MatrixBlockData<double, 'b'>>(
                                row,
                                col,
                                blockBHeight,
                                blockBWidth,
                                ldb,
                                matrixB,
                                startLocationB));

            }
        }

        taskGraph->finishPushingData();
        while (auto matrix = taskGraph->getBlockingResult()) {
            auto endFirstItem = std::chrono::high_resolution_clock::now();
            double timeFirst = std::chrono::duration_cast<std::chrono::milliseconds>(
                    endFirstItem - startFirstItem).count();
            getResultTimes.push_back(timeFirst);
            startFirstItem = std::chrono::high_resolution_clock::now();
        }

        taskGraph->waitForTermination();
        auto end = std::chrono::high_resolution_clock::now();

        double timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        double numGflops = ((2.0 * matrixAWidth * matrixAHeight * matrixAWidth) + pow(matrixAHeight * matrixBWidth, 2)) * 1.0e-9;
        double gflops = numGflops / (timeMs / 1000.0);

        taskGraph->createDotFile("MatrixMultiplication.dot", ColorScheme::EXECUTION, StructureOptions::QUEUE);
        times[i] = (timeMs / 1000.0);
        flops[i] = gflops;

        firstBlock[i] = getResultTimes.at(0) / 1000.0;
        std::cout << "Time for first element: " << firstBlock[i] << std::endl;

        double average = std::accumulate(getResultTimes.begin(), getResultTimes.end(), 0.0) / getResultTimes.size();

        avgBlock[i] = average / 1000.0;
        std::cout << "Average time per element: " << average / 1000.0 << std::endl;

        std::cout << "Time " << i << ": " << times[i] << std::endl;



        /// This portion of code is designed to test for accuracy of matrix multiplication ///////////////////////
        
        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, matrixAHeight, matrixBWidth, matrixAWidth, 1.0,
             matrixA, lda, matrixB, ldb, 1.0, checkMatrix, ldb);
        for (int i = 0; i < matrixAHeight; i++){
            for (int j = 0; j < matrixBWidth; j++) {
                auto diff = fabs(checkMatrix[i * numBlocksBCols + j] - matrixC[i * numBlocksBCols + j]);
                if (diff > std::numeric_limits<double>::epsilon()) {
                    std::cerr << "Results are different at Row: " << i << ", Col: " << j << std::endl;
                }
            }
        }

        std::cout << "Matrix C: " << std::endl;
        for (size_t i = 0; i < matrixAHeight; ++i ) {
            for (size_t j = 0; j < matrixBWidth; ++j){
                std::cout <<  matrixC[i * matrixBWidth + j] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;

        std::cout << "Check Matrix: " << std::endl;
        for (size_t i = 0; i < matrixAHeight; ++i ) {
            for (size_t j = 0; j < matrixBWidth; ++j){
                std::cout <<  checkMatrix[i * matrixBWidth + j] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;

        delete[] checkMatrix;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////

        delete[] matrixA;
        delete[] matrixB;
        delete[] matrixC;

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

    std::ofstream matMulStats;
    matMulStats.open ("MatMulStats" + std::to_string(blockWidth) + "x" + std::to_string(blockWidth) + ".txt");

    matMulStats << "Times: | ";

    for (int i = 0; i < n; ++i) {
        matMulStats << times[i] << std::endl;
    }

    matMulStats << "First Block Times: | ";

    for (int i = 0; i < n; ++i) {
        matMulStats << firstBlock[i] << std::endl;
    }
    matMulStats << std::endl;


    matMulStats << "Average Block Times: | ";

    for (int i = 0; i < n; ++i) {
        matMulStats << avgBlock[i] << std::endl;
    }

    matMulStats << std::endl;
    matMulStats << std::endl;
    matMulStats << "Mean computation time: " << timeMean << "s" << std::endl;
    matMulStats << "Standard Deviation: " << timeStdDeviation << "s" << std::endl;

    matMulStats << std::endl;
    matMulStats << "Mean GFlops: " << flopMean << "GFlops" << std::endl;
    matMulStats << "Standard Deviation: " << flopStdDeviation << "GFlops" << std::endl;

    matMulStats << std::endl;
    matMulStats << "Mean First Block Time: " << firstBMean << "s" << std::endl;
    matMulStats << "Standard Deviation: " << firstBStdDev << "s" << std::endl;

    matMulStats << std::endl;
    matMulStats << "Mean Avg Block Time: " << avgBMean << "s" << std::endl;
    matMulStats << "Standard Deviation: " << avgBStdDev << "s" << std::endl;

    matMulStats << std::endl;

    matMulStats.close();

    delete[] times;
    delete[] flops;
    delete[] firstBlock;
    delete[] avgBlock;
}