 ############################
 # @file	CMakeList.txt	#
 # @author	Philippe Warren	#
 #			Dérick Gagnon	#
 # @date	2021-02-14		#
 ############################

cmake_minimum_required (VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project ("AVCs")

set (CLIENT_DIR "${CMAKE_SOURCE_DIR}/Client")
set (SERVER_DIR "${CMAKE_SOURCE_DIR}/Server")
set (COMMON_DIR "${CMAKE_SOURCE_DIR}/Common")

set(VS_GLOBAL_ApplicationIcon "${COMMON_DIR}/Ressources/AVCs_Icon_Squr.ico")


# Incluez les sous-projets.
# add_subdirectory ("lib/SFML")
add_subdirectory ("Client")
add_subdirectory ("Server")