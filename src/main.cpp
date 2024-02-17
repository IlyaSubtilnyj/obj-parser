#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <array>
#include <functional>
#include <math.h>
#include <vector>
#include "Scene/Scene.hpp"
#include <string>
#include "parser/parser.hpp"
#include "fps.hpp"
#include "TMConsumer.hpp"

using namespace simple_matrix;

int main(int argc, char** argv)
{
    parser::ObjParser* modelParser = new parser::ObjParser("/home/ilya/cprojects/cmake-sfml-project/resources/model2.obj");
    auto model = modelParser->parse();

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "SFML works!");
    sf::Clock clock;

    bool is_pressed = false;
    int prev_x_coo, x_coo, prev_y_coo, y_coo;
    //double angleThetha = 0, anglePhi = 0;
    double radius = 80.0;

    sf::Image screenshot;
    screenshot.create(window.getSize().x, window.getSize().y, sf::Color::Transparent);

    const int32_t thread_count = 20;
    std::shared_ptr<TaskManager> sharedTaskManager = std::make_shared<TaskManager>(thread_count);

    Scene Scene(sharedTaskManager, thread_count, std::move(model), 0.6, screenshot);

    Fps fps;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {

            if (event.type == sf::Event::MouseButtonPressed)
            {
                is_pressed = true;
                prev_x_coo = event.mouseButton.x;
                prev_y_coo = event.mouseButton.y;
            }
            else if (event.type == sf::Event::MouseButtonReleased) 
            {
                is_pressed = false;
            }
            else if (event.type == sf::Event::MouseMoved)
            {
                if (is_pressed) 
                {
                    Scene.transformed();
                    x_coo = event.mouseMove.x;
                    y_coo = event.mouseMove.y;

                    // angleThetha -= (y_coo - prev_y_coo) / 720.0 * 2 * M_PI;
                    // anglePhi += (x_coo - prev_x_coo) / 1280.0 * 2 * M_PI;

                    // Scene.eye = simple_matrix::matrix(1, 4, std::initializer_list<double>{
                    //     radius * std::sin(angleThetha) * std::cos(anglePhi),
                    //     radius * std::cos(angleThetha),
                    //     radius * std::sin(angleThetha) * std::sin(anglePhi),
                    //     1.0
                    // });

                    int xdel = x_coo - prev_x_coo;
                    int ydel = y_coo - prev_y_coo;
                    if (std::abs(xdel) > 0) {
                        const double rad = -(xdel/10.0) * M_PI / 180.0;
                        Scene.eye = Scene.eye * simple_matrix::matrix(4, 4, std::initializer_list<double>{
                            std::cos(rad),  0, sin(rad),        0,
                            0,              1, 0,               0,
                            -std::sin(rad), 0, std::cos(rad),   0,
                            0,              0, 0,               1
                        }).transpose();
                    }
                    if (std::abs(ydel) > 0) {
                        const double rad = -(ydel/10.0) * M_PI / 180.0;
                        Scene.eye = Scene.eye * simple_matrix::matrix(4, 4, std::initializer_list<double>{
                            1,              0,              0,                  0,
                            0,              std::cos(rad),  -std::sin(rad),     0,
                            0,              std::sin(rad),  std::cos(rad),      0,
                            0,              0,              0,                  1
                        }).transpose();
                    }

                    prev_x_coo = x_coo;
                    prev_y_coo = y_coo;
                }
            }
            else if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::MouseWheelScrolled) {
                Scene.transformed();
                double del = event.mouseWheelScroll.delta;
                if (del) {
                    double delta = -(del * 10);
                    radius += delta;
                    Scene.eye = Scene.eye * simple_matrix::matrix(4, 4, std::initializer_list<double>{
                        1, 0, 0, 0,
                        0, 1, 0, 0,
                        0, 0, 1, delta,
                        0, 0, 0, 1
                    }).transpose() ; 
                }
                
                // Scene.eye = simple_matrix::matrix(1, 4, std::initializer_list<double>{
                //     radius * std::sin(angleThetha) * std::cos(anglePhi),
                //     radius * std::cos(angleThetha),
                //     radius * std::sin(angleThetha) * std::sin(anglePhi),
                //     1.0
                // });
            }
        }

        Scene.fitTheClock();
        Scene.drawUseBuffer(window);
        window.display();
        fps.update();
        std::cout << "fps: " << fps.get() << std::endl;
    }
    
    return 0;
}

#if 0
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) // вокруг y
            {
                const double rad = 5 * M_PI / 180;
                std::cout << Scene.eye << std::endl;
                Scene.eye = Scene.eye * simple_matrix::matrix(4, 4, std::initializer_list<double>{
                    std::cos(rad),  0, sin(rad),        0,
                    0,              1, 0,               0,
                    -std::sin(rad), 0, std::cos(rad),   0,
                    0,              0, 0,               1
                }).transpose();
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) //вокруг z
            {
                const double rad = 5 * M_PI / 180;
                std::cout << Scene.eye << std::endl;
                Scene.eye = Scene.eye * simple_matrix::matrix(4, 4, std::initializer_list<double>{
                    std::cos(rad),  -sin(rad),      0,      0,
                    std::sin(rad),  std::cos(rad),  0,      0,
                    0,              0,              1,      0,
                    0,              0,              0,      1
                }).transpose();
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
            {
                const double rad = 5 * M_PI / 180;
                std::cout << Scene.eye << std::endl;
                Scene.eye = Scene.eye * simple_matrix::matrix(4, 4, std::initializer_list<double>{
                    1,              0,              0,                  0,
                    0,              std::cos(rad),  -std::sin(rad),     0,
                    0,              std::sin(rad),  std::cos(rad),      0,
                    0,              0,              0,                  1
                }).transpose();
            }
            // int xdel = x_coo - prev_x_coo;
                // int ydel = y_coo - prev_y_coo;
                // if (std::abs(xdel) > 0) {
                //     const double rad = -(xdel/10) * M_PI / 180;
                //     std::cout << Scene.eye << std::endl;
                //     Scene.eye = Scene.eye * simple_matrix::matrix(4, 4, std::initializer_list<double>{
                //         std::cos(rad),  0, sin(rad),        0,
                //         0,              1, 0,               0,
                //         -std::sin(rad), 0, std::cos(rad),   0,
                //         0,              0, 0,               1
                //     }).transpose();
                // }
                // if (std::abs(ydel) > 0) {
                //     const double rad = -(ydel/10) * M_PI / 180;
                //     std::cout << Scene.eye << std::endl;
                //     Scene.eye = Scene.eye * simple_matrix::matrix(4, 4, std::initializer_list<double>{
                //         1,              0,              0,                  0,
                //         0,              std::cos(rad),  -std::sin(rad),     0,
                //         0,              std::sin(rad),  std::cos(rad),      0,
                //         0,              0,              0,                  1
                //     }).transpose();
                //     std::cout << Scene.eye << std::endl;
                // }
#endif