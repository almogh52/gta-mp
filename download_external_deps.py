import requests
import tempfile
import shutil
import os

url = 'http://opensource.spotify.com/cefbuilds/cef_binary_75.0.14%2Bg3b211d4%2Bchromium-75.0.3770.100_windows64_minimal.tar.bz2'

print("Downloading CEF...")
response = requests.get(url, stream=True)
if response.status_code == 200:
	tempfile_path = ""
	with tempfile.NamedTemporaryFile() as f:
		tempfile_path = f.name

	with open(tempfile_path, "wb") as f:
		f.write(response.raw.read())

	print("Extracting CEF...")
	shutil.unpack_archive(tempfile_path, "client/vendor", format="bztar")
	
	dir_name = list(filter(lambda name: "cef" in name, [x[0] for x in os.walk("client/vendor")]))[0]

	os.rename(dir_name, "client/vendor/cef")