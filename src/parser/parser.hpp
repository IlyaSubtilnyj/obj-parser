#pragma once

#include <array>
#include <functional>
#include <string>
#include <vector>

#include <memory>
#include <SFML/Graphics.hpp>

typedef std::array<double, 4> vertex_t;
typedef std::array<int, 4> face_t;
typedef std::vector<vertex_t> vectore_store_t;
typedef std::vector<face_t> face_store_t;

namespace parser {
    class ObjParser;
}

class ModelInterface
{
    protected:
    vectore_store_t vertexStore;
    vectore_store_t textureStore;
    vectore_store_t normalStore;
    face_store_t faceStore;
    public:
    const vectore_store_t& getVertexes() const;
    const face_store_t& getFaces() const;

    friend class parser::ObjParser;
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

namespace parser {

    class Pipeline {
        protected:
        std::vector<std::function<bool(std::string)>> _pipeline;
        public:
        void bind(std::function<bool(std::string)> pipe);
        void pushThrough(std::string& line);
    };

    class ObjParser: public Pipeline
    {
        public:
            class ObjModel: public ModelInterface {};
        private:
            std::shared_ptr<ModelInterface> _model;
            const std::string obj_file_name;
        public:
            ObjParser(std::string&& object_file);
            ObjParser(std::string& object_file);
            /**
             * @brief Parse object file (.obj) and store vertexes and faces;
             * @param none
             * @return void
             */
            std::shared_ptr<ModelInterface> parse();
        protected:
            void parseAsVertex(std::string& fstring, vectore_store_t&store);
            void parseAsFace(std::string& fstring, face_store_t& store);
        protected:
            void pipeline_init();
    };

}
