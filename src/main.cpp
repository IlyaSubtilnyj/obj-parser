#include <SFML/Graphics.hpp>
#include <assert.h>
#include <array>
#include <math.h>
#include <vector>
#include "matrix.hpp"

using namespace simple_matrix;

std::vector<matrix> matrix_vec;
std::vector<matrix> change_matrix_vec;

std::vector<std::array<double, 4>> vertex_mas;
void fillVertexes() {
    vertex_mas.push_back({1, 0, 0, 1});
    vertex_mas.push_back({1, 1, 0, 1});
    vertex_mas.push_back({0, 1, 0, 1});
    vertex_mas.push_back({0, 0, 0, 1});

    vertex_mas.push_back({1, 0, 1, 1});
    vertex_mas.push_back({0, 0, 1, 1});
    vertex_mas.push_back({0, 0, 0, 1});
    vertex_mas.push_back({1, 0, 0, 1});

    vertex_mas.push_back({1, 0, 1, 1});
    vertex_mas.push_back({1, 1, 0, 1});
    vertex_mas.push_back({0, 1, 0, 1});
    vertex_mas.push_back({0, 0, 1, 1});

    vertex_mas.push_back({5, 0, -5, 1});
    vertex_mas.push_back({-5, 0, -5, 1});
    vertex_mas.push_back({-5, 0, 5, 1});
    vertex_mas.push_back({5, 0, 5, 1});

}
std::vector<std::array<int, 4>> faces_mas;
void fillFaces() {
    faces_mas.push_back({1, 2, 3, 4});
    faces_mas.push_back({5, 6, 7, 8});
    faces_mas.push_back({9, 10, 11, 12});
    faces_mas.push_back({13, 14, 15, 16});
}

