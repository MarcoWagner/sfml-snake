cmake_minimum_required(VERSION 3.14)
project(sfml-snake VERSION 1.0 HOMEPAGE_URL "https://www.afdsaf.de" LANGUAGES CXX)

add_executable(game-exe
	src/main.cpp
	src/logger.cpp
	# additional files
)
set_target_properties(game-exe PROPERTIES
	OUTPUT_NAME snake
)
target_compile_features(game-exe 
	PRIVATE cxx_std_17
)
target_link_libraries(game-exe
	PRIVATE sfml-window sfml-graphics sfml-system
)

