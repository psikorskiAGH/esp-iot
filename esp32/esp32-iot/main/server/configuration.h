#ifndef CONFIGURATION_H // include guard
#define CONFIGURATION_H

#include "../global/includes.h"
#include "api/path.h"
#include "api/response.h"

class Configuration : api::Path
{
private:
    std::string setting = "hello";
    std::shared_ptr<rapidjson::Value> data;

public:
    inline Configuration() : api::Path("configuration") { (*data)["v1"] = "hello"; };

    inline api::Response GET() { return std::move(api::ResponseOK(this->data)); }
};

#endif