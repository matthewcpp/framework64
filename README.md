# framework64
This framework consists of an asset pipeline and minimal C library providing a quick and easy path to create n64 games.
This framework utilizes [Crash's Modern n64 SDK](https://github.com/CrashOveride95/n64sdkmod).

### Requirements
In order to build you will need to have [cmake](https://cmake.org/), [nodejs](https://nodejs.org/en/) and [docker](https://www.docker.com/) installed.

### Building for N64

To Build all the sample roms run the following command from your terminal, using the actual path to your framework64 repo as the mount source:
```
npm install
npm run prepare-n64-assets
docker run -it --rm --mount type=bind,source=/path/to/framework64,target=/src matthewcpp/n64-modern-sdk bash /build_all_n64_examples.sh
```

All roms will be placed in the `build_n64/bin` folder.

#### Visual Studio Code Development Setup
- Ensure that you have the remote containers extension installed.
- Copy `config/devcontainer/devcontainer.n64.json` to `.devcontainer.json` in the repo's root folder
- Open the repo root folder in VS Code.
- Install the C/C++ development extension into the container via the extensions panel.
- After opening the project, add the following entries to `.vscode/c_cpp_properties.json` for IntelliSense assistance:
  - defines:
    - PLATFORM_N64
  - includePath:
    - /usr/include/n64
    - /usr/include/n64/nusys
    - /usr/include/n64/nustd


### Building For Desktop
Dependencies are configured using [vcpkg](https://vcpkg.io/en/index.html)
Use the following commands to configure vcpkg, note that this only needs to be done once.
```shell
git clone https://github.com/microsoft/vcpkg.git /some/path/to/vcpkg
cd /some/path/to/vcpkg
./bootstrap-vcpkg.sh 
```

#### MacOS
- Run `brew install pkg-config`

#### Ubuntu Linux
- Run `apt-get install libvorbis-dev`

#### Build with CMAKE
```shell
npm install
npm run prepare-desktop-assets
npm run prepare-desktop-shaders
cd build_desktop
cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake ..
cmae --build .
```

Executables will be placed in the `build_desktop/bin` folder.

##### Clion
The default build folder will be something along the lines of `cmake-build-debug`.  In the Cmake settings dialog set the build folder to `build_desktop`.


