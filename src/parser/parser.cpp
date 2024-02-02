#include "parser.hpp"

#include <array>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/regex.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>

namespace parser {

    void parse_string(std::string& line, vectore_store& store) {
        boost::regex pattern("-?(\\d+\\.)?\\d+");
        boost::sregex_iterator iter(line.begin(), line.end(), pattern);
        boost::sregex_iterator end;
        std::array<double, 4> array;
        int count{};
        while (iter != end) {
            array[count] = std::stod(iter->str());
            iter++; count++;
        }
        if (count < 4)
            array[count] = 1;
        store.push_back({array.at(0), array.at(1), array.at(2), array.at(3)});
    }

    void parse_string_array(std::string& line, array_vector_store& store) {
        boost::regex pattern("\\d+(\\/(\\d+\\/)?\\d+)?");
        boost::sregex_iterator iter(line.begin(), line.end(), pattern);
        boost::sregex_iterator end;
        std::stack<std::array<int, 3>> face_vertex_stack;
        std::array<int, 3> array{};
        int count{};
        while (iter != end) {
            std::string substring = iter->str();
            boost::regex sub_pattern("\\d+");
            boost::sregex_iterator sub_iter(substring.begin(), substring.end(), sub_pattern);
            int sub_count{};
            while (sub_iter != end) {
                int i = std::stoi(sub_iter->str());
                array[sub_count] = i;
                sub_iter++; sub_count++;
            }
            if (sub_count == 2) {
                int swap_value = array[1];
                array[1] = 0;
                array[2] = swap_value;
            }
            face_vertex_stack.push(std::move(array));
            iter++; count++;
        }
        //how handle this?
        // if (count < 4)
        //     array[count] = 1;
    }

    void ObjParser::parse() {
        std::string line;
        std::ifstream file(this->obj_file_name);
        if (file.is_open()) {
            while (std::getline(file, line)) {
                
                if (boost::starts_with(line, "v ")) {
                    parse_string(line, this->vertexStore);    
                }
                else if (boost::starts_with(line, "vt")) {
                    parse_string(line, this->vertexTextureStore);
                }
                else if (boost::starts_with(line, "vn")) {
                    parse_string(line, this->vertexNormalStore);
                }
                else if (boost::starts_with(line, "f ")) {
                    parse_string_array(line, this->faceStore);
                }

            }
            file.close();
        }
    }
    
}