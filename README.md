# WinCrashpad
WinCrashpad is a crash-collecting system for Windows.

Build with CMake or [vcpkg](https://github.com/microsoft/vcpkg).

---

**How to install with vcpkg:**
1. Copy `vcpkg_port\crashreport` folder to `<vcpkg>\ports` folder.
2. Using `vcpkg` command to install.

    ```bat
    # Shared library
    vcpkg install wincrashpad:x86-windows
    vcpkg install wincrashpad:x64-windows

    # Static library
    vcpkg install wincrashpad:x86-windows-static
    vcpkg install wincrashpad:x64-windows-static
    ```

