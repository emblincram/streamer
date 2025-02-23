#include "application_streamer.hpp"

int main() {
    application::streamer::Server server(8080);
    server.run();
    return 0;
}
