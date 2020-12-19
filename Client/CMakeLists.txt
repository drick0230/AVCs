# CMakeList.txt : projet CMake pour AVCs, incluez la source et définissez
# la logique spécifique au projet ici.
#
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

# add_executable (AVCs_client "${SRC_DIR}/*.cpp" "${HDR_DIR}/*.h" "${COMMON_SRC_DIR}/*.cpp" "${COMMON_HDR_DIR}/*.h")
add_executable ("${PROJECT_NAME}" "${G_COMMON_SRC}" "${G_COMMON_HDR}" "${G_SRC}" "${G_HDR}")
IF (WIN32)
	target_link_libraries ("${PROJECT_NAME}" sfml-audio sfml-network sfml-system Mf.lib Mfplat.lib)
ELSE()
	target_link_libraries ("${PROJECT_NAME}" sfml-audio sfml-network sfml-system)
ENDIF()

# TODO: Ajoutez des tests et installez des cibles si nécessaire.
