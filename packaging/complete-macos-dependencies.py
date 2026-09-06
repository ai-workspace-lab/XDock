#!/usr/bin/env python3
"""Complete split-Qt dependencies that macdeployqt cannot resolve on Homebrew."""
import pathlib
import shutil
import subprocess
import sys
import os

bundle = pathlib.Path(sys.argv[1]).resolve()
qt_lib = pathlib.Path(sys.argv[2]).resolve()
frameworks = bundle / "Contents/Frameworks"
qml_root = bundle / "Contents/Resources/qml"
source_qml = pathlib.Path(subprocess.check_output(["qmake6", "-query", "QT_INSTALL_QML"], text=True).strip())
plugins = bundle / "Contents/PlugIns/quick"
# macdeployqt may leave a previously-created import directory only partially
# populated on retry. Complete each deployed module from its installed source.
for directory in [qml_root, *[p for p in qml_root.rglob("*") if p.is_dir()]]:
    original = source_qml / directory.relative_to(qml_root)
    if not original.is_dir():
        continue
    for source in original.iterdir():
        if not source.is_file() or source.name.endswith(("_debug.dylib", ".qrc")):
            continue
        target = directory / source.name
        if source.suffix == ".dylib":
            deployed = plugins / source.name
            if not deployed.exists():
                shutil.copy2(source.resolve(), deployed)
            if not target.exists():
                if target.is_symlink():
                    target.unlink()
                target.symlink_to(os.path.relpath(deployed, directory))
        elif not target.exists():
            shutil.copy2(source.resolve(), target)
magic = {b"\xcf\xfa\xed\xfe", b"\xce\xfa\xed\xfe", b"\xca\xfe\xba\xbe", b"\xca\xfe\xba\xbf"}
processed = set()
while True:
    binaries = []
    for path in bundle.rglob("*"):
        if path in processed or path.is_symlink() or not path.is_file():
            continue
        with path.open("rb") as source:
            if source.read(4) in magic:
                binaries.append(path)
    if not binaries:
        break
    for binary in binaries:
        processed.add(binary)
        lines = subprocess.check_output(["otool", "-L", str(binary)], text=True).splitlines()[1:]
        for line in lines:
            dependency = line.strip().split(" (compatibility version", 1)[0]
            if dependency.startswith("@rpath/"):
                relative = dependency[len("@rpath/"):]
                source = qt_lib / relative
            elif dependency.startswith(("/opt/homebrew/", "/usr/local/")):
                source = pathlib.Path(dependency)
                if ".framework/" in dependency:
                    relative = dependency[dependency.rfind("/", 0, dependency.index(".framework/")) + 1:]
                else:
                    relative = source.name
            else:
                continue
            target = frameworks / relative
            if not target.exists():
                if not source.exists():
                    sys.exit(f"Cannot find required library: {dependency}")
                if ".framework/" in relative:
                    framework_name = relative.split("/", 1)[0]
                    source_framework = source
                    while source_framework.name != framework_name:
                        source_framework = source_framework.parent
                    shutil.copytree(source_framework.resolve(), frameworks / framework_name,
                                    symlinks=True, dirs_exist_ok=True)
                else:
                    shutil.copy2(source.resolve(), target)
                print(f"Bundled missing dependency: {relative}")
            installed_name = "@executable_path/../Frameworks/" + relative
            if target.resolve() == binary.resolve():
                subprocess.run(["install_name_tool", "-id", installed_name, str(binary)], check=True)
            else:
                subprocess.run(["install_name_tool", "-change", dependency, installed_name, str(binary)], check=True)
