# CMakeList.txt : fichier projet CMake de niveau supérieur, effectuez une configuration globale
# et incluez les sous-projets ici.
#
cmake_minimum_required (VERSION 3.8)

project ("AVCs")

# Mettre SFML en Static
add_compile_definitions(SFML_STATIC)
set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_USE_STATIC_LIBS TRUE)
set(BUILD_SHARED_LIBS FALSE)

# Incluez les sous-projets.
add_subdirectory ("SFML-2.5.1")
add_subdirectory ("AVCs")
