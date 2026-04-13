import socket
import threading
import json
import time
import struct
from queue import Queue

# Protocol:
# Header: 4 bytes (Big Endian Uint32) indicating payload size
# Payload: JSON encoded string

PORT = 5555
HOST = '127.0.0.1'

class Packet:
    def __init__(self, cmd, data=None):
        self.cmd = cmd
        self.data = data or {}

    def to_bytes(self):
        payload = json.dumps({"cmd": self.cmd, "data": self.data}).encode('utf-8')
        header = struct.pack('>I', len(payload))
        return header + payload

    @staticmethod
    def from_socket(sock):
        try:
            # Read Header
            header = sock.recv(4)
            if not header:
                return None
            length = struct.unpack('>I', header)[0]
            
            # Read Payload
            payload = b""
            while len(payload) < length:
                chunk = sock.recv(length - len(payload))
                if not chunk:
                    return None
                payload += chunk
            
            obj = json.loads(payload.decode('utf-8'))
            return Packet(obj.get("cmd"), obj.get("data"))
        except Exception as e:
            print(f"[IPC] Error reading packet: {e}")
            return None

class EditorServer:
    """Runs inside the Game Process."""
    def __init__(self, app):
        self.app = app
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.running = False
        self.client_socket = None
        self.send_queue = Queue()
        self.recv_thread = None
        self.send_thread = None

    def start(self):
        try:
            self.server_socket.bind((HOST, PORT))
            self.server_socket.listen(1)
            self.running = True
            print(f"[EditorServer] Listening on {HOST}:{PORT}...")
            
            # Use non-blocking accept in main loop or separate accept thread
            # For simplicity, we'll spawn a thread to handle connection
            accept_thread = threading.Thread(target=self._accept_loop, daemon=True)
            accept_thread.start()
        except OSError as e:
            print(f"[EditorServer] Failed to bind port {PORT}: {e}")

    def _accept_loop(self):
        while self.running:
            try:
                # This blocks, so we run in thread
                client, addr = self.server_socket.accept()
                print(f"[EditorServer] Connected to Editor at {addr}")
                self.client_socket = client
                
                # Start handling threads
                self.recv_thread = threading.Thread(target=self._recv_loop, args=(client,), daemon=True)
                self.recv_thread.start()
                
                self._send_loop(client) # Reuse this thread for sending or create another?
                # Actually, sending loop might block queue get, so thread is better.
                # But accept loop needs to be free to re-accept if connection drops?
                # For Phase 13, one connection is enough.
                self._send_loop(client)
                
            except Exception as e:
                pass
                # print(f"[EditorServer] Accept error: {e}")

    def _recv_loop(self, client):
        while self.running and self.client_socket == client:
            packet = Packet.from_socket(client)
            if packet:
                self.handle_command(packet)
            else:
                print("[EditorServer] Client disconnected.")
                self.client_socket = None
                break

    def _send_loop(self, client):
        while self.running and self.client_socket == client:
            try:
                packet = self.send_queue.get() # Blocks
                data = packet.to_bytes()
                client.sendall(data)
            except Exception as e:
                print(f"[EditorServer] Send error: {e}")
                break

    def handle_command(self, packet):
        # Allow queuing commands to be executed on Main Thread content if needed
        # For now, print
        # print(f"[EditorServer] Recv: {packet.cmd}")
        
        if packet.cmd == "PING":
            self.send(Packet("PONG", {"time": time.time()}))
        elif packet.cmd == "GET_SCENE_TREE":
            try:
                # Late import to avoid circular dependency issues at module level
                from .. import framework
                scene = framework.get_active_scene()
                
                entities_data = []
                if scene:
                    for ent in scene.entities:
                        entities_data.append({
                            "id": ent.id,
                            "name": ent.name
                        })
                
                self.send(Packet("SCENE_TREE", {"entities": entities_data}))
            except Exception as e:
                print(f"[EditorServer] Error getting scene tree: {e}")
                self.send(Packet("SCENE_TREE", {"entities": []}))

        elif packet.cmd == "GET_ENTITY_DETAILS":
            try:
                from .. import framework
                scene = framework.get_active_scene()
                ent_id = packet.data.get("id")
                
                found_ent = None
                if scene:
                    for ent in scene.entities:
                        if ent.id == ent_id:
                            found_ent = ent
                            break
                
                if found_ent:
                    details = {
                        "id": found_ent.id,
                        "name": found_ent.name,
                        "transform": {
                            "position": found_ent.transform.position,
                            "rotation": found_ent.transform.rotation,
                            "scale": found_ent.transform.scale
                        },
                        "mesh": found_ent._mesh,
                        "diffuse": found_ent._diffuse,
                        "normal": found_ent._normal
                        # Add light info if we can access it, but Entity class stores it in backend mostly?
                        # Starlight Entity wrapper doesn't store light params locally in __init__ usually.
                        # We might need to fetch from backend if wrapper doesn't have it.
                        # For Phase 13, let's stick to what's in Python wrapper.
                    }
                    self.send(Packet("ENTITY_DETAILS", details))
                else:
                    self.send(Packet("ERROR", {"msg": "Entity not found"}))
            except Exception as e:
                print(f"[EditorServer] Error getting details: {e}")

        elif packet.cmd == "SAVE_SCENE":
            path = packet.data.get("path")
            if path:
                try:
                    from . import serialization
                    from .. import framework
                    scene = framework.get_active_scene()
                    if scene:
                        serialization.save_scene(scene, path)
                        self.send(Packet("LOG", {"msg": f"Scene saved to {path}"}))
                except Exception as e:
                    print(f"[EditorServer] Error saving scene: {e}")
                    self.send(Packet("LOG", {"msg": f"Error saving scene: {e}"}))

        elif packet.cmd == "LOAD_SCENE":
            path = packet.data.get("path")
            if path:
                try:
                    from . import serialization
                    scene = serialization.load_scene(path)
                    if scene:
                        entities_data = []
                        for ent in scene.entities:
                            entities_data.append({"id": ent.id, "name": ent.name})
                        
                        self.send(Packet("SCENE_TREE", {"entities": entities_data}))
                        self.send(Packet("LOG", {"msg": f"Scene loaded from {path}"}))
                except Exception as e:
                    print(f"[EditorServer] Error loading scene: {e}")
                    self.send(Packet("LOG", {"msg": f"Error loading scene: {e}"}))

        elif packet.cmd == "SAVE_PREFAB":
            try:
                from . import serialization
                from .. import framework
                scene = framework.get_active_scene()
                
                ent_id = packet.data.get("entity_id")
                path = packet.data.get("path")
                
                if scene and ent_id and path:
                    found_ent = None
                    for ent in scene.entities:
                        if ent.id == ent_id:
                            found_ent = ent
                            break
                    
                    if found_ent:
                        serialization.save_prefab(found_ent, path)
                        self.send(Packet("LOG", {"msg": f"Prefab saved to {path}"}))
                    else:
                        self.send(Packet("LOG", {"msg": "Entity not found for Prefab"}))
            except Exception as e:
                print(f"[EditorServer] Error saving prefab: {e}")
                self.send(Packet("LOG", {"msg": f"Error saving prefab: {e}"}))

        elif packet.cmd == "INSTANTIATE_PREFAB":
            try:
                from . import serialization
                from .. import framework
                scene = framework.get_active_scene()
                path = packet.data.get("path")
                
                if scene and path:
                    entity = serialization.load_prefab(path)
                    if entity:
                        scene.add(entity)
                        
                        entities_data = []
                        for ent in scene.entities:
                            entities_data.append({"id": ent.id, "name": ent.name})
                        
                        self.send(Packet("SCENE_TREE", {"entities": entities_data}))
                        self.send(Packet("LOG", {"msg": f"Instantiated Prefab from {path}"}))
                    else:
                        self.send(Packet("LOG", {"msg": "Failed to load prefab"}))
            except Exception as e:
                print(f"[EditorServer] Error instantiating prefab: {e}")
                self.send(Packet("LOG", {"msg": f"Error instantiating prefab: {e}"}))

        elif packet.cmd == "UPDATE_ENTITY":
            try:
                from .. import framework
                scene = framework.get_active_scene()
                ent_id = packet.data.get("id")
                updates = packet.data.get("updates", {})
                
                if scene:
                    found_ent = None
                    for ent in scene.entities:
                        if ent.id == ent_id:
                            found_ent = ent
                            break
                    
                    if found_ent:
                        if "position" in updates:
                            found_ent.set_position(*updates["position"])
                        if "rotation" in updates:
                            found_ent.set_rotation(*updates["rotation"])
                        if "scale" in updates:
                            found_ent.set_scale(*updates["scale"])
                        
                        # Acknowledge? (Optional, maybe log)
            except Exception as e:
                 print(f"[EditorServer] Error updating entity: {e}")

    def send(self, packet):
        if self.client_socket:
            self.send_queue.put(packet)
            
    def update(self):
        # Called from Game Main Loop if we need main-thread processing
        pass

    def stop(self):
        self.running = False
        if self.server_socket:
            self.server_socket.close()

class EditorClient:
    """Runs inside the Editor (Studio) Process."""
    def __init__(self):
        self.socket = None
        self.connected = False
        self.recv_queue = Queue()
        self.send_queue = Queue()

    def connect(self):
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.connect((HOST, PORT))
            self.connected = True
            print(f"[EditorClient] Connected to Game.")
            
            # Start threads
            t_recv = threading.Thread(target=self._recv_loop, daemon=True)
            t_recv.start()
            
            t_send = threading.Thread(target=self._send_loop, daemon=True)
            t_send.start()
            return True
        except Exception as e:
            print(f"[EditorClient] Connection failed: {e}")
            self.connected = False
            return False

    def _recv_loop(self):
        while self.connected:
            packet = Packet.from_socket(self.socket)
            if packet:
                self.recv_queue.put(packet)
            else:
                print("[EditorClient] Disconnected.")
                self.connected = False
                break

    def _send_loop(self):
        while self.connected:
            try:
                packet = self.send_queue.get()
                self.socket.sendall(packet.to_bytes())
            except Exception as e:
                print(f"[EditorClient] Send error: {e}")
                self.connected = False

    def send(self, cmd, data=None):
        if self.connected:
            self.send_queue.put(Packet(cmd, data))

    def poll(self):
        """Returns all pending packets."""
        packets = []
        while not self.recv_queue.empty():
            packets.append(self.recv_queue.get())
        return packets
