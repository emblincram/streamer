#include "application_streamer.hpp"

#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>

namespace application::streamer {

// Konstruktor: Initialisiert den Server
Server::Server(int port) : port(port) {
    struct lws_context_creation_info info = {};
    memset(&info, 0, sizeof(info));

    info.port = port;
    info.protocols = new struct lws_protocols[2]{{"websocket-protocol", callback, 0, 4096}, {nullptr, nullptr, 0, 0}};

    context = lws_create_context(&info);
    if (!context) {
        std::cerr << "Fehler beim Erstellen des WebSocket-Kontexts\n";
        exit(1);
    }
}

// Destruktor: Server stoppen
Server::~Server() { lws_context_destroy(context); }

// Server-Loop starten
void Server::run() {
    while (true) {
        lws_service(context, 100);
    }
}

// WebSocket-Callback-Funktion
int Server::callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED:
            std::cout << "Client verbunden\n";
            break;

        case LWS_CALLBACK_RECEIVE:
            if (!lws_frame_is_binary(wsi)) {
                std::string message((char *)in, len);
                std::cout << "JSON empfangen: " << message << std::endl;
            }
            break;

        case LWS_CALLBACK_SERVER_WRITEABLE:
            sendBinaryData(wsi);
            break;

        default:
            break;
    }
    return 0;
}

// Binär-Sensordaten senden
void Server::sendBinaryData(struct lws *wsi) {
    uint8_t sensorData[32] = {0x01, 0x02, 0x03};  // Dummy-Sensordaten
    lws_write(wsi, sensorData, sizeof(sensorData), LWS_WRITE_BINARY);
}

}  // namespace application::streamer