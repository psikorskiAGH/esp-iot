#ifndef SERVER_HTTP_DATA_STREAM_H
#define SERVER_HTTP_DATA_STREAM_H

#include "../global/includes.h"

namespace server
{
    // template <typename CharType = char>
    class HttpDataStream
    {
    public:
        typedef char Ch; //!< Character type of the stream.
    private:
        httpd_req_t *req;
        uint32_t read_offset = 0;
        char buffer[HTTP_DATA_STREAM_BUFFER_SIZE + 1] = {'\0'}; // +1 safety value
        size_t buffer_index = 0;
        size_t buffer_size = 0;
        bool has_more_data = false;

        bool read_next()
        {
            buffer_index = 0;
            for (size_t i = 0; i < 3; i++)
            {
                // Retry 3 times
                int read_amount = httpd_req_recv(req, buffer, HTTP_DATA_STREAM_BUFFER_SIZE);
#ifdef DEBUG_L2
                ESP_LOGI("server", "POST download: read_amount = %d, has_more_data = %u", read_amount, has_more_data);
#endif
                if (read_amount < 0)
                {
                    if (read_amount == HTTPD_SOCK_ERR_TIMEOUT)
                    {
                        continue;
                    }
                    // Panic
                    has_more_data = false;
                    buffer_size = 0;
                    buffer[0] = '\0';
                    return false;
                }
                else
                {
                    read_offset += read_amount;
                    buffer_size = read_amount;
                    has_more_data = req->content_len > read_offset;
                    buffer[read_amount + 1] = '\0';
                    return true;
                }
            }
            // Timeout
            httpd_resp_send_408(req);
            has_more_data = false;
            buffer_size = 0;
            buffer[0] = '\0';
            return false;
        }

    public:
        HttpDataStream(httpd_req_t *req) : req(req)
        {
            read_next();
        }

        //! Read the current character from stream without moving the read cursor.
        Ch Peek() const
        {
            return buffer[buffer_index];
        };

        //! Read the current character from stream and moving the read cursor to next character.
        Ch Take()
        {
            Ch curr = Peek();
            if (++buffer_index >= buffer_size)
            {
                if (has_more_data)
                {
                    read_next();
                }
                else
                {
                    if (buffer_index > buffer_size)
                    {
                        --buffer_index;
                        buffer[buffer_index] = '\0'; // Safety
                    }
                }
            }
            return curr;
        };

        //! Get the current read cursor.
        //! \return Number of characters read from start.
        size_t Tell() { return read_offset + buffer_index; };

        // Functions below relate only to writing - kept for compliance

        Ch *PutBegin()
        {
            RAPIDJSON_ASSERT(false);
            return 0;
        }
        void Put(Ch) { RAPIDJSON_ASSERT(false); }
        void Flush() { RAPIDJSON_ASSERT(false); }
        size_t PutEnd(Ch *)
        {
            RAPIDJSON_ASSERT(false);
            return 0;
        }
    };

} // namespace server

#endif