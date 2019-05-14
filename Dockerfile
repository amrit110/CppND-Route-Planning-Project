FROM ubuntu:latest

# Install deps.
RUN apt update
RUN apt install -y g++ build-essential git wget libcairo2-dev libgraphicsmagick1-dev libpng-dev

# Get latest cmake binary and set sym link to it.
RUN cd /opt && mkdir cmake && cd cmake && \
	wget https://github.com/Kitware/CMake/releases/download/v3.14.3/cmake-3.14.3-Linux-x86_64.sh && \
	sh cmake-3.14.3-Linux-x86_64.sh --skip-license && ln -s /opt/cmake/bin/cmake /usr/bin/cmake

# Install the io2d library.
RUN cd /opt && git clone --recurse-submodules https://github.com/cpp-io2d/P0267_RefImpl && cd P0267_RefImpl && \
	mkdir Debug && cd Debug && cmake --config Debug "-DCMAKE_BUILD_TYPE=Debug" .. && cmake --build . && make && \
	make install

# Set docker entrypoint to /workspace. 
WORKDIR /workspace
