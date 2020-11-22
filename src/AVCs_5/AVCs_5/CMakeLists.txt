# CMakeList.txt : projet CMake pour AVCs_5, incluez la source et définissez
# la logique spécifique au projet ici.
#
cmake_minimum_required (VERSION 3.8)

# Ajoutez une source à l'exécutable de ce projet.
add_subdirectory ("SFML-2.5.1")
include_directories ("hdr")
add_executable (AVCs_5 "src/AVCs_5.cpp" "hdr/AVCs_5.h" "src/VOIP.cpp" "hdr/VOIP.h")
target_link_libraries(AVCs_5 sfml-graphics sfml-audio sfml-network sfml-system)

# TODO: Ajoutez des tests et installez des cibles si nécessaire.
