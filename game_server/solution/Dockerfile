FROM gcc:11.3 AS build

RUN apt update && \
    apt install -y \
      python3-pip \
      python3-venv \
      cmake


WORKDIR /app
RUN python3 -m venv venv && \
    venv/bin/pip install --upgrade pip && \
    venv/bin/pip install conan==1.65.0

COPY conanfile.txt /app/
RUN mkdir /app/build && cd /app/build && \
    /app/venv/bin/conan install .. -s build_type=Debug -s compiler.libcxx=libstdc++11


COPY ./src /app/src
COPY ./data /app/data
COPY ./static /app/static
COPY CMakeLists.txt /app/


RUN cd /app/build && \
    cmake .. -DCMAKE_BUILD_TYPE=Debug .. && \
    cmake --build .

FROM ubuntu:22.04 AS run

RUN groupadd -r www && useradd -r -g www www
USER www

COPY --from=build /app/build/game_server /app/
COPY ./data /app/data
COPY ./static /app/static

ENTRYPOINT ["/app/game_server", "--config-file", "/app/data/config.json", "--www-root", "/app/static"] 