// api/path.h
#ifndef API_PATH_H // include guard
#define API_PATH_H

#include "../global/includes.h"
#include "response.h"
#include "error.h"

namespace api
{
    class Path
    {
    protected:
        /// @brief Path part of this class.
        std::string const path_dir;
        /// @brief Path children of this Path.
        std::unordered_map<std::string, Path *> children = {};

    public:
        Path(std::string path_dir) : path_dir(path_dir){};
        virtual ~Path(){};

        /// @brief Registers Path as a child of this one.
        /// @param child_path Child Path to add. Pass as shared pointer.
        void register_child_path(Path *child_path) { this->children[child_path->path_dir] = child_path; }

        /// @brief Get child Path by its string path element.
        /// @param path_dir Path name of child in question.
        /// @return Pointer to a Path object or nullptr.
        Path *get_child_path(std::string const &path_dir) { return children[path_dir]; }

        /// @brief
        /// @return Pointer to response (ownership passed)
        virtual Response *GET() = 0;
        virtual Response *POST(rapidjson::Value const &data) { return new MethodNotAllowedError("Resource does not support POST method."); };
    };
} // namespace api

#endif