# framework64
This framework consists of an asset pipeline and minimal C library providing a quick and easy path to create n64 games.
This framework utilizes [Crash's modern n64 SDK](https://github.com/CrashOveride95/n64sdkmod).

### Requirements
In order to build you will need to have [nodejs](https://nodejs.org/en/) and [docker](https://www.docker.com/) installed.


### Building for N64

Build the assets:
```
npm install
node tools/prepare_assets.js --manifest assets/manifest.n64.json --out-dir assets/build_n64 -f
```

Build the example ROM files:
```
docker run --rm -v $(pwd):/workspace -u $(id -u):$(id -g) matthewcpp/n64-modern-sdk /bin/bash -c "cd /workspace; ./build_all.sh"
```

#### Visual Studio Code Setup
- Ensure that you have the remote containers extension installed.
- Copy `tools/.devcontainer.n64.json` to `.devcontainer.json`
- Open the repo root folder in VS Code.
- Install the C/C++ development extension into the container via the extensions panel.
- In the terminal override the `ROOT` environment variable: `ROOT=/etc/n64`


### Building For Desktop
Dependencies are configured using [vcpkg](https://vcpkg.io/en/index.html)
Use the following commands to configure vcpkg, note that this only needs to be done once.
```shell
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh 
```

#### MacOS
- Run `brew install pkg-config`

#### Build with CMAKE
- Create build directory: `mkdir build`
- Configure the project: `cmake -S . -B build .. -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake`
- Build the project: `cmake --build build`


