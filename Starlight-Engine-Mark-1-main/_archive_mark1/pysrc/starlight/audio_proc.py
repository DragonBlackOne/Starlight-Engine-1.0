
import os
import math
import random
import struct
import wave
from starlight import backend

class AudioSystem:
    def __init__(self):
        self.enabled = hasattr(backend, 'play_sound')
        self.sfx_path = os.path.abspath("assets/sfx")
        if not os.path.exists(self.sfx_path):
            os.makedirs(self.sfx_path, exist_ok=True)
            
        # Assets map
        self.sounds = {
            "jump": os.path.join(self.sfx_path, "jump.wav"),
            "collect": os.path.join(self.sfx_path, "collect.wav"),
            "step": os.path.join(self.sfx_path, "step.wav"),
            "low_entropy": os.path.join(self.sfx_path, "alarm.wav"),
            "beacon_active": os.path.join(self.sfx_path, "beacon.wav")
        }
        
        # Generate initial assets
        self._generate_assets()

    def _generate_tone(self, filename: str, freq: float, duration: float, volume: float = 0.5, wave_type: str = 'sine', slide: float = 0.0):
        """
        Generate a WAV file with specific characteristics.
        slide: Frequency slide per second (Hz)
        """
        sample_rate = 44100
        n_samples = int(sample_rate * duration)
        
        try:
            with wave.open(filename, 'w') as wav_file:
                wav_file.setnchannels(1)
                wav_file.setsampwidth(2)
                wav_file.setframerate(sample_rate)
                
                data = []
                current_freq = freq
                
                for i in range(n_samples):
                    t = float(i) / sample_rate
                    
                    # Frequency slide
                    current_freq += slide / sample_rate
                    
                    if wave_type == 'sine':
                        val = math.sin(2.0 * math.pi * current_freq * t)
                    elif wave_type == 'square':
                        val = 1.0 if math.sin(2.0 * math.pi * current_freq * t) > 0 else -1.0
                    elif wave_type == 'sawtooth':
                        val = 2.0 * (current_freq * t - math.floor(current_freq * t + 0.5))
                    elif wave_type == 'noise':
                        val = random.uniform(-1.0, 1.0)
                    else:
                        val = math.sin(2.0 * math.pi * current_freq * t)
                    
                    # Envelope (Fade In/Out)
                    if i < 1000: val *= i / 1000.0
                    if i > n_samples - 1000: val *= (n_samples - i) / 1000.0
                    
                    sample = int(val * volume * 32767.0)
                    data.append(struct.pack('<h', sample))
                
                wav_file.writeframes(b''.join(data))
        except Exception as e:
            print(f"Failed to generate {filename}: {e}")

    def _generate_assets(self):
        if not os.path.exists(self.sounds["jump"]):
            self._generate_tone(self.sounds["jump"], 440.0, 0.2, 0.5, 'square', slide=-200.0)
            
        if not os.path.exists(self.sounds["collect"]):
            # Arpeggio-like (approximated by high pitched sine with slide up)
            self._generate_tone(self.sounds["collect"], 880.0, 0.3, 0.5, 'sine', slide=400.0)

        if not os.path.exists(self.sounds["step"]):
            self._generate_tone(self.sounds["step"], 100.0, 0.05, 0.3, 'noise')
            
        if not os.path.exists(self.sounds["low_entropy"]):
            # Alarm sound
            self._generate_tone(self.sounds["low_entropy"], 600.0, 0.5, 0.6, 'sawtooth', slide=-300.0)

        if not os.path.exists(self.sounds["beacon_active"]):
            # Deep boom
            self._generate_tone(self.sounds["beacon_active"], 120.0, 2.0, 0.7, 'sine', slide=-30.0)

    def play(self, sound_name: str, volume: float = 1.0, pitch: float = 1.0):
        if not self.enabled: return
        
        path = self.sounds.get(sound_name)
        if path and os.path.exists(path):
            # Backend doesn't support pitch shifting at runtime fully yet?
            # We can simulate volume/panning
            backend.play_sound(path, False, volume, 0.5)

    def play_3d(self, sound_name: str, source_pos: tuple, listener_pos: tuple, max_dist: float = 50.0):
        """Simple distance attenuation and panning"""
        if not self.enabled: return
        
        dx = source_pos[0] - listener_pos[0]
        dy = source_pos[1] - listener_pos[1]
        dz = source_pos[2] - listener_pos[2]
        
        dist = math.sqrt(dx*dx + dy*dy + dz*dz)
        if dist > max_dist: return
        
        # Volume falloff
        volume = 1.0 - (dist / max_dist)
        volume = max(0.0, volume * volume) # Quadratic falloff
        
        # Panning (simple X-axis based)
        # Normalized relative X
        # We need relative to camera rotation, but for now simple world X
        pan = 0.5 + (dx / max_dist) * 0.5
        pan = max(0.0, min(1.0, pan))
        
        path = self.sounds.get(sound_name)
        if path:
            backend.play_sound(path, False, volume, pan)
