# n64-sandbox
Experimentation with n64 SDK

### Visual Studio Code
- Ensure that you have the remote containers extension installed.
- Copy `tools/.devcontainer.n64.json` to `.devcontainer.json`
- Open the repo root folder in VS Code.
- In the terminal override the `ROOT` environment variable: `ROOT=/etc/n64`

### Building

docker run --rm -v $(pwd):/src -u $(id -u):$(id -g) matthewcpp/n64-modern-sdk /bin/bash -c "cd /src; make FINAL=YES"
