#include <pybind11/pybind11.h>
#include <iostream>
#include <string>
#include <sstream>

// Keep pybind11 stuff in its own namespace


// --- Your C++ Game Engine Code ---
// This is a stand-in for your actual engine classes and functions.

// A sample function
void log_message(const std::string& message) {
    std::cout << "[C++ LOG]: " << message << std::endl;
}

// A sample class
struct Player {
    std::string name;
    int health = 100;

    Player(std::string name) : name(name) {}

    void take_damage(int amount) {
        health -= amount;
        std::cout << "[C++ PLAYER]: " << name << " took " << amount << " damage. Health is now " << health << std::endl;
    }
};


// --- Pybind11 Module Definition ---
// This is the magic part. This macro creates a function that will be called
// when Python runs "import game_engine".

PYBIND11_MODULE(game_engine, m) {
    m.doc() = "A C++ game engine module for scripting"; // Optional docstring

    // Expose the log_message function
    m.def("log", &log_message, "Prints a message from the C++ core");

    // Expose the Player class
    py::class_<Player>(m, "Player")
        .def(py::init<std::string>()) // Expose the constructor (takes a string)
        .def("take_damage", &Player::take_damage) // Expose the take_damage method
        .def_readwrite("name", &Player::name)       // Expose 'name' as a read/write variable
        .def_readonly("health", &Player::health);   // Expose 'health' as a read-only variable
}