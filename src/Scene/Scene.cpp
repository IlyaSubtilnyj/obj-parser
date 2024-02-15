#include "Scene.hpp"

#include <assert.h>
#include <array> 
#include <functional>
#include <math.h>
#include <memory>

void Scene::fitTheClock() {
    setCameraProps();
    Transform(); 
    return;
}

Scene::GatherScenePromise Scene::turkTheClockDelay(sf::Time timer) {
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

void Scene::turnTheClock(sf::Time timer) {
    int mill = timer.asMilliseconds();
    this->_elapsed += mill;
    if (this->_elapsed > 28) {
        fitTheClock();
    }
}

void Scene::takeTheStage(sf::RenderTarget& stage) {
    stage.draw(*this, sf::RenderStates::Default);
}

inline void line(int x0, int y0, int x1, int y1, sf::RenderTarget& target) {
    bool steep = false;
    if (std::abs(x0-x1)<std::abs(y0-y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0>x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1-x0;
    int dy = y1-y0;
    int derror2 = std::abs(dy)*2;
    int error2 = 0;
    int y = y0;
    sf::Vertex point;
    point.color = sf::Color::White;
    for (int x=x0; x<=x1; x++) {
        if (steep) {
            point.position.x = y;
            point.position.y = x;
            target.draw(&point, 1, sf::Points);
        } else {
            point.position.x = x;
            point.position.y = y;
            target.draw(&point, 1, sf::Points);
        }
        error2 += derror2;

        if (error2 > dx) {
            y += (y1>y0?1:-1);
            error2 -= dx*2;
        }
    }
}

void Scene::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (int i = 0; i < draw_coordinates.size(); i++)
    {
        auto& oper_vert_mas = draw_coordinates[i];
        for (int j = 0; j < oper_vert_mas.size() - 1; j++) {
            line(oper_vert_mas.at(j).position.x, oper_vert_mas.at(j).position.y, oper_vert_mas.at(j + 1).position.x, oper_vert_mas.at(j + 1).position.y, target);
        }
    }
}

void Scene::transformed() { is_transformed = !is_transformed; }
int Scene::isTransformed() const { return is_transformed; }
void Scene::Transform() {
    change_matrix_vec.clear();   
    for (int i = 0; i < modelVertexes.size(); i++) {
        auto& vec = modelVertexes[i];
        auto first = worldTransform * vec;
        auto result = cameraTransform * first;
        auto res1 =  projectionMatrix * result;
        res1 = res1 / res1.get(3, 0);
        auto res2 = viewportMatrix * res1;
        change_matrix_vec.push_back(res2);
    }
    this->draw_coordinates.clear();
    auto& daFaces = _modele->getFaces();
    for (int i = 0; i < daFaces.size(); i++)
    {
        auto& oper_face = daFaces.at(i);
        auto oper_face_vertex_count = (daFaces.at(i)[3] == 0) ? 3 : daFaces.at(i).size();
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
    result.set(0, 1, vector1.get(0, 2) * vector2.get(0, 0) - vector2.get(0, 2) * vector1.get(0, 0));
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

int Scene::setCameraProps() {
    std::cout << target << std::endl;
    std::cout << eye << std::endl;

    matrix ZAxis = vectorNormalize(eye - target);
    std::cout << ZAxis << std::endl;

    matrix XAxis = vectorNormalize(vectorMultiplication(up, ZAxis));
    std::cout << XAxis << std::endl;

    matrix YAxis = vectorNormalize(vectorMultiplication(XAxis, ZAxis));
    std::cout << YAxis << std::endl;

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
    std::cout << this->cameraTransform  << std::endl;
    return 0;
}

int Scene::setProjectionTransform() {
    const double aspect = (double)1920/1080;
    const double fov = 90;
    const double z_near = 0.001;
    const double z_far = 200;
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
        0, height/2, 0, y_min + height/2,
        0, 0, 1, 0,
        0, 0, 0, 1
    });

    return 0;
}
