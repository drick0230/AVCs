# Install script for directory: C:/Users/drick/source/repos/drick0230/AVCs/src/AVCs_5/AVCs_5/SFML-2.5.1/src/SFML

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Users/drick/Source/Repos/drick0230/AVCs/src/AVCs_5/out/install/x64-Release")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/drick/source/repos/drick0230/AVCs/src/AVCs_5/out/build/x64-Release/AVCs_5/SFML-2.5.1/src/SFML/System/cmake_install.cmake")
  include("C:/Users/drick/source/repos/drick0230/AVCs/src/AVCs_5/out/build/x64-Release/AVCs_5/SFML-2.5.1/src/SFML/Main/cmake_install.cmake")
  include("C:/Users/drick/source/repos/drick0230/AVCs/src/AVCs_5/out/build/x64-Release/AVCs_5/SFML-2.5.1/src/SFML/Window/cmake_install.cmake")
  include("C:/Users/drick/source/repos/drick0230/AVCs/src/AVCs_5/out/build/x64-Release/AVCs_5/SFML-2.5.1/src/SFML/Network/cmake_install.cmake")
  include("C:/Users/drick/source/repos/drick0230/AVCs/src/AVCs_5/out/build/x64-Release/AVCs_5/SFML-2.5.1/src/SFML/Graphics/cmake_install.cmake")
  include("C:/Users/drick/source/repos/drick0230/AVCs/src/AVCs_5/out/build/x64-Release/AVCs_5/SFML-2.5.1/src/SFML/Audio/cmake_install.cmake")

endif()

