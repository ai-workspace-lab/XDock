#!/bin/bash
set -euo pipefail

repo_dir="$(cd "$(dirname "$0")/.." && pwd)"
build_dir="$repo_dir/build-macos-release"
version=0.1.0
arch="$(uname -m)"
release_dir="$repo_dir/dist/XDock-$version-macos-$arch"
qt_lib_dir="${XDOCK_QT_LIB_DIR:-$(qmake6 -query QT_INSTALL_LIBS)}"
qt_scxml_lib_dir=""
if command -v brew >/dev/null 2>&1; then
    candidate_scxml_lib="$(brew --prefix qtscxml 2>/dev/null || true)/lib"
    if [[ -d "$candidate_scxml_lib" ]]; then
        qt_scxml_lib_dir="$candidate_scxml_lib"
    fi
fi

if [[ "$(uname -s)" != Darwin ]]; then
    echo 'Run this packaging script on macOS.' >&2
    exit 1
fi

cmake -S "$repo_dir" -B "$build_dir" -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=26.0 -DCMAKE_OSX_ARCHITECTURES="$arch" -DBUILD_TESTING=ON
cmake --build "$build_dir" --parallel 4
ctest --test-dir "$build_dir" --output-on-failure

if [[ -e "$release_dir" ]]; then
    mv "$release_dir" "$release_dir.previous-$(date +%Y%m%d-%H%M%S)"
fi
mkdir -p "$release_dir"
ditto "$build_dir/xdock.app" "$release_dir/XDock.app"
deploy_args=("$release_dir/XDock.app" "-qmldir=$repo_dir/qml" "-libpath=$qt_lib_dir" -always-overwrite -no-codesign)
if [[ -n "$qt_scxml_lib_dir" ]]; then
    # Qt Quick Controls' QML module depends on these optional qtscxml
    # frameworks, which are keg-only outside Qt Declarative on Homebrew.
    deploy_args+=("-libpath=$qt_scxml_lib_dir")
fi
macdeployqt "${deploy_args[@]}"
python3 "$repo_dir/packaging/complete-macos-dependencies.py" "$release_dir/XDock.app" "$qt_lib_dir"
codesign --force --deep --sign - "$release_dir/XDock.app"
codesign --verify --deep --strict "$release_dir/XDock.app"
python3 "$repo_dir/packaging/verify-macos.py" "$release_dir/XDock.app"
env -u QT_PLUGIN_PATH -u QML_IMPORT_PATH -u QML2_IMPORT_PATH -u DYLD_FRAMEWORK_PATH -u DYLD_LIBRARY_PATH \
    "$release_dir/XDock.app/Contents/MacOS/xdock" --capture "$release_dir/package-preview.png"
ditto -c -k --sequesterRsrc --keepParent "$release_dir/XDock.app" "$release_dir/XDock-$version-macos-$arch.zip"
echo "Packaged application: $release_dir/XDock.app"
echo "Archive: $release_dir/XDock-$version-macos-$arch.zip"
