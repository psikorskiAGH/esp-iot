#include "error.h"

namespace api
{
    ResponseBody *Error::create_body()
    {
        ResponseBody *body = API_MACRO_CREATE_BODY;
        auto alloc = body->GetAllocator();
        body->AddMember(std::move("status"), this->get_status_number(), alloc);
        body->AddMember(std::move("description"), rapidjson::StringRef(this->description.c_str(), this->description.length()), alloc);
        body->AddMember(std::move("error"), rapidjson::StringRef(this->error_name.c_str(), this->error_name.length()), alloc);
        return body;
    };
} // namespace api