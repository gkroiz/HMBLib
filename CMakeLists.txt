# NIST-developed software is provided by NIST as a public service. You may use, copy and distribute copies of the
# software in any medium, provided that you keep intact this entire notice. You may improve, modify and create
# derivative works of the software or any portion of the software, and you may copy and distribute such modifications
# or works. Modified works should carry a notice stating that you changed the software and should note the date and
# nature of any such change. Please explicitly acknowledge the National Institute of Standards and Technology as the
# source of the software. NIST-developed software is expressly provided "AS IS." NIST MAKES NO WARRANTY OF ANY KIND,
# EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
# WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE
# CORRECTED. NIST DOES NOT WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE SOFTWARE OR THE RESULTS
# THEREOF, INCLUDING BUT NOT LIMITED TO THE CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE. You
# are solely responsible for determining the appropriateness of using and distributing the software and you assume
# all risks associated with its use, including but not limited to the risks and costs of program errors, compliance
# with applicable laws, damage to or loss of data, programs or equipment, and the unavailability or interruption of 
# operation. This software is not intended to be used in any situation where a failure could cause risk of injury or
# damage to property. The software developed by NIST employees is not subject to copyright protection within the
# United States.
cmake_minimum_required(VERSION 3.10)
#project(matrixLib VERSION 1.0 DESCRIPTION "Hedgehog Matrix Library")
project(matrixLib VERSION 1.0)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")


find_package(Hedgehog REQUIRED)
find_package(OpenBLAS REQUIRED)

set(API_HEADERS
        src/MatMul/MatMulData/MatrixMulData.hpp
        src/data/MatrixData_Old.hpp
        src/blasWrappers/matmul.hpp)

add_library(matrixLib SHARED
        src/data/MatrixData_Old.hpp
        src/MatMul/MatMulData/MatrixMulData.hpp
        src/blasWrappers/matmul.cpp
        src/blasWrappers/matmul.hpp
        src/data/MatrixRequestData.hpp
        src/Hadamard/HadamardTasks/InMemoryHadamardProductTask.cpp
        src/Hadamard/HadamardTasks/InMemoryHadamardProductTask.hpp
        src/data/HadamardData.hpp
        src/Hadamard/HadamardState/HadamardState.cpp
        src/Hadamard/HadamardState/HadamardState.hpp
        src/graphs/InMemoryHadamardGraph.cpp
        src/graphs/InMemoryHadamardGraph.hpp
        src/MatMul/MatMulManager/AccumlateStateManager.h
        src/MatMul/MatMulTasks/AddCTask.h
        src/MatMul/MatMulState/AddCState.h
        src/graphs/InMemoryMatrixMultiplicationGraph.cpp
        src/graphs/InMemoryMatrixMultiplicationGraph.h
        src/data/MatrixBlockData.h
        src/LuDecompNoPiv/LuDecompState/GaussElimState.h
        src/LuDecompNoPiv/LuDecompTasks/GaussElimTask.h
        src/LuDecompNoPiv/LuDecompState/TRSMState.h
        src/LuDecompNoPiv/LuDecompTasks/TRSMTask.h
        src/LuDecompNoPiv/LuDecompState/MatMulState.h
        src/LuDecompNoPiv/LuDecompTasks/MatMulTask.h
        src/LuDecompNoPiv/LuDecompState/OutputState.h
        src/LuDecompNoPiv/LuDecompData/TRSMData.h
        src/LuDecompNoPiv/LuDecompData/UpdateData.h
        src/LuDecompNoPiv/LuStateManagers/GaussElimStateManager.h
        src/LuDecompNoPiv/LuStateManagers/TRSMStateManager.h
        src/State/InputState.h
        src/BlockDecomposition/BlockDecompositionTask.h
        src/data/MatrixData.h
        src/graphs/InMemoryLUDecompositionGraph.cpp
        src/graphs/InMemoryLUDecompositionGraph.h src/LuDecompPartPiv/LuDecompPartPivData/LASWPData.h src/LuDecompPartPiv/LuDecompPartPivState/GETRFState.h src/LuDecompPartPiv/LuDecompPartPivTasks/GETRFTask.h src/LuDecompPartPiv/LuDecompPartPivState/LASWPRightState.h src/LuDecompPartPiv/LuDecompPartPivTasks/LASWPRightTask.h src/LuDecompPartPiv/LuDecompPartPivData/PermutedBlockData.h src/LuDecompPartPiv/LuDecompPartPivTasks/TRSMTask.h src/LuDecompPartPiv/LuDecompPartPivTasks/MatMulTask.h src/LuDecompPartPiv/LuDecompPartPivState/MatMulState.h src/LuDecompPartPiv/LuDecompPartPivState/TRSMState.h src/LuDecompPartPiv/LuDecompPartPivState/OutputState.h src/LuDecompPartPiv/LuDecompPartPivData/TRSMData.h src/LuDecompPartPiv/LuDecompPartPivData/UpdateData.h src/LuDecompPartPiv/LuDecompPartPivStateManagers/GETRFStateManager.h src/LuDecompPartPiv/LuDecompPartPivState/LASWPLeftState.h src/LuDecompPartPiv/LuDecompPartPivStateManagers/LASWPLeftStateManager.h src/LuDecompPartPiv/LuDecompPartPivTasks/LASWPLeftTask.h src/LuDecompPartPiv/LuDecompPartPivData/IPivPermutedBlockData.h src/LuDecompPartPiv/LuDecompPartPivState/BlocktoPanelState.h)


set_target_properties(matrixLib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(matrixLib PROPERTIES SOVERSION 1)

#set_target_properties(matrixLib PROPERTIES PUBLIC_HEADER ${API_HEADERS})


target_include_directories(matrixLib PUBLIC ${Hedgehog_INCLUDE_DIR})
target_link_libraries(matrixLib ${Hedgehog_LIBRARIES})

target_include_directories(matrixLib PUBLIC ${OpenBLAS_INCLUDE_DIRS})
target_link_libraries(matrixLib ${OpenBLAS_LIBRARIES})

### Include MKL

#LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/home/gck2/intel/mkl/include)
#
#include_directories(/home/gck2/intel/mkl/include)
#
#link_libraries(/home/gck2/intel/mkl/lib/intel64/libmkl_intel_ilp64.a)
#link_libraries(/home/gck2/intel/mkl/lib/intel64/libmkl_sequential.a)
#link_libraries(/home/gck2/intel/mkl/lib/intel64/libmkl_core.a)
#link_libraries(pthread m dl)


if (OpenBLAS_FOUND)
    target_compile_definitions(matrixLib PUBLIC -DHAVE_OPENBLAS)
endif()


### Include NVTX
#if(NVTX_FOUND)
#    message(STATUS "FOUND NVTX!")
#    add_definitions(-DUSE_NVTX)
#    include_directories(${NVTX_INCLUDE_DIR})
#    link_libraries(${NVTX_LIBRARIES})
#endif()

add_subdirectory(test)
