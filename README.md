# rogue

a roguelike thing

## building

> requirements: Python, internet connection

```bash
# run this only once, after cloning the repo.
python gl3w_gen.py
```

> this is temporary
>
> requirements: C≥11 compiler (preferably C23), GLFW3

```bash
cc -std=c2x -Iinclude -lglfw src/*.c -o main
```

## running

> requirements: OpenGL 4.6

```bash
./main
```
