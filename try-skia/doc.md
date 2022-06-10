I don't plan to build skia standalone for now. But use the built libraries inside the chromium build.

# Key APIs
- SkCanvas::translate(x, y): the effect is like moving the origin to (x, y)

# Reference
- [Official Site](https://skia.org)
- [Skia Fiddle](https://fiddle.skia.org): contains a lot of examples
- [API Reference and Overview](https://skia.org/docs/user/api/): Briefly talked about he importance of SkCanvas and an example of drawing a rectangle.
- [Official Documentation](https://skia.org/docs/): Contains tutorials for a few wonderful examples.
- [Codesearch - specialization of the chromium codesearch](https://source.chromium.org/chromium/chromium/src/+/main:third_party/skia/)

# Scratch - don't read
- [SkCanvas Creation](https://skia.org/docs/user/api/skcanvas_creation/) [TODO HERE]
- [SkCanvas Overview](https://skia.org/docs/user/api/skcanvas_overview/)
- [SkPath Overview](https://skia.org/docs/user/api/skpath_overview/)

- So cool skia examples: https://skia.org/docs/user/modules/canvaskit/
- geometric examples: https://skia.org/docs/user/modules/pathkit/
- I really like the animations here: https://skia.org/docs/user/modules/skottie/
- partitles: https://skia.org/docs/user/modules/particles/

- [coordinates](https://skia.org/docs/user/coordinates/)

## TODO
  - what's the relationship between skia and OpenGL
  - next steps: try more tutorials and backends for skia
