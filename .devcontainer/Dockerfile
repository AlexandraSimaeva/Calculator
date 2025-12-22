FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    cmake \
    git \
    gdb \
    python3 \
    libboost-all-dev \       
    curl \                  
    net-tools \
    && rm -rf /var/lib/apt/lists/*

RUN apt-get update && apt-get install -y \
    libcpprest-dev \        
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

EXPOSE 8080
CMD ["bash"]
