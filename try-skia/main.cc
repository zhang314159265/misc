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
};

unordered_map<std::string, DrawFuncT> draw_func_dict = {
  {"draw_line", DrawFuncCollection::drawLine},
  {"draw_rotated_rect", DrawFuncCollection::drawRotatedRect},
  {"draw_star", DrawFuncCollection::drawStar},
  {"draw_geo_and_text", DrawFuncCollection::drawGeoAndText},
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
    int width = 256, height = 256;
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

unique_ptr<Backend> createBackend(const string& backend_name) {
  if (backend_name == "pdf") {
    return make_unique<PdfBackend>();
  } else if (backend_name == "raster") {
    return make_unique<RasterBackend>();
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
