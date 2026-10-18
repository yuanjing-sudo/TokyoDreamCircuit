# Making it look real

Your request was "stunning, realistic, like real life 3D and cinematic". Here is
the honest answer to what that actually takes, and what is already wired up.

## The uncomfortable truth first

Photorealism is **mostly assets and lighting, not code**. The engine features are
free and already enabled in `Config/DefaultEngine.ini`. What costs money and time
is high-poly, correctly-scanned geometry and materials. No amount of shader
sourcing makes an untextured box look like a Tokyo highway.

I cannot download or install any of the assets below. Every one is a large binary
package that requires an Epic account, a desktop machine, and in some cases a
purchase. What follows is the shopping list and the exact settings to apply.

## Already enabled in this project

These need no extra download — they ship with UE5 and are switched on in
`Config/DefaultEngine.ini`:

| Feature | What it gives you |
| --- | --- |
| **Lumen** (GI + reflections, hardware RT) | Bounces light off wet asphalt. This is the single biggest realism win. |
| **Nanite** | Renders film-grade meshes with no LOD popping. |
| **Virtual Shadow Maps** | Crisp contact shadows from every neon sign. |
| **Substrate** | Layered physically-based materials (clearcoat over metal flake over base). |
| **TSR** | Clean temporal anti-aliasing at 360 km/h. |
| **Volumetric Fog** | Light shafts through rain and under street lamps. |
| **Motion Blur** | The speed sensation. Tuned via `r.MotionBlur.Amount`. |
| **Sky Atmosphere** | Physically correct sky, aerial perspective, sun angle. |

## Required hardware

Lumen with hardware ray tracing and Nanite realistically wants:

- **GPU**: RTX 3060 / RX 6700 XT or better. RTX 4070+ for 1440p high refresh.
- **VRAM**: 8 GB minimum. Wet-road Lumen reflections are VRAM-hungry.
- **System RAM**: 32 GB comfortable, 16 GB will work for a small track.
- **Disk**: engine ~120 GB plus ~60 GB per large asset pack.

On a GTX 10-series card, turn off `r.Lumen.HardwareRayTracing` and
`r.RayTracing` and you get software Lumen — still good, roughly half the cost.

## Assets — the part that actually makes it look real

All links are to the official sources. I have not verified current availability
or pricing; check before relying on any of them.

### Free, and you should start here

| Resource | What it is | Where |
| --- | --- | --- |
| **Quixel Megascans** | Thousands of scanned, tiled, battle-tested surfaces. Free for UE. This is where your asphalt, concrete, kerbs and puddles come from. | Fab (fab.com), formerly Quixel Bridge |
| **Epic's City Sample** | The Matrix Awakens city: a full procedural Tokyo-like metropolis with Nanite buildings and a drivable simulation. The closest free thing to what you described. Also ships Epic's Houdini city-generation tooling — see `CITY_SAMPLE.md`. | Epic Games Launcher / Fab |
| **Electric Dreams Environment** | Epic's procedural nature/road sample demonstrating PCG, Substrate and Lumen together. | Epic Games Launcher > Samples |
| **Automotive Materials / Automotive Configurator** | Epic's car paint, glass, chrome and interior shaders, already Substrate-aware. | Epic Games Launcher > Samples |
| **Poly Haven** | CC0 HDRIs and textures. Useful for background skies. | polyhaven.com |
| **ambientCG** | CC0 PBR textures, including wet-road sets. | ambientcg.com |

### Paid, if the budget exists

| Resource | Why |
| --- | --- |
| **SpeedTree** | Foliage. Nothing free matches it for cherry blossom trees. |
| **Substance 3D Designer / Painter** | Authoring custom wet asphalt with tyre-worn tracks and puddles that Lumen reflections read correctly. |
| **Fab marketplace car models** | Rigged, Nanite-ready vehicles with LODs and interiors. |
| **Houdini** (Indie licence) | What your original pipeline described for road generation. The procedural highway in this project is a lightweight replacement for it. |

### Plugins already listed in the `.uproject`

Enabled but unused by the C++ so far — turn them on as you grow:

- **ChaosVehiclesPlugin** — if you later want a skeletal-mesh car with proper
  suspension instead of the single-rigid-body model in `TokyoDreamCar.cpp`.
- **PCG** (Procedural Content Generation) — scatter the city, trees and crowds
  across the circuit. Epic's Electric Dreams shows the pattern.
- **GeometryScripting** — runtime mesh editing and boolean track construction.
- **Niagara** — rain, tyre spray, sparks, exhaust heat haze.
- **MovieRenderPipeline** — offline cinematic output. This is how you get the
  "cinematic" look rather than a game screenshot.
- **MetaSounds** — engine note synthesised from RPM, which pairs with the
  telemetry functions already exposed on `ATokyoDreamCar`.
- **Water** — if you add Tokyo Bay or wet-surface simulation.

## The settings that matter most, in priority order

If you only do five things:

1. **Manual exposure.** Auto exposure is the single most common reason UE scenes
   look "gamey". Use the `PostProcessVolume`: `Exposure > Metering Mode = Manual`,
   `EV100 ≈ 13`. Your brightness stays locked and the image reads as photographic.
2. **Real light intensity units.** Set `DirectionalLight > Intensity Units = Lux`
   and use ~100000 for a sunny sky, or ~5–20 for night-time neon-only lighting.
   The default "unitless" intensity is physically meaningless and flattens
   everything.
3. **Nanite + Virtual Shadow Maps on every static mesh.** Free quality, no
   LOD authoring.
4. **Motion blur and film grain, sparingly.** `r.MotionBlur.Amount 0.35` and a low
   grain amount. This is what sells speed and hides Lumen noise.
5. **Movie Render Queue, not a screenshot.** `MovieRenderPipeline` with temporal
   sub-sampling (e.g. 8 spatial × 8 temporal samples) removes Lumen shimmer and
   TSR artifacts entirely for output video. Real-time play will always be noisier
   than an offline render.

## Driving the effects from the car

`ATokyoDreamCar` and `UChaseCameraComponent` already expose what a post-process
rig needs:

| Function | Use it for |
| --- | --- |
| `GetSpeed01()` | Motion blur amount, chromatic aberration, FOV, engine pitch |
| `GetSpeedKmh()` / `GetAbsoluteSpeedKmh()` | HUD, audio |
| `GetRearSlipAngleDeg()` | Tyre smoke and screech when drifting |
| `IsDrifting()` | Camera shake, particle burst |
| `GetBrakeInput()` | Brake lights |
| `UChaseCameraComponent::GetSpeed01()` | Convenient from the camera side |

## What this is not

To be blunt about the limits of what was delivered:

- It is **not compiled**. No UE install exists on the device that wrote it.
- It is **not a game** — no menus, no lap timing, no AI opponents, no sound.
- It is **not photorealistic on its own**. It is the skeleton and the settings;
  the realism comes from the Megascans textures, the City Sample buildings and
  the lighting discipline described above.
- The car's tyre model is an approximation, not a simulation. See the README.

Closing that gap is desktop work in the editor. This project exists so that work
can start immediately rather than from an empty screen.
