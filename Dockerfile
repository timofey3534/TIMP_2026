FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
# Needed so QImage can work without a display
ENV QT_QPA_PLATFORM=offscreen

RUN apt-get update && apt-get install -y \
    qtbase5-dev \
    qt5-qmake \
    libqt5sql5-sqlite \
    libqt5gui5 \
    libssl-dev \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY EchoServer/ ./

RUN qmake EchoServer.pro && make -j"$(nproc)"

EXPOSE 33333

CMD ["./EchoServer"]
