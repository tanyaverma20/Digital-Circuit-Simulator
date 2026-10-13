#ifndef SFML_COMPAT_HPP
#define SFML_COMPAT_HPP

#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

namespace compat {

template <typename R>
inline float rectWidth(const R& r) {
#if SFML_VERSION_MAJOR >= 3
    return r.size.x;
#else
    return r.width;
#endif
}

template <typename R>
inline float rectHeight(const R& r) {
#if SFML_VERSION_MAJOR >= 3
    return r.size.y;
#else
    return r.height;
#endif
}

template <typename T>
inline void setOrigin(T& obj, float x, float y) {
#if SFML_VERSION_MAJOR >= 3
    obj.setOrigin(sf::Vector2f(x, y));
#else
    obj.setOrigin(x, y);
#endif
}

template <typename T>
inline void setScale(T& obj, float x, float y) {
#if SFML_VERSION_MAJOR >= 3
    obj.setScale(sf::Vector2f(x, y));
#else
    obj.setScale(x, y);
#endif
}

template <typename T>
inline void setPosition(T& obj, float x, float y) {
#if SFML_VERSION_MAJOR >= 3
    obj.setPosition(sf::Vector2f(x, y));
#else
    obj.setPosition(x, y);
#endif
}

template <typename T>
inline void setRotationDegrees(T& obj, float degrees) {
#if SFML_VERSION_MAJOR >= 3
    obj.setRotation(sf::degrees(degrees));
#else
    obj.setRotation(degrees);
#endif
}

} // namespace compat


#if SFML_VERSION_MAJOR >= 3
    #define SF_MOUSE_LEFT sf::Mouse::Button::Left
    #define SF_MOUSE_RIGHT sf::Mouse::Button::Right
    #define SF_KEY_ESCAPE sf::Keyboard::Key::Escape
    #define SF_KEY_C sf::Keyboard::Key::C
    #define SF_KEY_X sf::Keyboard::Key::X
    #define SF_KEY_LCONTROL sf::Keyboard::Key::LControl
    #define SF_KEY_LALT sf::Keyboard::Key::LAlt
    #define SF_KEY_LSHIFT sf::Keyboard::Key::LShift
#else
    #define SF_MOUSE_LEFT sf::Mouse::Left
    #define SF_MOUSE_RIGHT sf::Mouse::Right
    #define SF_KEY_ESCAPE sf::Keyboard::Escape
    #define SF_KEY_C sf::Keyboard::C
    #define SF_KEY_X sf::Keyboard::X
    #define SF_KEY_LCONTROL sf::Keyboard::LControl
    #define SF_KEY_LALT sf::Keyboard::LAlt
    #define SF_KEY_LSHIFT sf::Keyboard::LShift
#endif

#endif // SFML_COMPAT_HPP
