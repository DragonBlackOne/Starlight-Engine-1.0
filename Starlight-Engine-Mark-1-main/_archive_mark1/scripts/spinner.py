from starlight.behaviour import StarlightBehaviour

class Spinner(StarlightBehaviour):
    def on_start(self):
        self.studio.console.log(f"Spinner started on {self.entity.name}", "INFO")
        self.speed = 2.0
        
    def update(self, dt):
        if not hasattr(self, "rot_y"):
            self.rot_y = 0.0
        self.rot_y += self.speed * dt
        
        # We can update the transform rotation via studio's backend proxy
        # Since fw_ent is bound to backend directly via `set_rotation`
        try:
            from starlight import backend
            backend.set_rotation(self.entity.id, 0, self.rot_y, 0)
        except Exception as e:
            pass
            
    def on_destroy(self):
        self.studio.console.log(f"Spinner destroyed", "WARN")
