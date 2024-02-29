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

/** Model file name*/
const std::string objFileName = "/home/ilya/cprojects/cmake-sfml-project/resources/model2.obj";
/** */
const int32_t threadCount = 12;

/** Window size credentials*/
const uint32_t windowWidth = 1920;
const uint32_t windowHeight = 1080;
/** Title of the window*/
const std::string windowTitle = "SFML works!";

/**Model Zoom*/
const double modelZoom = 0.6f;

int main(int argc, char** argv)
{
    (void)argc, (void)argv;

    parser::ObjParser* modelParser = new parser::ObjParser(objFileName);
     /**Parsing model credentials*/
    auto model = modelParser->parse();

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), windowTitle);

    /** Image to check the size of pixel aaray buffer*/
    sf::Image dummy_screenshot;
    dummy_screenshot.create(windowWidth, windowHeight, sf::Color::Transparent);
    auto screenshotBufferSize = dummy_screenshot.getSize();

    /** Task manager for concurrent scane model calculations*/
    std::shared_ptr<TaskManager> sceneTaskManager = std::make_shared<TaskManager>(threadCount);

    /** Scene*/
    Scene Scene(sceneTaskManager, threadCount, std::move(model), modelZoom, screenshotBufferSize.y, screenshotBufferSize.x);

    /** Mouse credentials*/
    int isMousePressed = false;
    std::pair<uint32_t, uint32_t> mouseCoordinates;
    std::pair<uint32_t, uint32_t> operMouseCoordinates;

    double angleThetha = 0, anglePhi = 0;
    double radius = 80.0;

    /** Fps counter*/
    Fps fps;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::MouseButtonPressed:
                isMousePressed = true;
                mouseCoordinates = {event.mouseButton.x, event.mouseButton.y};
                break;
            
            case sf::Event::MouseButtonReleased:
                isMousePressed = false;
                break;

            case sf::Event::MouseMoved:
                if (isMousePressed) 
                {
                    operMouseCoordinates = {event.mouseMove.x, event.mouseMove.y};

                    // angleThetha -= (y_coo - prev_y_coo) / 720.0 * 2 * M_PI;
                    // anglePhi += (x_coo - prev_x_coo) / 1280.0 * 2 * M_PI;

                    // Scene.eye = simple_matrix::matrix(1, 4, std::initializer_list<double>{
                    //     radius * std::sin(angleThetha) * std::cos(anglePhi),
                    //     radius * std::cos(angleThetha),
                    //     radius * std::sin(angleThetha) * std::sin(anglePhi),
                    //     1.0
                    // });

                    int xMouseDel = operMouseCoordinates.first - mouseCoordinates.first;
                    int yMouseDel = operMouseCoordinates.second - mouseCoordinates.second;

                    if (std::abs(xMouseDel) > 0) {
                        const double rad = -(xMouseDel/10.0) * M_PI / 180.0;
                        Scene.eye = Scene.eye * simple_matrix::matrix(4, 4, std::initializer_list<double>{
                            std::cos(rad),  0, sin(rad),        0,
                            0,              1, 0,               0,
                            -std::sin(rad), 0, std::cos(rad),   0,
                            0,              0, 0,               1
                        }).transpose();
                    }
                    if (std::abs(yMouseDel) > 0) {
                        const double rad = -(yMouseDel/10.0) * M_PI / 180.0;
                        Scene.eye = Scene.eye * simple_matrix::matrix(4, 4, std::initializer_list<double>{
                            1,              0,              0,                  0,
                            0,              std::cos(rad),  -std::sin(rad),     0,
                            0,              std::sin(rad),  std::cos(rad),      0,
                            0,              0,              0,                  1
                        }).transpose();
                    }

                    mouseCoordinates = operMouseCoordinates;
                }
                break;

            default:
                break;
            }
          
            if (event.type == sf::Event::Closed)
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

                // if (del > 0)
                //     radius -= 10;
                // else radius += 10;
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
        fps.update();
        std::cout << "fps: " << fps.get() << std::endl;
    }
    
    return 0;
}