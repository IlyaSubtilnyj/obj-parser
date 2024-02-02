#include "Model.hpp"

int StubModel::setWorldTransform(simple_matrix::matrix& transform) {
    transformed();
    worldTransform = transform;
}

void StubModel::Transform() {
    change_matrix_vec.clear();   
    for (auto& vec : daVertexes)
        change_matrix_vec.push_back(vec * worldTransform);
}

void StubModel::goOnStage(sf::RenderTarget& stage) {
    if (isTransformed()) 
        Transform();
    stage.draw(*this, sf::RenderStates::Default);
}

void StubModel::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    //target.draw();
}

void StubModel::transformed() { is_transformed = true; }
int StubModel::isTransformed() const { return is_transformed; }