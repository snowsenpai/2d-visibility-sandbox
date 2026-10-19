# 2D Visibility Sandbox

A C++17 / SFML 3 learning sandbox for exploring **2D ray casting, line-segment intersections, visibility approximations, and eventually visibility polygons and field-of-view systems**.

**Current milestone:** Lab A — single-ray intersection and uniform 360° ray casting, with a live debug visualization. The current fan is an **approximation**, not an exact visibility polygon.

## Purpose

This project isolates one game-programming system so its geometry, implementation trade-offs, and debugging techniques can be learned and reused in future SFML games, engine experiments, and other game projects.

Goals:
- Derive and implement ray–segment intersection instead of treating it as a black box.
- Keep reusable geometry separate from SFML rendering and game input.
- Visualize intersection behavior and identify edge cases.
- Validate geometry independently of the game loop.
- Establish a baseline before moving to endpoint-directed rays, angular sweeps, or optimized field-of-view (FoV) queries.

## Technology and project structure

- **Language:** C++17
- **Rendering / input:** SFML 3
- **Build / IDE:** CMake, Visual Studio
- **Game architecture:** existing `Game`, `EntityManager`, `Entity`, and component-based player
- **Tests:** `GeometryTests.cpp` with assertions invoked by `runGeometryTests()`

Relevant translation units:

| File | Responsibility |
| --- | --- |
| `Geometry.h` | Geometry data types and public function declarations |
| `Geometry.cpp` | Intersection, closest-hit search, and uniform-ray sampling |
| `GeometryTests.h/.cpp` | Deterministic geometry checks |
| `Game.h/.cpp` | Scene setup, player/input integration, raycast updates, and debug drawing |
| `MathUtils.h` | Mathematical constants, including `TwoPi` |

This describes the files discussed during implementation; it is not a verified complete repository tree.

## Current features

- Movable player whose position acts as the visibility origin.
- Mouse-directed test ray, with a fallback direction when the mouse overlaps the origin.
- Finite wall segments: a rectangular scene boundary and interior horizontal, vertical, diagonal, and short walls.
- Nearest-hit selection with a configurable maximum ray distance.
- Uniform 360° sampling with selectable ray counts: **16, 32, 64, 128, 256, 512** (default: 128).
- Debug display of obstacle segments, a yellow mouse ray, a red hit marker, optional uniform rays, and a translucent triangle fan.
- Assertion-based geometry tests for intersections and edge cases.

### Controls

| Input | Behavior |
| --- | --- |
| `W`, `A`, `S`, `D` | Move player |
| Mouse position | Aim the single debug ray |
| `R` | Toggle uniform debug rays |
| `F` | Toggle triangle fan |
| `Up` / `Down` | Increase / decrease ray count |
| `C` | Toggle player collision debug shapes |
| Window close | Exit |

## Data flow and responsibilities

```text
Game::update()
  ├─ sPlayerInput()           read keyboard / window events
  ├─ sMovement()              update player position
  ├─ sCollision()             constrain player to window bounds
  ├─ sRaycast()
  │    ├─ read player position as ray origin
  │    ├─ map mouse pixel position to world coordinates
  │    ├─ castRay(...)         nearest intersection for mouse ray
  │    └─ castUniformRays(...) sample visibility endpoints
  │         └─ castRay(...)    nearest wall for each direction
  │              └─ intersectRaySegment(...)
  └─ sRender()
       ├─ drawVisibilityDebug()  fan, rays, walls, hit marker
       └─ draw entities          player appears above debug geometry
```

`Geometry.cpp` is responsible for geometry, not input or rendering. `Game.cpp` owns scene state and converts geometry results into SFML primitives. This separation makes geometry testable without a window and permits future rendering changes without rewriting intersection mathematics.

## Mathematical foundation

### Geometry types

- `Ray { origin, direction }`: direction is expected to be normalized.
- `Segment { start, end }`: finite obstacle edge.
- `RayHit { point, distance, segmentIndex }`: hit location, ray distance, and index of the nearest blocking segment.

