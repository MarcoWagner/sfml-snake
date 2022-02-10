# snake game 

A game where a worm has to eat food to grow up. 
It was done as a exercise while reading into SFML.

Controls: W A S D ESC


# dependencies

* [cmake](https://cmake.org/)
* [SFML](https://www.sfml-dev.org)

# build and run
        mkdir build && cd build
        cmake .. && cmake --build .
        ./snake

# windows build and run (all commands done in MSVC "nativ tools command prompt")
        SET sfml=C:\SFML-2.5.1
        SET PATH=%PATH%;%SFML%\bin
        md build && cd build
        cmake .. && cmake --build . --Config Release
        Release\snake.exe