#pragma once

#include <array>
#include <memory>
#include <SFML/Graphics.hpp>
#include <vector>

#include "matrix.hpp"

class ModelInterface: public sf::Drawable
{
    public:
    virtual int setWorldTransform() = 0;
    virtual int setCameraProps() = 0;
    virtual int setProjectionTransform() = 0;
    virtual int setViewportTransform() = 0;
    virtual void turnTheClock(sf::Time) = 0;
    virtual void goOnStage(sf::RenderTarget& stage) = 0;
    protected:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
};

class StubModel: public ModelInterface 
{
    
    protected:
    std::vector<simple_matrix::matrix> daVertexes;
    std::vector<std::array<int, 4>> daFaces;
    std::vector<simple_matrix::matrix> change_matrix_vec;

    //<--Transformations-->
    simple_matrix::matrix worldTransform;
    simple_matrix::matrix cameraTransform;
    simple_matrix::matrix projectionMatrix;
    simple_matrix::matrix viewportMatrix;

    private:
    bool is_transformed;

    public:
    virtual int setWorldTransform() override;
    virtual int setCameraProps() override;
    virtual int setProjectionTransform() override;
    virtual int setViewportTransform() override;

    public:
        virtual void turnTheClock(sf::Time) override;
        virtual void goOnStage(sf::RenderTarget& stage) override;
        protected:
        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
    protected:
        void transformed();
        int isTransformed() const;
        void Transform();

    private:
    void stubfillVertexesProperty() {
        std::vector<std::array<double, 4>> daVertexes;
        daVertexes.push_back({1, 0, 0, 1});
        daVertexes.push_back({1, 1, 0, 1});
        daVertexes.push_back({0, 1, 0, 1});
        daVertexes.push_back({0, 0, 0, 1});

        daVertexes.push_back({1, 0, 1, 1});
        daVertexes.push_back({0, 0, 1, 1});
        daVertexes.push_back({0, 0, 0, 1});
        daVertexes.push_back({1, 0, 0, 1});

        daVertexes.push_back({1, 0, 1, 1});
        daVertexes.push_back({1, 1, 0, 1});
        daVertexes.push_back({0, 1, 0, 1});
        daVertexes.push_back({0, 0, 1, 1});

        daVertexes.push_back({5, 0, -5, 1});
        daVertexes.push_back({-5, 0, -5, 1});
        daVertexes.push_back({-5, 0, 5, 1});
        daVertexes.push_back({5, 0, 5, 1});

        for (size_t i = 0; i < daVertexes.size(); i++)
        {
            auto& vec = daVertexes.at(i);
            this->daVertexes.push_back({4, 1, {vec[0], vec[1], vec[2], vec[3]}});   
        }

    }
    void stubfillFacesProperty() {
        daFaces.push_back({1, 2, 3, 4});
        daFaces.push_back({5, 6, 7, 8});
        daFaces.push_back({9, 10, 11, 12});
        daFaces.push_back({13, 14, 15, 16});
    }

    public:
    StubModel()
    : is_transformed(false)
    {
        stubfillVertexesProperty(),
        stubfillFacesProperty();
        setWorldTransform();
        setCameraProps();
        setProjectionTransform();
        setViewportTransform();
        change_matrix_vec.clear();
        for (auto& vec : daVertexes)
        {
            auto init = worldTransform * vec;
            auto result = cameraTransform * init;
            auto res1 =  projectionMatrix * result;
            auto res2 = viewportMatrix * res1;
            change_matrix_vec.push_back(res2);
        }  
    }
};