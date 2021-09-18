# framework64
This framework consists of a simple asset pipeline and minimal C library providing a quick and easy path to create n64 games.  This framework utilizes [Crash's modern n64 SDK](https://github.com/CrashOveride95/n64sdkmod).

### Building
In order to build you will need to have nodejs and docker installed. 

Begin by installing the dependencies:
```
npm install
```

### Building For desktop

#### OSX
- Run `brew install pkg-config`

#### Build with CMAKE
- Create build directory: `mkdir build`
- Configure the project: `cmake -S . -B build .. -DCMAKE_TOOLCHAIN_FILE=lib/vcpkg/scripts/buildsystems/vcpkg.cmake`
- Build the project: `cmake --build build`

# Building for N64

Build the assets:
```
node tools/prepare_assets.js --manifest assets/manifest.n64.json --out-dir assets/build_n64 -f
```

Build the example ROM files:
```
docker run --rm -v $(pwd):/workspace -u $(id -u):$(id -g) matthewcpp/n64-modern-sdk /bin/bash -c "cd /workspace; ./build_all.sh"
```

### Visual Studio Code Development
- Ensure that you have the remote containers extension installed.
- Copy `tools/.devcontainer.n64.json` to `.devcontainer.json`
- Open the repo root folder in VS Code.
- Install the C/C++ development extension into the container via the extensions panel.
- In the terminal override the `ROOT` environment variable: `ROOT=/etc/n64`
