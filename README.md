# Mandelbrot Set Visualiser 


Simple app for zooming into the
<a href="https://en.wikipedia.org/wiki/Mandelbrot_set">mandelbrot set</a>.


The set is color by using this equation:
```math
(r, g, b) = \frac{255}{2K} * (1 - cos(R * n), 1 - cos(G * n), 1 - cos(B * n))
```
where n is the number of iterations and K, R, G, B are chosen parameters,
initialized as
```math
R = 1
G = \frac{1}{3\sqrt{2}}
B = \frac{1}{7 \cdot 3^{1/8}}
```
## How to use

<table>
    <tr>
        <th>Control</th>
        <th>Action</th>
        <th>Notes</th>
    </tr>
    <tr>
        <td>w</td>
        <td>move up</td>
        <td></td>
    </tr>
    <tr>
        <td>s</td>
        <td>move down</td>
        <td></td>
    </tr>
    <tr>
        <td>a</td>
        <td>move left</td>
        <td></td>
    </tr>
    <tr>
        <td>d</td>
        <td>move right</td>
        <td></td>
    </tr>
    <tr>
        <td>j</td>
        <td>zoom in</td>
        <td></td>
    </tr>
    <tr>
        <td>k</td>
        <td>zoom out</td>
        <td></td>
    </tr>
    <tr>
        <td>h</td>
        <td>decrease number of iterations</td>
        <td>-5</td>
    </tr>
    <tr>
        <td>l</td>
        <td>increase number of iterations</td>
        <td>+5</td>
    </tr>
    <tr>
        <td>b</td>
        <td>zoom back to initial state</td>
        <td>It keeps the position, just resets the zoom,
            so you can zoom back when you were.</td>
    </tr>
    <tr>
        <td>r</td>
        <td>zoom back and reset all parameters</td>
        <td>It zooms back to the original state
            and also keeps the position. In addition,
            it resets all parameters to their default values.
        </td>
    </tr>
    <tr>
        <td>u</td>
        <td>K = K + 1</td>
        <td>Press with shift for decrease.</td>
    </tr>
    <tr>
        <td>i</td>
        <td>R = R + 0.1</td>
        <td>Press with shift for decrease.</td>
    </tr>
    <tr>
        <td>o</td>
        <td>R = R + 0.1</td>
        <td>Press with shift for decrease.</td>
    </tr>
    <tr>
        <td>p</td>
        <td>R = R + 0.1</td>
        <td>Press with shift for decrease.</td>
    </tr>
    <tr>
        <td>enter</td>
        <td>capture</td>
        <td>Saves the current part of the set as a .ppm image in 8K.
        </td>
    </tr>
</table>

## Building the app
There are two options as for how to build this app. You can compile the source code
for gpu or for cpu. Performance of the gpu version is much higher, but it only allows me to
use less precise numerical types, so the maximum zoom is lower. <br>
The performance of cpu version is much worse, but it allows larger zoom.

To build the app, simply use the provided makefile and type `make` or `make ACC=gpu`.
