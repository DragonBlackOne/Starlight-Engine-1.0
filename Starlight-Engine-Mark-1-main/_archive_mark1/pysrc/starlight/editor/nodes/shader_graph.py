import dearpygui.dearpygui as dpg

class ShaderGraphEditor:
    
    def __init__(self, bg_compile_callback=None):
        self.window_tag = "shader_graph_window"
        self.node_editor_tag = "shader_node_editor"
        self.bg_compile_callback = bg_compile_callback
        self.links = [] # list of (id1, id2, link_id) where id1 = output pin, id2 = input pin
        
        self.logical_nodes = {} # node_id -> dict
        self.pin_to_node = {} # pin_id -> node_id
        self.pin_to_name = {} # pin_id -> name
        
        self.master_node_id = None
        self.master_pins = {} # "Albedo" -> pin_id
        
    def show(self):
        if dpg.does_item_exist(self.window_tag):
            dpg.focus_item(self.window_tag)
            return

        with dpg.window(label="Shader Graph (PBR)", tag=self.window_tag, width=800, height=600):
            with dpg.group(horizontal=True):
                dpg.add_button(label="Compile WGSL", callback=self.compile_graph)
                dpg.add_button(label="Math: Multiply", callback=lambda: self.add_math_node("Multiply"))
                dpg.add_button(label="Math: Add", callback=lambda: self.add_math_node("Add"))
                dpg.add_button(label="Color", callback=lambda: self.add_color_node())
                dpg.add_button(label="Time (Float)", callback=lambda: self.add_time_node())
                dpg.add_button(label="Texture Sample", callback=lambda: self.add_texture_node())

            # The main canvas
            with dpg.node_editor(tag=self.node_editor_tag, callback=self.link_callback, delink_callback=self.delink_callback):
                self._create_master_node()

    def _create_master_node(self):
        with dpg.node(label="Material PBR Master", pos=[500, 200]) as node_id:
            self.master_node_id = node_id
            self.logical_nodes[node_id] = {"type": "Master", "inputs": [], "outputs": []}
            
            pins = ["Albedo (vec3)", "Normal (XYZ)", "Metallic (f32)", "Roughness (f32)", "Emission (vec3)"]
            for p in pins:
                with dpg.node_attribute(attribute_type=dpg.mvNode_Attr_Input, shape=dpg.mvNode_PinShape_CircleFilled) as pin_id:
                    dpg.add_text(p)
                    self.master_pins[p] = pin_id
                    self.pin_to_node[pin_id] = node_id
                    self.pin_to_name[pin_id] = p
                    self.logical_nodes[node_id]["inputs"].append((p, pin_id))

    def _register_node(self, node_id, ntype, inputs, outputs):
        self.logical_nodes[node_id] = {"type": ntype, "inputs": inputs, "outputs": outputs}
        for (name, pid) in inputs:
            self.pin_to_node[pid] = node_id
            self.pin_to_name[pid] = name
        for (name, pid) in outputs:
            self.pin_to_node[pid] = node_id
            self.pin_to_name[pid] = name

    def link_callback(self, sender, app_data):
        # app_data -> (id1, id2). One is input, one output.
        # Ensure id1 is output and id2 is input
        id1, id2 = app_data[0], app_data[1]
        
        node1 = self.pin_to_node.get(id1)
        node2 = self.pin_to_node.get(id2)
        if not node1 or not node2: return
        
        is_id1_out = any(pid == id1 for (n, pid) in self.logical_nodes[node1]["outputs"])
        out_pin, in_pin = (id1, id2) if is_id1_out else (id2, id1)

        link = dpg.add_node_link(out_pin, in_pin, parent=sender)
        self.links.append((out_pin, in_pin, link))

    def delink_callback(self, sender, app_data):
        dpg.delete_item(app_data)
        self.links = [l for l in self.links if l[2] != app_data]

    def add_math_node(self, operation="Multiply"):
        with dpg.node(label=f"Math: {operation}", parent=self.node_editor_tag, pos=[50, 50]) as node_id:
            with dpg.node_attribute(attribute_type=dpg.mvNode_Attr_Input) as pin_a:
                dpg.add_text("A (vec3/f32)")
            with dpg.node_attribute(attribute_type=dpg.mvNode_Attr_Input) as pin_b:
                dpg.add_text("B (vec3/f32)")
            with dpg.node_attribute(attribute_type=dpg.mvNode_Attr_Output) as pin_out:
                dpg.add_text("Out")
        self._register_node(node_id, operation, [("A", pin_a), ("B", pin_b)], [("Out", pin_out)])

    def add_color_node(self):
        with dpg.node(label="Color", parent=self.node_editor_tag, pos=[50, 150]) as node_id:
            with dpg.node_attribute(attribute_type=dpg.mvNode_Attr_Output) as pin_out:
                color_id = dpg.add_color_edit(label="RGB", no_alpha=True, width=150)
        self._register_node(node_id, "Color", [], [("Out", pin_out)])
        self.logical_nodes[node_id]["color_id"] = color_id

    def add_time_node(self):
        with dpg.node(label="Time", parent=self.node_editor_tag, pos=[50, 250]) as node_id:
            with dpg.node_attribute(attribute_type=dpg.mvNode_Attr_Output) as pin_out:
                dpg.add_text("Elapsed Time (f32)")
        self._register_node(node_id, "Time", [], [("Out", pin_out)])

    def add_texture_node(self):
        with dpg.node(label="Texture Sample", parent=self.node_editor_tag, pos=[50, 350]) as node_id:
            with dpg.node_attribute(attribute_type=dpg.mvNode_Attr_Input) as pin_uv:
                dpg.add_text("UV (vec2)")
            with dpg.node_attribute(attribute_type=dpg.mvNode_Attr_Output) as pin_out:
                dpg.add_text("RGB (vec3)")
        self._register_node(node_id, "TextureSample", [("UV", pin_uv)], [("Out", pin_out)])

    def compile_graph(self):
        print("[Shader Graph] Compiling Visual Logic to WGSL...")
        wgsl_code = self._generate_wgsl()
        print("--- GENERATED WGSL ---")
        print(wgsl_code)
        print("----------------------")
        if self.bg_compile_callback:
            self.bg_compile_callback(wgsl_code)

    def _generate_wgsl(self):
        generated_nodes = {} # node_id -> variable name
        wgsl_lines = []
        
        def resolve_input(pin_id, default="vec3<f32>(0.0)"):
            for out_p, in_p, link_id in self.links:
                if in_p == pin_id:
                    out_node = self.pin_to_node[out_p]
                    return evaluate_node(out_node)
            return default

        def evaluate_node(n_id):
            if n_id in generated_nodes:
                return generated_nodes[n_id]
                
            n_data = self.logical_nodes[n_id]
            ntype = n_data["type"]
            var_name = f"node_{n_id}_out"
            
            if ntype == "Color":
                col = dpg.get_value(n_data["color_id"])
                r, g, b = col[0], col[1], col[2]
                if r > 1.0 or g > 1.0 or b > 1.0:
                    r, g, b = r/255.0, g/255.0, b/255.0
                wgsl_lines.append(f"    let {var_name} = vec3<f32>({r:.3f}, {g:.3f}, {b:.3f});")
            elif ntype == "Time":
                # Fallback implementation assuming uniforms.time exists
                wgsl_lines.append(f"    let {var_name} = uniforms.time;")
            elif ntype == "Multiply":
                in_pins = n_data["inputs"]
                val_a = resolve_input(in_pins[0][1], "vec3<f32>(1.0)")
                val_b = resolve_input(in_pins[1][1], "vec3<f32>(1.0)")
                wgsl_lines.append(f"    let {var_name} = {val_a} * {val_b};")
            elif ntype == "Add":
                in_pins = n_data["inputs"]
                val_a = resolve_input(in_pins[0][1], "vec3<f32>(0.0)")
                val_b = resolve_input(in_pins[1][1], "vec3<f32>(0.0)")
                wgsl_lines.append(f"    let {var_name} = {val_a} + {val_b};")
            elif ntype == "TextureSample":
                in_pins = n_data["inputs"]
                val_uv = resolve_input(in_pins[0][1], "in.tex_coords") 
                wgsl_lines.append(f"    let {var_name} = textureSample(t_diffuse, s_diffuse, {val_uv}).rgb;")
            else:
                wgsl_lines.append(f"    let {var_name} = vec3<f32>(0.0);")
                
            generated_nodes[n_id] = var_name
            return var_name

        albedo_code = resolve_input(self.master_pins["Albedo (vec3)"], "vec3<f32>(1.0, 1.0, 1.0)")
        normal_code = resolve_input(self.master_pins["Normal (XYZ)"], "vec3<f32>(0.0, 0.0, 1.0)")
        metallic_code = resolve_input(self.master_pins["Metallic (f32)"], "0.0")
        roughness_code = resolve_input(self.master_pins["Roughness (f32)"], "0.5")
        emission_code = resolve_input(self.master_pins["Emission (vec3)"], "vec3<f32>(0.0, 0.0, 0.0)")

        final_code = "\n".join(wgsl_lines)
        final_code += f"\n    var generated_albedo = {albedo_code};"
        final_code += f"\n    var generated_normal = {normal_code};"
        final_code += f"\n    var generated_metallic = {metallic_code};"
        final_code += f"\n    var generated_roughness = {roughness_code};"
        final_code += f"\n    var generated_emission = {emission_code};"
        
        return final_code

