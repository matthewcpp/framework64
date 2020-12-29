# n64-sandbox
Experimentation with n64 SDK

### Building

docker run --rm -v $(pwd):/src -u $(id -u):$(id -g) matthewcpp/n64-modern-sdk /bin/bash -c "cd /src; make FINAL=YES"
