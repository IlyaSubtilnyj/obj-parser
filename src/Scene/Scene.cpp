#include "Scene.hpp"

#include <assert.h>
#include <array> 
#include <functional>
#include <math.h>
#include <memory>

void Scene::fitTheClock() {
    static double angle = 0;
    const static double radius = 5.0;
    angle += 3;
    if (angle >= 360) angle = 0;
    double ex = sin(angle * M_PI / 180) * radius;
    double ez = cos(angle * M_PI / 180) * radius;
    setCameraProps(ex, ez);
    Transform(); 
    return;
}

Scene::GatherScenePromise Scene::turnTheClock(sf::Time timer) {
    int mill = timer.asMilliseconds();
    this->_elapsed += mill;
    if (this->_elapsed > 28) {
        this->_elapsed = 0;
        GatherScenePromise gsp([&](){
            fitTheClock();
        });
        gsp.execute();
        return gsp;
    } else {
        GatherScenePromise gsp([&](){
            return;
        });
        gsp.execute();
        return gsp;
    }
}

void Scene::takeTheStage(sf::RenderTarget& stage) {
    stage.draw(*this, sf::RenderStates::Default);
}

void Scene::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (int i = 0; i < draw_coordinates.size(); i++)
    {
        auto& oper_vert_mas = draw_coordinates[i];
        target.draw(oper_vert_mas.data(), oper_vert_mas.size(), sf::LineStrip);
    }
}

void Scene::transformed() { is_transformed = !is_transformed; }
int Scene::isTransformed() const { return is_transformed; }
void Scene::Transform() {
    change_matrix_vec.clear();   
    for (auto& vec : modelVertexes) {
        auto first = worldTransform * vec;
        auto result = cameraTransform * first;
        auto res1 =  projectionMatrix * result;
        res1 = res1 / res1.get(3, 0);
        auto res2 = viewportMatrix * res1;
        change_matrix_vec.push_back(res2);
    }
    this->draw_coordinates.clear();
    auto& daFaces = _modele->getFaces();
    for (size_t i = 0; i < daFaces.size(); i++)
    {
        auto& oper_face = daFaces.at(i);
        auto oper_face_vertex_count = daFaces.at(i).size();
        std::vector<sf::Vertex> vec(oper_face_vertex_count+1);
        int j;
        for (j = 0; j < oper_face_vertex_count; j++)
        {
            auto& oper_line_strip_vertex = vec[j];
            oper_line_strip_vertex.position.x = (float)change_matrix_vec[oper_face[j]-1].get(0, 0);
            oper_line_strip_vertex.position.y = (float)change_matrix_vec[oper_face[j]-1].get(1, 0);
        }
        vec[j] = vec[0];
        this->draw_coordinates.push_back(std::move(vec));
    }
}

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

int Scene::setCameraProps(double xpos, double zpos) {
    matrix eye(1, 4, std::initializer_list<double>({xpos, 1, zpos, 1}));
    matrix target(1, 4, std::initializer_list<double>{0.5, 0, 0, 1});
    matrix up(1, 4, std::initializer_list<double>{0, 1, 0, 1});
    matrix ZAxis = vectorNormalize(eye - target);
    matrix XAxis = vectorNormalize(vectorMultiplication(up, ZAxis));
    matrix YAxis = up;
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

int Scene::setProjectionTransform() {
    const double aspect = (double)1920/1080;
    const double fov = 90;
    const double z_near = 0.001;
    const double z_far = 20;
    this->projectionMatrix = matrix(4, 4, std::initializer_list<double>{
        1.0 / (aspect * tan(fov * M_PI / 360)), 0, 0, 0,
        0, 1 / tan(fov * M_PI / 360), 0, 0,
        0, 0, z_far / (z_near - z_far), z_near * z_far / (z_near - z_far),
        0, 0, -1, 0
    });

    return 0;
}

int Scene::setViewportTransform() {
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
