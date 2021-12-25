#include "Colors.hpp"
#pragma once

glm::vec3 gps::Colors::convertFromRGBToVector(glm::vec3 rgb)
{
    return glm::vec3(rgb.x / 255, rgb.y / 255, rgb.z / 255);
}

glm::vec3 gps::Colors::convertFromHEXToVector(std::string hexValue)
{
    int r, g, b;
    sscanf_s(hexValue.c_str(), "%02hhx%02hhx%02hhx", &r, &g, &b);

    return glm::vec3(r / 255, g / 255, b / 255);

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
