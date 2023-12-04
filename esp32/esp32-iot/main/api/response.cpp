#include "freertos/FreeRTOS.h"
#include "response.h"

namespace api
{
    /* === Response === */

    Response::~Response()
    {
        if (body_cache != nullptr)
        {
            delete body_cache;
        }
#ifdef DEBUG
        printf("Device response deleted\r\n");
#endif
    };

    ResponseBody *Response::get_body()
    {
        if (body_cache == nullptr)
        {
            body_cache = create_body();
        }
        return body_cache;
    };

    unsigned Response::get_status_number()
    {
        return ((unsigned)this->status[0] - 48) * 100 + ((unsigned)this->status[1] - 48) * 10 + ((unsigned)this->status[2] - 48);
    };

    /* === ResponseOK === */

    ResponseBody *ResponseOK::create_body()
    {
        ESP_ERROR_CHECK(ESP_ERR_NOT_SUPPORTED);
        return nullptr;
    }

} // namespace api
