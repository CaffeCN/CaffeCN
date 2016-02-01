#caffe and dependencies auto-install on ubuntu, tested on ubuntu 14.04 
OPENBLAS=1
SPYDER=1
NTHREAD=32
sudo apt-get update
sudo apt-get install git libboost-all-dev libprotobuf-dev \
libleveldb-dev libsnappy-dev libopencv-dev libhdf5-serial-dev \
protobuf-compiler libgflags-dev libgoogle-glog-dev liblmdb-dev \
python-pip clang
#python-numpy python-pip python-scipy
sudo pip install -r requirements.txt

#openblas
if [ $OPENBLAS ]
then
    echo 'install OpenBLAS...'
	cd /opt/ 
	if [ -d "./OpenBLAS" ];then
	    rm -rf OpenBLAS
	fi
	sudo git clone https://github.com/xianyi/OpenBLAS 
	cd OpenBLAS 
	sudo make -j$NTHREAD
	sudo make install
	sudo cp include/* /usr/local/include/
	sudo cp lib/* /usr/local/lib/
fi

if [ $SPYDER ]
then
    echo 'install spyder...'
	cd /opt/ 
	if [ -d "./spyder" ];then
	    rm -rf spyder
	fi
	sudo git clone https://github.com/spyder-ide/spyder
	sudo ln /opt/spyder/bootstrap.py /usr/local/bin/spyder
fi
echo 'done, now building caffe...'
cd ..
make -j$NTHREAD
