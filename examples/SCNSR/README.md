***********************************************************************************************************


Training code for "Deep Networks for Image Super-Resolution with Sparse Prior" (ICCV 2015) implemented by Zehao Huang.

***********************************************************************************************************



Usage:


1. Place the "SCNSR" folder into "($Caffe_Dir)/examples/"



2. Open MATLAB and direct to ($Caffe_Dir)/example/SCNSR, run 
"generate_train.m" and "generate_test.m" to generate training and test data. (From "SRCNN" by Chao Dong)

3. To train SCNSR, run
./build/tools/caffe train --solver examples/SCNSR/SCNSR_solver.prototxt --weights examples/SCNSR/scn_ini.caffemodel

