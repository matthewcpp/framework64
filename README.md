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

#### N64 Visual Studio Code Development Setup
- Copy `config/devcontainer/devcontainer.n64.json` to `.devcontainer.json` in the repo's root folder.
- Open the repo root folder in VS Code.  If prompted to reopen the folder in a container, click OK.
- When the container is opened for the first time C/C++, GDB, and Cmake extensions will be installed in the container.
- Configure the project by running the `Cmake:Configure` command from the palette or the popup window when the project opens.
- If prompted to specify a kit, choose `Unspecified`.
- After configuration is complete, choose a target and press build on the bottom status bar.

#### N64 Debugging with cen64
- Download or build a recent version of [cen64](https://github.com/n64dev/cen64).
- Ensure that your CMake build type is set to Debug.
- Build the rom you wish to debug.
- Start cen64 using the `-debug localhost:8080` parameter and point it to your newly built ROM.
- From the VS Code debugging panel, use the dropdown to select the configuration based on your *Host* platform.  You will need to edit the `launch.json` file to contain the name of the ROM you just built.
- The debugger will start and attach to cen64.


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


