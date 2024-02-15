#include "parser.hpp"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/regex.hpp>
#include <fstream>

const vectore_store_t& ModelInterface::getVertexes() const {
    return vertexStore; 
}
const face_store_t& ModelInterface::getFaces() const {
    return faceStore; 
}

void StubModel::stubfillVertexesProperty() {
    vertexStore.push_back({1, 0, 0, 1});
    vertexStore.push_back({1, 1, 0, 1});
    vertexStore.push_back({0, 1, 0, 1});
    vertexStore.push_back({0, 0, 0, 1});

    vertexStore.push_back({1, 0, 1, 1});
    vertexStore.push_back({0, 0, 1, 1});
    vertexStore.push_back({0, 0, 0, 1});
    vertexStore.push_back({1, 0, 0, 1});

    vertexStore.push_back({1, 0, 1, 1});
    vertexStore.push_back({1, 1, 0, 1});
    vertexStore.push_back({0, 1, 0, 1});
    vertexStore.push_back({0, 0, 1, 1});

    vertexStore.push_back({2, 0, -2, 1});
    vertexStore.push_back({-2, 0, -2, 1});
    vertexStore.push_back({-2, 0, 2, 1});
    vertexStore.push_back({2, 0, 2, 1});
}
void StubModel::stubfillFacesProperty() {
    faceStore.push_back({1, 2, 3, 4});
    faceStore.push_back({5, 6, 7, 8});
    faceStore.push_back({9, 10, 11, 12});
    faceStore.push_back({13, 14, 15, 16});
}

namespace parser {

    void Pipeline::bind(std::function<bool(std::string)> pipe) {
        _pipeline.push_back(pipe);
    }

    void Pipeline::pushThrough(std::string& line) {
        for (auto& pipe : _pipeline)
        {
            if (pipe(line)) break;
        }
    }

    ObjParser::ObjParser(std::string&& object_file)
    : obj_file_name(std::move(object_file))
    {
        pipeline_init();
    }

    ObjParser::ObjParser(std::string& object_file)
    : obj_file_name(object_file)
    {
        pipeline_init();
    }

    std::shared_ptr<ModelInterface> ObjParser::parse() {
        this->_model = std::make_shared<ObjModel>();
        std::string line;
        std::ifstream file(this->obj_file_name);
        if (file.is_open()) {
            while (std::getline(file, line)) {
                pushThrough(line);
            }
            file.close();
        }
        return _model;
    }

    void ObjParser::parseAsVertex(std::string& fstring, vectore_store_t& store) {
        const static boost::regex VERTEX_PATTERN("-?(\\d+\\.)?\\d+");

        vertex_t vertex;
        boost::sregex_iterator iter(fstring.begin(), fstring.end(), VERTEX_PATTERN);
        boost::sregex_iterator end;

        int oper_coo_index{};
        while (iter != end) {
            vertex[oper_coo_index] = std::stod(iter->str());
            iter++; oper_coo_index++;
        }
        if (oper_coo_index < 4) {
            vertex[oper_coo_index] = 1;
        }
        
        store.push_back(std::move(vertex));
    }

    void ObjParser::parseAsFace(std::string& fstring, face_store_t& store) {
        const static boost::regex FACE_ELEMENT_PATTERN("\\d+(\\/(\\d+\\/)?\\d+)?");

        face_t face;
        boost::sregex_iterator iter(fstring.begin(), fstring.end(), FACE_ELEMENT_PATTERN);
        boost::sregex_iterator end;

        int oper_coo_index{};
        while (iter != end) {
            const static boost::regex VERTEX_INDEX_PATTERN("\\d+");
            
            std::string substring = iter->str();
            boost::sregex_iterator sub_iter(substring.begin(), substring.end(), VERTEX_INDEX_PATTERN);

            int vertex_index = std::stoi(sub_iter->str());
            face[oper_coo_index] = vertex_index;

            iter++; oper_coo_index++;
        }
        if (oper_coo_index < 4) {
            face[oper_coo_index] = 0;
        }

        store.push_back(std::move(face));
    }

    void ObjParser::pipeline_init() {
        this->bind([&](std::string fstring){
            if (boost::starts_with(fstring, "v ")) {
                this->parseAsVertex(fstring, this->_model->vertexStore);
                return true;
            }
            return false;
        });
        this->bind([&](std::string fstring){
            if (boost::starts_with(fstring, "vt ")) {
                this->parseAsVertex(fstring, this->_model->textureStore);
                return true;
            }
            return false;
        });
        this->bind([&](std::string fstring){
            if (boost::starts_with(fstring, "vn ")) {
                this->parseAsVertex(fstring, this->_model->normalStore);
                return true;
            }
            return false;
        });
        this->bind([&](std::string fstring){
            if (boost::starts_with(fstring, "f ")) {
                this->parseAsFace(fstring, this->_model->faceStore);
                return true;
            }
            return false;
        });
    }
    
}