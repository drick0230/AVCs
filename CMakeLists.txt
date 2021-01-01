# CMakeList.txt : fichier projet CMake de niveau supérieur, effectuez une configuration globale
# et incluez les sous-projets ici.
#
cmake_minimum_required (VERSION 3.8)

project ("AVCs")

set (SFML_DIR "${CMAKE_SOURCE_DIR}/lib/SFML")
set (CLIENT_DIR "${CMAKE_SOURCE_DIR}/Client")
set (SERVER_DIR "${CMAKE_SOURCE_DIR}/Server")
set (COMMON_DIR "${CMAKE_SOURCE_DIR}/Common")

# Mettre SFML en Static
add_compile_definitions (SFML_STATIC)
set (SFML_STATIC_LIBRARIES TRUE)
set (SFML_USE_STATIC_LIBS TRUE)
set (BUILD_SHARED_LIBS FALSE)

set(VS_GLOBAL_ApplicationIcon "${COMMON_DIR}/Ressources/AVCs_Icon_Squr.ico")


# Incluez les sous-projets.
# add_subdirectory ("lib/SFML")
add_subdirectory ("Client")
add_subdirectory ("Server")