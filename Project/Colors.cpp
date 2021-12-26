#include "Colors.hpp"

glm::vec3 gps::Colors::convertFromRGBToVector(glm::vec3 rgb)
{
    return glm::vec3(rgb.x / 255, rgb.y / 255, rgb.z / 255);
}

glm::vec3 gps::Colors::convertFromHEXToVector(std::string hexValue)
{
    float r, g, b;
    std::istringstream converter(hexValue);
    unsigned int value;
    converter >> std::hex >> value;
    r = ((value >> 16) & 0xFF) / 255.0;  // Extract the RR byte
    g = ((value >> 8) & 0xFF) / 255.0;   // Extract the GG byte
    b = ((value) & 0xFF) / 255.0;
    return glm::vec3(r,g,b);

}

glm::vec3 gps::Colors::convertFromEnumToVector(COLORS color)
{
    glm::vec3 returnValue;
    switch (color)
    {
    case gps::Colors::WHITE:
        return glm::vec3(1.0f, 1.0f, 1.0f);
        break;
    case gps::Colors::BLACK:
        return glm::vec3(0.0f, 0.0f, 0.0f);
        break;
    case gps::Colors::RED:
        return glm::vec3(1.0f, 0.0f, 0.0f);
        break;
    case gps::Colors::GREEN:
        return glm::vec3(0.0f, 1.0f, 0.0f);
        break;
    case gps::Colors::BLUE:
        return glm::vec3(0.0f, 0.0f, 1.0f);
        break;
    case gps::Colors::ORANGE:
        return convertFromRGBToVector(glm::vec3(239, 105 , 0));
        break;
    case gps::Colors::LIGHT_ORANGE:
        return convertFromRGBToVector(glm::vec3(239, 135, 20));
        break;
    default:
        break;
    }
}
