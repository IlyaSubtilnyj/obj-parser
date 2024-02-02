#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include "parser/parser.hpp"

int main()
{

    parser::ObjParser* modelParser = new parser::ObjParser("/home/ilya/cprojects/cmake-sfml-project/resources/model.obj");
    modelParser->parse();

    sf::RenderWindow window(sf::VideoMode(800, 800), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    const std::string textrure_file = "../../resources/img.jpg";
    sf::Texture texeture;
    if (!texeture.loadFromFile(textrure_file)) {
        return EXIT_FAILURE;
    }
    sf::Sprite sprite(texeture);
    
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }
    
    return 0;
}