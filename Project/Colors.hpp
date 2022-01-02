#ifndef Colors_hpp
#define Colors_hpp

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <string>
#include <sstream>
#include <iomanip>


namespace gps {

    /// <summary>
    /// Helper class that converts color codes and vectors to OpenGL compatible rgb vectors
    /// </summary>
    class Colors {
    public:
        /// <summary>
        /// Enum that stores the main colors that the application uses
        /// </summary>
        enum COLORS { WHITE, BLACK, RED, GREEN, BLUE, ORANGE, LIGHT_ORANGE};

        /// <summary>
        /// Method that converts from a web-based rgb vector to a OpenGL rgb vector
        /// </summary>
        /// <param name="rgb"></param>
        /// <returns></returns>
        glm::vec3 convertFromRGBToVector(glm::vec3 rgb);

        /// <summary>
        /// Method that converts from a hex value to a OpenGL rgb vector
        /// </summary>
        /// <param name="hexValue"></param>
        /// <returns></returns>
        glm::vec3 convertFromHEXToVector(std::string hexValue);

        /// <summary>
        /// Method that converts from the enum COLORS to a OpenGL rgb vector
        /// </summary>
        /// <param name="color"></param>
        /// <returns></returns>
        glm::vec3 convertFromEnumToVector(COLORS color);

    private:
    };

}

#endif // !Colors_hpp

