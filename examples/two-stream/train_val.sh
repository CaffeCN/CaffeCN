#!/usr/bin/env sh

./build/tools/caffe train --solver=examples/two-stream/solver.prototxt --gpu 8
