FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    cmake \
    git \
    gdb \
    python3 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace