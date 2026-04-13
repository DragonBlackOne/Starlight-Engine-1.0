import sys
import os

_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
sys.path.insert(0, os.path.join(_root, "pysrc"))

from starlight.network.serializer import NetworkSerializer, MsgType

def test_serializer():
    # Test Join Request
    p_name = "Player_Zero"
    packed = NetworkSerializer.pack_join_request(p_name)
    assert packed[0] == MsgType.JOIN_REQUEST
    name_out = NetworkSerializer.unpack_join_request(packed)
    assert name_out == p_name, f"Expected {p_name}, got {name_out}"
    print(f"Join Request OK! Bytes: {len(packed)}")

    # Test Join Accept
    ent_id = 998877665544
    packed = NetworkSerializer.pack_join_accept(ent_id)
    assert packed[0] == MsgType.JOIN_ACCEPT
    out_id = NetworkSerializer.unpack_join_accept(packed)
    assert out_id == ent_id, f"Expected {ent_id}, got {out_id}"
    print(f"Join Accept OK! Bytes: {len(packed)}")

    # Test Input State
    seq = 100
    mx = -1.5
    mz = 0.5
    jp = True
    packed = NetworkSerializer.pack_input_state(seq, mx, mz, jp)
    assert packed[0] == MsgType.INPUT_STATE
    o_s, o_x, o_z, o_j = NetworkSerializer.unpack_input_state(packed)
    assert o_s == seq
    assert (abs(o_x - mx) < 1e-4)
    assert (abs(o_z - mz) < 1e-4)
    assert o_j == jp
    print(f"Input State OK! Bytes: {len(packed)}")

    # Test World Snapshot
    tick = 55
    entities = [
        (101, 10.0, 5.0, -2.0),
        (102, 0.0, 20.2, 33.1)
    ]
    packed = NetworkSerializer.pack_world_snapshot(tick, entities)
    assert packed[0] == MsgType.WORLD_SNAPSHOT
    o_tick, o_entities = NetworkSerializer.unpack_world_snapshot(packed)
    assert o_tick == tick
    assert len(o_entities) == 2
    
    for i in range(2):
        assert o_entities[i][0] == entities[i][0]
        assert (abs(o_entities[i][1] - entities[i][1]) < 1e-4)
        assert (abs(o_entities[i][2] - entities[i][2]) < 1e-4)
        assert (abs(o_entities[i][3] - entities[i][3]) < 1e-4)
    print(f"World Snapshot OK! Bytes: {len(packed)}")
    print("ALL TESTS PASSED!")

if __name__ == '__main__':
    test_serializer()
