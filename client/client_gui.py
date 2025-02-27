import asyncio
import websockets
import json
import struct
import threading
import queue
import tkinter as tk
import time

SERVER_URL = "ws://192.168.42.1:8080"  # IP deines Yocto-Geräts anpassen
SAMPLE_RATE_HZ = 40000  # 40 kHz Abtastrate


class WebSocketClient:
    def __init__(self, data_queue):
        self.websocket = None
        self.running = False
        self.data_queue = data_queue
        self.start_time = None  # Virtueller Zeitstempel für das erste Sample
        self.sample_index = 0

    async def connect(self):
        try:
            self.websocket = await websockets.connect(SERVER_URL)
            print(f"Verbunden mit {SERVER_URL}")
        except Exception as e:
            print(f"Verbindung fehlgeschlagen: {e}")
            return

        # JSON-Startbefehl senden
        await self.websocket.send(json.dumps({"command": "start"}))
        print(f"Streaming gestartet!")

        # Daten empfangen
        self.running = True
        while self.running:
            try:
                message = await self.websocket.recv()
                if isinstance(message, bytes):  # Binärdaten vom Server
                    num_values = len(message) // 4  # 4 Bytes pro Wert (float32)
                    values = struct.unpack(f"{num_values}f", message)

                    # Setze Startzeit beim ersten Paket
                    if self.start_time is None:
                        self.start_time = time.time()

                    # Berechne die Zeitstempel für jedes Sample
                    sample_timestamps = [
                        self.start_time + (self.sample_index + i) * (1 / SAMPLE_RATE_HZ)
                        for i in range(len(values))
                    ]
                    self.sample_index += len(values)

                    # Speichere (Zeit, Werte) in die Queue
                    for timestamp, value in zip(sample_timestamps, values):
                        self.data_queue.put((timestamp, value))

                else:
                    print(f"JSON Nachricht: {message}")
            except Exception as e:
                print(f"Fehler beim Empfangen: {e}")
                break

    async def stop(self):
        self.running = False
        if self.websocket:
            await self.websocket.send(json.dumps({"command": "stop"}))
            await self.websocket.close()
            print("Verbindung geschlossen")

    def start_async_loop(self):
        asyncio.run(self.connect())


class TkApp:
    def __init__(self, root):
        self.root = root
        self.root.title("WebSocket Sensor Client")

        # Buttons
        self.start_button = tk.Button(
            root, text="Start", command=self.start_stream, width=10
        )
        self.start_button.grid(row=0, column=0, padx=5, pady=5)

        self.stop_button = tk.Button(
            root, text="Stop", command=self.stop_stream, width=10
        )
        self.stop_button.grid(row=0, column=1, padx=5, pady=5)

        # Datenfelder
        self.labels = []
        for i in range(8):
            tk.Label(root, text=f"Sensor {i+1}:", width=10).grid(row=i + 1, column=0)
            label = tk.Label(root, text="0.0", bg="white", width=15, relief="sunken")
            label.grid(row=i + 1, column=1, padx=5, pady=5)
            self.labels.append(label)

        # Queue für eingehende Daten
        self.data_queue = queue.Queue()
        self.client = WebSocketClient(self.data_queue)
        self.client_thread = None

        # Letzte Frame-Zeit
        self.last_frame_time = time.time()

        # UI-Update Timer (z. B. alle 33 ms = 30 Hz)
        self.update_display_interval = 33
        self.update_display()

    def start_stream(self):
        if self.client_thread and self.client_thread.is_alive():
            return
        self.client_thread = threading.Thread(
            target=self.client.start_async_loop, daemon=True
        )
        self.client_thread.start()

    def stop_stream(self):
        asyncio.run(self.client.stop())

    def update_display(self):
        """Wählt das passendste Sample basierend auf der exakten Zeitdifferenz."""
        current_time = time.time()
        elapsed_time = current_time - self.last_frame_time  # Zeit seit letztem Update
        self.last_frame_time = current_time  # Aktuelle Zeit speichern

        target_time = current_time - elapsed_time
        best_sample = None
        best_time_diff = float("inf")

        # Suche das Sample mit der nächsten Zeitmarke
        while not self.data_queue.empty():
            timestamp, value = self.data_queue.queue[0]  # Erstes Element anschauen

            time_diff = abs(timestamp - target_time)

            if time_diff < best_time_diff:
                best_time_diff = time_diff
                best_sample = value

            # Falls das Sample älter ist als die gewünschte Zeit, aus der Queue entfernen
            if timestamp < target_time:
                self.data_queue.get()
            else:
                break  # Stoppe, sobald wir ein jüngeres Sample haben

        # Falls ein passendes Sample gefunden wurde, UI aktualisieren
        if best_sample:
            for i in range(min(len(self.labels), 8)):
                self.labels[i].config(text=f"{best_sample:.3f}")

        # Nächstes UI-Update planen
        self.root.after(self.update_display_interval, self.update_display)


if __name__ == "__main__":
    root = tk.Tk()
    app = TkApp(root)
    root.mainloop()
