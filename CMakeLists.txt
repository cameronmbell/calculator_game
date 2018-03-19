# based on a stack overflow post - most simple but complete cmake example
# https://stackoverflow.com/questions/21163188/most-simple-but-complete-cmake-example

# create a C++ project
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project("calculator_game" CXX)

# load all files into SRCS and DATA strings
file(GLOB_RECURSE SRCS src/*.cpp src/*.hpp)
file(GLOB_RECURSE DATA res/*)

# create the executable from the sources
add_executable(${PROJECT_NAME} ${SRCS} ${DATA})

# use the C++11 standard
target_compile_options(${PROJECT_NAME} PUBLIC -std=c++11 -Wall)

# make file includes relative to the src/ dir
target_include_directories(${PROJECT_NAME} PUBLIC src/)

file(COPY ${DATA} DESTINATION res)

# start handling dependencies
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

# lpthreads on windows/linux for the std::thread interface
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)

find_package(Threads REQUIRED)

target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

# need SFML for cross-platform graphics, sound and windowing
find_package(SFML 2 REQUIRED COMPONENTS network audio graphics window system)

if (SFML_FOUND)
	include_directories(${SFML_INCLUDE_DIR})
	target_link_libraries(${PROJECT_NAME} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
endif()

# for direct gl class, used internally by SFML so this should always be available
find_package(OpenGL REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})

if (OPENGL_FOUND)
	target_link_libraries(${PROJECT_NAME} ${OPENGL_gl_LIBRARY})

	# needed if using math.h
	# not used yet:
	# target_link_libraries(${EXECUTABLE_NAME} m)
endif()
