#include "application_streamer.hpp"

#include <json/json.h>

#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>

#define SAMPLE_RATE_HZ 40000                                           // 40 kHz Abtastrate
#define PACKET_SIZE 512                                                // 512 Samples pro Paket (2er Potenz: 256, 512, 1024 ...)
#define CHANNELS 8                                                     // 8 Sensorkanäle
#define PACKET_INTERVAL_US ((PACKET_SIZE * 1000000) / SAMPLE_RATE_HZ)  // Berechnet die Paket-Sendezeit

namespace application::streamer {

bool streaming_active = false;  // Neuer globaler Status

// WebSockets-Protokolle definieren
struct lws_protocols Server::protocols[] = {
    {
        "streamer-protocol",  // Protokollname
        Server::callback,     // Callback-Funktion für WebSockets
        0,                    // Per-Session-Daten
        4096                  // Max. Buffer-Größe
    },
    {NULL, NULL, 0, 0}  // Terminator-Eintrag
};

// Konstruktor: Initialisiert den Server
Server::Server(int port) : port(port) {
    struct lws_context_creation_info info = {};
    info.port = port;
    info.protocols = protocols;
    info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;

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
    std::cout << "WebSocket-Server läuft mit " << SAMPLE_RATE_HZ << " Hz, ";
    std::cout << "Paketgröße: " << PACKET_SIZE << ", ";
    std::cout << "Sendeintervall: " << PACKET_INTERVAL_US << " µs" << std::endl;

    auto last_time = std::chrono::high_resolution_clock::now();

    while (true) {
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - last_time).count();

        if (elapsed >= PACKET_INTERVAL_US) {  // Exaktes Timing
            last_time = now;

            if (streaming_active) {
                lws_callback_on_writable_all_protocol(context, &protocols[0]);  // Aktiviert alle Clients
            }
        }

        std::this_thread::sleep_for(std::chrono::microseconds(50));  // CPU entlasten
    }
}

// WebSocket-Callback-Funktion
int Server::callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED:
            std::cout << "Client verbunden\n";
            break;

        case LWS_CALLBACK_CLOSED:
            std::cout << "Verbindung geschlossen.\n";
            break;

        case LWS_CALLBACK_RECEIVE:
            if (!lws_frame_is_binary(wsi)) {
                std::string message((char *)in, len);
                std::cout << "JSON empfangen: " << message << std::endl;

                // JSON auswerten
                Json::Value root;
                Json::Reader reader;
                if (reader.parse(message, root)) {
                    if (root["command"].asString() == "start") {
                        streaming_active = true;
                        std::cout << "▶️ Streaming gestartet!\n";
                        lws_callback_on_writable(wsi);  // Veranlasst das Senden von Daten
                    } else if (root["command"].asString() == "stop") {
                        streaming_active = false;
                        std::cout << "⏹ Streaming gestoppt!\n";
                    }
                }
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
    if (!streaming_active)
        return;

    float sensorData[PACKET_SIZE * CHANNELS];  // 512 Samples * 8 Kanäle

    // Simulierte Sensordaten generieren
    for (int i = 0; i < PACKET_SIZE * CHANNELS; i++) {
        sensorData[i] = static_cast<float>(rand()) / RAND_MAX * 100.0f;
    }

    // Libwebsockets Puffer
    uint8_t buffer[LWS_SEND_BUFFER_PRE_PADDING + sizeof(sensorData) + LWS_SEND_BUFFER_POST_PADDING] = {0};
    std::memcpy(&buffer[LWS_SEND_BUFFER_PRE_PADDING], sensorData, sizeof(sensorData));

    // Binärdaten senden
    lws_write(wsi, &buffer[LWS_SEND_BUFFER_PRE_PADDING], sizeof(sensorData), LWS_WRITE_BINARY);
}

}  // namespace application::streamer
