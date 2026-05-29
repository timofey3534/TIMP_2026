# ── Stage 1: build ────────────────────────────────────────────────────────────
FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    qtbase5-dev \
    qt5-qmake \
    libqt5sql5-sqlite \
    libqt5gui5 \
    libssl-dev \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build
COPY EchoServer/ ./
RUN qmake EchoServer.pro && make -j"$(nproc)"

# ── Stage 2: runtime ──────────────────────────────────────────────────────────
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
# Allow QImage to work in a headless container
ENV QT_QPA_PLATFORM=offscreen

RUN apt-get update && apt-get install -y \
    libqt5core5a \
    libqt5network5 \
    libqt5sql5 \
    libqt5sql5-sqlite \
    libqt5gui5 \
    libssl3 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=builder /build/EchoServer /app/EchoServer

EXPOSE 33333

CMD ["/app/EchoServer"]
