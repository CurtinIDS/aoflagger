cmake_minimum_required(VERSION 2.6)

project(AOFlagger)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0>=3.0.0)
pkg_check_modules(GLIBMM glibmm-2.4)
pkg_check_modules(SIGCXX sigc++-2.0)

SET(CASACORE_MAKE_REQUIRED_EXTERNALS_OPTIONAL TRUE)
find_package(Casacore REQUIRED COMPONENTS casa ms tables measures)
find_package(CFITSIO REQUIRED)

find_path(FFTW3_INCLUDE_DIR NAMES fftw3.h)

find_library(GSL_LIB NAMES gsl)
find_library(GSL_CBLAS_LIB NAMES gslcblas)
find_path(GSL_INCLUDE_DIR NAMES gsl/gsl_version.h)

find_package(Threads REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(PNG REQUIRED)
find_package(Boost REQUIRED COMPONENTS date_time thread filesystem signals system)
find_library(PTHREAD_LIB pthread REQUIRED)
find_library(FFTW3_LIB fftw3 REQUIRED)
enable_language(Fortran OPTIONAL)
find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  find_library(RT_LIBRARY rt)
#  list(APPEND LOFAR_EXTRA_LIBRARIES ${RT_LIBRARY})
endif(CMAKE_SYSTEM_NAME MATCHES "Linux")

include_directories(${CASACORE_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIR})
include_directories(${LIBXML2_INCLUDE_DIR})
include_directories(${CFITSIO_INCLUDE_DIR})
include_directories(${FFTW3_INCLUDE_DIR})

# The following stuff will set the "rpath" correctly, so that
# LD_LIBRARY_PATH doesn't have to be set.

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")

add_subdirectory(src)

install(DIRECTORY data/icons data/applications DESTINATION share )

# add target to generate API documentation with Doxygen
find_package(Doxygen)

if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc
	${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Generating API documentation with Doxygen" VERBATIM)

endif(DOXYGEN_FOUND)
