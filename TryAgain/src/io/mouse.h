#ifndef MOUSE_H
#define MOUSE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

/*
    mouse class to handle mouse callbacks
*/

class Mouse {
public:
    /*
        callbacks
    */

    // list of extended callbacks to call after interrupt
    static std::vector<void(*)(GLFWwindow* window, double _x, double _y)> cursorPosCallbacks;
    static std::vector<void(*)(GLFWwindow* window, int button, int action, int mods)> mouseButtonCallbacks;
    static std::vector<void(*)(GLFWwindow* window, double dx, double dy)> mouseWheelCallbacks;

    // cursor position changed
    static void cursorPosCallback(GLFWwindow* window, double _x, double _y);
    // mouse button state changed
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    // scroll wheel moved
    static void mouseWheelCallback(GLFWwindow* window, double dx, double dy);

    /*
        accessors
    */

    // get mouse x position
    static double getMouseX();
    // get mouse y position
    static double getMouseY();

    // get mouse change in x
    static double getDX();
    // get mouse change in y
    static double getDY();

    // get scroll value in x
    static double getScrollDX();
    // get scroll value in y
    static double getScrollDY();
    //calculate mouse ray
    static glm::vec4 calculateMouseRay(float width, float height, glm::mat4 proj);
    // get button state
    static bool button(int button);
    // return if button changed then reset it in the changed array
    static bool buttonChanged(int button);
    // return if button changed and is now up
    static bool buttonWentUp(int button);
    // return if button changed and is now down
    static bool buttonWentDown(int button);

private:
    /*
        static mouse values
    */

    // x posiiton
    static double x;
    // y position
    static double y;

    // previous x position
    static double lastX;
    // previous y position
    static double lastY;

    // change in x position from lastX
    static double dx;
    // change in y position from lastY
    static double dy;

    // change in scroll x
    static double scrollDx;
    // change in scroll y
    static double scrollDy;

    // if this is the first change in the mouse position
    static bool firstMouse;

    // button state array (true for down, false for up)
    static bool buttons[];
    // button changed array (true if changed)
    static bool buttonsChanged[];
};

#endif