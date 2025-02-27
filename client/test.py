import asyncio
import websockets

async def test_websocket():
    #uri = "ws://localhost:8080"  # Falls direkt getestet wird
    uri = "ws://192.168.42.1:8080"  # Falls über nginx getestet wird
    async with websockets.connect(uri) as ws:
        await ws.send('{"command": "start"}')
        response = await ws.recv()
        print(f"Empfangen: {response}")

asyncio.run(test_websocket())
