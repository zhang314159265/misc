I don't plan to build skia standalone for now. But use the built libraries inside the chromium build.

# Key APIs
- SkCanvas::translate(x, y): the effect is like moving the origin to (x, y)

# Questions Not Fully Resolved Yet
- what's the relationship between skia and OpenGL

# Reference
- [Official Site](https://skia.org)
- [Skia Fiddle](https://fiddle.skia.org): contains a lot of examples
- [API Reference and Overview](https://skia.org/docs/user/api/): Briefly talked about he importance of SkCanvas and an example of drawing a rectangle.
- [Official Documentation](https://skia.org/docs/): Contains tutorials for a few wonderful examples.
- [Codesearch - specialization of the chromium codesearch](https://source.chromium.org/chromium/chromium/src/+/main:third_party/skia/)
- [SkCanvas Overview](https://skia.org/docs/user/api/skcanvas_overview/)
- [SkCanvas Creation](https://skia.org/docs/user/api/skcanvas_creation/)
- [SkPath Overview](https://skia.org/docs/user/api/skpath_overview/)
- [coordinates](https://skia.org/docs/user/coordinates/)
- [CanvasKit - Skia + WebAssembly](https://skia.org/docs/user/modules/canvaskit/): there is one cool example of popping out particles.
- [PathKit - Geometry in the Browser](https://skia.org/docs/user/modules/pathkit/): contains examples of animating geometries.
- [Skottie - Lottie Animation Player](https://skia.org/docs/user/modules/skottie/): some really cool animations!
- [Partitles](https://skia.org/docs/user/modules/particles/)
