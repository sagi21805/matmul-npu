cd ~    

sudo apt update -y 

sudo apt upgrade -y 

git clone --depth=1 https://github.com/airockchip/rknn-toolkit2.git

cd ~/rknn-toolkit2-master/rknpu2/runtime/Linux/librknn_api/aarch64

sudo cp ./librknnrt.so /usr/local/lib

cd ~/rknn-toolkit2-master/rknpu2/runtime/Linux/librknn_api/include

sudo cp ./rknn_* /usr/local/include/rknpu

cd ~

sudo rm -rf ./rknn-toolkit2-master