import ctypes
dll = ctypes.CDLL("C:\\DevCppProjs\\PythonCallCppClass\\PythonCallCppClass.dll")
dll.display()
dll.display_int(1000)