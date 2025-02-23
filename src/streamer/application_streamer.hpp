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
    static int callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);
    static void sendBinaryData(struct lws *wsi);

   private:
    struct lws_context *context;
    static struct lws_protocols protocols[];  // Protokolle für WebSockets
    int port;
};
}  // namespace application::streamer
