# stock-simulator

This project simulates a system where generator write data in shared memory and handler read from it.

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Running
### generator
run
```bash
./stock_generator
```

### handler
To run a handler which handles info for `time` ms, simply run
```bash
./stock_handler [time]
```
