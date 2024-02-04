#include "Model.hpp"

#include <assert.h>
#include <math.h>

using namespace simple_matrix;

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


int StubModel::setWorldTransform(/*std::unique_ptr<matrix> transform*/) {
    std::array<double, 3> scaleFactors{1, 1, 1};
    this->worldTransform = matrix(4, 4, std::initializer_list<double>{
        scaleFactors[0],    0,                  0,                  0,
        0,                  scaleFactors[1],    0,                  0,
        0,                  0,                  scaleFactors[2],    0,
        0,                  0,                  0,                  1
    });
    return 0;
}

int StubModel::setCameraProps() {
   
    matrix eye(1, 4, std::initializer_list<double>{3, 2, 7, 1});
    matrix target(1, 4, std::initializer_list<double>{0, 0, 0, 1});
    matrix up(1, 4, std::initializer_list<double>{0, 1, 0, 1});

    matrix ZAxis = vectorNormalize(eye - target);
    matrix XAxis = vectorNormalize(vectorMultiplication(up, ZAxis));
    matrix YAxis = up;

    std::cout << "ZAxis\n" << ZAxis << std::endl;
    std::cout << "XAxis\n" << XAxis << std::endl;
    std::cout << "YAxis\n" << YAxis << std::endl;

    this->cameraTransform = matrix(4, 4);
    cameraTransform.set_row(0, XAxis.get_row(0));
    cameraTransform.set_row(1, YAxis.get_row(0));
    cameraTransform.set_row(2, ZAxis.get_row(0));
    cameraTransform.set(0, 3, -scalarVectorMultiplication(XAxis, eye));
    cameraTransform.set(1, 3, -scalarVectorMultiplication(YAxis, eye));
    cameraTransform.set(2, 3, -scalarVectorMultiplication(ZAxis, eye));
    cameraTransform.set(3, 0, 0);
    cameraTransform.set(3, 1, 0);
    cameraTransform.set(3, 2, 0);
    cameraTransform.set(3, 3, 1);

    return 0;
}

int StubModel::setProjectionTransform() {
    const double aspect = (double)1920/1080;
    const double fov = 90;
    const double z_near = 0.001;
    const double z_far = 10;
    this->projectionMatrix = matrix(4, 4, std::initializer_list<double>{
        1.0 / (aspect * tan(fov * M_PI / 360)), 0, 0, 0,
        0, 1 / tan(fov * M_PI / 360), 0, 0,
        0, 0, z_far / (z_near - z_far), z_near * z_far / (z_near - z_far),
        0, 0, -1, 0
    });

    return 0;
}

int StubModel::setViewportTransform() {
    const double aspect = (double)1920/1080;
    const double width = 1920;
    const double height = 1080;
    const double x_min = 0;
    const double y_min = 0;
    assert(abs(aspect - width / height) <= 0.00001);
    this->viewportMatrix = matrix(4, 4, std::initializer_list<double>{
        width/2, 0, 0, x_min + width/2,
        0, -height/2, 0, y_min + height/2,
        0, 0, 1, 0,
        0, 0, 0, 1
    });

    return 0;
}

void StubModel::Transform() {
    change_matrix_vec.clear();   
    for (auto& vec : daVertexes) {
        //change_matrix_vec.push_back(viewportMatrix * projectionMatrix * cameraTransform * worldTransform);
        auto init = worldTransform * vec;
        auto result = cameraTransform * init;
        auto res1 =  projectionMatrix * result;
        res1 = res1 / res1.get(3, 0);
        auto res2 = viewportMatrix * res1;
        std::cout << "Vec " << vec << std::endl;
        std::cout << "Result" << res2 << std::endl;
        change_matrix_vec.push_back(res2);
    }
    transformed();
}

void StubModel::turnTheClock(sf::Time timer) {
    static int time = 0;
    static double e1 = 0.0;
    static const double e2 = 1;
    static double e3 = 10.0;
    static double angle = 0;
    const static double radius = 10.0;
    int mill = timer.asMilliseconds();
    time += mill;
    if (time > 28) {
        // time = 0;

        // angle += 3;
        // if (angle >= 360) angle = 0;
        // e1 = sin(angle * M_PI / 180) * radius;
        // e3 = cos(angle * M_PI / 180) * radius;

        // matrix eye(1, 4, std::initializer_list<double>{e1, e2, e3, 1});
        // matrix target(1, 4, std::initializer_list<double>{0, 0, 0, 1});
        // matrix up(1, 4, std::initializer_list<double>{0, 1, 0, 1});

        // matrix ZAxis = vectorNormalize(eye - target);
        // matrix XAxis = vectorNormalize(vectorMultiplication(up, ZAxis));
        // matrix YAxis = up;

        // this->cameraTransform = matrix(4, 4);
        // cameraTransform.set_row(0, XAxis.get_row(0));
        // cameraTransform.set_row(1, YAxis.get_row(0));
        // cameraTransform.set_row(2, ZAxis.get_row(0));
        // cameraTransform.set(0, 3, -scalarVectorMultiplication(XAxis, eye));
        // cameraTransform.set(1, 3, -scalarVectorMultiplication(YAxis, eye));
        // cameraTransform.set(2, 3, -scalarVectorMultiplication(ZAxis, eye));
        // cameraTransform.set(3, 0, 0);
        // cameraTransform.set(3, 1, 0);
        // cameraTransform.set(3, 2, 0);
        // cameraTransform.set(3, 3, 1);

        transformed(); 
    }
}

void StubModel::goOnStage(sf::RenderTarget& stage) {
    if (isTransformed()) 
        Transform();
    stage.draw(*this, sf::RenderStates::Default);
}

void StubModel::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (size_t i = 0; i < daFaces.size(); i++)
    {
        auto& arr = daFaces.at(i);
        std::vector<sf::Vector2f> mas;
        int j;
        for (j = 0; j < arr.size(); j++)
        {
            mas.push_back({(float)change_matrix_vec[arr.at(j)-1].get(0, 0), (float)change_matrix_vec[arr.at(j)-1].get(1, 0)});
        }
        //const sf::Vertex center(sf::Vector2f(1920/2, 1080/2));
        sf::Vertex vec[mas.size()+1];
        for (size_t i = 0; i < mas.size(); i++)
        {
            vec[i].position.x = mas[i].x;// + center.position.x;
            vec[i].position.y = mas[i].y; //center.position.y + mas[i].y;
        }
        vec[mas.size()] = vec[0];
        target.draw(vec, mas.size() + 1, sf::LineStrip);
    }
}

void StubModel::transformed() { is_transformed = !is_transformed; }
int StubModel::isTransformed() const { return is_transformed; }