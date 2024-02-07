#include <SFML/Graphics.hpp>
#include <array>
#include <functional>
#include <vector>
#include "Model/Model.hpp"
#include "Scene/Scene.hpp"
#include <iostream>
#include <string>
#include "parser/parser.hpp"

using namespace simple_matrix;

int main(int argc, char** argv)
{
    Scene Scene(std::make_unique<StubModel>(), 1);

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "SFML works!");
    sf::Clock clock;
    
    parser::ObjParser* modelParser = new parser::ObjParser("/home/ilya/cprojects/cmake-sfml-project/resources/model.obj");
    modelParser->parse();
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        sf::Time elapsed = clock.restart();
        auto promise = Scene.turnTheClock(elapsed);
        sf::sleep(sf::seconds(0.03));

        window.clear();
        promise.then([&](){
            Scene.takeTheStage(window);
        });
        Scene.takeTheStage(window);
        window.display();
    }
    
    return 0;
}
