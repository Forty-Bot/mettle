# Mettle - A roguelike using sfml
# Copyright (C) 2014 Sean Anderson
#
# Mettle is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# Mettle is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# mettle.  If not, see <http://www.gnu.org/licenses/>.

#Stick stuff in root
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

#Set the name of the executable
set(executable_name mettle)

#Create the executable
add_executable(${executable_name} mettle.c)

#Link to math.h
if(UNIX)
  target_link_libraries(${executable_name} m)
endif(UNIX)

#Add custom modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

#Make sure we're using a C99 compiler, and enable c99 flags
find_package(C99 REQUIRED)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C99_C_FLAGS}")

#Find CSFML
find_package(CSFML 2.1 REQUIRED system window graphics) #network audio)
if(CSFML_FOUND)
  include_directories(${CSFML_INCLUDE_DIR})
  target_link_libraries(${executable_name} ${CSFML_LIBRARIES})
endif()

if(CMAKE_BUILD_TYPE STREQUAL "debug")
  add_definitions(-DDEBUG)
endif()