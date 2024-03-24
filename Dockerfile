# Use a base image with necessary tools and libraries for your project
FROM ubuntu:latest

# Install necessary packages for vcpkg
RUN apt-get update && \
    apt-get install -y \
    git \
    build-essential \
    cmake \
    pkg-config \
    curl \
    zip \
    unzip \
    tar

# Clone vcpkg repository
RUN git clone https://github.com/microsoft/vcpkg.git
#RUN apt-get install curl zip unzip tar
# Bootstrap vcpkg

RUN vcpkg/bootstrap-vcpkg.sh

# Set up environment variables for vcpkg
ENV VCPKG_ROOT=/vcpkg

# Copy your project files into the container
COPY tools .
#COPY CMakeLists.txt .

# Set the working directory
WORKDIR /tools

# Build your project
RUN mkdir build && cd build
RUN cmake .. -DCMAKE_TOOLCHAIN_FILE=/vcpkg/scripts/buildsystems/vcpkg.cmake
RUN cmake --build .




# Set the command to execute when the container starts
CMD ["./tools/tools"]
