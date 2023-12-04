// api/error.h
#ifndef API_RESPONSE_H // include guard
#define API_RESPONSE_H

#define API_MACRO_CREATE_BODY (new api::ResponseBody(rapidjson::Type::kObjectType))

#include "../global/includes.h"

namespace api
{
    typedef rapidjson::Document ResponseBody;

    class Response
    {
    protected:
        ResponseBody *body_cache;

    public:
        std::string const status;

        Response(std::string status) : body_cache(nullptr),status(std::move(status)) {};
        virtual ~Response();
        /// @brief
        /// @return (gives ownership)
        virtual ResponseBody *create_body() = 0;
        ResponseBody *get_body();
        unsigned get_status_number();
    };

    class ResponseOK : public Response
    {
        // private:
        //     ResponseBody *body;

    public:
        /// @brief
        /// @param body (takes ownership)
        ResponseOK(ResponseBody *body) : Response("200 OK") { body_cache = body; };
        ResponseOK operator=(ResponseOK b) = delete;
        ResponseBody *create_body();
    };
}

#endif