#!/bin/bash


INSTALL_OPENCV=false

case "$1" in
    --opencv=true)
        INSTALL_OPENCV=true
        echo "[INFO]: Installing with OpenCv"
        ;;
    --opencv=false)
        INSTALL_OPENCV=false
        echo "[INFO]: Installing without OpenCv"
        ;;
esac

echo "[INFO]: Installing rknn-toolkit2 dependencies"

cd ~    
sudo apt update -y && sudo apt upgrade -y
sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test 
sudo apt install -y gcc-13 g++-13
sudo apt install git make libomp-dev

echo "[INFO]: Installing rknn-toolkit2"

git clone --depth=1 https://github.com/airockchip/rknn-toolkit2.git
cd ~/rknn-toolkit2-master/rknpu2/runtime/Linux/librknn_api/aarch64
sudo cp ./librknnrt.so /usr/local/lib
cd ~/rknn-toolkit2-master/rknpu2/runtime/Linux/librknn_api/include
sudo cp ./rknn_* /usr/local/include/rknpu
cd ~
sudo rm -rf ./rknn-toolkit2-master


# Check if INSTALL_OPENCV is true and install OpenCV
if [ "$INSTALL_OPENCV" = true ]; then

    echo "Installing and Building Opencv"

    cd ~
    sudo apt install cmake
    sudo apt install -y pkg-config ffmpeg libavformat-dev libavcodec-dev libswscale-dev
    git clone --depth=1 https://github.com/opencv/opencv.git 
    cd opencv
    cmake -B build
    cd build
    make -j5
    sudo make install
    cd ~ 
    rm -rf ./opencv

fi
