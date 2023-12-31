#include "error.h"

namespace api
{
    Error::Error(std::string status, std::string error_name, std::string description) : Response(std::move(status)), error_name(error_name), description(description)
    {
#ifdef DEBUG_L2
        ESP_LOGI("api.error", "Creating error of code '%s', description '%s'.", this->status.c_str(), this->description.c_str());
#endif

        ResponseBody *body = body_cache = new rapidjson::Document(rapidjson::kObjectType);
        auto *alloc = &body->GetAllocator();
        // printf("alloc %u\r\n", (size_t) alloc);
        // alloc->Clear();

        body->AddMember(std::move("status"), this->get_status_number(), *alloc);
        body->AddMember(std::move("description"), rapidjson::StringRef(this->description.c_str(), this->description.length()), *alloc);
        body->AddMember(std::move("error"), rapidjson::StringRef(this->error_name.c_str(), this->error_name.length()), *alloc);

    };

    ResponseBody *Error::create_body()
    {
        // ResponseBody *body = API_MACRO_CREATE_BODY;
        // auto *alloc = &body->GetAllocator();
        // // printf("alloc %u\r\n", (size_t) alloc);
        // alloc->Clear();

        // body->AddMember(std::move("status"), this->get_status_number(), *alloc);
        // body->AddMember(std::move("description"), rapidjson::StringRef(this->description.c_str(), this->description.length()), *alloc);
        // body->AddMember(std::move("error"), rapidjson::StringRef(this->error_name.c_str(), this->error_name.length()), *alloc);
        return body_cache;
    };
} // namespace api