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
// Created by tjb3 on 4/29/19.
//

#include <strings.h>
#include <cblas.h>
#include <hedgehog.h>
#include <MatMul/MatMulData/MatrixMulData.hpp>
#include <blasWrappers/matmul.hpp>
//#include <graphs/InMemoryHadamardGraph.hpp>
#include <graphs/InMemoryMatrixMultiplicationGraph.h>
#include "MatMul/MatMulState/MulBlockState.h"
#include "MatMul/MatMulTasks/AccumulateTask.h"
#include "MatMul/MatMulState/AccumulateState.h"
#include "MatMul/MatMulTasks/AccumulateTask.h"
#include "MatMul/MatMulState/SendState.h"
#include "BlockDecomposition/BlockDecompositionTask.h"
#include "MatMul/MatMulTasks/MulBlockTask.h"
#include "MatMul/MatMulManager/AccumlateStateManager.h"
#include "MatMul/MatMulTasks/AddCTask.h"
#include "MatMul/MatMulState/AddCState.h"
#include "MatMul/MatMulData/MatrixMulData.hpp"

std::string durationPrinter(uint64_t duration) {
    std::ostringstream oss;
    uint64_t
            s = (duration % 1000000000) / 1000000,
            mS = (duration % 1000000) / 1000,
            uS = (duration % 1000);

    if (s > 0) {
        oss << s << "." << std::setfill('0') << std::setw(3) << mS << "s";
    } else if (mS > 0) {
        oss << mS << "." << std::setfill('0') << std::setw(3) << uS << "ms";
    } else {
        oss << duration << "us";
    }
    return oss.str();
}

