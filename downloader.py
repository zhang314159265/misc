import sys
import urllib.request as urllib

url = "https://www.google.com"
argv = sys.argv[1:]
if argv:
    url = argv[0]

print(f"Downloading {url}")

opener = urllib.build_opener()
headers = [('Accept', '*/*')]
opener.addheaders = headers
urllib.install_opener(opener)
src = urllib.urlopen(url, timeout=30)
cont = src.read()
src.close()

with open("/tmp/downloader.out", "wb") as f:
    f.write(cont)
print("bye!")
