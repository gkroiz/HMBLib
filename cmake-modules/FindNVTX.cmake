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
# Script to find the NVTX include directory (Version 3.0, header-only)
# NVTX also depends on lib dl.
# Defines the following:
#   NVTX_FOUND       - if the package is found
#   NVTX_INCLUDE_DIR - the directory for 'nvtx3'
#   NVTX_LIBRARIES   - the libraries needed to linked for using NVTX
#
# For use with HTGS, add the '-DUSE_NVTX' during compilation to enable profiling with NVTX
# If your HTGS graph contains more than 24 tasks, then use both '-DUSE_NVTX' and '-DUSE_MINIMAL_NVTX'
# The visualization tool NsightSystems must be used to profile and display the output.

include(FindPackageHandleStandardArgs)
unset(NVTX_FOUND)

# typical root dirs of installations, exactly one of them is used

find_path(NVTX_INCLUDE_DIR
        NAMES
        nvtx3/nvToolsExt.h
        HINTS
        /usr/local/cuda/include/nvtx3
        /usr/local/include/nvtx3
        /usr/include/nvtx3
        )


#find_library(NVTX_LIBRARY
#        NAMES
#        nvToolsExt
#        HINTS
#        /usr/lib
#        /usr/local/lib
#        /usr/local/cuda/lib64
#        )


# set NVTX_FOUND
#find_package_handle_standard_args(NVTX DEFAULT_MSG NVTX_INCLUDE_DIR NVTX_LIBRARY)
find_package_handle_standard_args(NVTX DEFAULT_MSG NVTX_INCLUDE_DIR)

# set external variables for usage in CMakeLists.txt
if(NVTX_FOUND)
    set(NVTX_FOUND true)
    set(NVTX_INCLUDE_DIR ${NVTX_INCLUDE_DIR})
    set(NVTX_LIBRARIES "-pthread -ldl") # ${NVTX_LIBRARY}
    MESSAGE(STATUS "NVTX found. Includes: ${NVTX_INCLUDE_DIR}, Libs: ${NVTX_LIBRARIES}")
else()
#    MESSAGE(STATUS "NVTX not found. Includes: ${NVTX_INCLUDE_DIR}")
    MESSAGE(STATUS "NVTX not found. Includes: ${NVTX_INCLUDE_DIR}, Libs: ${NVTX_LIBRARIES}")
endif()

# hide locals from GUI
#mark_as_advanced(NVTX_INCLUDE_DIR NVTX_LIBRARY)
#mark_as_advanced(NVTX_INCLUDE_DIR)
