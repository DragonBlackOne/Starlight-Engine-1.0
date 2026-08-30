import struct
from typing import Tuple, List, Optional
import math

class MsgType:
    # 0x01 - 0x0F: Handshake & Connection
    JOIN_REQUEST = 0x01
    JOIN_ACCEPT = 0x02
    DISCONNECT = 0x03
    
    # 0x10 - 0x1F: Gameplay & Input
    INPUT_STATE = 0x10  # Client sending input to server
    
    # 0x20 - 0x2F: World State
    WORLD_SNAPSHOT = 0x20  # Server sending replicated state to all clients

class NetworkSerializer:
    """
    Handles robust binary serialization using struct.
    Reduces JSON overhead by packing floats and ints directly into bytes.
    Endianness: Network (!) -> Big Endian standard.
    """
    
    @staticmethod
    def pack_join_request(player_name: str) -> bytes:
        """MsgType, String Length, String Bytes"""
        encoded_name = player_name.encode('utf-8')
        fmt = f'!B B {len(encoded_name)}s'
        return struct.pack(fmt, MsgType.JOIN_REQUEST, len(encoded_name), encoded_name)

    @staticmethod
    def unpack_join_request(data: bytes) -> str:
        name_len = data[1]
        fmt = f'!B B {name_len}s'
        _, _, name = struct.unpack(fmt, data)
        return name.decode('utf-8')

    @staticmethod
    def pack_join_accept(assigned_entity_id: int) -> bytes:
        """MsgType, Entity ID (unsigned long long)"""
        return struct.pack('!B Q', MsgType.JOIN_ACCEPT, assigned_entity_id)
        
    @staticmethod
    def unpack_join_accept(data: bytes) -> int:
        _, entity_id = struct.unpack('!B Q', data)
        return entity_id

    @staticmethod
    def pack_input_state(seq: int, move_x: float, move_z: float, jump: bool) -> bytes:
        """MsgType, Sequence(uint32), MoveX(float), MoveZ(float), Jump(bool)"""
        return struct.pack('!B I f f ?', MsgType.INPUT_STATE, seq, move_x, move_z, jump)

    @staticmethod
    def unpack_input_state(data: bytes) -> Tuple[int, float, float, bool]:
        _, seq, move_x, move_z, jump = struct.unpack('!B I f f ?', data)
        return seq, move_x, move_z, jump

    @staticmethod
    def pack_world_snapshot(tick: int, entities: List[Tuple[int, float, float, float]]) -> bytes:
        """
        MsgType, Tick(uint32), NumEntities(uint16), array of:
        [EntityID(uint64), X(float), Y(float), Z(float)]
        """
        num_entities = len(entities)
        header = struct.pack('!B I H', MsgType.WORLD_SNAPSHOT, tick, num_entities)
        
        body = bytearray()
        for ent_id, x, y, z in entities:
            body.extend(struct.pack('!Q f f f', ent_id, x, y, z))
            
        return header + bytes(body)
        
    @staticmethod
    def unpack_world_snapshot(data: bytes) -> Tuple[int, List[Tuple[int, float, float, float]]]:
        header_size = struct.calcsize('!B I H')
        _, tick, num_entities = struct.unpack('!B I H', data[:header_size])
        
        entities = []
        offset = header_size
        entity_size = struct.calcsize('!Q f f f')
        
        for _ in range(num_entities):
            ent_chunk = data[offset:offset+entity_size]
            ent_id, x, y, z = struct.unpack('!Q f f f', ent_chunk)
            entities.append((ent_id, x, y, z))
            offset += entity_size
            
        return tick, entities
