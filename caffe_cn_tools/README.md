Tools Created by CaffeCN
========================

easy_caffe
----------

在ubuntu系统自动安装caffe以及依赖项. 

以root权限执行脚本`easy_caffe.sh`

如果需要安装**CUDA**, 请先下载[cuda](https://developer.nvidia.com/cuda-downloads)安装包, 根据自己操作系统的版本选择下载. 然后以root权限执行`cuda.sh`安装`cuda`, 其中`cuda.sh`脚本中对应的的文件名应改为你下载的`CUDA`安装包文件.


caffe_output_filter
-------------------

### 简介
`caffe_output_filter` 可提取 `Caffe` 日志里损失和精度的数值，以供其他程序使用。

### 使用方法
```
caffe_output_filter.sh [<train|test>] [<loss|top1|top5>] [file].
```
如果不指定 Caffe 日志文件，就从标准输入里读取数据

### 例子
从 `Caffe` 日志里读取数据，并输出 训练 `top-1` 精度。
```
./caffe_output_filter.sh train top1 caffe_output.txt
```
通过管道，从标准输入中读取数据，并提取 测试 `top-5` 精度。
```
cat caffe_output.txt | ./caffe_output_filter.sh test top5
```
一边训练，一遍提取测试 `top-1` 精度。
```
build/tools/caffe train --solver solver.prototxt |& ./caffe_output_filter.sh test top1
```


