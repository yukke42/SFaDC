FROM gcc:8.2

# https://askubuntu.com/questions/860207/how-to-install-eigen-3-3-in-ubuntu-14-04
RUN apt-get update && apt-get install -y --no-install-recommends \
    cmake build-essential \
    libopencv-dev \
    libeigen3-dev \
    libboost-dev

WORKDIR /root/projects/src/build
