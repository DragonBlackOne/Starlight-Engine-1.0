import time
from typing import Dict, List, Tuple
from starlight import backend
from starlight.network.serializer import NetworkSerializer, MsgType

class PlayerConnection:
    def __init__(self, address: str, entity_id: int):
        self.address = address
        self.entity_id = entity_id
        self.last_input_seq = 0
        self.last_seen = time.time()


class NetworkManager:
    def __init__(self):
        self.is_server = False
        self.is_connected = False
        self.players: Dict[str, PlayerConnection] = {} # address -> Player
        self.local_player_id = 0
        self.current_tick = 0
        
        # Client Side Simulation
        self.snapshot_buffer = []

    def host(self, port: int = 7777):
        """Starts a UDP Server using Rust Backend"""
        backend.start_server(port)
        self.is_server = True
        self.is_connected = True
        print(f"[NetManager] Server Host started on port {port}")

    def join(self, ip: str, port: int = 7777, player_name: str = "Player"):
        """Connects to server and sends a Join handshake"""
        backend.connect_client(ip, port)
        self.is_server = False
        self.is_connected = True
        
        # Envia pacote binário de handshake
        req_bytes = NetworkSerializer.pack_join_request(player_name)
        backend.send_message(req_bytes) # sem target, vai pro servidor associado no Rust
        print(f"[NetManager] Client joining {ip}:{port}...")

    def update(self):
        """Called every frame. Polls incoming packets via Rust FFI."""
        if not self.is_connected:
            return
            
        # Pega todas as mensagens enfileiradas da VBlank no backend
        messages = backend.get_messages()
        for from_addr, data in messages:
            self._process_message(from_addr, bytes(data))
            
        if self.is_server:
            self._server_tick()

    def _process_message(self, from_addr: str, data: bytes):
        if not data: return
        msg_type = data[0]

        if self.is_server:
            if msg_type == MsgType.JOIN_REQUEST:
                self._handle_join_request(from_addr, data)
            elif msg_type == MsgType.INPUT_STATE:
                self._handle_input_state(from_addr, data)
        else:
            if msg_type == MsgType.JOIN_ACCEPT:
                self._handle_join_accept(data)
            elif msg_type == MsgType.WORLD_SNAPSHOT:
                self._handle_world_snapshot(data)

    # --- Server Side Logic ---
    def _handle_join_request(self, addr: str, data: bytes):
        name = NetworkSerializer.unpack_join_request(data)
        print(f"[Server] Join Request from {addr} ({name})")
        
        # Spawna personagem de rede via ECS FFI
        ent_id = backend.spawn_entity(0.0, 10.0, 0.0)
        # Seta propriedades cosméticas
        backend.set_mesh(ent_id, "suzanne_mesh")
        backend.set_color(ent_id, 0.2, 0.8, 0.2, 1.0)
        
        self.players[addr] = PlayerConnection(addr, ent_id)
        
        # Responde
        accept_msg = NetworkSerializer.pack_join_accept(ent_id)
        backend.send_message(accept_msg, addr)
        print(f"[Server] Assigned Entity {ent_id} to {addr}")

    def _handle_input_state(self, addr: str, data: bytes):
        if addr not in self.players: return
        player = self.players[addr]
        player.last_seen = time.time()
        
        seq, mx, mz, jump = NetworkSerializer.unpack_input_state(data)
        
        # Ignora pacotes de movimentação defasados (packet loss/out of order UDP)
        if seq <= player.last_input_seq:
            return
        player.last_input_seq = seq
        
        # Applica movimentação no backend (Authoritative)
        # O ideal aqui é ter um Character Controller
        try:
            t = backend.get_transform(player.entity_id)
            new_x = t.x + mx * 0.1 # Delta Speed
            new_z = t.z + mz * 0.1
            backend.set_transform(player.entity_id, new_x, t.y, new_z)
        except Exception as e:
            pass

    def _server_tick(self):
        """Transmite o Snapshot a 20 ticks/segundo"""
        self.current_tick += 1
        
        # Se não bateu tick frame ou no traffic, ignora
        # Assumindo q Update é chamado 60x p/s, então moduleamos pra dar 20 tickrates
        if self.current_tick % 3 != 0: 
            return
            
        snapshot_ents = []
        for p in self.players.values():
            try:
                t = backend.get_transform(p.entity_id)
                snapshot_ents.append((p.entity_id, t.x, t.y, t.z))
            except:
                continue
                
        if not snapshot_ents: return
            
        snap_msg = NetworkSerializer.pack_world_snapshot(self.current_tick, snapshot_ents)
        for addr in self.players.keys():
            backend.send_message(snap_msg, addr)

    # --- Client Side Logic ---
    def _handle_join_accept(self, data: bytes):
        self.local_player_id = NetworkSerializer.unpack_join_accept(data)
        print(f"[Client] Accepted! Controlling entity {self.local_player_id}")

    def _handle_world_snapshot(self, data: bytes):
        tick, ents = NetworkSerializer.unpack_world_snapshot(data)
        # Replicação Basica (Teleporte direto, sem interpolação na v1)
        for ent_id, x, y, z in ents:
            # Se for nossa entidade, não aplicamos diretamente ou vai dar jitter (Prediction)
            if ent_id == self.local_player_id:
                pass # Em V2 será Client Prediction Snap Logic
            else:
                try:
                    backend.set_transform(ent_id, x, y, z)
                except:
                    # Se Entidade não sumiu, pode precisar ser Spawnada no Cliente via Replication
                    pass
