# Mandelbrot Set Visualiser 


Simple app for zooming into
<a href="https://en.wikipedia.org/wiki/Mandelbrot_set">Mandelbrot Set</a>,
<a href="https://en.wikipedia.org/wiki/Mandelbrot_set">Julia Set</a>,
and <a href="https://en.wikipedia.org/wiki/Mandelbrot_set">The Burning Ship</a> fractals.<br>
Here are some examples of Mandelbrot Set:


**No zoom** <br>
<img src="example_images_low_res/twice.png" alt="No zoom" width="600"/><br>


**Zoomed a hundred times** <br>
<img src="example_images_low_res/hundred.png" alt="Zoomed a hundred times" width="600"/><br>


**Zoomed a million times** <br>
<img src="example_images_low_res/million.png" alt="Zoomed a million times" width="600"/><br>


**Zoomed a trillion times** <br>
<img src="example_images_low_res/trillion.png" alt="Zoomed a trillion times" width="600"/><br>


<br>

All fractals are colored by this formula:


<img alt="formula" src="https://render.githubusercontent.com/render/math?math=(r, g, b) = \frac{255}{2K} \cdot (1 - \cos(R \cdot n), 1 - \cos(G \cdot n), 1 - \cos(B \cdot n))">

[comment]: <> (```math)
[comment]: <> (&#40;r, g, b&#41; = \frac{255}{2K} * &#40;1 - cos&#40;R * n&#41;, 1 - cos&#40;G * n&#41;, 1 - cos&#40;B * n&#41;&#41;)
[comment]: <> (```)
where n is the number of computed iterations before the point exceeded from the set and K, R, G, B are chosen parameters,
initialized as:


<img alt="K" src="https://render.githubusercontent.com/render/math?math=K = 10"><br>
<img alt="R" src="https://render.githubusercontent.com/render/math?math=R = 1"><br>
<img alt="G" src="https://render.githubusercontent.com/render/math?math=G = \frac{1}{3\sqrt{2}}"><br>
<img alt="B" src="https://render.githubusercontent.com/render/math?math=B = \frac{1}{7 \cdot 3^{1 / 8}}"><br>

For the formula I was inspired
by <a href="https://www.math.univ-toulouse.fr/~cheritat/wiki-draw/index.php/Mandelbrot_set">this article</a>.


[comment]: <> (```math)
[comment]: <> (R = 1)
[comment]: <> (G = \frac{1}{3\sqrt{2}})
[comment]: <> (B = \frac{1}{7 \cdot 3^{1/8}})
[comment]: <> (```)

## How to use

<table>
    <tr>
        <th>Control</th>
        <th>Action</th>
        <th>Notes</th>
    </tr>
    <tr>
        <td>w, up</td>
        <td>move up</td>
        <td></td>
    </tr>
    <tr>
        <td>s, down</td>
        <td>move down</td>
        <td></td>
    </tr>
    <tr>
        <td>a, left</td>
        <td>move left</td>
        <td></td>
    </tr>
    <tr>
        <td>d, right</td>
        <td>move right</td>
        <td></td>
    </tr>
    <tr>
        <td>j</td>
        <td>zoom in</td>
        <td>Press shift for faster zooming.</td>
    </tr>
    <tr>
        <td>k</td>
        <td>zoom out</td>
        <td>Press shift for faster zooming.</td>
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
        <td>zooms back to initial state</td>
        <td>It keeps the position and number of maximum iterations, just resets the zoom,
            so you can zoom in again to where you were before as long as you won't move.</td>
    </tr>
    <tr>
        <td>r</td>
        <td>zooms back and reset the number of iterations</td>
        <td>
            Zooms back to the original state
            and also keeps the position. In addition,
            it resets the value of maximum iterations.
        </td>
    </tr>
    <tr>
        <td>c</td>
        <td>color reset</td>
        <td>Resets K, R, G and B constants to theirs default values.</td>
    </tr>
    <tr>
        <td>1</td>
        <td>K = K + 0.5</td>
        <td>Press shift for faster change.
            Press ctrl for decrease</td>
    </tr>
    <tr>
        <td>2</td>
        <td>R = R + 0.05</td>
        <td>Press shift for faster change.
            Press ctrl for decrease</td>
    </tr>
    <tr>
        <td>3</td>
        <td>G = G + 0.05</td>
        <td>Press shift for faster change.
            Press ctrl for decrease</td>
    </tr>
    <tr>
        <td>4</td>
        <td>B = B + 0.05</td>
        <td>Press shift for faster change.
            Press ctrl for decrease</td>
    </tr>
    <tr>
        <td>5</td>
        <td>increases real part of c constant when viewing the Julia Set</td>
        <td>Press shift for faster change.
            Press ctrl for decrease.</td>
    </tr>
    <tr>
        <td>6</td>
        <td>increases imaginary part of c constant when viewing the Julia Set</td>
        <td>Press shift for faster change.
            Press ctrl for decrease</td>
    </tr>
    <tr>
        <td>t</td>
        <td>toggle info</td>
        <td></td>
    </tr>
    <tr>
        <td>enter</td>
        <td>capture</td>
        <td>Saves the current part of the set as a .ppm image in 8K.
        </td>
    </tr>
</table>

You can also move around using mouse.


## Building the app (for Linux)
To build the app, simply use the provided makefile and type `make`. <br>
The command will create build directory with the FractalsExplorer in it. <br>
For the compilation to work, one must have installed ``pgc++`` compiler.
Installation instructions are <a href="https://developer.nvidia.com/nvidia-hpc-sdk-downloads">here</a>.
<br>
There also must be <a href="https://www.sfml-dev.org/">SFML library</a> available on the system.
It should be possible to get it easily from your system's repositories.

## Running the app
To run the app, you have to specify which of the fractals you want to explore via command line arguments.
You can also specify the width of the window. The height will be computed automatically to fit the 
ratio of chosen fractal. <br>
``
$ ./FractalsExplorer WIDTH [mandelbrot|julia|ship]
``