Represent a ray and segment parametrically:

```text
Ray:      P(t) = O + tD,       t >= 0
Segment:  Q(u) = A + u(B-A),   0 <= u <= 1
```

Let `E = B-A` and `AO = A-O`. Solving `O + tD = A + uE` using the scalar 2D cross product gives:

```text
cross(a,b) = a.x*b.y - a.y*b.x

denom = cross(D,E)
t = cross(AO,E) / denom
u = cross(AO,D) / denom
```

If `abs(denom) < 1e-6`, the current implementation rejects the parallel or collinear case. Otherwise, reject `t < 0`, `u < 0`, or `u > 1`. The accepted hit is `O + tD`.

Because `D` is normalized, **`t` is a distance in world units**. The current policy accepts `t == 0` and segment endpoints (`u == 0` or `u == 1`).

### Nearest-hit selection

`castRay()` checks each segment and retains the valid hit with the smallest `distance`, provided that distance is at most `maxDistance`. `segmentIndex` is assigned by `castRay()`, not by the primitive intersection function. If no valid hit is found, the function returns `std::nullopt`.

### Uniform sampling

For `N` rays, sample angles in increasing order:

```text
angle_i = i * (2π/N), i = 0, ..., N-1
D_i = (cos(angle_i), sin(angle_i))
```

For each direction, use the nearest wall hit or the fallback endpoint `origin + direction * maxDistance`. The result is an ordered list of `N` points. The renderer draws each debug ray and builds a `TriangleFan` consisting of the origin, each endpoint, and the first endpoint repeated to close the loop.

In SFML's default screen coordinate system, positive Y points downward, so increasing angles appear clockwise on screen.

## Engineering decisions and trade-offs

| Decision | Reason | Consequence / follow-up |
| --- | --- | --- |
| Normalize ray direction | Keep `t` equal to geometric distance | Zero-length direction must be guarded before normalization |
| Return `std::optional<RayHit>` | Express a hit or no hit explicitly | Callers handle misses and max-distance fallbacks |
| Use finite segments | Model actual wall edges | Reject intersections outside `[0,1]` |
| Accept ray origin and endpoints as hits | Define deterministic boundary behavior | Self-hits can occur when the origin lies on a wall |
| Reject near-parallel / collinear cases | Keep initial intersection code simple | Overlapping geometry is not handled robustly |
| Scan all segments | Simple, inspectable baseline | Linear work per ray; no spatial acceleration |
| Use uniform angles | Easy to implement and debug | Narrow walls/corners can be missed |
| Keep geometry separate from `Game` | Reuse and unit-test mathematics | Rendering remains a distinct integration layer |
| Draw a triangle fan | Quick visualization of sampled visibility | It is not a proof of an exact or valid visibility polygon |

**Complexity:** with `R` rays and `S` wall segments, uniform casting costs **O(R × S)** intersection tests per update; storing the endpoint list costs **O(R)**. The single mouse ray adds **O(S)** work. This ignores rendering overhead and any future acceleration structure.

## Tests and verification

`runGeometryTests()` checks the core intersection behavior. Covered or planned test cases from the current implementation workflow:

1. Horizontal ray intersects a vertical wall at the expected point and distance.
2. Ray pointing away from a wall misses.
3. The nearer of two walls wins, with the correct segment index.
4. Diagonal ray meets a horizontal wall at approximately `(4,4)` with distance `sqrt(32)`.
5. Infinite ray-line intersection outside the finite segment is rejected.
6. Parallel ray misses.
7. Ray beginning on a segment reports distance zero under the chosen policy.
8. Exact segment-endpoint hit is accepted.
9. Collinear ray and segment return no hit under the current policy.
10. Wall beyond `maxDistance` is rejected.

