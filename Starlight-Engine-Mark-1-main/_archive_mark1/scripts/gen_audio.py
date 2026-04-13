import math
import os
import struct
import wave


def create_sine_wave(path, frequency=440.0, duration=1.0, volume=0.5, sample_rate=44100):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    n_frames = int(sample_rate * duration)

    with wave.open(path, "w") as wav_file:
        wav_file.setnchannels(1)  # Mono
        wav_file.setsampwidth(2)  # 2 bytes per sample (16-bit)
        wav_file.setframerate(sample_rate)

        data = []
        for i in range(n_frames):
            value = int(volume * 32767.0 * math.sin(2.0 * math.pi * frequency * i / sample_rate))
            data.append(struct.pack("<h", value))

        wav_file.writeframes(b"".join(data))

    print(f"Generated {path}")


def create_noise_wave(path, duration=0.5, volume=0.5, sample_rate=44100):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    n_frames = int(sample_rate * duration)
    with wave.open(path, "w") as wav_file:
        wav_file.setnchannels(1)
        wav_file.setsampwidth(2)
        wav_file.setframerate(sample_rate)
        data = []
        import random

        for i in range(n_frames):
            # Decay volume
            v = volume * (1.0 - i / n_frames)
            value = int(v * 32767.0 * (random.random() * 2.0 - 1.0))
            data.append(struct.pack("<h", value))
        wav_file.writeframes(b"".join(data))
    print(f"Generated {path}")


def create_square_wave(path, frequency=440.0, duration=0.1, volume=0.5, sample_rate=44100):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    n_frames = int(sample_rate * duration)
    with wave.open(path, "w") as wav_file:
        wav_file.setnchannels(1)
        wav_file.setsampwidth(2)
        wav_file.setframerate(sample_rate)
        data = []
        for i in range(n_frames):
            # Square wave
            t = frequency * i / sample_rate
            value = 32767.0 * volume if (t - int(t)) < 0.5 else -32767.0 * volume
            data.append(struct.pack("<h", int(value)))
        wav_file.writeframes(b"".join(data))
    print(f"Generated {path}")


if __name__ == "__main__":
    create_sine_wave("assets/hit.wav", frequency=440.0, duration=0.5)
    create_sine_wave("assets/bgm.wav", frequency=220.0, duration=2.0)
    create_square_wave("assets/shoot.wav", frequency=880.0, duration=0.1)
    create_noise_wave("assets/explosion.wav", duration=0.5)
