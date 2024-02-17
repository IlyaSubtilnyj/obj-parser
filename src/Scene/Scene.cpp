#include "Scene.hpp"

#include <assert.h>
#include <array> 
#include <functional>
#include <math.h>
#include <memory>
//#include <x86intrin.h>
//#include <emmintrin.h>

#define deprecated

using namespace simple_matrix;

deprecated
#if 1
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
                //line(oper_vert_mas.at(j).position.x, oper_vert_mas.at(j).position.y, oper_vert_mas.at(j + 1).position.x, oper_vert_mas.at(j + 1).position.y, target);
            }
        }
    }

#endif

__m256d divide_by_scalar(__m256d vector, double scalar) {
    return _mm256_div_pd(vector, _mm256_set1_pd(scalar));
}

void Scene::fitTheClock() {
    //if (!isTransformed()) return;
    setCameraProps();
    Transform();
    transformed();
}

inline void Scene::line(int x0, int y0, int x1, int y1) {
    const static sf::Color color = sf::Color::White;
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
    for (int x=x0; x<=x1; x++) {
        if (steep) {
            if ((y < 0) || (y >= imageWidth)) return;
            if ((x < 0) || (x >= imageHeight)) return;
            gTarget.setPixel(y, x, color);
        } else {
            if ((x < 0) || (x >= imageWidth)) return;
            if ((y < 0) || (y >= imageHeight)) return;
            gTarget.setPixel(x, y, color);
        }
        error2 += derror2;

        if (error2 > dx) {
            y += (y1>y0?1:-1);
            error2 -= dx*2;
        }
    }
}

class ClearImageTask : public TaskType {
public:
    ClearImageTask(sf::Image& image, concurrent_queue<std::shared_ptr<TaskType>>* queue) 
        : TaskType(queue), _image(image)
    {}
    unsigned long virtual one_thread_method(void*) override {
        const static sf::Color color = sf::Color::Black;
        for (size_t i = 0; i < _image.getSize().x; i++)
        {
            for (size_t j = 0; j < _image.getSize().y; j++)
            {
                _image.setPixel(i, j, color);
            }
        }
        return 0;
    }
private:
    sf::Image& _image;
};

void Scene::drawUseBuffer(sf::RenderWindow& window) {

    auto& daFaces = _modele->getFaces();
    for (int i = 0; i < daFaces.size(); i++)
    {
        auto& oper_face = daFaces.at(i);
        auto oper_face_vertex_count = (oper_face[3] == 0) ? 3 : oper_face.size();
        int j;
        for (j = 1; j < oper_face_vertex_count - 1; j++)
        {
            line(realtimeCoordinates[oper_face[j - 1] - 1](0, 0), realtimeCoordinates[oper_face[j - 1] - 1](1, 0),
             realtimeCoordinates[oper_face[j] - 1](0, 0), realtimeCoordinates[oper_face[j] - 1](1, 0));
        }
        line(realtimeCoordinates[oper_face[j - 1] - 1](0, 0), realtimeCoordinates[oper_face[j - 1] - 1](1, 0),
             realtimeCoordinates[oper_face[0] - 1](0, 0), realtimeCoordinates[oper_face[0] - 1](1, 0));
    }

    this->texture.loadFromImage(gTarget);
    this->sprite.setTexture(this->texture);
    window.draw(sprite);
    auto task = ClearImageTask(std::ref(gTarget), &complitedTasksQueue);
    manager->try_add_task(task);
    //complitedTasksQueue.pop();
}

inline void Scene::transformed() { is_transformed = !is_transformed; }
inline int Scene::isTransformed() const { return is_transformed; }

std::vector<std::pair<int, int>> sliceVector(int size, int n) {
    std::vector<std::pair<int, int>> result;

    if (size < n) {
        throw std::runtime_error("The input vector is too small to be sliced into n parts.");
    }

    int size_per_part = size / n;
    int remaining_size = size % n;

    int end_index = 0;
    int start_index = 0;
    for (int i = 0; i < n; ++i) {
        start_index = end_index;
        end_index = start_index + size_per_part;

        if (i < remaining_size) {
            end_index += 1; // Include the remaining elements if there are any
        }

        result.push_back(std::make_pair(start_index, end_index - start_index));
    }

    return result;
}

