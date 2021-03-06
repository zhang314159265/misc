#include <iostream>
#include <unordered_map>
#include "SkCanvas.h"
#include "SkStream.h"
#include "SkPath.h"
#include "SkPDFDocument.h"
#include "SkRRect.h"
#include "SkTextBlob.h"
#include "SkSurface.h"
#include <gflags/gflags.h>
#include <fmt/format.h>
#include <cstdlib>

using namespace std;

DEFINE_string(draw_func, "draw_line", "Name of the draw function");
DEFINE_string(backend, "pdf", "Name of the skia backend to create SkCanvas");
// assumes height is the same as width
// TODO: this parameter may only work when drawing conic using the raster backend
// so far since I have not tested other case.
// For drawing conic, setting this to 4096 would be a good choice.
DEFINE_int32(width, 256, "Width of the canvas");

// hack
void SkDebugf(const char* fmt, ...) {
  assert(false && "skDebugf");
}

using DrawFuncT = std::function<void(SkCanvas*)>;
class DrawFuncCollection {
 public:
  static void drawLine(SkCanvas* canvas) {
    SkPaint p;
    p.setColor(SK_ColorYELLOW);
    p.setAntiAlias(true);
    p.setStyle(SkPaint::kStroke_Style);
    p.setStrokeWidth(10);
    canvas->drawLine(20, 20, 100, 100, p);
  }

  static void drawRotatedRect(SkCanvas* canvas) {
    canvas->save();
    canvas->translate(SkIntToScalar(128), SkIntToScalar(128));
    canvas->rotate(SkIntToScalar(45));
    SkRect rect = SkRect::MakeXYWH(-90.5f, -90.5f, 181.0f, 181.0f);
    SkPaint paint;
    paint.setColor(SK_ColorGREEN);
    canvas->drawRect(rect, paint);
    canvas->restore();
  }

  static void drawStar(SkCanvas* canvas) {
    const SkScalar scale = 256.0f;
    const SkScalar R = 0.45f * scale;
    const SkScalar twopi = 3.14159265f * 2;
    SkPath path;
    path.moveTo(R, 0.0f);
    int nangle = 5;
    for (int i = 1; i < nangle; ++i) {
      SkScalar theta = 2 * i * twopi / nangle;
      path.lineTo(R * cos(theta), R * sin(theta));
    }
    path.close();
    SkPaint p;
    p.setColor(SK_ColorBLUE);
    canvas->clear(SK_ColorWHITE);
    canvas->translate(0.5f * scale, 0.5f * scale);
    canvas->drawPath(path, p);
  }

  // example to draw geometrics and texts.
  static void drawGeoAndText(SkCanvas* canvas) {
    canvas->drawColor(SK_ColorTRANSPARENT);
    SkPaint paint;
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(4);
    paint.setColor(SK_ColorRED);

    SkRect rect = SkRect::MakeXYWH(50, 50, 40, 60);
    canvas->drawRect(rect, paint);

    // we can use SkRRect to draw ovals
    SkRRect oval;
    oval.setOval(rect);
    oval.offset(40, 60);
    paint.setColor(SK_ColorBLUE);
    canvas->drawRRect(oval, paint);

    paint.setColor(SK_ColorCYAN);
    canvas->drawCircle(180, 50, 25, paint);

    rect.offset(80, 0);
    paint.setColor(SK_ColorYELLOW);
    canvas->drawRoundRect(rect, 10, 10, paint);

    SkPath path;
    path.cubicTo(768, 0, -512, 256, 256, 256);
    paint.setColor(SK_ColorGREEN);
    canvas->drawPath(path, paint);

    // the text may not be visible using imgcat since the background of the canvas
    // is transparent and both the background of the iterm and the color of the text
    // are black.
    auto font = SkFont(nullptr, 18);
    auto text = SkTextBlob::MakeFromString("Hello, Skia!", font);
    canvas->drawTextBlob(text.get(), 50, 25, SkPaint());
  }

