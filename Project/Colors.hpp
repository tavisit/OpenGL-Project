#ifndef Colors_hpp
#define Colors_hpp

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <string>


namespace gps {
    class Colors {
    public:

        enum COLORS { WHITE, BLACK, RED, GREEN, BLUE, ORANGE, LIGHT_ORANGE};

        static glm::vec3 convertFromRGBToVector(glm::vec3 rgb);
        static glm::vec3 convertFromHEXToVector(std::string hexValue);
        static glm::vec3 convertFromEnumToVector(COLORS color);

    private:
    };

}

#endif // !Colors_hpp

