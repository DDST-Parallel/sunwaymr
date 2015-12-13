# SunwayMR

A parallel computing framework written in C++. We reference much on Spark's RDD design.

## Dependencies

You will need `gcc/g++` when using GNU `make`.

## Usage

0. You can show usage by running `./sunwaymr`

1. Download and make source on each node

```bash
    make
```

2. Start SunwayMR resource manager on each node, while specifying `master IP` `master port` `shared threads of node` `shared memory of node`

```bash
    ./sunwaymr -t 192.168.1.85 19113 4 1024
```

3. Now, you can run example program **on master** (in a new terminal)

```bash
    ./sunwaymr -a examples/SunwayMRPi.cpp
```

4. See results in listening terminal

