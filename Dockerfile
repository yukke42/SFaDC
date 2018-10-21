FROM gcc:8.2

# build and install opencv
# https://docs.opencv.org/3.4.3/d7/d9f/tutorial_linux_install.html
RUN apt-get update && apt-get install -y --no-install-recommends \
    cmake build-essential libgtk2.0-dev \
    libavcodec-dev libavformat-dev libswscale-dev \
    aria2 unzip

RUN aria2c -x 5 https://github.com/opencv/opencv/archive/3.4.3.zip \
    && unzip -qq opencv-3.4.3.zip \
    && mkdir opencv-3.4.3/build \
    && cd opencv-3.4.3/build \
    && cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local .. \
    && make -j7 \
    && make install \
    && rm -rf opencv* \
    && echo "/usr/local/lib/" > /etc/ld.so.conf.d/opencv.conf \
    && ldconfig -v

## add command to delete zipfile and unzipped directories
### it takes a long time to build opencv, so add other libralies bellow. ###

WORKDIR /root/projects
