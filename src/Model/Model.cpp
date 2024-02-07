#include "Model.hpp"

const std::vector<std::array<double, 4>>& ModelInterface::getVertexes() const {
    return _vertexes; 
}
const std::vector<std::array<int, 4>>& ModelInterface::getFaces() const {
    return _faces; 
}

void StubModel::stubfillVertexesProperty() {
    _vertexes.push_back({1, 0, 0, 1});
    _vertexes.push_back({1, 1, 0, 1});
    _vertexes.push_back({0, 1, 0, 1});
    _vertexes.push_back({0, 0, 0, 1});

    _vertexes.push_back({1, 0, 1, 1});
    _vertexes.push_back({0, 0, 1, 1});
    _vertexes.push_back({0, 0, 0, 1});
    _vertexes.push_back({1, 0, 0, 1});

    _vertexes.push_back({1, 0, 1, 1});
    _vertexes.push_back({1, 1, 0, 1});
    _vertexes.push_back({0, 1, 0, 1});
    _vertexes.push_back({0, 0, 1, 1});

    _vertexes.push_back({2, 0, -2, 1});
    _vertexes.push_back({-2, 0, -2, 1});
    _vertexes.push_back({-2, 0, 2, 1});
    _vertexes.push_back({2, 0, 2, 1});
}
void StubModel::stubfillFacesProperty() {
    _faces.push_back({1, 2, 3, 4});
    _faces.push_back({5, 6, 7, 8});
    _faces.push_back({9, 10, 11, 12});
    _faces.push_back({13, 14, 15, 16});
}