#include <SFML/Graphics.hpp>
#include <assert.h>
#include <array>
#include <math.h>
#include <vector>
#include "matrix.hpp"

std::vector<std::array<double, 4>> vertex_mas;
void fillVertexes() {
    vertex_mas.push_back({0, 0, 0, 1});
    vertex_mas.push_back({0, 0, 5, 1});
    vertex_mas.push_back({5, 0, 0, 1});
    vertex_mas.push_back({5, 0, 5, 1});
}
std::vector<std::array<int, 4>> faces_mas;
void fillFaces() {
    faces_mas.push_back({1, 2, 3, 4});
}

using namespace simple_matrix;

matrix vectorNormalize(matrix vector) {
    assert(vector.m());
    auto cols = vector.n();
    int magnitude{};
    for (size_t i = 0; i < cols; i++)
    {
        magnitude += vector.get(0, i) * vector.get(0, i);
    }
    return vector / magnitude;
}

matrix vectorMultiplication(matrix vector1, matrix vector2) {
    assert(vector1.m() == vector2.m() == 1);
    assert(vector1.n() == vector2.n());
    matrix result(1, 4); result.set(0, 3, 1);
    result.set(0, 0, vector1.get(0, 1) * vector2.get(0, 2) - vector2.get(0, 1) * vector1.get(0, 2));
    result.set(0, 1, vector1.get(0, 0) * vector2.get(0, 2) - vector2.get(0, 0) * vector1.get(0, 2));
    result.set(0, 2, vector1.get(0, 0) * vector2.get(0, 1) - vector2.get(0, 0) * vector1.get(0, 1));
    return result;
}

double scalarVectorMultiplication(matrix vector1, matrix vector2) {
    assert(vector1.m() == vector2.m() == 1);
    assert(vector1.n() == vector2.n());
    double result{};
    for (size_t i = 0; i < vector1.n(); i++)
        result += vector1.get(0, i) * vector2.get(0, i);
    return result;
}

void printVecMat(std::vector<matrix>& matrix, std::string mes) {
    std::cout << mes << std::endl;
    for (auto& vec : matrix)
    {
        std::cout << vec << std::endl;
    }
    std::cout << std::endl;
}

int main()
{

    fillVertexes(); fillFaces();

    std::vector<matrix> matrix_vec;

    for (size_t i = 0; i < vertex_mas.size(); i++)
    {
        auto& vec = vertex_mas.at(i);
        matrix_vec.push_back({1, 4, {vec[0], vec[1], vec[2], vec[3]}});   
    }

    printVecMat(matrix_vec, "Before global");

    //--------------------global--------------------

    std::array<double, 3> scaleFactors{1, 2, 2};
    matrix scaleMatrix(4, 4, std::initializer_list<double>{
        scaleFactors[0],    0,                  0,                  0,
        0,                  scaleFactors[1],    0,                  0,
        0,                  0,                  scaleFactors[2],    0,
        0,                  0,                  0,                  1
    });
    for (auto& vec : matrix_vec)
    {
        vec = vec * scaleMatrix;
    }
    printVecMat(matrix_vec, "After global");

    //---------------------viewport-----------------
    
    matrix eye(1, 4, std::initializer_list<double>{0, 5, 0, 1});
    matrix target(1, 4, std::initializer_list<double>{0, 0, 0, 1});
    matrix up(1, 4, std::initializer_list<double>{0, 1, 0, 1});

    matrix ZAxis = vectorNormalize(eye - target);
    matrix XAxis = vectorNormalize(vectorMultiplication(up, ZAxis));
    matrix YAxis = up;

    std::cout << "ZAxis\n" << ZAxis << std::endl;
    std::cout << "ZAxis\n" << XAxis << std::endl;
    std::cout << "YAxis\n" << YAxis << std::endl;

    matrix viewportMatrix(4, 4);
    viewportMatrix.set_row(0, XAxis.get_row(0));
    viewportMatrix.set_row(1, YAxis.get_row(0));
    viewportMatrix.set_row(2, ZAxis.get_row(0));
    viewportMatrix.set(0, 3, -scalarVectorMultiplication(XAxis, eye));
    viewportMatrix.set(1, 3, -scalarVectorMultiplication(YAxis, eye));
    viewportMatrix.set(2, 3, -scalarVectorMultiplication(ZAxis, eye));
    viewportMatrix.set(3, 0, 0);
    viewportMatrix.set(3, 1, 0);
    viewportMatrix.set(3, 2, 0);
    viewportMatrix.set(3, 3, 1);
    std::cout << "viewportMatrix\n" << viewportMatrix << std::endl;

    //-------------------projection--------------------

    const double aspect = 1920/1080;
    const double fov = 1.2;
    const double z_near = 0.20;
    const double z_far = 5;
    matrix projectionMatrix(4, 4, std::initializer_list<double>{
        1.0 / (aspect * tan(fov / 2)), 0, 0, 0,
        0, 1 / tan(fov / 2), 0, 0,
        0, 0, 1 / (z_near - z_far), z_near / (z_near - z_far),
        0, 0, 0, 1
    });
    std::cout << "projectionMatrix\n" << projectionMatrix << std::endl;

    //----------------------view------------------------


    const int width = 1920;
    const int height = 1080;
    const int x_min = 0;
    const int y_min = 0;
    assert(abs(aspect - width / height) <= 0.00001);
    matrix viewMatrix(4, 4, std::initializer_list<double>{
        width/2, 0, 0, x_min + width/2,
        0, -height/2, 0, y_min + height/2,
        0, 0, 1, 0,
        0, 0, 0, 1
    });

    //----------------------result-----------------

    matrix superMatrix = viewportMatrix * projectionMatrix * viewMatrix;

    for (auto& vec : matrix_vec)
    {
        vec = vec * superMatrix;
    }
    printVecMat(matrix_vec, "After all");

    /*{
        sf::RenderWindow window(sf::VideoMode(1920, 1080), "SFML works!");
        sf::CircleShape shape(100.f);
        shape.setFillColor(sf::Color::Green);

        sf::Texture texeture;
        if (!texeture.loadFromFile("../resources/img.jpg")) {
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
    }*/

    return 0;
}
