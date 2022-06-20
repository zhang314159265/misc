# API learned from https://pythonspot.com/pyqt5-pixels/
import sys
from PyQt5.QtWidgets import QApplication, QWidget
from PyQt5.QtGui import QColor, QPainter

class App(QWidget):
    def __init__(self):
        super(App, self).__init__();
        self.title = "Draw an image pixel by pixel"
        self.left = 10
        self.top = 10
        self.width = 640
        self.height = 480

        # self.initUI()
        self.setWindowTitle(self.title)    
        self.setGeometry(self.left, self.top, self.width, self.height)
        self.show()

    def paintEvent(self, event):
        """
        Note:
        1. it seems we can only create a single QPainter for the widget
        2. we can not create/use QPainter in the ctor. Thus do that in paintEvent
           method when the widget has already been created.
        """
        super().paintEvent(event)

        p = QPainter(self)
        p.setPen(QColor(0xFF, 0, 0))
        for x in range(self.width):
            if x == self.width // 2:
                p.setPen(QColor(0, 0, 0))
            for y in range(self.height):
                p.drawPoint(x, y)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    ex = App()
    sys.exit(app.exec_())
