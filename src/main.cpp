#include <SFML/Graphics.hpp>
#include <array>
#include <vector>
#include "Model/Model.hpp"

using namespace simple_matrix;

int main(int argc, char** argv)
{
    StubModel stubModel;

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "SFML works!");
    sf::Clock clock;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        sf::Time elapsed = clock.restart();
        stubModel.turnTheClock(elapsed);
        sf::sleep(sf::seconds(0.03));

        window.clear();
        stubModel.goOnStage(window);
        window.display();
    }

    return 0;
}
