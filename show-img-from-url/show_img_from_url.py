# code are adapted from https://pythonspot.com/pyqt5-image/
import sys
import urllib.request

from PyQt5.QtWidgets import QApplication, QWidget, QLabel
from PyQt5.QtGui import QPixmap
import tempfile
import os

class App(QWidget):
    def __init__(self, url=None):
        super(App, self).__init__()
        self.title = "Display an image from the url"
        self.left = 10
        self.top = 10
        self.width = 640
        self.height = 480
        self.url = url if url else "https://cdn.computerhoy.com/sites/navi.axelspringer.es/public/styles/1200/public/media/image/2021/08/naruto-naruto-uzumaki-2425679.jpg"
        self.initUI()

    def downloadUrl(self) -> str:
        """
        Return the temp file path storing the downloaded image.
        """
        print(f"Start downloading the url {self.url}")
        sock = urllib.request.urlopen(self.url)
        cont = sock.read()
        sock.close()
        print(f"Done downloading the url {self.url}")

        _, path = tempfile.mkstemp()
        with open(path, "wb") as f:
            f.write(cont)
        print(f"Saving image to {path}")
        return path

    def initUI(self):
        self.setWindowTitle(self.title)
        self.setGeometry(self.left, self.top, self.width, self.height)

        # create widget
        label = QLabel(self)
        imgpath = self.downloadUrl()
        pixmap = QPixmap(imgpath)

        # delete the temp file
        os.unlink(imgpath)

        label.setPixmap(pixmap)
        self.resize(pixmap.width(), pixmap.height())
        self.show()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    ex = App(sys.argv[1] if len(sys.argv) >= 2 else None)
    sys.exit(app.exec_())
