#pragma once

#include <array>
#include <memory>
#include <SFML/Graphics.hpp>
#include <vector>

#include "matrix.hpp"

class ModelInterface
{
    protected:
    std::vector<std::array<double, 4>> _vertexes;
    std::vector<std::array<int, 4>> _faces;
    public:
    const std::vector<std::array<double, 4>>& getVertexes() const;
    const std::vector<std::array<int, 4>>& getFaces() const;
};

class StubModel: public ModelInterface 
{
    private:
        void stubfillVertexesProperty();
        void stubfillFacesProperty();
    public:
    StubModel()
    {
        stubfillVertexesProperty(),
        stubfillFacesProperty();
    }
};