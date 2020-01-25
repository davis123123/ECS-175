
Run instructions:
make
./proj3

Once program starts it will have menu for options.
Default read file is "bunny", I scaled and translated it for testing pruposes
I left scaling and translate in menu if users wishes to test with multiple polygons
Scaling and translating currently DOES NOT redraw, need to recall phong or gouraud to redisplay.

Phong shading algorithm has very high brightness,
works with low ambient of (1,20,20) and low intensity (0.2)
and moderate light color( 50, 50, 50).
After finish inputs on phong model please wait about 20sec for vectors to normalize


Gouraud Shading is VERY slow and takes a couple minutes to render
I also seperated phong vertex shading and gouraud rasterize shading since gouraud takes long time

Did not do Half-tone

Applied Painter's algorithm, uncertain of effectiveness
