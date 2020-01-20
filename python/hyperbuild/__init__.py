import importlib
import inspect
import os
import platform
import sys


def _load_native_module():
    os_name_raw = platform.system()
    if os_name_raw == "Linux":
        os_name = "linux"
    elif os_name_raw == "Darwin":
        os_name = "macos"
    elif os_name_raw == "Windows":
        os_name = "windows"
    else:
        os_name = "unknown"

    os_arch_raw = platform.machine()
    if os_arch_raw == "AMD64" or os_arch_raw == "x86_64":
        os_arch = "x86_64"
    else:
        os_arch = "unknown"

    this_folder = os.path.realpath(os.path.abspath(os.path.split(inspect.getfile(inspect.currentframe()))[0]))
    pyv = sys.version_info
    sys.path.insert(0, this_folder)
    module = importlib.import_module(''.join([os_name, "-", os_arch, '-', str(pyv.major), '_', str(pyv.minor), ".hyperbuild"]))
    sys.path.pop(0)
    return module


minify = _load_native_module().minify
