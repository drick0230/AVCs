# CMakeList.txt : projet CMake pour AVCs, incluez la source et définissez
# la logique spécifique au projet ici.
#
cmake_minimum_required (VERSION 3.8)

project ("AVCs_server")

# Ajoutez une source à l'exécutable de ce projet.
include_directories ("hdr")
include_directories ("../commun/hdr")

add_executable (AVCs_server "src/AVCs_server.cpp" "hdr/AVCs_server.h" "../commun/src/VOIP.cpp" "../commun/hdr/VOIP.h")
target_link_libraries(AVCs_server sfml-graphics sfml-audio sfml-network sfml-system)

# TODO: Ajoutez des tests et installez des cibles si nécessaire.