  /*
   * This one works well on raster backend but does not work as expected in the
   # pdf backend.
   */
  static void drawPixels(SkCanvas* canvas) {
    int width = 256, height = 256;
    SkPaint paints[2];
    paints[0].setColor(SK_ColorRED);
    paints[1].setColor(SK_ColorBLACK);
    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
        canvas->drawPoint(x, y, paints[x > width / 2]);
      }
    }
  }

  /*
   * Draw a collection of parabola, ellipse and hyperbola.
   */
  static void drawConic(SkCanvas* canvas) {
    canvas->drawColor(SK_ColorWHITE);
    int side = FLAGS_width;
    canvas->translate(side / 2, side / 2);

    SkPaint paint;
    // paint.setStrokeWidth(2);
    paint.setStrokeWidth(4);
    paint.setColor(SK_ColorBLACK);

    // set focus on (0, 0)
    canvas->drawPoint(0, 0, paint);
    // set directrix at x = 1
    canvas->drawLine(side / 4, -side / 2, side / 4, side / 2, paint);

    // draw circle manually
    // TODO: such a dumb implementation :(
    auto circle_helper = [&](int r) {
      for (int x = -r; x <= r; ++x) {
        int y = (int) sqrt(r * r - x * x);
        canvas->drawPoint(x, y, paint);
        canvas->drawPoint(x, -y, paint);
      }
    };

    circle_helper(side / 4);

    // parabola fomula
    // x = a * y * y + b
    auto parabola_helper = [&](double a, double b) {
      for (int x = -side / 2; x <= b; ++x) {
        int y = sqrt((x - b) / a);
        if (y < side / 2) {
          canvas->drawPoint(x, y, paint);
          canvas->drawPoint(x, -y, paint);
        }
      }
    };
    // x = -2/side y*y + side / 8
    paint.setColor(SK_ColorYELLOW);
    parabola_helper(-2.0 / side, side / 8.0);

    // ellipse
    paint.setColor(SK_ColorRED);
    auto ellipse_helper = [&](double a, double b, double x_shift) {
      for (int x = -a; x <= a; ++x) {
        int realx = x + x_shift;
        int y = sqrt(1 - x * x / (a * a)) * b;
        if (abs(realx) < side / 2 && y < side / 2) {
          canvas->drawPoint(realx, y, paint);
          canvas->drawPoint(realx, -y, paint);
        }
      }
    };
    int distFocusDirect = side / 4;
    /*
     * for ellipse with formula: x*x / a*a + y*y / b*b = 1
     * e = c / a
     * right focus: (c, 0)
     * right directrix: x = a * a / c
     * distFocusDirect = a * a / c - c = b * b / c
     *   = a * a / (a * e) - a * e = a / e - a * e = (1 - e*e) / e * a
     * 
     */
    for (double e = 0.1; e < 1; e += 0.2) {
      double a = e * distFocusDirect / (1.0 - e * e);
      double c = e * a;
      double b = sqrt(a * a - c * c);
      // focus move from (c, 0) to (0, 0)
      double x_shift = -c;
      ellipse_helper(a, b, x_shift);
    }

    // hyperbola
    paint.setColor(SK_ColorBLUE);
    auto hyperbola_helper = [&](double a, double b, double x_shift) {
      for (int x = a; x <= side * 2; ++x) {
        int realx1 = x + x_shift;
        int realx2 = -x + x_shift;
        if (abs(realx1) >= side / 2 && abs(realx2) >= side / 2) {
          continue;
        }
        double y = sqrt(x * x / (a * a) - 1) * b;
        if (abs(y) >= side / 2) {
          continue;
        }
        canvas->drawPoint(realx1, y, paint);
        canvas->drawPoint(realx1, -y, paint);
        canvas->drawPoint(realx2, y, paint);
        canvas->drawPoint(realx2, -y, paint);
      }
    };
    /*
     * for hyperbola with formula: x*x / a*a - y*y / b*b = 1
     * e = c / a
     * left focus: (-c, 0)
     * left directrix: x = -a * a / c
     *
     * distFocusDirect = c - a * a / c = ae - a/e = (e*e - 1) / e * a
     */
    for (double e = 1.5; e < 10; e += 3) {
      double a = e * distFocusDirect / (e * e - 1);
      double c = e * a;
      double b = sqrt(c * c - a * a);
      // focus move from (-c, 0) to (0, 0)
      double x_shift = c;
      hyperbola_helper(a, b, x_shift);
    }
  }
};

unordered_map<std::string, DrawFuncT> draw_func_dict = {
  {"line", DrawFuncCollection::drawLine},
  {"rotated_rect", DrawFuncCollection::drawRotatedRect},
  {"star", DrawFuncCollection::drawStar},
  {"geo_and_text", DrawFuncCollection::drawGeoAndText},
  {"pixels", DrawFuncCollection::drawPixels},
  {"conic", DrawFuncCollection::drawConic},
  {"", [](SkCanvas*) {}} // sentinel
};

