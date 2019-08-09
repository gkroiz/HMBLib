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

include_directories(../src)

add_executable(main main.cpp ../src/data/MatrixBlockData.h)
target_link_libraries(main ${OpenBLAS_LIBRARIES})
target_include_directories(main PUBLIC ${OpenBLAS_INCLUDE_DIRS})

### Include NVTX
#find_package(NVTX QUIET)

#includes OpenBLAS
#include_directories(/opt/OpenBLAS/include)
#link_libraries(/opt/OpenBLAS/lib/libopenblas.so)
#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-fsanitize=address")

add_executable(MatMul matmulMain.cpp
        ../src/MatMul/MatMulState/AccumulateState.h
        ../src/MatMul/MatMulState/MulBlockState.h
        ../src/MatMul/MatMulTasks/MulBlockTask.h
        ../src/MatMul/MatMulTasks/AccumulateTask.h
        ../src/MatMul/MatMulState/SendState.h
        ../src/data/MatrixBlockData.h)

add_executable(LuDecompNoPiv LuDecompNoPivMain.cpp
        ../src/LuDecompNoPiv/LuDecompState/GaussElimState.h
        ../src/LuDecompNoPiv/LuDecompTasks/GaussElimTask.h
        ../src/LuDecompNoPiv/LuDecompTasks/TRSMTask.h
        ../src/LuDecompNoPiv/LuDecompTasks/MatMulTask.h
        ../src/LuDecompNoPiv/LuDecompState/TRSMState.h
        ../src/LuDecompNoPiv/LuDecompState/MatMulState.h
        ../src/LuDecompNoPiv/LuDecompState/OutputState.h
        ../src/data/MatrixBlockData.h)

add_executable(LuDecompPartPivot LuDecompPartPivMain.cpp)

target_include_directories(MatMul PUBLIC ${Hedgehog_INCLUDE_DIR})
target_link_libraries(MatMul ${Hedgehog_LIBRARIES})

target_include_directories(LuDecompNoPiv PUBLIC ${Hedgehog_INCLUDE_DIR})

target_link_libraries(LuDecompNoPiv ${OpenBLAS_LIBRARIES})
target_include_directories(LuDecompNoPiv PUBLIC ${OpenBLAS_INCLUDE_DIRS})

target_include_directories(LuDecompPartPivot PUBLIC ${Hedgehog_INCLUDE_DIR})

target_link_libraries(LuDecompPartPivot ${OpenBLAS_LIBRARIES})
target_include_directories(LuDecompPartPivot PUBLIC ${OpenBLAS_INCLUDE_DIRS})

### include NVTX
#if (NVTX_FOUND)
#    message(STATUS "NVTX Enabled")
#    target_compile_definitions(LuDecompPartPivot PUBLIC -DHH_USE_NVTX)
#    target_include_directories(LuDecompPartPivot PUBLIC ${NVTX_INCLUDE_DIR})
#    target_link_libraries(LuDecompPartPivot ${NVTX_LIBRARIES})
#endif()

#target_link_libraries(LuDecompPartPivot ${NVTX_INCLUDE_DIRS})
#target_include_directories(LuDecompPartPivot PUBLIC ${NVTX_INCLUDE_DIRS})


target_link_libraries(main matrixLib)
target_link_libraries(MatMul matrixLib)
target_link_libraries(LuDecompNoPiv matrixLib)
target_link_libraries(LuDecompPartPivot matrixLib)


