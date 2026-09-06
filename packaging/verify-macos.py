#!/usr/bin/env python3
"""Reject missing or developer-machine-only Mach-O dependencies in a bundle."""
import pathlib
import subprocess
import sys

bundle = pathlib.Path(sys.argv[1]).resolve()
frameworks = bundle / "Contents/Frameworks"
executable_dir = bundle / "Contents/MacOS"
magic = {b"\xcf\xfa\xed\xfe", b"\xce\xfa\xed\xfe", b"\xca\xfe\xba\xbe", b"\xca\xfe\xba\xbf"}
errors = []
count = 0
for binary in bundle.rglob("*"):
    if binary.is_symlink() or not binary.is_file():
        continue
    with binary.open("rb") as source:
        if source.read(4) not in magic:
            continue
    count += 1
    listing = subprocess.check_output(["otool", "-L", str(binary)], text=True)
    for line in listing.splitlines()[1:]:
        dependency = line.strip().split(" (compatibility version", 1)[0]
        if dependency.startswith(("/System/Library/", "/usr/lib/")):
            continue
        if dependency.startswith("@rpath/"):
            target = frameworks / dependency[len("@rpath/"):]
        elif dependency.startswith("@executable_path/"):
            target = executable_dir / dependency[len("@executable_path/"):]
        elif dependency.startswith("@loader_path/"):
            target = binary.parent / dependency[len("@loader_path/"):]
        else:
            errors.append(f"External dependency: {binary.relative_to(bundle)} -> {dependency}")
            continue
        if not target.exists() or not target.resolve().is_relative_to(bundle):
            errors.append(f"Missing bundled dependency: {binary.relative_to(bundle)} -> {dependency}")
if errors:
    sys.exit("\n".join(errors))
print(f"Verified {count} Mach-O files: dependencies are bundled or macOS system libraries.")
