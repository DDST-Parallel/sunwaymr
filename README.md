# SunwayMR

A parallel computing framework written in C++. We reference much on Spark's RDD design.

## Dependencies

You will need `gcc/g++` when using GNU `make`.

## Usage

* Download and make source on each node

```bash
    make
```

* Start SunwayMR resource manager on each node, while specifying `master IP`, `master port`, `shared threads of node`, `shared memory of node`

```bash
    ./sunwaymr -t 192.168.1.85 19113 4 1024
```

* Now, you can run example program **on master** (in a new terminal)

```bash
    ./sunwaymr -a examples/SunwayMRPi.cpp
```

* See results in listening terminal

