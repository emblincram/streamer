#pragma once

#include <libwebsockets.h>

#include <string>
#include <vector>

namespace application::streamer {

class Server {
   public:
    Server(int port);
    ~Server();
    void run();

   private:
    static int callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);
    static void sendBinaryData(struct lws *wsi);

    struct lws_context *context;
    int port;
};
}  // namespace application::streamer
