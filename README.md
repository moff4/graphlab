# GraphLab #

Point and edge 3D object rendering engine  

## Build

```bash
make prog
```

## Run

```bash
# .prog [input] [output] [CamX] [CamY] [CamZ]
./prog
# CLI args for input and output files
./prog input.dat output.bmp
# CLI args for controll camera position (X Y Z)
./prog input.dat output.bmp 10 15 4
```

## Run example

```bash
make prog
./prog ./prog examples/input.dat output.bmp 10 5 0
```