int main(int argc, char *argv[]) {

    int n = 10;
    double *times = new double[n];
    double *flops = new double[n];

    for (int i = 0; i < n; i++) {
//    size_t matrixAHeight = 1024;
//    size_t matrixAWidth = 1024;
//    size_t matrixBHeight = 1024;
//    size_t matrixBWidth = 1024;

//    openblas_set_num_threads(1);

//        size_t matrixAHeight = 5;
//        size_t matrixAWidth = 4;
//        size_t matrixBHeight = 4;
//        size_t matrixBWidth = 6;
//
//        size_t blockHeight = 3;
//        size_t blockWidth = 3;
//        size_t numThreads = 1;

        size_t matrixAHeight = 5;
        size_t matrixAWidth = 4;
        size_t matrixBHeight = 4;
        size_t matrixBWidth = 4;

        size_t blockHeight = 2;
        size_t blockWidth = 2;
        size_t numThreads = 1;

        size_t numBlocksARows = ceil(double(matrixAHeight) / double(blockHeight));
        size_t numBlocksACols = ceil(double(matrixAWidth) / double(blockWidth));

        size_t numBlocksBRows = ceil(double(matrixBHeight) / double(blockHeight));
        size_t numBlocksBCols = ceil(double(matrixBWidth) / double(blockWidth));

        size_t lda = matrixAWidth;
        size_t ldb = matrixBWidth;

        double *matrixA = new double[matrixAHeight * matrixAWidth];
        double *matrixB = new double[matrixBHeight * matrixBWidth];
        double *matrixC = new double[matrixAHeight * matrixBWidth];

//    for (size_t i = 0; i < matrixAHeight * matrixBWidth; i++) {
//        matrixA[i] = 2.0;
//        matrixB[i] = 2.0;
//        matrixC[i] = 3.0;
//    }

        for (size_t i = 0; i < ceil(double(matrixAHeight) * double(matrixAWidth) / 2.0); i++) {
            matrixA[i] = 3;
        }

        for (size_t i = ceil(double(matrixAHeight) * double(matrixAWidth) / 2.0);
             i < matrixAHeight * matrixAWidth; i++) {
            matrixA[i] = 2;
        }

        for (size_t i = 0; i < ceil(double(matrixBHeight) * double(matrixBWidth) / 2.0); i++) {
            matrixB[i] = 5;
        }

        for (size_t i = ceil(double(matrixBHeight) * double(matrixBWidth) / 2.0);
             i < matrixBHeight * matrixBWidth; i++) {
            matrixB[i] = 4;
        }

        for (size_t i = 0; i < matrixAHeight * matrixBWidth; i++) {
            matrixC[i] = 1;
        }

        std::cout << "Matrix A: " << std::endl;
        for (size_t i = 0; i < matrixAHeight; ++i ) {
            for (size_t j = 0; j < matrixAWidth; ++j){
                std::cout <<  *(matrixA + (i * matrixAWidth + j)) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;

        std::cout << "Matrix B: " << std::endl;
        for (size_t i = 0; i < matrixBHeight; ++i ) {
            for (size_t j = 0; j < matrixBWidth; ++j){
                std::cout <<  *(matrixB + (i * matrixBWidth + j)) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;

        std::cout << "Matrix C: " << std::endl;
        for (size_t i = 0; i < matrixAHeight; ++i ) {
            for (size_t j = 0; j < matrixBWidth; ++j){
                std::cout <<  *(matrixC + (i * matrixBWidth + j)) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;


//        auto blockDecompositionTask = std::make_shared<BlockDecompositionTask<double, 'a'>>("BlockDecomposiitonTask", numThreads);
        auto matMulGraph = std::make_shared<InMemoryMatrixMultiplicationGraph<double, 'c'>>(matrixC, matrixAHeight, matrixAWidth,
                                                                            matrixBHeight, matrixBWidth, blockHeight,
                                                                            blockWidth, numThreads);

//        auto totalGraph = std::make_shared<Graph<MatrixBlockData()
        auto start = std::chrono::high_resolution_clock::now();
        
        matMulGraph->executeGraph();

        // Send matrix A
        for (size_t col = 0; col < numBlocksACols; ++col) {
            for (size_t row = 0; row < numBlocksARows; ++row) {
                double *startLocationA = &(matrixA[row * blockHeight * lda + col * blockWidth]);
                size_t blockAHeight = blockHeight;
                size_t blockAWidth = blockWidth;

                if (row == numBlocksARows - 1 && matrixAHeight % blockHeight != 0)
                    blockAHeight = matrixAHeight % blockHeight;
                if (col == numBlocksACols - 1 && matrixAWidth % blockWidth != 0)
                    blockAWidth = matrixAWidth % blockWidth;

                matMulGraph->pushData(
                        std::make_shared<MatrixBlockData<double, 'a'>>(row, col, blockAHeight, blockAWidth, lda,
                                                                       matrixA, startLocationA));
            }
        }


        // Send matrix B
        for (size_t row = 0; row < numBlocksBRows; ++row) {
            for (size_t col = 0; col < numBlocksBCols; ++col) {
                double *startLocationB = &(matrixB[row * blockHeight * ldb + col * blockWidth]);
                size_t blockBHeight = blockHeight;
                size_t blockBWidth = blockWidth;

                if (row == numBlocksBRows - 1 && matrixBHeight % blockHeight != 0)
                    blockBHeight = matrixBHeight % blockHeight;
                if (col == numBlocksBCols - 1 && matrixBWidth % blockWidth != 0)
                    blockBWidth = matrixBWidth % blockWidth;

                matMulGraph->pushData(std::make_shared<MatrixBlockData<double, 'b'>>(row, col, blockBHeight, blockBWidth, ldb,
                                                                       matrixB, startLocationB));
            }
        }

        matMulGraph->finishPushingData();
        matMulGraph->waitForTermination();

        auto end = std::chrono::high_resolution_clock::now();

        std::cout << "Matrix C: " << std::endl;
        for (size_t i = 0; i < matrixAHeight; ++i ) {
            for (size_t j = 0; j < matrixBWidth; ++j){
                std::cout <<  *(matrixC + (i * matrixBWidth + j)) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;

        double timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        double numGflops = (2.0 * matrixAWidth * matrixAHeight * matrixAWidth) * 1.0e-9;
        double gflops = numGflops / (timeMs / 1000.0);
        std::cout << "Finished processing " << matrixAWidth << "x" << matrixAHeight << " in " << (timeMs / 1000.0)
                  << "s, " << gflops << "GFlops" << std::endl;

        matMulGraph->createDotFile("MatrixMultiplication.dot", ColorScheme::EXECUTION, StructureOptions::NONE);


        times[i] = (timeMs / 1000.0);
        flops[i] = gflops;

        delete[] matrixA;
        delete[] matrixB;
        delete[] matrixC;
    }
    std::cout << std::endl;

    double timeSum = 0.0, flopSum = 0.0, timeMean, flopMean, timeVariance = 0.0, flopVariance = 0.0, timeStdDeviation, flopStdDeviation;

    for (int i = 0; i < n; ++i) {
        timeSum += times[i];
        flopSum += flops[i];

    }
    timeMean = timeSum / n;
    flopMean = flopSum / n;

    for (int i = 0; i < n; ++i) {
        timeVariance += pow(times[i] - timeMean, 2);
        flopVariance += pow(flops[i] - flopMean, 2);

    }
    timeVariance = timeVariance / n;
    flopVariance = flopVariance / n;
    timeStdDeviation = sqrt(timeVariance);
    flopStdDeviation = sqrt(flopVariance);

    std::cout << "Times: | ";

    for (int i = 0; i < n; ++i) {
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

//int main(int argc, char *argv[]) {
//  int width = 4096;
//  int height = 4096;
//  int numReplication = 1;
//  std::ofstream outputFile = std::ofstream("result_hadamard_hedgehog.csv");
//  outputFile << "blockSize";
//
//  for(int replication = 0; replication < numReplication; ++replication){
//    outputFile << "," << replication;
//  }
//  outputFile << std::endl;
//  outputFile.flush();
//
//  std::vector<int> blockSizes = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
//
//  for (auto blockSize : blockSizes) {
//    outputFile << blockSize << ",";
//    for (int replication = 0; replication < numReplication; ++replication) {
//      std::ostringstream graphName;
//      graphName << "graph_" << blockSize << "_" << replication << ".dot";
//
//      auto a = std::shared_ptr<double>(new double[width * height], std::default_delete<double[]>());
//      auto b = std::shared_ptr<double>(new double[width * height], std::default_delete<double[]>());
//      auto c = std::shared_ptr<double>(new double[width * height], std::default_delete<double[]>());
//
//      bzero(a.get(), sizeof(double) * width * height);
//      bzero(b.get(), sizeof(double) * width * height);
//      bzero(c.get(), sizeof(double) * width * height);
//
//      auto aData = std::make_shared<MatrixData_Old<double, 'a'>>(0, 0, a, width, height, width);
//      auto bData = std::make_shared<MatrixData_Old<double, 'b'>>(0, 0, b, width, height, width);
//      auto cData = std::make_shared<MatrixData_Old<double, 'c'>>(0, 0, c, width, height, width);
//
//      auto dData = std::make_shared<MatrixData_Old<double, 'd'>>(aData);
//
//      std::shared_ptr<InMemoryHadamardGraph<double>>
//          hadGraph = std::make_shared<InMemoryHadamardGraph<double>>(c, width, width, height, blockSize, 1);
//
//      std::chrono::time_point<std::chrono::high_resolution_clock>
//          start,
//          finish;
//      std::chrono::duration<uint64_t, std::micro> sum = std::chrono::duration<uint64_t, std::micro>::zero();
//
//      start = std::chrono::high_resolution_clock::now();
//
//      hadGraph->executeGraph();
//
//      auto numRows = static_cast<size_t>(ceil(static_cast<double>(height) / blockSize));
//      auto numCols = static_cast<size_t>(ceil(static_cast<double>(width) / blockSize));
//
//      for (int i = 0; i < numRows; ++i) {
//        for (int j = 0; j < numCols; ++j) {
//          auto matDataA = std::make_shared<MatrixData_Old<double, 'a'>>(i, j, a, blockSize, blockSize, width);
//          auto matDataB = std::make_shared<MatrixData_Old<double, 'b'>>(i, j, b, blockSize, blockSize, width);
//
//          hadGraph->pushData(matDataA);
//          hadGraph->pushData(matDataB);
//        }
//      }
//
//      hadGraph->finishPushingData();
//
//      int count = 0;
//      while (auto result = hadGraph->getBlockingResult()) {
//        count++;
//      }
//
//      hadGraph->waitForTermination();
//      finish = std::chrono::high_resolution_clock::now();
//      outputFile << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << ",";
//      hadGraph->createDotFile(graphName.str(), ColorScheme::EXECUTION, StructureOptions::ALL);
//    }
//    outputFile << std::endl;
//  }
//  outputFile.close();
//}
