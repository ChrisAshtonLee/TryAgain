cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

# Add the pybind11 subdirectory from your extern folder
add_subdirectory(extern/pybind11)

# ... your existing executable or library target ...
add_executable(TryAgain main.cpp)

# Link pybind11 to your target
target_link_libraries(TryAgain PRIVATE pybind11::module)