# Tokyo Dream Circuit

Procedural highway racing, built for Unreal Engine 5.

- **Procedural highway** — road mesh generated along a spline, with collision and
  adjustable width, elevation, curvature, banking, cross-slope and kerbs.
- **Physics-driven car** — rigid body with a simplified bicycle-model tyre solver,
  throttle / steering / braking, aero downforce, and grip that fades on a wet road.
- **Cinematic rendering** — Lumen GI and reflections, Nanite, Virtual Shadow Maps,
  Substrate materials, TSR, volumetric fog, motion blur.

---

## Important: this project cannot be built on Android

This code was written on an Android phone and **has never been compiled**. There is
no Unreal Editor for Android, so nothing here has been run, rendered or tested.

Treat it as a reviewed starting point, not a verified build. Expect to fix small
API mismatches on first compile — that is normal for hand-written UE C++.

## Building it

You need a desktop machine with:

- Unreal Engine **5.5** (install via the Epic Games Launcher)
- A C++ toolchain: Visual Studio 2022 with "Game development with C++" on Windows,
  or clang + `build-essential` on Linux
- ~40 GB free disk for the engine's derived data on top of the project

Then:

1. Open `TokyoDreamCircuit.uproject` in the Epic Launcher's Unreal Engine 5.5.
2. It will say "missing modules, would you like to rebuild?" — choose **Yes**.
3. The editor opens with an empty level. Proceed to *Setting up a scene* below.

If you are on a different engine version, edit `EngineAssociation` in
`TokyoDreamCircuit.uproject`, change `IncludeOrderVersion` in the two `.Target.cs`
files, and change `BuildSettingsVersion` to match. Version drift in UE C++ is real,
so expect a few fixes.

## Setting up a scene

The project ships C++ only — no levels or assets, because those are binary files
that cannot be authored outside the editor.

1. **Create a level** (`File > New Level > Empty`), save it as
   `/Game/Maps/TokyoCircuit`.
2. **Add the road**: place a `ProceduralHighway` actor, then in the details panel
   press **Build Default Circuit**. Set `Circuit Radius` to ~60000 (600 m),
   `Curvature` to ~0.28, and press **Rebuild Road**. The mesh appears immediately.
3. **Add the car**: place a `TokyoDreamCar`, set its rotation, and make sure it
   spawns slightly above the road. It auto-possesses player 0, so it is drivable
   the moment you press Play.
4. **Assign meshes**: the car falls back to engine primitives. Point `Body` and the
   four `Wheels` at real static meshes for the actual look.
5. **Lights**: add a `DirectionalLight` (set `Intensity Units` to *Lux*, ~100000,
   pitch around -35°), a `SkyAtmosphere`, `VolumetricCloud`, `ExponentialHeightFog`
   with `Volumetric Fog` ticked, and a `SkyLight` set to `Real Time Capture`.
6. **Post process**: add a `PostProcessVolume`, tick `Infinite Extent (Unbound)`,
   enable Bloom, Motion Blur and Film Grain, and set the exposure manually
   (`Metering Mode > Manual`, EV100 around 13) so brightness never pumps.

## Controls

| Key | Action |
| --- | --- |
| `W` / `Up` / right trigger | Throttle |
| `S` / `Down` / left trigger | Brake |
| `A D` / arrows / left stick | Steer |
| `Space` / gamepad A | Handbrake |

Negative throttle reverses.

## Tuning the car

Everything is in the details panel under `Car|*`, and every number is in SI units
(metres, m/s, newtons, kg) — the conversion to Unreal's centimetres happens in one
place, `ATokyoDreamCar::ApplyForceAtLocationSI`. Useful starting points:

| What you want | Knob |
| --- | --- |
| Faster | `Max Engine Force`, `Max Speed Kmh` |
| Grippier | `Dry Friction Coefficient`, `Downforce Coefficient` |
| More tail-happy (oversteer) | lower `Rear Cornering Stiffness`, or raise `Throttle Grip Loss` |
| More stable (understeer) | keep `Rear Cornering Stiffness` above `Front Cornering Stiffness` |
| Wet handling | raise `Wetness` (0 dry → 1 soaked) |
| Steering the wrong way | flip `Lateral Force Sign` to `-1` |

`Wetness` is the interesting one: it blends dry and wet friction, so you can drive
it from a weather system and watch the car's cornering speed fall in real time.

## If it steers like a drift car, that is intentional

The tyre model is a single-track (bicycle) approximation: two slip angles, two
cornering forces, clamped by a friction circle built from weight plus downforce.
It is not a four-wheel Pacejka model and it does not simulate suspension, load
transfer, or tyre temperatures. It is accurate enough to feel like a fast car on
a wet road, and it costs one rigid body per vehicle.

## Files

```
TokyoDreamCircuit.uproject          project + plugin list
Source/
  TokyoDreamCircuit.Target.cs       game build target
  TokyoDreamCircuitEditor.Target.cs editor build target
  TokyoDreamCircuit/
    TokyoDreamCircuit.Build.cs      module dependencies
    ProceduralHighway.h/.cpp        spline road mesh + collision + banking
    TokyoDreamCar.h/.cpp            rigid-body car + wet-grip tyre model
    ChaseCameraComponent.h/.cpp     spring-arm chase camera
Config/
  DefaultEngine.ini                 Lumen / Nanite / VSM / Substrate / TSR
  DefaultInput.ini                  throttle, brake, steer, handbrake
  DefaultGame.ini                   project identity
```

See `REALISM.md` for the assets, plugins and settings that turn this into a
photorealistic-looking game, and `CITY_SAMPLE.md` for the verified Epic City
Sample + Houdini setup procedure (and the corrections to the download list).
