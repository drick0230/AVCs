 ############################
 # @file	CMakeList.txt	#
 # @author	Philippe Warren	#
 #			Dérick Gagnon	#
 # @date	2021-02-14		#
 ############################

cmake_minimum_required (VERSION 3.8)

project ("AVCs_client")

set (SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set (HDR_DIR "${PROJECT_SOURCE_DIR}/hdr")
set (COMMON_PROJECT "${CMAKE_SOURCE_DIR}/Common")
set (COMMON_SRC_DIR "${CMAKE_SOURCE_DIR}/Common/src")
set (COMMON_HDR_DIR "${CMAKE_SOURCE_DIR}/Common/hdr")

# Ajoutez une source à l'exécutable de ce projet.
include_directories ("${HDR_DIR}")
include_directories ("${COMMON_HDR_DIR}")

file (GLOB G_SRC "${SRC_DIR}/*.cpp")
file (GLOB G_HDR "${SRC_DIR}/*.h")

file (GLOB G_COMMON_SRC "${COMMON_SRC_DIR}/*.cpp")
file (GLOB G_COMMON_HDR "${COMMON_SRC_DIR}/*.h")

add_executable ("${PROJECT_NAME}" "${G_COMMON_SRC}" "${G_COMMON_HDR}" "${G_SRC}" "${G_HDR}" "${COMMON_DIR}/Ressources/Icon.rc")
set_property(TARGET "${PROJECT_NAME}" PROPERTY VS_GLOBAL_ApplicationIcon "${COMMON_DIR}/Ressources/AVCs_Icon_Squr.ico")
IF (WIN32)
	target_link_libraries ("${PROJECT_NAME}" Mf.lib Mfplat.lib Ole32.lib Mfreadwrite.lib Ws2_32.lib mfuuid.lib) # Audioses.dll and Mmdevapi.dll Audiodg.dll
ELSE()
	target_link_libraries ("${PROJECT_NAME}" )
ENDIF()

# TODO: Ajoutez des tests et installez des cibles si nécessaire.
