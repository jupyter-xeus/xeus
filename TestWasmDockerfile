ARG EMSCRIPTEN_VER=2.0.32
FROM emscripten/emsdk:$EMSCRIPTEN_VER

ARG USER_ID
ARG GROUP_ID

RUN mkdir -p /install
RUN mkdir -p /install/lib

# make install dir
RUN mkdir install

##################################################################
# xtl
##################################################################
RUN mkdir -p /deps/xtl/build && \
    git clone --branch 0.7.2 https://github.com/xtensor-stack/xtl.git  /deps/xtl/src
#RUN cd  /deps/xtl/src && git checkout tags/0.7.2

RUN cd /deps/xtl/build && \
    emcmake cmake ../src/   -DCMAKE_INSTALL_PREFIX=/install

RUN cd /deps/xtl/build && \
    emmake make -j8 install


##################################################################
# nloman json
##################################################################
RUN mkdir -p /deps/nlohmannjson/build && \
    git clone --branch v3.9.1 --depth 1 https://github.com/nlohmann/json.git /deps/nlohmannjson/src

RUN cd /deps/nlohmannjson/build && \
    emcmake cmake ../src/   -DCMAKE_INSTALL_PREFIX=/install -DJSON_BuildTests=OFF

RUN cd /deps/nlohmannjson/build && \
    emmake make -j8 install


##################################################################
# xeus itself
##################################################################
ADD ./CMakeLists.txt /
ADD ./src /src 
ADD ./include /include 
ADD ./test /test 
ADD ./cmake /cmake 
ADD ./xeusConfig.cmake.in /

RUN mkdir -p /xeus-build && cd /xeus-build && \
    emcmake cmake  .. \
        -DCMAKE_INSTALL_PREFIX=/install \
        -Dnlohmann_json_DIR=/install/lib/cmake/nlohmann_json \
        -Dxtl_DIR=/install/share/cmake/xtl \
        -DXEUS_BUILD_TESTS=ON \
        -DXEUS_EMSCRIPTEN_WASM_BUILD=ON \
        -DXEUS_EMSCRIPTEN_WASM_TEST_BUILD=ON

RUN cd /xeus-build && \
    emmake make -j8
#####################################################