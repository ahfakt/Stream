# Stream

```shell
git clone https://github.com/ahfakt/Stream.git

# Debug, Release, RelWithDebInfo, MinSizeRel
BUILD_TYPE='Release'

# Configure
cmake \
    -D "CMAKE_BUILD_TYPE:STRING=${BUILD_TYPE}" \
    -S 'Stream' \
    -B "build/${BUILD_TYPE}/Stream" \
    -G "Unix Makefiles"

# Build
cmake \
    --build "build/${BUILD_TYPE}/Stream" \
    --config "${BUILD_TYPE}" \
    --target all
```
