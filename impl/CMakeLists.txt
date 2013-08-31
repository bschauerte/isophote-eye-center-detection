###############################################################################
# Example CMakeLists.txt for using the OKAPI library with CMake               #
###############################################################################

# CMake 2.6 or higher is required
cmake_minimum_required(VERSION 2.6)

# Give your project a suitable name
project("EyeCenterDetection")

# Find the OKAPI library
# ----------------------
# The directories below are just guesses. If your OKAPI installation is
# somewhere else, provide the path to the build tree in the OKAPI_DIR
# variable, either by using "ccmake" or "cmake-gui" or by passing it
# on the command line as in "cmake -DOKAPI_DIR=/some/path .."
find_package("OKAPI" REQUIRED
    PATHS
        /home/bschauer/devel/okapi/build
)

# This variable will be set to TRUE if OKAPI could be found    
if (OKAPI_FOUND)
    # This command includes all important settings from the OKAPI project.
    # After this you will be able to link against the OKAPI libraries by
    # just providing their name to "target_link_libraries" as can be seen
    # below
    include(${OKAPI_USE_FILE})
    
    # Use the same compiler settings as OKAPI. This is important for things
    # like OpenMP etc.
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${OKAPI_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OKAPI_CXX_FLAGS}")    
    add_definitions("${OKAPI_DEFINITIONS} -D_OPENMP_ROW_FILTER")
    
    # List all of your source files here
    set(SRCS isophote.cpp gauss_filter.cpp separable_filter.cpp)

    # Create an executable file from them
    add_executable(separable-filter-demo ${SRCS})
    add_executable(isophote-eye-center-detector-demo isophoteeyedetector.cpp)
    add_executable(EyeCenterDetectorDemo EyeCenterDetectorDemo.cpp)

    set_target_properties(separable-filter-demo PROPERTIES COMPILE_FLAGS "-D__STANDALONE")
    set_target_properties(isophote-eye-center-detector-demo PROPERTIES COMPILE_FLAGS "-D__STANDALONE")

    # Create libraries
    add_library(separable-filter SHARED ${SRCS})
    add_library(isophote-eye-center-detector SHARED ${SRCS} isophoteeyedetector.cpp)
    add_library(separable-filter-st STATIC ${SRCS})
    add_library(isophote-eye-center-detector-st STATIC ${SRCS} isophoteeyedetector.cpp)

    set_target_properties(separable-filter PROPERTIES VERSION 0.1)
    set_target_properties(isophote-eye-center-detector PROPERTIES VERSION 0.1)
    set_target_properties(separable-filter-st PROPERTIES VERSION 0.1)
    set_target_properties(isophote-eye-center-detector-st PROPERTIES VERSION 0.1)
    
    # Link them against the necessary libraries
    target_link_libraries(separable-filter-demo okapi-gui-st okapi-st)
    target_link_libraries(isophote-eye-center-detector-demo separable-filter okapi-gui-st okapi-st)
    target_link_libraries(EyeCenterDetectorDemo isophote-eye-center-detector okapi-gui-st okapi-st okapi-videoio-st)
    
    # Installation information
    install(TARGETS separable-filter DESTINATION lib)
    install(TARGETS isophote-eye-center-detector DESTINATION lib)
    install(TARGETS separable-filter-demo DESTINATION bin)
    install(TARGETS isophote-eye-center-detector-demo DESTINATION bin)
    install(TARGETS EyeCenterDetectorDemo DESTINATION bin)
    install(FILES epsilon.hpp gauss_filter.hpp isophoteeyedetector.hpp isophote.hpp separable_filter.hpp DESTINATION include/isophote)
endif (OKAPI_FOUND)