matrix vectorNormalize(matrix vector) {
    assert(vector.m());
    auto cols = vector.n();
    double magnitude{};
    for (size_t i = 0; i < cols - 1; i++)
    {
        magnitude += powf64(vector.get(0, i), 2);
    }
    return vector / sqrtf64(magnitude);
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
    for (size_t i = 0; i < vector1.n() - 1; i++)
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

void draww(sf::RenderWindow& window, std::vector<sf::Vector2f> mas) {
    const sf::Vertex center(sf::Vector2f(1920/2, 1080/2));
    sf::Vertex vec[mas.size()+1];
    for (size_t i = 0; i < mas.size(); i++)
    {
        vec[i].position.x = mas[i].x + center.position.x;
        vec[i].position.y = center.position.y + mas[i].y;
    }
    vec[mas.size()] = vec[0];
    window.draw(vec, mas.size() + 1, sf::LineStrip);
}

void update(sf::Time timer, matrix& projectionMatrix, matrix& viewportMatrix) {
    static int time = 0;
    static double e1 = 0.0;
    static const double e2 = 1;
    static double e3 = 10.0;
    static double angle = 0;
    const static double radius = 10.0;
    int mill = timer.asMilliseconds();
    time += mill;
    if (time > 28) {
        time = 0;

        angle += 3;
        if (angle >= 360) angle = 0;
        e1 = sin(angle * M_PI / 180) * radius;
        e3 = cos(angle * M_PI / 180) * radius;

        matrix eye(1, 4, std::initializer_list<double>{e1, e2, e3, 1});
        matrix target(1, 4, std::initializer_list<double>{0, 0, 0, 1});
        matrix up(1, 4, std::initializer_list<double>{0, 1, 0, 1});

        matrix ZAxis = vectorNormalize(eye - target);
        matrix XAxis = vectorNormalize(vectorMultiplication(up, ZAxis));
        matrix YAxis = up;

        matrix viewMatrix(4, 4);
        viewMatrix.set_row(0, XAxis.get_row(0));
        viewMatrix.set_row(1, YAxis.get_row(0));
        viewMatrix.set_row(2, ZAxis.get_row(0));
        viewMatrix.set(0, 3, -scalarVectorMultiplication(XAxis, eye));
        viewMatrix.set(1, 3, -scalarVectorMultiplication(YAxis, eye));
        viewMatrix.set(2, 3, -scalarVectorMultiplication(ZAxis, eye));
        viewMatrix.set(3, 0, 0);
        viewMatrix.set(3, 1, 0);
        viewMatrix.set(3, 2, 0);
        viewMatrix.set(3, 3, 1);

        change_matrix_vec.clear();
        for (auto& vec : matrix_vec)
        {
            change_matrix_vec.push_back(vec * viewMatrix * projectionMatrix * viewportMatrix);
        }   
    }
}

int main()
{

    fillVertexes(); fillFaces();

    for (size_t i = 0; i < vertex_mas.size(); i++)
    {
        auto& vec = vertex_mas.at(i);
        matrix_vec.push_back({1, 4, {vec[0], vec[1], vec[2], vec[3]}});   
    }

    printVecMat(matrix_vec, "Before global");

    //--------------------global--------------------

    std::array<double, 3> scaleFactors{1, 1, 1};
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

    //---------------------view-----------------
    
    matrix eye(1, 4, std::initializer_list<double>{2, 1, 10, 1});
    matrix target(1, 4, std::initializer_list<double>{0, 1, 0, 1});
    matrix up(1, 4, std::initializer_list<double>{0, 1, 0, 1});

    matrix ZAxis = vectorNormalize(eye - target);
    matrix XAxis = vectorNormalize(vectorMultiplication(up, ZAxis));
    matrix YAxis = up;

    std::cout << "ZAxis\n" << ZAxis << std::endl;
    std::cout << "XAxis\n" << XAxis << std::endl;
    std::cout << "YAxis\n" << YAxis << std::endl;

    matrix viewMatrix(4, 4);
    viewMatrix.set_row(0, XAxis.get_row(0));
    viewMatrix.set_row(1, YAxis.get_row(0));
    viewMatrix.set_row(2, ZAxis.get_row(0));
    viewMatrix.set(0, 3, -scalarVectorMultiplication(XAxis, eye));
    viewMatrix.set(1, 3, -scalarVectorMultiplication(YAxis, eye));
    viewMatrix.set(2, 3, -scalarVectorMultiplication(ZAxis, eye));
    viewMatrix.set(3, 0, 0);
    viewMatrix.set(3, 1, 0);
    viewMatrix.set(3, 2, 0);
    viewMatrix.set(3, 3, 1);
    std::cout << "viewMatrix\n" << viewMatrix << std::endl;

    //-------------------projection--------------------

    const double aspect = (double)1920/1080;
    const double fov = 120;
    const double z_near = 0.001;
    const double z_far = 10;
    matrix projectionMatrix(4, 4, std::initializer_list<double>{
        1.0 / (aspect * tan(fov * M_PI / 360)), 0, 0, 0,
        0, 1 / tan(fov * M_PI / 360), 0, 0,
        0, 0, z_far / (z_near - z_far), z_near * z_far / (z_near - z_far),
        0, 0, -1, 0
    });
    std::cout << "projectionMatrix\n" << projectionMatrix << std::endl;

    //----------------------viewport------------------------


    const double width = 1920;
    const double height = 1080;
    const double x_min = 0;
    const double y_min = 0;
    assert(abs(aspect - width / height) <= 0.00001);
    matrix viewportMatrix(4, 4, std::initializer_list<double>{
        width/2, 0, 0, x_min + width/2,
        0, -height/2, 0, y_min + height/2,
        0, 0, 1, 0,
        0, 0, 0, 1
    });

    //----------------------result-----------------

    for (auto& vec : matrix_vec)
    {
        change_matrix_vec.push_back(vec * viewMatrix * projectionMatrix * viewportMatrix);
    }   
    printVecMat(matrix_vec, "After all");

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    sf::Texture texeture;
    if (!texeture.loadFromFile("../../resources/img.jpg")) {
        return EXIT_FAILURE;
    }
    sf::Sprite sprite(texeture);

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
        update(elapsed, projectionMatrix, viewportMatrix);

         sf::sleep(sf::seconds(0.03));

        window.clear();
        //window.draw(sprite);

        for (size_t i = 0; i < faces_mas.size(); i++)
        {
            auto& arr = faces_mas.at(i);
            std::vector<sf::Vector2f> mas;
            int j;
            for (j = 0; j < arr.size(); j++)
            {
                mas.push_back({(float)change_matrix_vec[arr.at(j)-1].get(0, 0), (float)change_matrix_vec[arr.at(j)-1].get(0, 1)});
            }
            draww(window, mas);
        }

        window.display();
    }

    return 0;
}
