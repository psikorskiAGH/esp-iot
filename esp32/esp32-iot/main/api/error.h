// api/error.h
#ifndef API_ERROR_H // include guard
#define API_ERROR_H

#include "../global/includes.h"
#include "response.h"

namespace api
{
    class Error : public Response
    {
    public:
        std::string const error_name;
        std::string const description;

        /// @brief Prepares response to be sent to client as an error.
        /// @param description
        Error(std::string status, std::string error_name, std::string description);

        ResponseBody *create_body();
    };

    class BadRequestError : public Error
    {
    public:
        BadRequestError(std::string description) : Error("400 BAD_REQUEST", "Bad Request", std::move(description)){};
    };

    class NotFoundError : public Error
    {
    public:
        NotFoundError(std::string description) : Error("404 NOT_FOUND", "Not Found", std::move(description)){};
    };

    class MethodNotAllowedError : public Error
    {
    public:
        MethodNotAllowedError(std::string description) : Error("405 METHOD_NOT_ALLOWED", "Method Not Allowed", std::move(description)){};
    };

    class InternalServerError : public Error
    {
    public:
        InternalServerError(std::string description) : Error("500 INTERNAL_SERVER_ERROR", "Internal Server Error", std::move(description)){};
    };
} // namespace api

#endif