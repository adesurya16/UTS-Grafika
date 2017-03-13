void drawcircle(int x0, int y0, int radius)
{
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        if(y0+y>y0) {
          putpixel(x0 + x, y0 + y);
          putpixel(x0 - x, y0 + y);
        }
        if(y0+x>y0) {
          putpixel(x0 + y, y0 + x);
          putpixel(x0 - y, y0 + x); 
        }
        if(y0-x>y0) {
          putpixel(x0 - y, y0 - x);
          putpixel(x0 + y, y0 - x);
        }
        if(y0-y>y0) {
          putpixel(x0 - x, y0 - y);
          putpixel(x0 + x, y0 - y);
        }

        if (err <= 0)
        {
            y += 1;
            err += 2*y + 1;
        }
        if (err > 0)
        {
            x -= 1;
            err -= 2*x + 1;
        }
    }
}
