cmake_minimum_required(VERSION 2.8.11)

project(MultilinearReconstruction)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(CMAKE_AUTOUIC ON)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# Boost
find_package(Boost COMPONENTS filesystem timer program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES} -lboost_filesystem -lboost_system)

# OpenCV
find_package( OpenCV REQUIRED )
if (OpenCV_FOUND)
  message("-- Found OpenCV")
  message("-- Linking with OpenCV libraries: ${OpenCV_LIBS}")
  link_libraries(${OpenCV_LIBS})
else (OpenCV_FOUND)
  message("-- OpenCV not found.")
endif()

# OpenMP
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

# OpenGL
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )
link_libraries(${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

find_package(GLEW REQUIRED)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_libraries(${GLEW_LIBRARIES})
endif()

# Eigen
find_package(Eigen)
include_directories(${EIGEN_INCLUDE_DIR})

# SuiteSparse
set(SUITESPARSE_INCLUDE_DIR "/usr/include/suitesparse")
include_directories(${SUITESPARSE_INCLUDE_DIR})

# MKL
set(MKL_INCLUDE_DIRS "/opt/intel/mkl/include")
message([] ${MKL_INCLUDE_DIRS} )
include_directories(${MKL_INCLUDE_DIRS})
set(MKLROOT "/opt/intel/mkl")
set(MKLLIBS_DIRS "${MKLROOT}/lib/intel64/" "/opt/intel/lib/intel64_lin")
link_directories(${MKLLIBS_DIRS})
set(MKLLIBS "-Wl,--start-group -lmkl_intel_lp64 -lmkl_core -lmkl_intel_thread -Wl,--end-group -liomp5 -ldl -lpthread -lm")

# PhGLib
include_directories("$ENV{HOME}/SDKs/PhGLib/include")
link_directories("$ENV{HOME}/SDKs/PhGLib/lib")
set(PhGLib "-lPhGLib")

# Qt5
find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5OpenGL)
find_package(Qt5Test)

# Ceres solver
find_package(Ceres REQUIRED)
include_directories(${CERES_INCLUDE_DIRS})
link_libraries(${CERES_LIBRARIES})

# Openmesh
set(OPENMESH_DIR "$ENV{HOME}/SDKs/OpenMesh-6.3")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${OPENMESH_DIR}/cmake)
include(ACGCommon)
include_directories("/usr/local/include")
link_libraries(OpenMeshCore OpenMeshTools)

# json
message(STATUS "Found json cpp")
message(STATUS "${CMAKE_CURRENT_LIST_DIR}/third_party/json/include")
include_directories("${CMAKE_CURRENT_LIST_DIR}/third_party/json/include")

# Targets
add_library(multilinearmodel multilinearmodel.cpp)
target_link_libraries(multilinearmodel
        Qt5::Widgets
        Qt5::OpenGL
        ${MKLLIBS}
        ${PhGLib})

add_library(basicmesh basicmesh.cpp)
target_link_libraries(basicmesh
        Qt5::Widgets
        Qt5::OpenGL
        ${MKLLIBS}
        ${PhGLib})

add_library(meshvisualizer meshvisualizer.cpp)
target_link_libraries(meshvisualizer
        Qt5::Widgets
        Qt5::OpenGL
        ${MKLLIBS}
        ${PhGLib})

add_library(meshvisualizer2 meshvisualizer2.cpp)
target_link_libraries(meshvisualizer2
        Qt5::Widgets
        Qt5::OpenGL
        ${MKLLIBS}
        ${PhGLib})

add_library(offscreenmeshvisualizer OffscreenMeshVisualizer.cpp)
target_link_libraries(offscreenmeshvisualizer
                Qt5::Widgets
                Qt5::OpenGL
                ${MKLLIBS}
                ${PhGLib})

add_library(tensor tensor.hpp)
target_link_libraries(tensor ${MKLLIBS})

add_library(ioutilities ioutilities.cpp)
target_link_libraries(ioutilities Qt5::Core Qt5::Widgets)

add_library(reporter reporter.cpp)
target_link_libraries(reporter ${Boost_LIBRARIES})

#configure_file(vert.glsl vert.glsl COPYONLY)
configure_file(frag.glsl frag.glsl COPYONLY)

