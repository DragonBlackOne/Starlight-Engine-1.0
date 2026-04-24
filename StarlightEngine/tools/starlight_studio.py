
import dearpygui.dearpygui as dpg
import sys
import os
import time
from PIL import Image

# Add pysrc to path
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "pysrc")))

from starlight.editor.ipc import EditorClient

WINDOW_WIDTH = 1400
WINDOW_HEIGHT = 900
ASSET_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "assets"))

class StarlightStudio:
    def __init__(self):
        self.client = EditorClient()
        self.last_update = time.time()
        self.log_messages = []
        self.asset_files = [] # For caching asset list

    def log(self, msg):
        timestamp = time.strftime("%H:%M:%S")
        self.log_messages.append(f"[{timestamp}] {msg}")
        if len(self.log_messages) > 100:
            self.log_messages.pop(0)
        
        if dpg.does_item_exist("log_text_container"):
             dpg.set_value("log_text_container", "\n".join(self.log_messages))

    def connect_to_game(self, sender, app_data):
        self.log("Attempting to connect to Game Process...")
        if self.client.connect():
            self.log("Connected successfully!")
            dpg.configure_item("status_text", default_value="Status: Connected", color=(0, 255, 0))
            dpg.configure_item("connect_btn", enabled=False, label="Connected")
        else:
            self.log("Connection failed. Is the game running with --editor?")
            dpg.configure_item("status_text", default_value="Status: Disconnected", color=(255, 0, 0))

    def send_ping(self):
        if self.client.connected:
            self.client.send("PING")
            self.log("Sent: PING")
        else:
            self.log("Not connected.")

    # === SCENE HIERARCHY ===
    def request_scene_tree(self):
        if self.client.connected:
            self.client.send("GET_SCENE_TREE")
            self.log("Requested Scene Tree...")

    def build_hierarchy(self, entities):
        if dpg.does_item_exist("hierarchy_root"):
             dpg.delete_item("hierarchy_root", children_only=True)
        
        for ent in entities:
             # Use add_selectable instead of tree_node for flat list to avoid callback error
             tag_id = f"ent_sel_{ent['id']}"
             dpg.add_selectable(label=f"{ent['name']} ({ent['id']})", parent="hierarchy_root", 
                               callback=self.on_entity_selected, user_data=ent['id'], tag=tag_id)
             
             # Context Menu
             with dpg.popup(tag_id, mousebutton=dpg.mvMouseButton_Right):
                 dpg.add_menu_item(label="Save as Prefab...", callback=self.save_prefab_prompt, user_data=ent['id'])

    def on_entity_selected(self, sender, app_data, user_data):
        entity_id = user_data
        self.log(f"Selected Entity ID: {entity_id}")
        self.client.send("GET_ENTITY_DETAILS", {"id": entity_id})

    # === INSPECTOR ===
    def on_transform_change(self, sender, app_data, user_data):
        entity_id, field = user_data
        updates = {field: app_data}
        self.client.send("UPDATE_ENTITY", {"id": entity_id, "updates": updates})

    def build_inspector(self, details):
        if dpg.does_item_exist("inspector_root"):
            dpg.delete_item("inspector_root", children_only=True)
            
        with dpg.group(parent="inspector_root"):
            dpg.add_text(f"Name: {details['name']}")
            dpg.add_text(f"ID: {details['id']}")
            dpg.add_separator()
            
            # Transform
            t = details['transform']
            dpg.add_text("Transform", color=(255, 200, 100))
            dpg.add_input_floatx(label="Position", default_value=t['position'], size=3, tag="inp_pos",
                                 callback=self.on_transform_change, user_data=(details['id'], "position"))
            dpg.add_input_floatx(label="Rotation", default_value=t['rotation'], size=3, tag="inp_rot",
                                 callback=self.on_transform_change, user_data=(details['id'], "rotation"))
            dpg.add_input_floatx(label="Scale", default_value=t['scale'], size=3, tag="inp_scale",
                                 callback=self.on_transform_change, user_data=(details['id'], "scale"))
            
            dpg.add_separator()
            dpg.add_text("Components", color=(255, 200, 100))
            dpg.add_input_text(label="Mesh", default_value=str(details['mesh']), readonly=True)
            dpg.add_input_text(label="Diffuse", default_value=str(details['diffuse']), readonly=True)
            dpg.add_input_text(label="Normal", default_value=str(details['normal']), readonly=True)

    # === ASSET BROWSER HELPERS ===
    def get_asset_files(self, root_dir):
        asset_files = []
        if not os.path.exists(root_dir):
            return []
        for root, dirs, files in os.walk(root_dir):
            for file in files:
                full_path = os.path.join(root, file)
                rel_path = os.path.relpath(full_path, root_dir)
                asset_files.append((rel_path, full_path, file))
        return asset_files

    def load_texture(self, file_path):
        try:
            image = Image.open(file_path).convert("RGBA")
            width, height = image.size
            data = list(image.getdata())
            flat_data = []
            for pixel in data:
                flat_data.extend([pixel[0] / 255.0, pixel[1] / 255.0, pixel[2] / 255.0, pixel[3] / 255.0])
            return width, height, flat_data
        except Exception as e:
            self.log(f"Failed to load texture {file_path}: {e}")
            return 0, 0, []

    def populate_asset_list(self):
        if dpg.does_item_exist("file_list_group"):
             dpg.delete_item("file_list_group", children_only=True)
             
        self.asset_files = self.get_asset_files(ASSET_DIR)
        self.asset_files.sort(key=lambda x: x[0])
        
        for rel_path, full_path, filename in self.asset_files:
            dpg.add_selectable(label=rel_path, callback=self.on_asset_selected, user_data=(rel_path, full_path), parent="file_list_group")

    def filter_assets(self, sender, filter_text, user_data):
        if dpg.does_item_exist("file_list_group"):
             dpg.delete_item("file_list_group", children_only=True)
        
        filter_lower = filter_text.lower()
        for rel_path, full_path, filename in self.asset_files:
            if filter_lower in rel_path.lower():
                dpg.add_selectable(label=rel_path, callback=self.on_asset_selected, user_data=(rel_path, full_path), parent="file_list_group")

    def on_asset_selected(self, sender, app_data, user_data):
        rel_path, full_path = user_data
        dpg.delete_item("preview_container", children_only=True)
        ext = os.path.splitext(full_path)[1].lower()
        
        if ext in ['.png', '.jpg', '.jpeg', '.tga']:
            width, height, data = self.load_texture(full_path)
            if width > 0:
                texture_tag = f"tex_{rel_path}"
                if not dpg.does_item_exist(texture_tag):
                    dpg.add_static_texture(width=width, height=height, default_value=data, tag=texture_tag, parent="asset_registry")
                
                # Preview
                display_w, display_h = width, height
                max_size = 300
                if display_w > max_size or display_h > max_size:
                    ratio = min(max_size/display_w, max_size/display_h)
                    display_w = int(display_w * ratio)
                    display_h = int(display_h * ratio)
                dpg.add_image(texture_tag, parent="preview_container", width=display_w, height=display_h)
                dpg.add_text(f"{width}x{height}", parent="preview_container")
        elif ext == '.prefab':
             dpg.add_text(f"Prefab: {rel_path}", parent="preview_container")
             dpg.add_button(label="Instantiate Prefab", callback=self.on_instantiate_prefab, user_data=full_path, parent="preview_container")
        else:
             dpg.add_text(f"File: {rel_path}", parent="preview_container")

    # === MAIN LOOP ===
    def update(self):
        if self.client.connected:
            packets = self.client.poll()
            for packet in packets:
                if packet.cmd == "SCENE_TREE":
                    self.build_hierarchy(packet.data.get("entities", []))
                    self.log(f"Scene Tree Updated ({len(packet.data.get('entities', []))} entities)")
                elif packet.cmd == "ENTITY_DETAILS":
                    self.build_inspector(packet.data)
                    self.log(f"Inspector Updated for {packet.data['name']}")

    def save_scene_dialog(self, sender, app_data):
        with dpg.file_dialog(directory_selector=False, show=True, callback=self.on_save_scene, tag="file_dialog_save", width=700, height=400):
            dpg.add_file_extension(".json", color=(0, 255, 0, 255))
            dpg.add_file_extension(".*")

    def load_scene_dialog(self, sender, app_data):
        with dpg.file_dialog(directory_selector=False, show=True, callback=self.on_load_scene, tag="file_dialog_load", width=700, height=400):
            dpg.add_file_extension(".json", color=(0, 255, 0, 255))
            dpg.add_file_extension(".*")

    def on_save_scene(self, sender, app_data):
        path = app_data['file_path_name']
        self.log(f"Saving scene to: {path}")
        self.client.send("SAVE_SCENE", {"path": path})

    def on_load_scene(self, sender, app_data):
        path = app_data['file_path_name']
        self.log(f"Loading scene from: {path}")
        self.client.send("LOAD_SCENE", {"path": path})
        # Note: server will send updated tree later

    # === PREFAB HANDLERS ===
    def save_prefab_prompt(self, sender, app_data, user_data):
        entity_id = user_data
        self.pending_prefab_entity_id = entity_id # Store ID to usage in callback
        with dpg.file_dialog(directory_selector=False, show=True, callback=self.on_save_prefab, tag="file_dialog_prefab", width=700, height=400):
            dpg.add_file_extension(".prefab", color=(0, 255, 255, 255))
            dpg.add_file_extension(".*")

    def on_save_prefab(self, sender, app_data):
        path = app_data['file_path_name']
        if not path.endswith(".prefab"):
            path += ".prefab"
            
        entity_id = getattr(self, "pending_prefab_entity_id", None)
        if entity_id is not None:
            self.log(f"Saving Prefab for Entity {entity_id} to {path}")
            self.client.send("SAVE_PREFAB", {"entity_id": entity_id, "path": path})

    def on_instantiate_prefab(self, sender, app_data, user_data):
        path = user_data
        self.log(f"Instantiating Prefab from {path}")
        self.client.send("INSTANTIATE_PREFAB", {"path": path})


    def run(self):
        dpg.create_context()
        
        # Texture Registry
        with dpg.texture_registry(tag="asset_registry", show=False):
            pass

        with dpg.window(tag="Primary Window"):
            with dpg.menu_bar():
                with dpg.menu(label="File"):
                    dpg.add_menu_item(label="Save Scene As...", callback=self.save_scene_dialog)
                    dpg.add_menu_item(label="Load Scene...", callback=self.load_scene_dialog)
                    dpg.add_menu_item(label="Exit", callback=dpg.destroy_context)
                with dpg.menu(label="Project"):
                    dpg.add_menu_item(label="Connect", callback=self.connect_to_game)
                    dpg.add_menu_item(label="Refresh assets", callback=self.populate_asset_list)

            # Top Toolbar
            with dpg.group(horizontal=True):
                dpg.add_text("Status: Disconnected", tag="status_text", color=(255, 100, 100))
                dpg.add_button(label="Connect", callback=self.connect_to_game, tag="connect_btn")
                dpg.add_button(label="Refresh Tree", callback=self.request_scene_tree)

            dpg.add_separator()

            # TAB BAR for Switchable Layouts
            with dpg.tab_bar():
                
                # TAB 1: SCENE EDITOR
                with dpg.tab(label="Scene Editor"):
                    with dpg.group(horizontal=True):
                        # Hierarchy
                        with dpg.child_window(width=300, height=-1, border=True):
                            dpg.add_text("Hierarchy", color=(0, 255, 0))
                            dpg.add_separator()
                            dpg.add_group(tag="hierarchy_root")
                        
                        # Inspector
                        with dpg.child_window(width=400, height=-1, border=True):
                            dpg.add_text("Inspector", color=(0, 255, 0))
                            dpg.add_separator()
                            dpg.add_group(tag="inspector_root")
                            dpg.add_text("Select an entity.", parent="inspector_root")

                # TAB 2: ASSET BROWSER
                with dpg.tab(label="Asset Browser"):
                     with dpg.group(horizontal=True):
                        # File List
                        with dpg.child_window(width=300, height=-1):
                             dpg.add_input_text(label="Filter", callback=self.filter_assets)
                             dpg.add_separator()
                             dpg.add_group(tag="file_list_group")
                        
                        # Preview
                        with dpg.child_window(width=-1, height=-1, tag="preview_window"):
                             dpg.add_text("Preview", color=(0, 255, 0))
                             dpg.add_group(tag="preview_container")

                # TAB 3: CONSOLE
                with dpg.tab(label="Console"):
                    dpg.add_text("", tag="log_text_container")

        dpg.create_viewport(title='Starlight Studio', width=WINDOW_WIDTH, height=WINDOW_HEIGHT)
        dpg.setup_dearpygui()
        dpg.show_viewport()
        dpg.set_primary_window("Primary Window", True)
        
        # Initial Population
        self.populate_asset_list()

        while dpg.is_dearpygui_running():
            self.update()
            dpg.render_dearpygui_frame()

        dpg.destroy_context()

if __name__ == "__main__":
    studio = StarlightStudio()
    studio.run()
