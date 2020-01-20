import platform
import importlib

def _load_native_module():
    os_raw = platform.system()
    if os_raw == "Linux":
        os = "linux"
    elif os_raw == "Darwin":
        os = "macos"
    elif os_raw == "Windows":
        os = "windows"
    else:
        os = "unknown"

    os_arch_raw = platform.machine()
    if os_arch_raw == "AMD64" or os_arch_raw == "x86_64":
        os_arch = "x86_64"
    else:
        os_arch = "unknown"

    return importlib.import_module(os + "-" + os_arch + ".hyperbuild")

minify = _load_native_module().minify
