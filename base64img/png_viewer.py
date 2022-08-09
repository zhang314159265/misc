# PNG Viewer:
# Use libpng to extract pixel data from a png file and show the pixels using PyQt5

import sys
from PyQt5.QtWidgets import QApplication, QWidget
from PyQt5.QtGui import QColor, QPainter
import png

class PNGViewer(QWidget):
  def __init__(self, path):
    super(PNGViewer, self).__init__()
    self.setWindowTitle("PNG Viewer")
    left = 10
    top = 10

    self.png = png.Reader(filename=path).read()
    self.pad = 250

    # first dimension is row/y rather than column/x
    self.color_indices = list(self.png[2])
    self.palette = self.png[3].get("palette", None)
    self.width = self.png[0]
    self.height = self.png[1]
    self.setGeometry(left, top, self.width + 2 * self.pad, self.height + 2 * self.pad)
    self.show()

  def paintEvent(self, event):
    super().paintEvent(event)

    p = QPainter(self)
    for x in range(self.width):
      for y in range(self.height):
        if self.palette:
            idx = self.color_indices[y][x]
            color = self.palette[idx] # rgb or rgba
        else:
            color = self.color_indices[y][x * 3 : (x + 1) * 3]
        p.setPen(QColor(*color))
        p.drawPoint(x + self.pad, y + self.pad)

if __name__ == "__main__":
  app = QApplication(sys.argv)
  path = sys.argv[1] if len(sys.argv) >= 2 else "green-leaf.png"
  ex = PNGViewer(path)
  sys.exit(app.exec_())
