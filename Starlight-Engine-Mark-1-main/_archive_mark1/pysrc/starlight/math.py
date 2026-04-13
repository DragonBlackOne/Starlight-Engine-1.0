import numpy as np
from scipy.spatial.transform import Rotation

def normalize(v):
    norm = np.linalg.norm(v)
    if norm == 0:
        return v
    return v / norm

def look_at(eye, target, up):
    # Standard implementation
    eye = np.array(eye, dtype=np.float32)
    target = np.array(target, dtype=np.float32)
    up = np.array(up, dtype=np.float32)

    z = normalize(eye - target)
    x = normalize(np.cross(up, z))
    y = np.cross(z, x)

    mat = np.identity(4, dtype=np.float32)
    mat[0, :3] = x
    mat[1, :3] = y
    mat[2, :3] = z
    mat[0, 3] = -np.dot(x, eye)
    mat[1, 3] = -np.dot(y, eye)
    mat[2, 3] = -np.dot(z, eye)

    return mat.T.tolist()

def perspective(fov, aspect, near, far):
    # Standard implementation
    f = 1.0 / np.tan(fov / 2.0)
    mat = np.zeros((4, 4), dtype=np.float32)
    mat[0, 0] = f / aspect
    mat[1, 1] = f
    mat[2, 2] = (far + near) / (near - far)
    mat[2, 3] = (2.0 * far * near) / (near - far)
    mat[3, 2] = -1.0
    return mat.tolist()

def euler_to_quat(pitch, yaw, roll):
    """
    Converts euler angles (radians) to quaternion [x, y, z, w].
    Uses SciPy for robustness.
    """
    r = Rotation.from_euler('xyz', [pitch, yaw, roll])
    return r.as_quat()

def get_forward_vector(pitch, yaw, roll):
    """
    Calculates the forward vector from Euler angles (radians).
    Assumes standard camera orientation where -Z is forward.
    Rotation order matches 'xyz' (Pitch, Yaw, Roll).
    """
    r = Rotation.from_euler('xyz', [pitch, yaw, roll])
    # Forward vector in local space (assuming looking down -Z)
    local_forward = np.array([0.0, 0.0, -1.0])
    world_forward = r.apply(local_forward)
    return world_forward.tolist()

def mat4_multiply(a, b):
    """Multiplies two 4x4 matrices."""
    a = np.array(a, dtype=np.float32)
    b = np.array(b, dtype=np.float32)
    return (a @ b).tolist()

