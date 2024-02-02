#pragma once

#include <initializer_list>
#include <string>
#include <vector>

namespace parser {

    typedef std::vector<std::initializer_list<double>> vectore_store;
    typedef std::vector<std::initializer_list<std::initializer_list<int>>> array_vector_store;

    class ObjParser 
    {
        private:
            vectore_store vertexStore;
            vectore_store vertexTextureStore;
            vectore_store vertexNormalStore; //TODO: rename
            array_vector_store faceStore;
            const std::string obj_file_name;
        public:
            ObjParser(std::string&& object_file)
            : obj_file_name(std::move(object_file))
            {}
            ObjParser(std::string& object_file)
            : obj_file_name(object_file)
            {}
            vectore_store& getVertexes();
            /**
             * @brief Parse object file (.obj) and store vertexes and faces;
             * @param none
             * @return void
             */
            void parse();
    };

}