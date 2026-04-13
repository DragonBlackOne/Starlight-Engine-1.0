
import starlight
import time

class AudioTest(starlight.Script):
    def on_create(self):
        print("[Script] Requesting sound playback: D:/Projetos/Starlight/Starlight-Engine-Mark-1/assets/audio/ball_hit_1.mp3")
        starlight.world.play_sound(r"D:\Projetos\Starlight\Starlight-Engine-Mark-1\assets\audio\ball_hit_1.mp3")
        self.start_time = time.time()
    
    def on_update(self, dt):
        # Exit after 3 seconds
        if time.time() - self.start_time > 3.0:
            print("[Script] Test complete, requesting shutdown (via generic means if available)")
            # Currently no API to exit form script?
            # We can panic or just rely on user closing, but for automation...
            # We'll just print and let the user hear it.
            pass
