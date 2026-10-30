# Ray Tracer (WIP)

A CPU ray tracer written in C, featuring both a **scalar reference path** and an **SSE SIMD path**, with a screenshot-based regression test harness.

---

## Build Notes

The renderer can be compiled in different modes via preprocessor defines:

- **`SCALAR`** — uses the portable scalar implementation (reference path)
- **`SIMD`** — uses the SSE-vectorized implementation (`_mm_*` intrinsics, 4-wide batches)
- **`PROFILE`** — suppresses timing output and PPM export (useful when benchmarking from an external harness)

Pass `-d` as the first CLI arg to render the **depth-only** pass (flat sphere colors, no lighting) instead of the full lit scene.

Output is written to `out.ppm` (P3 ASCII PPM, 640×480 default).

---

## Implemented Features

### Geometry & Intersection

- **Ray–sphere intersection** using discriminant math (`intersectRaySphere`)
- **Closest-hit selection** across the sphere list (`intersectRaySphereClosest`) with `t_min` / `t_max` gating
- **Precomputed `r²`** on spheres (`_rr`) to avoid repeated squaring

### Lighting (Phong-style)

- **Ambient** lights
- **Point** lights (with distance-limited shadow rays via `t_max = 1`)
- **Directional** lights (with infinite-range shadow rays)
- **Diffuse shading** via normalized N·L
- **Specular highlights** via reflected-vector R·V raised to the sphere's specular exponent
- **Shadow casting** — occluded lights are skipped per-light in `ComputeLighting`
- Handles the **no-light case** by returning flat sphere color

### Recursive Reflections

- Configurable recursion depth (default 3 in the scene)
- Per-sphere `reflective` coefficient
- Early-out when `recursion_depth <= 0` or `reflective <= 0`
- Final color is a linear blend: `local·(1−r) + reflected·r`

### Camera & Rasterization

- Perspective camera with configurable position, direction, viewport size, and projection plane
- Right/up basis derived from camera direction and world-up `{0,1,0}`
- Region-based rendering (`top/bot/left/right` in centered coordinates)
- Normalized per-pixel ray directions

### Output

- **PPM image export** (P3 ASCII, `.ppm`) via `ppmbuffer_save`
- Multiple pixel layouts supported for output buffers: **RGB, BGR, RGBA, BGRA** (`PixelLayout` + `swizzle`)
- Intermediate frame buffer used during rendering, then swizzled into the target buffer

### Performance / SIMD (`SIMD` build)

- 4-lane SSE batched sphere intersection (`intersectRaySphereBatched`) using `_mm_*`
- Struct-of-arrays **`SphereBuffer`** packing (x, y, z, radius, r², color, specular, reflective)
- **`LightBuffer`** packing (ambient / point / directional split into padded SoA arrays)
- **`DirectionBuffer`** packed ray directions
- Batched `ComputeLightingBatch` with vectorized ambient reduction, diffuse, and specular terms
- Batched trace (`traceRayBatch`, `traceRayBatchDepth`)
- Depth pass in both scalar (`traceRayDepthPass`) and SIMD paths (`fillRegionDepth`)

### Timing

- Frame timing via `QueryPerformanceCounter` / `QueryPerformanceFrequency`
- Prints **elapsed seconds** and **FPS**

---

## Testing

A screenshot-based regression harness lives in `main.c` (the test entry point, distinct from the renderer's `main.c`).

- **`RenderTestContext`** holds generated + loaded PPM buffers, spheres, lights, and per-test metadata
- Tests render the default scene into an in-memory `Buffer`, then validate against a stored reference
- **Missing reference** → generated image is written to `tests/<defines>/<test_name>.ppm` and the test fails
- **Pixel diff** counted via `ppmbuffer_same_px`; tolerance is `TEST_TOLERANCE_PIXEL` (default 10 differing bytes)
- On failure, a **3-panel comparison image** is saved (`stored | diff | generated`) to `tests/failed/<defines>/<test_name>.ppm` — matching pixels are grayed out, differing pixels highlighted red
- `defines` directory segregates references between `default` and `SIMD` builds, so both paths can be tested against their own baselines

### Test Suites

**`test_suite`**
- `default_scene` — full scene, all lights
- `light_ambient` — ambient light only
- `light_point` — single point light
- `light_directional` — single directional light

**`depth_suite`**
- `depth` — depth-only pass (flat colors, no lighting)

### Reference Screenshot Layout

Reference screenshots and failure dumps are organized by build variant, so the scalar (default) and SIMD paths each keep their own baselines:

tests/default/ — reference screenshots for the scalar build, e.g. tests/default/default_scene.ppm

tests/SIMD/ — reference screenshots for the SIMD build, e.g. tests/SIMD/default_scene.ppm

tests/failed/default/ — 3-panel diff images written when a scalar test fails

tests/failed/SIMD/ — 3-panel diff images written when a SIMD test fails

On a missing reference, the freshly rendered image is written into tests/<variant>/<test_name>.ppm and the test fails — so the first run "seeds" the baseline. On a mismatch, the 3-panel comparison (stored | diff | generated) is written into tests/failed/<variant>/<test_name>.ppm.

---

## Scene

The default scene (`DEFAULT_SPHERES` / `DEFAULT_LIGHTS`) renders five spheres:

| Position | Radius | Color | Specular | Reflective |
|---|---|---|---|---|
| (0, −1, 3) | 1 | red | 500 | 0.2 |
| (0, 1, 3) | 1 | red | 500 | 0.2 |
| (2, 0, 4) | 1 | blue | 500 | 0.3 |
| (−2, 0, 4) | 1 | green | 10 | 0.4 |
| (0, −5001, 0) | 5000 | yellow | 1000 | 0.5 |

Lights: ambient (0.2), point at (2,1,0) (0.6), directional (1,4,4) (0.2).

---

**TL;DR:** Scalar and SSE reference ray tracer with ambient/point/directional lighting, shadows, Phong specular, recursive reflections, flexible pixel layouts, PPM export, FPS timing, and a screenshot-diffing regression harness — with an active SIMD port and a growing test suite.

