import os
import sys
import argparse
import time

# Permite acesso à pasta raiz sem instalação do Pip
_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
sys.path.insert(0, os.path.join(_root, "pysrc"))
sys.path.insert(0, os.path.join(_root, "target", "release"))

from starlight import App, Scene, Entity, Camera, Input, Keys, backend
from starlight.network.manager import NetworkManager
from starlight.network.serializer import NetworkSerializer, MsgType

class MultiplayerDemo(App):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.net = NetworkManager()
        self.input_seq = 0
        self.args = None

    def set_args(self, args):
        self.args = args

    def on_start(self):
        print("[Demo] Initializing Multiplayer Scene...")
        self.scene = Scene()
        self.scene.set_sun(direction=(-0.5, -1.0, -0.5), color=(1.0, 0.9, 0.7), intensity=2.0)
        self.scene.set_ambient(0.15)
        
        self.camera = Camera()
        self.camera.set_position(0, 15, 20)
        self.camera.look_at((0, 0, 0))
        
        # Floor map
        ground = Entity("Ground", 0, -1, 0)
        ground.set_mesh("cube").set_scale(50.0, 1.0, 50.0).set_color(0.3, 0.3, 0.3)
        self.scene.add(ground)
        
        if self.args and self.args.server:
            print("[Demo] Hosting Server...")
            self.net.host(port=7777)
        elif self.args and self.args.client:
            print(f"[Demo] Joining Server {self.args.client}...")
            self.net.join(self.args.client, port=7777, player_name=self.args.player)
        else:
            print("No networking arguments provided. Running offline.")

    def on_update(self, dt):
        # Update UDP Queue e FFI Handshakes
        self.net.update()
        
        # Lógica de Entrada do Cliente
        if not self.net.is_server and self.net.is_connected:
            mx, mz = 0.0, 0.0
            if Input.get_key(Keys.W): mz -= 1.0
            if Input.get_key(Keys.S): mz += 1.0
            if Input.get_key(Keys.A): mx -= 1.0
            if Input.get_key(Keys.D): mx += 1.0
            
            if abs(mx) > 0 or abs(mz) > 0:
                self.input_seq += 1
                msg = NetworkSerializer.pack_input_state(self.input_seq, mx, mz, False)
                backend.send_message(msg)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--server", action="store_true", help="Start as authoritative server")
    parser.add_argument("--client", type=str, metavar="IP", help="Join an IP as client")
    parser.add_argument("--player", type=str, default="Player", help="Player name (client only)")
    args = parser.parse_args()
    
    demo = MultiplayerDemo(title="Starlight Multiplayer Demo")
    demo.set_args(args)
    demo.run()

if __name__ == '__main__':
    main()