Suggested additional checks: `castUniformRays()` returns exactly `rayCount` points; four empty-scene rays reach the cardinal endpoints; one wall clips only the appropriate rays; an empty segment list is handled; and a ray-count change preserves angular ordering.

Run tests in a **Debug** configuration because standard `assert` is normally disabled when `NDEBUG` is defined. A future improvement is to create a standalone CMake test executable and register it with CTest rather than running tests during game initialization.

## How this relates to the reading material

### 1. Red Blob Games — *2D Visibility*

https://www.redblobgames.com/articles/visibility/

**Implemented:** the introductory idea of emitting rays from an observer, intersecting them with walls, selecting the nearest wall, and drawing the resulting visible region.

**Not yet implemented:** rays directed at wall endpoints, angular sweep / active-wall tracking, and a robust visibility polygon. The article explicitly progresses beyond evenly spaced sampling because a fixed angular grid can miss critical changes in visibility. Its sweep approach is a **future alternative**, not something this sandbox currently performs.

### 2. Amit Patel — *2d Visibility* development notes

https://simblob.blogspot.com/2012/07/2d-visibility.html

This companion post documents the iterative relationship between demos and explanations, and notes issues with segment ordering, intersections, and corner cases. It motivates keeping the sandbox interactive and retaining regression tests as the algorithm evolves. It is background context, not a separate algorithm implemented here.

### 3. Nicky Case — *Sight & Light*

https://ncase.me/sight-and-light/

An interactive learning reference for rays, wall intersections, corners, and visibility regions. Use it as a visual comparison when implementing endpoint-directed rays and slight angular offsets around corners. The current implementation covers the ray/segment foundation only.

### 4. Sundaram Ramaswamy — *Field of View and Line of Sight in 2D*

https://legends2k.github.io/2d-fov/

https://legends2k.github.io/2d-fov/design.html

This treatment begins with regular radial sampling, then discusses endpoint-angle rays, corner handling, **bounded FoV** (direction, angle, and distance), and broad-/narrow-phase pruning of irrelevant edges. It maps naturally to future experiments in flashlight cones, enemy vision, and line-of-sight queries. The current sandbox supports a distance bound, but **not yet a viewing-angle bound or spatial culling**.

### 5. Google Scholar research links

- https://scholar.google.com/scholar?cluster=7815126113820798295
- https://scholar.google.com/scholar?cluster=11179288360296920832

These are preserved as research pointers. The specific publications behind the cluster IDs have **not been verified**, so this README does not assign them paper titles, algorithms, or conclusions. Identify the papers and record full bibliographic details before making implementation claims about them.

## Current limitations

- Uniform angular samples can miss thin obstacles, corners, and sharp visibility transitions; increasing `rayCount` reduces sampling gaps but does not guarantee exactness.
- A triangle fan joining sampled hit points may cover occluded regions between rays. Treat it as a debug approximation.
- Collinear overlap and degenerate (zero-length) segments have no robust special handling.
- The fixed absolute parallel epsilon is sensitive to coordinate scale and segment length.
- Floating-point behavior near endpoints, overlapping walls, and intersecting segments needs additional tests.
- The scene's boundary walls are generated from initial window dimensions; resizing the window updates the view, not the world geometry.
- The player collides with window boundaries but does not resolve collisions against interior wall segments.
- All rays test all segments; there is no broad-phase spatial acceleration.
- The current origin is the player's center; its collision shape may overlap obstacles.
- Rendering and tests are currently integrated into the existing game application rather than separate executable targets.

## Roadmap / proposed experiments

### Lab B — Endpoint-directed ray casting

- Gather wall endpoints and compute each endpoint angle with `atan2`.
- Cast rays at `angle - epsilon`, `angle`, and `angle + epsilon` to explore visibility just past corners.
- Sort hits by angle and render the resulting region.
- Handle the wraparound at `-π` / `π`, duplicate angles, and rays at coincident endpoints.
- Compare the result against Lab A on the short-wall scene and on deliberately tricky corner configurations.
- **Checkpoint:** demonstrate a case uniform sampling misses but endpoint sampling captures.

