init_venv:
	/usr/bin/python3 -m venv .venv
	. .venv/bin/activate && pip install --upgrade pip && pip install -r requirements.txt

build-release:
	conan install . --build=missing
	cmake --preset conan-release-ninja
	cmake --build build
