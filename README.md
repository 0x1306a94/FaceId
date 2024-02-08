# FaceId

* `Apple silicon` 环境编译需要进行如下修改 
```bash
# 修改 deps/SeetaFace6Open/TenniS/craft/build.linux.x64.sh cmake
cmake "$HOME/.." \
-DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
-DCONFIGURATION="$BUILD_TYPE" \
-DPLATFORM="$PLATFORM_TARGET" \
-DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
-DTS_USE_OPENMP=OFF \
-DTS_USE_CBLAS=ON \
-DTS_USE_SIMD=ON \
-DTS_ON_HASWELL=OFF \
-DTS_DYNAMIC_INSTRUCTION=OFF
```


### [Postman Documentation](https://documenter.getpostman.com/view/5162896/2s9Yyy9yba)