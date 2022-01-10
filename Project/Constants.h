#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types
#include <glm/gtx/transform.hpp>

#include <iostream>
#include <vector>

namespace gps {
    /// <summary>
    /// Enum that stores the move directions for Camera and InGameObject usage
    /// </summary>
    enum MOVE_DIRECTION { MOVE_FORWARD, MOVE_BACKWARD, MOVE_RIGHT, MOVE_LEFT, MOVE_UP, MOVE_DOWN };

    /// <summary>
    /// Enum that stores the rotation directions for Camera and InGameObject usage
    /// </summary>
    enum ROTATION_AXIS { X_AXIS, Y_AXIS, Z_AXIS};

}