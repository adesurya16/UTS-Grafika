#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

using namespace std;
int main()
{
     
    int fbfd = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;
    char *fbp = 0;
    int x = 0, y = 0;
    long int location = 0;

    //input
    int pixelrow;
    int pixelcol;
    cin >> pixelrow;
    cin >> pixelcol;

    /* Membaca text dari file gambar */
    string word;
    ifstream file;
    file.open("grafika.txt");

    int arraypixel[3][pixelrow][pixelcol];

    for (int i=0; i<3; i++) {
        for (int j=0; j<pixelrow; j++) {
            for (int k=0; k<pixelcol; k++) {
                file >> word;
                arraypixel[i][j][k] = stoi(word);
                //cout << "(" << j << "," << k << ")"<< stoi(word) << "," << word << endl;
            }        
        }
    }

    file.close();

    // Open the file for reading and writing framebuffer
    fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        exit(1);
    }
    //printf("The framebuffer device was opened successfully.\n");

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        exit(2);
    }
    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        exit(3);
    }
    //printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    // mendapat screensize layar monitor
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
    //printf("screensize %ld\n",screensize);

    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, (off_t)0);
    //printf("The framebuffer device was mapped to memory successfully.\n");

    //display merge center
    // Menulis ke layar tengah file
    for (y = vinfo.yres/2 - pixelrow/2; y < pixelrow + vinfo.yres/2 - pixelrow/2; y++)
        for (x = vinfo.xres/2 - pixelcol/2; x < pixelcol + vinfo.xres/2 - pixelcol/2; x++) {
            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
            //printf("location: %ld\n",location);
            if (vinfo.bits_per_pixel == 32) { 
                //4byte
                    *(fbp + location) = arraypixel[2][y - vinfo.yres/2 + pixelrow/2][x - vinfo.xres/2 + pixelcol/2];        // Some blue
                    *(fbp + location + 1) = arraypixel[1][y - vinfo.yres/2 + pixelrow/2][x - vinfo.xres/2 + pixelcol/2];     // A little green
                    *(fbp + location + 2) = arraypixel[0][y - vinfo.yres/2 + pixelrow/2][x - vinfo.xres/2 + pixelcol/2];    // A lot of red
                    *(fbp + location + 3) = 0;      // No transparency
            //location += 4;
            } else  { //assume 16bpp
                int b = 0;
                int g = 100;     // A little green
                int r = 0;    // A lot of red
                unsigned short int t = r<<11 | g << 5 | b;
                *((unsigned short int*)(fbp + location)) = t;
            }
        }
    int i=pixelrow + vinfo.yres/2 - pixelrow/2;
    //printf("%d\n",i);
    int count = 0;
    do{
        i--;       
        count++;
        for (y = vinfo.yres/2 - pixelrow/2; y < pixelrow + vinfo.yres/2 - pixelrow/2; y++){
            //printf("%d\n",y);
            for (x = vinfo.xres/2 - pixelcol/2; x < pixelcol + vinfo.xres/2 - pixelcol/2; x++) {
                location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
                //printf("location: %ld\n",location);
                //printf("%d %d\n",y,i);
                //usleep(400000);
                if(y>i-1){
                    *(fbp + location) = 0;      // Some blue
                    *(fbp + location + 1) = 0;   // A little green
                    *(fbp + location + 2) = 0;    // A lot of red
                    *(fbp + location + 3) = 0;      // No transparency
                }else if (vinfo.bits_per_pixel == 32){ 
                //4byte
                    *(fbp + location) = arraypixel[2][(y - vinfo.yres/2 + pixelrow/2 + count) % pixelrow +1][x - vinfo.xres/2 + pixelcol/2];        // Some blue
                    *(fbp + location + 1) = arraypixel[1][(y - vinfo.yres/2 + pixelrow/2 + count) % pixelrow +1][x - vinfo.xres/2 + pixelcol/2];     // A little green
                    *(fbp + location + 2) = arraypixel[0][(y - vinfo.yres/2 + pixelrow/2 + count) % pixelrow +1][x - vinfo.xres/2 + pixelcol/2];    // A lot of red
                    *(fbp + location + 3) = 0;      // No transparency
                //location += 4;
                }
                else  { //assume 16bpp
                    int b = 0;
                    int g = 100;     // A little green
                    int r = 0;    // A lot of red
                    unsigned short int t = r<<11 | g << 5 | b;
                    *((unsigned short int*)(fbp + location)) = t;
                }
            }
        }
        //printf("masuk\n");    
        usleep(40000);    
    }while(!(i==vinfo.yres/2 - pixelrow/2));
    munmap(fbp, screensize);
    close(fbfd);
return 0;
}