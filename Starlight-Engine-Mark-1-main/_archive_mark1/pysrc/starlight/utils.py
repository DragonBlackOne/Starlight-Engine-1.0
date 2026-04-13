import time
from functools import wraps
from loguru import logger

def timeit(func):
    """
    Decorator to measure execution time of a function.
    Skill: python-decorators
    """
    @wraps(func)
    def wrapper(*args, **kwargs):
        start_time = time.perf_counter()
        result = func(*args, **kwargs)
        end_time = time.perf_counter()
        execution_time = end_time - start_time
        logger.info(f"Function '{func.__name__}' took {execution_time:.4f} seconds to execute.")
        return result
    return wrapper

class Singleton(type):
    """
    Singleton Metaclass.
    Skill: python-decorators
    """
    _instances = {}
    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            cls._instances[cls] = super(Singleton, cls).__call__(*args, **kwargs)
        return cls._instances[cls]