# Single image reconstruction program
add_executable(SingleImageReconstruction singleimagereconstruction.cpp singleimagereconstructor.hpp utils.hpp ioutilities.h)
target_link_libraries(SingleImageReconstruction
                      meshvisualizer
                      meshvisualizer2
                      multilinearmodel
                      basicmesh
                      ioutilities
                      tensor
                      Qt5::Core
                      Qt5::Widgets
                      Qt5::OpenGL
                      Qt5::Test
                      ${MKLLIBS}
                      ${PhGLib})

# Single image reconstruction with blendshapes program
add_executable(SingleImageReconstruction_exp singleimagereconstruction_exp.cpp singleimagereconstructor_exp.hpp utils.hpp ioutilities.h)
target_link_libraries(SingleImageReconstruction_exp
                      meshvisualizer
                      meshvisualizer2
                      offscreenmeshvisualizer
                      multilinearmodel
                      basicmesh
                      ioutilities
                      tensor
                      Qt5::Core
                      Qt5::Widgets
                      Qt5::OpenGL
                      Qt5::Test
                      ${MKLLIBS}
                      ${PhGLib})

# Multiple image reconstruction program
add_executable(MultiImageReconstruction multiimagereconstruction.cpp multiimagereconstructor.h singleimagereconstructor.hpp utils.hpp multiimagereconstruction.cpp ioutilities.h)
target_link_libraries(MultiImageReconstruction
        meshvisualizer
        multilinearmodel
        basicmesh
        ioutilities
        offscreenmeshvisualizer
        tensor
        aammodel
        Qt5::Core
        Qt5::Widgets
        Qt5::OpenGL
        Qt5::Test
        ${MKLLIBS}
        ${PhGLib})

add_executable(VideoReconstruction videoreconstruction.cpp videoreconstructor.hpp utils.hpp ioutilities.h costfunctions_exp.h)
target_link_libraries(VideoReconstruction
        meshvisualizer
        multilinearmodel
        basicmesh
        ioutilities
        offscreenmeshvisualizer
        tensor
        aammodel
        Qt5::Core
        Qt5::Widgets
        Qt5::OpenGL
        Qt5::Test
        ${MKLLIBS}
        ${PhGLib})

# Multiple image reconstruction with blendshapes program
add_executable(MultiImageReconstruction_exp multiimagereconstruction_exp.cpp singleimagereconstructor_exp.hpp utils.hpp ioutilities.h)
target_link_libraries(MultiImageReconstruction_exp
                      meshvisualizer
                      meshvisualizer2
                      offscreenmeshvisualizer
                      multilinearmodel
                      basicmesh
                      ioutilities
                      tensor
                      aammodel
                      Qt5::Core
                      Qt5::Widgets
                      Qt5::OpenGL
                      Qt5::Test
                      ${MKLLIBS}
                      ${PhGLib})

# Multiple image reconstruction program for ML weights initialized using dense point clouds as constraints
add_executable(MultiImageReconstruction_dense multiimagereconstruction_dense.cpp singleimagereconstructor.hpp utils.hpp ioutilities.h)
target_link_libraries(MultiImageReconstruction_dense
                      meshvisualizer
                      meshvisualizer2
                      offscreenmeshvisualizer
                      multilinearmodel
                      basicmesh
                      ioutilities
                      tensor
                      aammodel
                      Qt5::Core
                      Qt5::Widgets
                      Qt5::OpenGL
                      Qt5::Test
                      ${MKLLIBS}
                      ${PhGLib})

# Multilinear model builder
add_executable(MultilinearModelBuilder multilinearmodelbuilder.cpp)
target_link_libraries(MultilinearModelBuilder multilinearmodel)

# Mesh Matcher
add_executable(mesh_matcher mesh_matcher.cpp)
target_link_libraries(mesh_matcher
                      meshvisualizer
                      meshvisualizer2
                      offscreenmeshvisualizer
                      multilinearmodel
                      basicmesh
                      ioutilities
                      tensor
                      aammodel
                      Qt5::Core
                      Qt5::Widgets
                      Qt5::OpenGL
                      Qt5::Test
                      ${MKLLIBS}
                      ${PhGLib})

# Mesh Visualization tool
add_executable(visualize_mesh visualize_mesh.cpp)
target_link_libraries(visualize_mesh
  meshvisualizer
  meshvisualizer2
  offscreenmeshvisualizer
  multilinearmodel
  basicmesh
  ioutilities
  tensor
  aammodel
  Qt5::Core
  Qt5::Widgets
  Qt5::OpenGL
  Qt5::Test
  ${MKLLIBS}
  ${PhGLib}
)

add_subdirectory(AAM)
add_subdirectory(ImageDedup)
add_subdirectory(tests)
