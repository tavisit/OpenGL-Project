#include "InGameObject.hpp"

namespace gps {
    InGameObject::InGameObject()
    {
        location = glm::vec3(0.0f, 0.0f, 0.0f);
        rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        scale = glm::vec3(1.0f, 1.0f, 1.0f);
        objectChanged();
    }
    glm::vec3 InGameObject::getLocation()
    {
        return location;
    }
    glm::vec3 InGameObject::getRotation()
    {
        return rotation;
    }
    glm::vec3 InGameObject::getScale()
    {
        return scale;
    }
    glm::mat4 InGameObject::getModel()
    {
        return model;
    }
    void InGameObject::objectChanged()
    {
        glm::mat4 modelRotate = glm::mat4(1.0f);
        modelRotate = glm::rotate(modelRotate, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        modelRotate = glm::rotate(modelRotate, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        modelRotate = glm::rotate(modelRotate, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 modelTranslate = glm::translate(glm::mat4(1.0f), location);
        glm::mat4 modelScaled = glm::scale(glm::mat4(1.0f), scale);

        model = modelTranslate * modelRotate  * modelScaled;
    }
    void InGameObject::translateDistance(GLfloat distance, MOVE_DIRECTION moveDirection, GLfloat speed)
    {
        glm::vec3 newDirection;
        switch (moveDirection)
        {
        case MOVE_BACKWARD:
            newDirection = glm::vec3(0, 0, distance);
            break;
        case MOVE_FORWARD:
            newDirection = glm::vec3(0, 0, -distance);
            break;
        case MOVE_RIGHT:
            newDirection = glm::vec3(distance, 0, 0);
            break;
        case MOVE_LEFT:
            newDirection = glm::vec3(-distance, 0, 0);
            break;
        case MOVE_UP:
            newDirection = glm::vec3(0, distance, 0);
            break;
        case MOVE_DOWN:
            newDirection = glm::vec3(0, -distance, 0);
            break;
        default:
            break;
        }
        newDirection *= speed;
        location += newDirection;
        objectChanged();
        // recalculate the sound position
        if (sound != NULL)
        {
            sound->setPosition(vec3df(location.x, location.y, location.z));
        }
    }
    void InGameObject::rotationDistance(GLfloat angle, ROTATION_DIRECTION rotationDirection, GLfloat speed)
    {
        glm::vec3 newRotation;
        switch (rotationDirection)
        {
        case gps::X_AXIS:
            newRotation = glm::vec3(angle, 0.0f, 0.0f);
            break;
        case gps::Y_AXIS:
            newRotation = glm::vec3(0.0f, angle, 0.0f);
            break;
        case gps::Z_AXIS:
            newRotation = glm::vec3(0.0f, 0.0f, angle);
            break;
        default:
            break;
        }
        newRotation *= speed;
        rotation += newRotation;
        objectChanged();
    }
    void InGameObject::scaleDistance(GLfloat scale)
    {
        this->scale *= scale;
        objectChanged();

    }
    void InGameObject::translateAbsolute(glm::vec3 translation, GLfloat speed)
    {
        this->location = translation * speed;
        objectChanged();
        // recalculate the sound position
        if (sound != NULL)
        {
            sound->setPosition(vec3df(location.x, location.y, location.z));
        }
    }
    void InGameObject::rotationAbsolute(glm::vec3 rotation, GLfloat speed)
    {
        this->rotation = rotation * speed;
        objectChanged();
    }
    void InGameObject::scaleAbsolute(glm::vec3 scale)
    {
        this->scale = scale;
        objectChanged();
    }
    void InGameObject::drawObject(glm::mat4 view, gps::Model3D* model3D, gps::Shader* shader)
    {
        if (alpha != 1.0f)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        }
        
        shader->useShaderProgram();

        glUniformMatrix4fv(glGetUniformLocation(shader->shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniform1f(glGetUniformLocation(shader->shaderProgram, "alpha"), alpha);

        glm::mat3 normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(glGetUniformLocation(shader->shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        
        model3D->Draw(*shader);

        if (alpha != 1.0f)
        {
            glDisable(GL_BLEND);
        }   
    }
    void InGameObject::drawShadow(gps::Model3D* model3D, gps::Shader* shader)
    {
        shader->useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(shader->shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        model3D->Draw(*shader);
    }
    void InGameObject::setAlpha(float alpha)
    {
        this->alpha = alpha;
    }

    void InGameObject::initializeSounds(ISoundEngine* SoundEngine, const char* soundsPath)
    {
        sound = SoundEngine->play3D(soundsPath, vec3df(this->location.x, this->location.y, this->location.z), true, false, true);
        sound->setVolume(1.0f);
        sound->setMinDistance(2.0f);
    }
    void InGameObject::initializeSounds(ISoundEngine* SoundEngine, const char* soundsPath, float volumeSound, float minDistance)
    {
        sound = SoundEngine->play3D(soundsPath, vec3df(this->location.x, this->location.y, this->location.z), true, false, true);
        sound->setVolume(volumeSound);
        sound->setMinDistance(minDistance);
    }
}
