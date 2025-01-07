#include "ScalingManager.hpp"

namespace Engine {

ScalingManager::ScalingManager()
    : currentWidth(BASE_WIDTH)
    , currentHeight(BASE_HEIGHT)
    , scaleX(1.0f)
    , scaleY(1.0f)
{
}

ScalingManager& ScalingManager::getInstance() {
    static ScalingManager instance;
    return instance;
}

void ScalingManager::updateWindowSize(unsigned int width, unsigned int height) {
    currentWidth = width;
    currentHeight = height;
    updateScaleFactors();
}

void ScalingManager::updateScaleFactors() {
    scaleX = static_cast<float>(currentWidth) / BASE_WIDTH;
    scaleY = static_cast<float>(currentHeight) / BASE_HEIGHT;
}

sf::Vector2f ScalingManager::convertNormalizedToScreen(float x, float y, Anchor anchor) const {
    float screenX, screenY;
    switch (anchor) {
        case Anchor::TopLeft:
            screenX = x * currentWidth;
            screenY = y * currentHeight;
            break;
        case Anchor::TopRight:
            screenX = currentWidth - x * currentWidth;
            screenY = y * currentHeight;
            break;
        case Anchor::TopCenter:
            screenX = (currentWidth * 0.5f) + (x * currentWidth - currentWidth * 0.5f);
            screenY = y * currentHeight;
            break;
        case Anchor::CenterLeft:
            screenX = x * currentWidth;
            screenY = (currentHeight * 0.5f) + (y * currentHeight - currentHeight * 0.5f);
            break;
        case Anchor::Center:
            screenX = (currentWidth * 0.5f) + (x * currentWidth - currentWidth * 0.5f);
            screenY = (currentHeight * 0.5f) + (y * currentHeight - currentHeight * 0.5f);
            break;
        case Anchor::CenterRight:
            screenX = currentWidth - x * currentWidth;
            screenY = (currentHeight * 0.5f) + (y * currentHeight - currentHeight * 0.5f);
            break;
        case Anchor::BottomLeft:
            screenX = x * currentWidth;
            screenY = currentHeight - y * currentHeight;
            break;
        case Anchor::BottomCenter:
            screenX = (currentWidth * 0.5f) + (x * currentWidth - currentWidth * 0.5f);
            screenY = currentHeight - y * currentHeight;
            break;
        case Anchor::BottomRight:
            screenX = currentWidth - x * currentWidth;
            screenY = currentHeight - y * currentHeight;
            break;
        default:
            screenX = x * currentWidth;
            screenY = y * currentHeight;
            break;
    }
    return sf::Vector2f(screenX, screenY);
}

sf::Vector2f ScalingManager::convertScreenToNormalized(float x, float y) const {
    float normalizedX = x / currentWidth;
    float normalizedY = y / currentHeight;
    return sf::Vector2f(normalizedX, normalizedY);
}

sf::Vector2f ScalingManager::getScaleFactors() const {
    return sf::Vector2f(scaleX, scaleY);
}

float ScalingManager::getScaledFontSize(float baseSize) const {
    return baseSize * std::min(scaleX, scaleY);
}

sf::Vector2f ScalingManager::getAnchorOffset(Anchor anchor) const {
    switch (anchor) {
        case Anchor::TopLeft:
            return sf::Vector2f(0.0f, 0.0f);
        case Anchor::TopCenter:
            return sf::Vector2f(-0.5f, 0.0f);
        case Anchor::TopRight:
            return sf::Vector2f(-1.0f, 0.0f);
        case Anchor::CenterLeft:
            return sf::Vector2f(0.0f, -0.5f);
        case Anchor::Center:
            return sf::Vector2f(-0.5f, -0.5f);
        case Anchor::CenterRight:
            return sf::Vector2f(-1.0f, -0.5f);
        case Anchor::BottomLeft:
            return sf::Vector2f(0.0f, -1.0f);
        case Anchor::BottomCenter:
            return sf::Vector2f(-0.5f, -1.0f);
        case Anchor::BottomRight:
            return sf::Vector2f(-1.0f, -1.0f);
        default:
            return sf::Vector2f(0.0f, 0.0f);
    }
}

void ScalingManager::scaleSprite(sf::Sprite& sprite, const sf::Vector2f& normalizedPosition, Anchor anchor) const {
    // Set scale based on window size
    sf::Vector2f scale = getScaleFactors();
    sprite.setScale(scale);

    // Convert normalized position to screen coordinates
    sf::Vector2f screenPos = convertNormalizedToScreen(normalizedPosition.x, normalizedPosition.y, anchor);
    sprite.setPosition(screenPos);
}

void ScalingManager::scaleSpriteToFill(sf::Sprite& sprite) const {
    const sf::Texture* texture = sprite.getTexture();
    if (!texture) return;

    sf::Vector2u textureSize = texture->getSize();
    float scaleX = static_cast<float>(currentWidth) / static_cast<float>(textureSize.x);
    float scaleY = static_cast<float>(currentHeight) / static_cast<float>(textureSize.y);
    float scale = std::max(scaleX, scaleY); // Use max to ensure no black borders

    sprite.setOrigin(textureSize.x / 2.f, textureSize.y / 2.f);
    sprite.setScale(scale, scale);
    sprite.setPosition(currentWidth / 2.f, currentHeight / 2.f);
}

void ScalingManager::scaleSpriteWithAspectRatio(sf::Sprite& sprite, bool fillScreen) const {
    const sf::Texture* texture = sprite.getTexture();
    if (!texture) return;

    sf::Vector2u textureSize = texture->getSize();
    float scaleX = static_cast<float>(currentWidth) / static_cast<float>(textureSize.x);
    float scaleY = static_cast<float>(currentHeight) / static_cast<float>(textureSize.y);
    float scale = fillScreen ? std::max(scaleX, scaleY) : std::min(scaleX, scaleY);

    sprite.setOrigin(textureSize.x / 2.f, textureSize.y / 2.f);
    sprite.setScale(scale, scale);
    sprite.setPosition(currentWidth / 2.f, currentHeight / 2.f);
}

Anchor ScalingManager::getAnchorFromString(const std::string& str) {
    if (str == "TopLeft") return Anchor::TopLeft;
    if (str == "TopCenter") return Anchor::TopCenter;
    if (str == "TopRight") return Anchor::TopRight;
    if (str == "CenterLeft") return Anchor::CenterLeft;
    if (str == "Center") return Anchor::Center;
    if (str == "CenterRight") return Anchor::CenterRight;
    if (str == "BottomLeft") return Anchor::BottomLeft;
    if (str == "BottomCenter") return Anchor::BottomCenter;
    if (str == "BottomRight") return Anchor::BottomRight;
    return Anchor::TopLeft; // Default
}

} // namespace Engine 