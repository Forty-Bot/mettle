#Stick stuff in root
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

#Enable debugging symbols by default
if(cmake_build_type STREQUAL "")
	set(cmake_build_type debug)
endif()

#Set the name of the executable
set(executable_name mettle)

#Create the executable
add_executable(${executable_name} mettle.c)

#Detect and add CSFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})
find_package(CSFML 2.1 REQUIRED system window graphics) #network audio)
if(CSFML_FOUND)
  include_directories(${CSFML_INCLUDE_DIR})
  target_link_libraries(${executable_name} ${CSFML_LIBRARIES})
endif()