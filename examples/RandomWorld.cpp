#include <SFML/Graphics.hpp>
#include "SimplexNoise.hpp"

#include <iostream>
#include <memory>
#include <sstream>
#include <cstdint>
#include <string>
#include <ctime>

/* ***** Simplex noise visualisation example *****/
static constexpr unsigned int WIDTH = 1270;
static constexpr unsigned int HEIGHT = 760;

static constexpr unsigned int TEXTURE_WIDTH = WIDTH / 3;
static constexpr unsigned int TEXTURE_HEIGHT = HEIGHT / 3;

template< typename T >
std::string toString(T number) {
    std::ostringstream ss;
    ss << number;

    return ss.str();
}

/* ***** WORLD ******/
class RandomWorld {
    public:
  explicit RandomWorld(unsigned int width, unsigned int height) : height(height), width(width) {
    [[maybe_unused]] auto resizeResult = noiseTexture.resize(sf::Vector2u{TEXTURE_WIDTH, TEXTURE_HEIGHT});
    pixels = std::unique_ptr< std::uint8_t[] >(new std::uint8_t[width * height * 4]);

            elevationNoise.randomizeSeed();
            moistureNoise.setSeed(static_cast<long unsigned int>(time(nullptr)) * 2);

            printCurrentValues();
        };

  void handleKey(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Key::Q) redistribution -= 0.1f;
    if (key == sf::Keyboard::Key::W) redistribution += 0.1f;
    if (key == sf::Keyboard::Key::A) frequency -= 0.1f;
    if (key == sf::Keyboard::Key::S) frequency += 0.1f;
    if (key == sf::Keyboard::Key::Z) gain -= 0.005f;
    if (key == sf::Keyboard::Key::X) gain += 0.005f;
    if (key == sf::Keyboard::Key::D) octaves -= 1;
    if (key == sf::Keyboard::Key::F) octaves += 1;
    if (key == sf::Keyboard::Key::C) lacunarity -= 0.1;
    if (key == sf::Keyboard::Key::V) lacunarity += 0.1;
    if (key == sf::Keyboard::Key::E) elevationNoise.randomizeSeed();
    if (key == sf::Keyboard::Key::R) moistureNoise.randomizeSeed();
  }

        void printCurrentValues() {
            std::cout << "Noise Redistribution: " << toString(redistribution) << " ";
            std::cout << "Frequency: " << toString(frequency) << " ";
            std::cout << "Gain: " << toString(gain) << " ";
            std::cout << "Octaves : " << toString(octaves) << " ";
            std::cout << "Lacunarity : " << toString(lacunarity) << std::endl;
        }

  void draw(sf::RenderWindow &window) {
    sf::Sprite sprite(noiseTexture);
    sprite.setScale(sf::Vector2f{3.f, 3.f});

    window.draw(sprite);
  };

        void create() {
            for (std::size_t y = 0; y < height; ++y) {
                for (std::size_t x = 0; x < width; ++x) {
                    double xPos = double(x) / double(width) - 0.5;
                    double yPos = double(y) / double(height) - 0.5;

                    double elevation = std::pow(
                            elevationNoise.unsignedFBM(frequency * xPos, frequency * yPos, octaves, lacunarity, gain),
                            redistribution);

                    double moisture =
                            moistureNoise.unsignedFBM(frequency * xPos, frequency * yPos, octaves, lacunarity, gain);

                    sf::Color color = biome(elevation, moisture);
                    setPixel(x, y, color);
                }
            }

            noiseTexture.update(pixels.get());
        };

    private:
        const sf::Color DEEP_WATER = sf::Color(40, 80, 102, 255);
        const sf::Color SHALLOW_WATER = sf::Color(59, 131, 158, 255);
        const sf::Color BEACH = sf::Color(246, 227, 212, 255);
        const sf::Color DESERT = sf::Color(255, 223, 164, 255);
        const sf::Color WASTELAND = sf::Color(241, 183, 132, 255);
        const sf::Color VEGETATION = sf::Color(162, 139, 71, 255);
        const sf::Color SHRUBS = sf::Color(128, 112, 68, 255);
        const sf::Color HILLS = sf::Color(134, 112, 91, 255);
        const sf::Color MOUNTAINS = sf::Color(79, 63, 52, 255);

        unsigned int height;
        unsigned int width;
        SimplexNoise elevationNoise;
        SimplexNoise moistureNoise;
        sf::Texture noiseTexture;
        std::unique_ptr< std::uint8_t[] > pixels;

        /* Values for FBM that produce 'nice' output */
        unsigned int octaves = 7;
        double lacunarity = 2.1042;
        double gain = 0.575;
        double redistribution = 1.7;
        double frequency = 1.5;

        void setPixel(unsigned int x, unsigned int y, sf::Color color) {
            pixels[4 * (y * width + x)] = color.r;
            pixels[4 * (y * width + x) + 1] = color.g;
            pixels[4 * (y * width + x) + 2] = color.b;
            pixels[4 * (y * width + x) + 3] = color.a;
        };

        sf::Color biome(const double &elevation, const double &moisture) {
            if (elevation <= 0.07) return DEEP_WATER;
            if (elevation <= 0.1) return SHALLOW_WATER;
            if (elevation <= 0.11) return BEACH;

            if (elevation <= 0.15) {
                if (moisture <= 0.15) return DESERT;
                if (moisture <= 0.3) return WASTELAND;

                return VEGETATION;
            }

            if (elevation <= 0.6) {
                if (moisture <= 0.5) return DESERT;
                if (moisture <= 0.75) return WASTELAND;
                if (moisture <= 0.8) return SHRUBS;

                return VEGETATION;
            }

            if (elevation <= 0.7) return HILLS;

            return MOUNTAINS;
        }
};

/* ***** Main *****/
int main() {
    RandomWorld world(TEXTURE_WIDTH, TEXTURE_HEIGHT);
    world.create();

  sf::RenderWindow window(sf::VideoMode(sf::Vector2u{WIDTH, HEIGHT}), "Simplex Noise 2D visualisation");
  window.setKeyRepeatEnabled(false);

  while (window.isOpen()) {
    while (const auto event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      } else if (const auto keyEvent = event->getIf<sf::Event::KeyPressed>()) {
        std::cout << "Updating... ";
        sf::Clock clock;
        clock.restart();
        world.handleKey(keyEvent->code);
        world.create();

        std::cout << "time taken: " + toString(clock.getElapsedTime().asMilliseconds()) << " ms" << std::endl;
        world.printCurrentValues();
      }
    }

    window.clear(sf::Color::Black);
    world.draw(window);
    window.display();
  }

    return EXIT_SUCCESS;
}