void Scene::Transform() {

        auto sliced = sliceVector(modelVertexes.size(), this->_thread_count);

        /*Create tasks*/
        std::vector<SceneTask> tasks;
        for (size_t i = 0; i < this->_thread_count; i++)
        {
            tasks.emplace_back(SceneTask(sliced.at(i).first, sliced.at(i).second, modelVertexes, this->realtimeCoordinates, worldTransform, cameraTransform, projectionMatrix,
            viewportMatrix, &complitedTasksQueue));
        }

        /*Insert tasks into manager*/
        for (size_t i = 0; i < this->_thread_count; i++)
        {
            manager->try_add_task(tasks.at(i));
        }

        /*Gather results*/
        std::vector<SceneTask> results;
        for (size_t i = 0; i < this->_thread_count; i++)
        {
            complitedTasksQueue.pop();
        }
        int debug_dummy{};
#if 0
        change_matrix_vec.clear(); 
        simple_matrix::matrix result;
        for (int i = 0; i < modelVertexes.size(); i++) {
            result = (projectionMatrix * (cameraTransform * (worldTransform * modelVertexes.at(i))));
            _mm256_storeu_pd(result.begin(), 
                divide_by_scalar(
                    _mm256_loadu_pd(result.begin()), 
                    result.get(3, 0)
                ));
            change_matrix_vec.emplace_back(viewportMatrix * result);
        }
#endif
}

matrix vectorNormalize(matrix&& vector) {
    double magnitude{};
    for (auto i = vector.begin(); i < vector.end() - 1; i++)
    {
        magnitude += powf64(*i, 2);
    }
    vector /= sqrtf64(magnitude);
    return std::move(vector);
}

matrix vectorMultiplication(const matrix& vector1, const matrix& vector2) {
    return simple_matrix::matrix(1, 4, {vector1.get(0, 1) * vector2.get(0, 2) - vector2.get(0, 1) * vector1.get(0, 2),
                        vector1.get(0, 2) * vector2.get(0, 0) - vector2.get(0, 2) * vector1.get(0, 0),
                        vector1.get(0, 0) * vector2.get(0, 1) - vector2.get(0, 0) * vector1.get(0, 1), 1.0});
}

double scalarVectorMultiplication(const matrix& vector1, const matrix& vector2) {
    double result{};
    for (size_t i = 0; i < vector1.n() - 1; i++)
        result += vector1.get(0, i) * vector2.get(0, i);
    return result;
}

void Scene::setCameraProps() {
    matrix ZAxis(vectorNormalize(eye - target));
    matrix XAxis(vectorNormalize(vectorMultiplication(up, ZAxis)));
    matrix YAxis(vectorNormalize(vectorMultiplication(XAxis, ZAxis)));

    cameraTransform.set_row(0, XAxis.get_row(0));
    cameraTransform.set_row(1, YAxis.get_row(0));
    cameraTransform.set_row(2, ZAxis.get_row(0));
    cameraTransform.set_col(3, simple_matrix::matrix(4, 1, {-scalarVectorMultiplication(XAxis, eye),
     -scalarVectorMultiplication(YAxis, eye), -scalarVectorMultiplication(ZAxis, eye), 1.0}));
}

void Scene::setProjectionTransform() {
    const double aspect = (double)1920/1080;
    const double fov = 120;
    const double z_near = 50;
    const double z_far = 200;
    this->projectionMatrix = matrix(4, 4, std::initializer_list<double>{
        1.0 / (aspect * tan(fov * M_PI / 360)), 0, 0, 0,
        0, 1 / tan(fov * M_PI / 360), 0, 0,
        0, 0, z_far / (z_near - z_far), z_near * z_far / (z_near - z_far),
        0, 0, -1, 0
    });
}

void Scene::setViewportTransform() {
    const double aspect = (double)1920/1080;
    const double width = 1920;
    const double height = 1080;
    const double x_min = 0;
    const double y_min = 0;
    this->viewportMatrix = matrix(4, 4, std::initializer_list<double>{
        width/2, 0, 0, x_min + width/2,
        0, height/2, 0, y_min + height/2,
        0, 0, 1, 0,
        0, 0, 0, 1
    });
}
