
import dearpygui.dearpygui as dpg
import os
import sys
from PIL import Image

# Add pysrc to path just in case we need engine utils later, though not needed for this simple tool
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "pysrc")))

# Constants
ASSET_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "assets"))
WINDOW_WIDTH = 1200
WINDOW_HEIGHT = 800

def get_asset_files(root_dir):
    asset_files = []
    if not os.path.exists(root_dir):
        return []
        
    for root, dirs, files in os.walk(root_dir):
        for file in files:
            # Rel path
            full_path = os.path.join(root, file)
            rel_path = os.path.relpath(full_path, root_dir)
            asset_files.append((rel_path, full_path, file))
    return asset_files

def load_texture(file_path):
    try:
        # Load with PIL
        image = Image.open(file_path)
        # Ensure RGBA
        image = image.convert("RGBA")
        width, height = image.size
        data = list(image.getdata())
        # Flatten and normalize
        flat_data = []
        for pixel in data:
            flat_data.extend([pixel[0] / 255.0, pixel[1] / 255.0, pixel[2] / 255.0, pixel[3] / 255.0])
        
        return width, height, flat_data
    except Exception as e:
        print(f"Failed to load texture {file_path}: {e}")
        return 0, 0, []

def on_selection(sender, app_data, user_data):
    # user_data is (rel_path, full_path)
    rel_path, full_path = user_data
    dpg.set_value("status_text", f"Selected: {rel_path}")
    
    # Check extension
    ext = os.path.splitext(full_path)[1].lower()
    
    # Clear previous preview content
    dpg.delete_item("preview_container", children_only=True)
    
    if ext in ['.png', '.jpg', '.jpeg', '.tga', '.bmp', '.gif']:
        width, height, data = load_texture(full_path)
        if width > 0:
            # Create texture in registry
            texture_tag = f"tex_{rel_path}"
            
            # Use the global texture registry
            if not dpg.does_item_exist(texture_tag):
                try:
                    dpg.add_static_texture(width=width, height=height, default_value=data, tag=texture_tag, parent="asset_texture_registry")
                except Exception as e:
                    dpg.add_text(f"Error creating texture: {e}", parent="preview_container")
                    return

            # Add image to preview
            # Scale down if too big
            display_w = width
            display_h = height
            max_size = 500
            if display_w > max_size or display_h > max_size:
                ratio = min(max_size/display_w, max_size/display_h)
                display_w = int(display_w * ratio)
                display_h = int(display_h * ratio)
                
            dpg.add_image(texture_tag, parent="preview_container", width=display_w, height=display_h)
            dpg.add_text(f"Dimensions: {width}x{height}", parent="preview_container")
            dpg.add_text(f"Path: {rel_path}", parent="preview_container")
            
    elif ext in ['.obj', '.glb', '.gltf', '.fbx', '.blend']:
         dpg.add_text(f"Model File: {os.path.basename(full_path)}", parent="preview_container", color=(255, 200, 100))
         dpg.add_separator(parent="preview_container")
         dpg.add_text("3D Preview not supported in this lightweight browser.", parent="preview_container")
         size_kb = os.path.getsize(full_path) / 1024
         dpg.add_text(f"Size: {size_kb:.2f} KB", parent="preview_container")
    
    elif ext in ['.wgsl', '.glsl', '.vert', '.frag', '.py', '.txt', '.md', '.json', '.toml', '.rs']:
        # Show text content
        dpg.add_text(f"Text File: {os.path.basename(full_path)}", parent="preview_container", color=(100, 200, 255))
        try:
            with open(full_path, 'r', encoding='utf-8') as f:
                content = f.read()
            # Truncate if too long
            if len(content) > 10000:
                content = content[:10000] + "\n... [Truncated]"
            dpg.add_input_text(default_value=content, multiline=True, readonly=True, width=-1, height=-1, parent="preview_container")
        except Exception as e:
             dpg.add_text(f"Could not read file: {e}", parent="preview_container")
    else:
        dpg.add_text(f"File: {os.path.basename(full_path)}", parent="preview_container")
        dpg.add_text("No preview available for this file type.", parent="preview_container")

def main():
    dpg.create_context()
    
    # Texture Registry
    with dpg.texture_registry(tag="asset_texture_registry", show=False):
        pass # Will add textures dynamically

    with dpg.window(tag="Primary Window"):
        dpg.add_text("Starlight Engine Asset Browser", color=(255, 215, 0))
        dpg.add_separator()
        
        with dpg.group(horizontal=True):
            # Left Panel: File List
            with dpg.child_window(width=350):
                dpg.add_text(f"Assets ({ASSET_DIR})", color=(0, 255, 0))
                dpg.add_separator()
                
                files = get_asset_files(ASSET_DIR)
                files.sort(key=lambda x: x[0])
                
                # Filter Box
                dpg.add_input_text(label="Filter", callback=lambda s, a, u: filter_files(s, a, u, files))
                
                # File List Container
                with dpg.group(tag="file_list_group"):
                    populate_file_list(files)

            # Right Panel: Preview
            with dpg.child_window(tag="preview_container", border=True):
                dpg.add_text("Select an asset to preview.")
        
        dpg.add_separator()
        dpg.add_text("Ready.", tag="status_text")

    dpg.create_viewport(title='Starlight Asset Browser', width=WINDOW_WIDTH, height=WINDOW_HEIGHT)
    dpg.setup_dearpygui()
    dpg.show_viewport()
    dpg.set_primary_window("Primary Window", True)
    dpg.start_dearpygui()
    dpg.destroy_context()

def populate_file_list(files):
    for rel_path, full_path, filename in files:
        dpg.add_selectable(label=rel_path, callback=on_selection, user_data=(rel_path, full_path), parent="file_list_group")

def filter_files(sender, filter_text, user_data, all_files):
    dpg.delete_item("file_list_group", children_only=True)
    filter_lower = filter_text.lower()
    for rel_path, full_path, filename in all_files:
        if filter_lower in rel_path.lower():
             dpg.add_selectable(label=rel_path, callback=on_selection, user_data=(rel_path, full_path), parent="file_list_group")

if __name__ == "__main__":
    main()
