#include <iostream>
#include <unordered_map>
#include "SkCanvas.h"
#include "SkStream.h"
#include "SkPath.h"
#include "SkPDFDocument.h"
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
};

unordered_map<std::string, DrawFuncT> draw_func_dict = {
  {"draw_line", DrawFuncCollection::drawLine},
  {"draw_rotated_rect", DrawFuncCollection::drawRotatedRect},
  {"draw_star", DrawFuncCollection::drawStar},
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
  virtual SkCanvas* createCanvas() = 0;
  virtual void display() = 0;
  virtual void flush() = 0;
  virtual ~Backend() = default;

  static void displayImg(const std::string& path) {
    auto cmd = fmt::format("imgcat {}", path);
    auto status = system(cmd.c_str());
    if (status != 0) {
      throw runtime_error(fmt::format("Fail to run command: {}", cmd));
    }
  }
};

class PdfBackend : public Backend {
 public:
  PdfBackend() : path_("/tmp/skia.pdf"), pdfStream_(make_unique<SkFILEWStream>(path_.c_str())) {
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

  ~PdfBackend() {
  }

  void display() override {
    displayImg(path_);
  }
 private:
  std::string path_;
  unique_ptr<SkFILEWStream> pdfStream_;
  sk_sp<SkDocument> pdfDoc_;
};

unique_ptr<Backend> createBackend(const string& backend_name) {
  if (backend_name == "pdf") {
    return make_unique<PdfBackend>();
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
