#pragma once

#include <atomic>
#include <functional>
#include <math.h>
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <thread>
#include <future>
#include <x86intrin.h>
#include <emmintrin.h>

#include "matrix.hpp"
#include "../parser/parser.hpp"
#include "TMConsumer.hpp"

// Helper function to divide a vector by a scalar value using SSE intrinsics
__m256d divide_by_scalar(__m256d vector, double scalar);

class SceneTask : public TaskType {
public:
    SceneTask(int32_t index, int32_t size, std::vector<simple_matrix::matrix>& array, simple_matrix::matrix* output_array, simple_matrix::matrix& worldTransform, simple_matrix::matrix& cameraTransform,
    simple_matrix::matrix& projectionMatrix, simple_matrix::matrix& viewportMatrix,
     concurrent_queue<std::shared_ptr<TaskType>>* queue) 
        : TaskType(queue), _index(index), _size(size), _array(array), _output_array(output_array), _worldTransform(worldTransform), _cameraTransform(cameraTransform),
        _projectionMatrix(projectionMatrix), _viewportMatrix(viewportMatrix)
    {}
    unsigned long virtual one_thread_method(void*) override {
        simple_matrix::matrix result;
        for (size_t i = _index; i < _index + _size; i++)
        {
            result = (_projectionMatrix * (_cameraTransform * (_worldTransform * _array.at(i))));
            _mm256_storeu_pd(result.begin(), 
                divide_by_scalar(
                    _mm256_loadu_pd(result.begin()), 
                    result.get(3, 0)
                ));
            _output_array[i] = _viewportMatrix * result;
        }   
        return 0;
    }
private:
    int32_t _index;
    int32_t _size;
    std::vector<simple_matrix::matrix>& _array;
    simple_matrix::matrix& _worldTransform;
    simple_matrix::matrix& _cameraTransform;
    simple_matrix::matrix& _projectionMatrix;
    simple_matrix::matrix& _viewportMatrix;
    simple_matrix::matrix* _output_array;
};

class Scene: public sf::Drawable, public Consumer
 {
    public:
        simple_matrix::matrix eye;
    protected:
        const int _n;
        int _elapsed;
    protected:
        std::shared_ptr<ModelInterface> _modele;
    protected:
        const simple_matrix::matrix up;
        const simple_matrix::matrix target;
        const double schera_radius;
        sf::Image& gTarget;
        int imageHeight, imageWidth;
        int32_t _thread_count;
    public:
        Scene(std::shared_ptr<TaskManager> manager, int32_t thread_count, std::shared_ptr<ModelInterface> modele, double scaleFactor, sf::Image& image)
        :   
            Consumer(manager),
            _thread_count(thread_count),
            _n(4),
            _elapsed(0),
            _modele(std::move(modele)), 
            schera_radius(80.0),
            up(1, 4, std::initializer_list<double>{0, 1, 0, 1}),
            target(1, 4, std::initializer_list<double>{0, 0, 0, 1}),
            is_transformed(false),
            gTarget(image),
            imageHeight(image.getSize().y),
            imageWidth(image.getSize().x)
        {   
            realtimeCoordinates = new simple_matrix::matrix[_modele->getVertexes().size()];
            eye = simple_matrix::matrix(1, 4, std::initializer_list<double>{0, 0, schera_radius, 1});
            worldTransform = simple_matrix::matrix({static_cast<simple_matrix::uint>(_n), static_cast<simple_matrix::uint>(_n),
            std::initializer_list<double>{
                scaleFactor, 0, 0, 0,
                0, scaleFactor, 0, 0,
                0, 0, scaleFactor, 0,
                0, 0, 0, 1
            }});

            for (const auto& vec : _modele->getVertexes())
            {
                modelVertexes.push_back(
                    worldTransform * simple_matrix::matrix({4, 1, {vec[0], vec[1], vec[2], vec[3]}})
                );
            }

            change_matrix_vec.reserve(_modele->getVertexes().size());

            cameraTransform = simple_matrix::matrix(4, 4);
            projectionMatrix = simple_matrix::matrix(4, 4);
            viewportMatrix = simple_matrix::matrix(4, 4);

            setCameraProps();
            setProjectionTransform();
            setViewportTransform();
        }

        ~Scene() {
            delete[] realtimeCoordinates;
        }


        class GatherScenePromise
        {
            protected:
            std::function<void()> _async_function;
            std::future<void> accamulate_future;
            public:
            GatherScenePromise(std::function<void()> async_function)
            {
                _async_function = async_function;
            }

            void execute() {
                accamulate_future = std::async(std::launch::async, _async_function);
            }

            void then(std::function<void()> callback) {
                accamulate_future.wait();
                callback();
            }
        };

    protected:

        void setCameraProps();
        void setProjectionTransform();
        void setViewportTransform();
        
        simple_matrix::matrix worldTransform;
        std::vector<simple_matrix::matrix> modelVertexes;
 
        simple_matrix::matrix cameraTransform;
        simple_matrix::matrix projectionMatrix;
        simple_matrix::matrix viewportMatrix;

        simple_matrix::matrix* realtimeCoordinates;
        std::vector<simple_matrix::matrix> change_matrix_vec;
        std::vector<std::vector<sf::Vertex>> draw_coordinates;
    protected:
        sf::Image screenshot;
        sf::Texture texture;
        sf::Sprite sprite;
    public:
        void fitTheClock();
    public:
        void turnTheClock(sf::Time timer);
        Scene::GatherScenePromise turkTheClockDelay(sf::Time time);
        virtual void takeTheStage(sf::RenderTarget& stage);
        void drawUseBuffer(sf::RenderWindow& window);
    protected:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    private:
        bool is_transformed;
    public:
        void transformed();
        int isTransformed() const;
    protected:
        void Transform();
        void line(int x0, int y0, int x1, int y1);
};