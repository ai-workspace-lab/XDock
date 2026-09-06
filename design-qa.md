# XDock visual QA — system-logo revision

final result: passed

## Latest revision — tighter spacing

User requested only closer logos, with everything else unchanged, implemented in the actual QML component. Icon pitch is 72 px instead of 80 px; slot size remains 52 px, reducing the slot gap from 28 px to 20 px. Left offset, vertical placement, panel, tray, fonts and icon resolver are unchanged. Overflow capacity now derives from the same spacing token.

The approved previous render is `docs/qa/spacing-before.png`; the new render is `docs/qa/classic-final.png`. Both are 1204 × 78 normalized native pixels. `docs/qa/spacing-comparison.png` contains both images plus equal 2× detail crops and was inspected together. Only horizontal application positions change; no actionable P0/P1/P2 drift. CMake build, eight interaction cases and diff whitespace checks pass. Changes are implemented in `qml/DockBar.qml`, not only a design image.

## Authoritative target

The user clarified: extract the reference Dock style; use system-default logos. The screenshot is now a geometry/surface reference only. Different application logos are required, not a fidelity defect. Both classic and system appearances use host icons. Screenshot artwork and reflections have been removed from executable resources.

## Evidence and normalization

- Source: `assets/reference/dock-strip.png`, 1204 × 78 pixels.
- Native implementation: `docs/qa/classic-final.png`, 1204 × 78 logical pixels.
- Combined full-view and 2× focused icon comparison: `docs/qa/comparison.png`; source and final render viewed together.
- Additional renders inspected: `docs/qa/system-final.png`, `docs/qa/narrow-final.png` (480 × 78).
- State: idle, no hover, clock 10:38, classic surface with system logos.
- Renderer: Qt Quick native Cocoa platform, software scene graph. Retina capture is normalized from 2408 × 156 to 1204 × 78 using smooth downsampling. Native platform capture is required for checking macOS bundle icons; offscreen Qt has no Cocoa icon service and displays generic standard icons.

## Revision findings and fixes

- [P1, fixed] The previous classic mode still used source screenshot logos. Replaced its atlas provider with `SystemIcons`; both themes now resolve host icons with no screenshot fallback. Removed screenshot reflections and atlas resources from the build.
- [P2, fixed] An initial sourceSize binding depended on control layout and produced a native binding loop. Raster request size is now independent of layout, with device-pixel-ratio scaling. Final native renders have no QML binding-loop warnings.
- [P2, fixed] Tinting a full application bundle logo in the tray produced a solid shape. Tray requests now use symbolic/theme icons, independently of application bundle lookup.
- [Expected] System artwork retains its built-in padding and original silhouette. It is not stretched or repainted to match the 2013 icons.

## Fidelity surfaces

- Typography: live 13 px clock and hover-only application labels retained; system mode follows Qt fonts. Browser label is generic “浏览器” so the macOS Safari preview is not mislabeled Firefox.
- Layout: original 78 px strip, 34 px lower surface, 52 px icon slots at y=18, first x=14 and 72 px pitch retained. Native icon padding is respected. Narrow layout retains the complete tray and overflow entry.
- Colors: classic translucent dark surface and preview-only amber backdrop retained. System surface follows platform palette; tray symbols follow a contrasting foreground.
- Assets: Linux desktop-entry Icon/theme lookup; macOS native application bundle icons; standard platform fallback if missing. No screenshot assets are linked into the executable. All application icons preserve their original color and aspect ratio.
- Content: no new idle Dock labels or decorative copy. Existing preview and unavailable-service disclosures retained.

## Validation and limits

CMake build and all eight Qt Quick interaction cases pass (pointer, keyboard, invalid index, narrow layout, theme, dynamic model, fish-eye and overflow routing). Native captures checked in both appearances and narrow mode. No remaining actionable P0/P1/P2 issue for this revised visual scope. The macOS IMK shutdown diagnostic can appear during capture; captures complete successfully.

Linux runtime launch/GTK theme/Wayland layer-shell integration remains pending live Linux validation. X11 is intentionally unsupported. This pass covers UI and host-icon behavior in the local macOS environment, not production Linux compatibility. Earlier native settings/overflow interaction verification remains applicable because those controls were preserved.

## Latest revision — horizontal fish-eye magnification

### Comparison target and evidence

- Source visual truth: `/var/folders/wx/dst2rbcx24jd3mnzwvb5w_9h0000gn/T/codex-clipboard-a2609000-dd91-400b-8536-a25af303a726.png` (246 × 1310 px), a vertical Dock hover state showing one dominant icon and distance-based neighboring enlargement.
- Implementation capture: `docs/qa/fisheye-hover.png` (1204 × 132 px), generated by the native app with `--capture --hover-index 4` at 1× logical density.
- Intended orientation: the source's vertical rail is animation reference only. The user explicitly retains XDock's default horizontal bottom layout, so orientation and window crop are intentionally not normalized.
- State: classic theme, hover centered on Music. The capture uses the same system-icon policy as the idle state and a fixed 10:38 clock.

### Findings

- No actionable P0/P1/P2 mismatch. The implementation preserves the source's dominant-center, progressively weaker-neighbor behavior while keeping the requested bottom-horizontal dock.
- Expected difference: the source has a persistent vertical rail; XDock expands a temporary transparent upper buffer from its normal 78 px footprint to 132 px only during hover. This prevents clipping without changing the resting layout or Linux edge reservation.

### Fidelity surfaces

- Typography and copy: no new visible idle text; existing labels remain unchanged.
- Spacing and layout rhythm: the resting 72 px icon pitch and 34 px surface are unchanged. Hover moves neighbors slightly outward to avoid visual collisions.
- Colors and visual tokens: classic translucent surface and native system-icon colors are retained.
- Image quality and asset fidelity: app icons continue to come from system/application sources; no screenshot assets were introduced.
- Interaction: center icon reaches 2.05× at full influence, with a cosine falloff over a 170 px horizontal influence radius. Its center stays under the pointer while neighboring centers make room from each adjacent pair's actual expanded width, preserving the visual gap through the whole wave. Icons, neighbor displacement and the transparent hover room use a 180 ms `OutCubic` response, avoiding spring overshoot and abrupt expansion/collapse. The native test confirms full center enlargement, distant icon neutrality, collision-free neighbor direction and reset on exit.

### Validation

`ctest --test-dir build-macos-release --output-on-failure` passes all eight cases, including fish-eye magnification. The installed macOS bundle and ZIP are re-signed and verified after each packaged build.

final result: passed
