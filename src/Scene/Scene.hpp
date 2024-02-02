#pragma once

#include <memory>

#include "matrix.hpp"
#include "Model.hpp"

class Scene {
    private:
        std::unique_ptr<ModelInterface> modele;
    public:
        Scene(std::unique_ptr<ModelInterface> modele)
        :   modele(std::move(modele))
        {
            
        }
    protected:
        
};