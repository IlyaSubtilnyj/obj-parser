#pragma once

// #include <array>
// #include <memory>
// #include <SFML/Graphics.hpp>
// #include <vector>

// typedef std::array<double, 4> vertex_t;
// typedef std::array<int, 4> face_t;
// typedef std::vector<vertex_t> vectore_store_t;
// typedef std::vector<face_t> face_store_t;

// class ModelInterface
// {
//     protected:
//     vectore_store_t vertexStore;
//     vectore_store_t textureStore;
//     vectore_store_t normalStore;
//     face_store_t faceStore;
//     public:
//     const vectore_store_t& getVertexes() const;
//     const face_store_t& getFaces() const;

//     friend class parser::ObjParser;
// };

// class StubModel: public ModelInterface 
// {
//     private:
//         void stubfillVertexesProperty();
//         void stubfillFacesProperty();
//     public:
//     StubModel()
//     {
//         stubfillVertexesProperty(),
//         stubfillFacesProperty();
//     }
// };