DrawFuncT findDrawFunc(const string& func_name) {
  auto itr = draw_func_dict.find(func_name);
  if (itr == draw_func_dict.end()) {
    throw runtime_error(fmt::format("Failed to find draw function with name {}", func_name));
  }
  return itr->second;
}

class Backend {
 public:
  explicit Backend(std::string path) : path_(path) { }
  // create canvas. May need to store some state in the backend object so they
  // don't get released too early
  virtual SkCanvas* createCanvas() = 0;
  // flush the image to the path
  virtual void flush() = 0;
  // display the image from the path
  virtual void display() {
    displayImg(path_);
  }
  virtual ~Backend() = default;

  static void displayImg(const std::string& path) {
    auto cmd = fmt::format("imgcat {}", path);
    auto status = system(cmd.c_str());
    if (status != 0) {
      throw runtime_error(fmt::format("Fail to run command: {}", cmd));
    }
  }
 protected:
  std::string path_;
};

class PdfBackend : public Backend {
 public:
  PdfBackend() : Backend("/tmp/skia.pdf"), pdfStream_(make_unique<SkFILEWStream>(path_.c_str())) {
  }

  SkCanvas* createCanvas() override {
    int width = 256, height = 256;
    pdfDoc_ = SkPDF::MakeDocument(pdfStream_.get());
    SkCanvas* pdfCanvas = pdfDoc_->beginPage(SkIntToScalar(width), SkIntToScalar(height));
    return pdfCanvas;
  }

  void flush() override {
    pdfDoc_->close();
    pdfDoc_.reset();

    pdfStream_.reset();
  }
 private:
  unique_ptr<SkFILEWStream> pdfStream_;
  sk_sp<SkDocument> pdfDoc_;
};

class RasterBackend : public Backend {
 public:
  explicit RasterBackend() : Backend("/tmp/skia.png") { }
  SkCanvas* createCanvas() override {
    int width = FLAGS_width, height = FLAGS_width;
    rasterSurface_ = SkSurface::MakeRasterN32Premul(width, height);
    SkCanvas* rasterCanvas = rasterSurface_->getCanvas();
    return rasterCanvas;
  }

  void flush() override {
    sk_sp<SkImage> img(rasterSurface_->makeImageSnapshot());
    assert(img);
    sk_sp<SkData> png(img->encodeToData());
    assert(png);
    SkFILEWStream out(path_.c_str());
    out.write(png->data(), png->size());
  }
 private:
  sk_sp<SkSurface> rasterSurface_;
};

#if 0 // the example does not work
/*
 * Pass in the storage from outside to create the surface.
 */
class RasterDirectBackend : public Backend {
 public:
  explicit RasterDirectBackend() : Backend("/tmp/skia.png") { }
  SkCanvas* createCanvas() override {
    int width = 256, height = 256;
    SkImageInfo info = SkImageInfo::MakeN32Premul(width, height);
    size_t rowBytes = info.minRowBytes();
    #if 0
    // the API is deleted already
    size_t size = info.getSafeSize(rowBytes);
    #else
    // just do some safe estimate myself
    size_t size = rowBytes * (height + 10);
    #endif
    pixelMemory_ = std::vector<char>(size); // allocate memory
    surface_ = SkSurface::MakeRasterDirect(info, &pixelMemory_[0], rowBytes);
    return surface_->getCanvas();
  }

  void flush() override {
    SkFILEWStream out(path_.c_str());
    out.write(pixelMemory_.data(), pixelMemory_.size());
  }
 private:
  std::vector<char> pixelMemory_;
  sk_sp<SkSurface> surface_;
};
#endif

unique_ptr<Backend> createBackend(const string& backend_name) {
  if (backend_name == "pdf") {
    return make_unique<PdfBackend>();
  } else if (backend_name == "raster") {
    return make_unique<RasterBackend>();
  } else if (backend_name == "raster_direct") {
    // return make_unique<RasterDirectBackend>();
    throw runtime_error("raster_direct backend does not work yet");
  } else {
    throw runtime_error(fmt::format("Unrecognized backend name {}", backend_name));
    return nullptr; // can not reach here
  }
}

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  cout << "FLAGS_draw_func is " << FLAGS_draw_func << endl;
  cout << "FLAGS_backend is " << FLAGS_backend << endl;
  auto draw = findDrawFunc(FLAGS_draw_func);
  auto backend = createBackend(FLAGS_backend);
  auto canvas = backend->createCanvas();
  draw(canvas);
  backend->flush();
  backend->display();
  cout << "bye!" << endl;
  return 0;
}
