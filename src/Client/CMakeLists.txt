# CMakeList.txt : projet CMake pour AVCs, incluez la source et définissez
# la logique spécifique au projet ici.
#
cmake_minimum_required (VERSION 3.8)

project ("AVCs_client")

# Ajoutez une source à l'exécutable de ce projet.
include_directories ("hdr")
add_executable (AVCs "src/AVCs_client.cpp" "hdr/AVCs_client.h" "src/VOIP.cpp" "hdr/VOIP.h")
target_link_libraries(AVCs sfml-graphics sfml-audio sfml-network sfml-system)

# TODO: Ajoutez des tests et installez des cibles si nécessaire.
