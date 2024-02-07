#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <thread>
#include <future>

#include "matrix.hpp"
#include "Model.hpp"

class Scene: public sf::Drawable
 {
    protected:
        const int _n;
        int _elapsed;
    protected:
        std::unique_ptr<ModelInterface> _modele;
    public:
        Scene(std::unique_ptr<ModelInterface> modele, double scaleFactor)
        :   
            _n(4),
            _elapsed(0),
            _modele(std::move(modele))
        {    
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

            setCameraProps(0, 5);
            setProjectionTransform();
            setViewportTransform();
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
                accamulate_future.get();
                callback();
            }
        };

    protected:

        int setCameraProps(double xpos, double zpos);
        int setProjectionTransform();
        int setViewportTransform();
        
        simple_matrix::matrix worldTransform;
        std::vector<simple_matrix::matrix> modelVertexes;
 
        simple_matrix::matrix cameraTransform;
        simple_matrix::matrix projectionMatrix;
        simple_matrix::matrix viewportMatrix;

        std::vector<simple_matrix::matrix> change_matrix_vec;
        std::vector<std::vector<sf::Vertex>> draw_coordinates;
    protected:
        void fitTheClock();
    public:
        virtual GatherScenePromise turnTheClock(sf::Time);
        virtual void takeTheStage(sf::RenderTarget& stage);
    protected:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    private:
        bool is_transformed;
    protected:
        void transformed();
        int isTransformed() const;
        void Transform();
};