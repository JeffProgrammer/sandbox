## Graphics Sandbox

This is a sandbox application that has various tests for graphics programming.

The goal right now is for me to brush up on OpenGL again personally as it has been a couple of years since I've done much with the API. 

The eventual plan is to have a modern, multithreaded command buffer and graphics device style API with a suite of sandbox projects to act as a series of Unit tests for it.

## Supported Platforms

| Operating System | Architecture | API        | Compiler  |
| :--------------: | :----------: | :--------: | :-------: |
| Windows 7 SP1+   | x64          | OpenGL 4.3 | MSVC 2019 |
| MacOS 10.13+     | x64, arm64   | OpenGL 4.1 | XCode 12  |

*Subject to change, not all OS versions actively tested for each commit*

## Sandbox Projects

- **01 Hello Cubes**
    Renders a simple cube
- **02 Cpu Particles**
    Renders 10,000 point particles, updating from the cpu to the gpu every frame.
- **03 Draw Performance**
    Stresses drawcalls by rendering a cube per drawcall.
- **04 Forward Rendering**
    Renders up to 64 lights in forward rendering.

## License
```
MIT License

Copyright (c) 2021 Jeff Hutchinson

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```