### Lab C — Robustness and geometry tests

- Add overlapping, crossing, collinear, zero-length, and near-parallel segment fixtures.
- Define consistent rules for rays starting on walls and hits at shared vertices.
- Evaluate scale-aware tolerances or more robust predicates.
- Add independent tests for endpoint ordering and fan construction.
- **Checkpoint:** document which cases are handled, deliberately unsupported, or numerically ambiguous.

### Lab D — Bounded field of view and line of sight

- Add facing direction, field-of-view angle, and radius.
- Handle sector boundary rays and obstacles intersecting the visibility radius.
- Add `isVisible(observer, target, walls)` as a query independent of rendering.
- Explore uses in AI detection, stealth, flashlight cones, and fog of war.
- **Checkpoint:** test targets in front, behind, beyond range, and behind a blocking segment.

### Lab E — Profiling and acceleration

- Instrument ray counts, segment counts, intersection tests, and update time.
- Compare uniform and endpoint-based methods on the same scene.
- Explore spatial grids / hashes, quadtrees, or other broad-phase filters only after profiling.
- Study angular sweeping as a distinct algorithm with an active set of walls.
- **Checkpoint:** preserve correctness fixtures while comparing performance.

### Lab F — Rendering and integration

- Separate debug rays from the final visibility mesh.
- Experiment with transparent overlays, gradients, masks, multiple light sources, and compositing.
- Evaluate integration into a reusable visibility subsystem or game-engine module.
- **Checkpoint:** geometry remains usable without SFML rendering dependencies beyond the current vector type.

## Engineering workflow for future changes

For each new mechanic:

1. **Problem statement:** define the behavior and a minimal scene that demonstrates it.
2. **Math and invariants:** derive the necessary equations and boundary rules.
3. **API design:** decide which data belongs in geometry, game state, and rendering.
4. **Tests first or alongside:** include success, miss, boundary, and degenerate cases.
5. **Small implementation checkpoint:** compile and verify before adding another feature.
6. **Visualization:** draw origins, rays, endpoints, hit points, and blockers.
7. **Comparison:** record how the result differs from the previous algorithm and from reference demos.
8. **Retrospective:** note limitations, design changes, complexity, and next experiments.

### Implementation log template

```text
Date:
Milestone:
Problem / hypothesis:
Reference and relevant section:
Geometry / algorithm change:
Game / rendering change:
Tests added:
Observed result:
Edge cases / known limitations:
Performance notes (if measured):
Next checkpoint:
```

## Definition of done for Lab A

- [x] Ray/segment intersection and nearest-hit functions implemented.
- [x] Uniform rays produce ordered endpoints.
- [x] Player position and mouse direction connected to ray casting.
- [x] Walls, single-ray hit marker, uniform rays, and fan rendered in SFML.
- [x] Ray-count and debug-display controls integrated.
- [x] Geometry assertion tests included.
- [x] First visual integration checked manually.
- [ ] Standalone CTest target (optional improvement).
- [ ] Uniform-ray-specific automated tests (recommended follow-up).
- [ ] Benchmarks / performance measurements (not yet recorded).

## References

- Amit Patel, *2D Visibility*, Red Blob Games: https://www.redblobgames.com/articles/visibility/
- Amit Patel, *2d Visibility* (development notes): https://simblob.blogspot.com/2012/07/2d-visibility.html
- Nicky Case, *Sight & Light*: https://ncase.me/sight-and-light/
- Sundaram Ramaswamy, *Field of View and Line of Sight in 2D*: https://legends2k.github.io/2d-fov/design.html
- Research pointer A: https://scholar.google.com/scholar?cluster=7815126113820798295
- Research pointer B: https://scholar.google.com/scholar?cluster=11179288360296920832

---

**Project status:** Lab A functional; later labs are proposals, not implemented features. Update this document when behavior, architecture, test coverage, or limitations change.
