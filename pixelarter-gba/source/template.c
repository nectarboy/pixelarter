#define gba_screenw 240
#define gba_screenh 160

#define reg_vcount *(volatile unsigned char*)0x04000006

#define reg_vidmode *(volatile unsigned short*)0x04000000
#define val_vidmode_reset 0x0483

#define reg_vram        ((volatile unsigned short*)0x06000000)
#define reg_oam         ((volatile unsigned short*)0x07000000)
#define reg_obj_pal     ((volatile unsigned short*)0x05000200)
#define reg_obj_tiles   ((volatile unsigned short*)0x06010000)

#define reg_joypad *(volatile unsigned short*)0x04000130

// macros
#define rgb15(r,g,b) (((r)<<0) | ((g)<<5) | ((b)<<10))
#define bmpCoord(x, y) ((y) * gba_screenw + (x))

#define getBit(num, bit) (((num) >> (bit)) & 1)
    
// ------------------------ // VARIABLES
static const unsigned short bgColor = rgb15(28,28,28);
static const unsigned char pixelSize = 8;

static const unsigned short colors[] = {
    // basic colors
    rgb15(0, 0, 0),  // black
    rgb15(12,12,12), // dark grey
    rgb15(23,23,23), // lite gray
    rgb15(31,31,31), // white

    rgb15(31, 0, 0), // red
    rgb15(0, 31, 0), // green
    rgb15(0, 0, 31), // blue

    // pollen boy palletes
    rgb15(5, 5, 18), // kirby 3
    rgb15(14,6, 28), // kirby 2
    rgb15(28,16,16), // kirby 1
    rgb15(30,23,30), // kirby 0

    rgb15(6, 5, 10), // yoshi 3
    rgb15(9, 16,17), // yoshi 2
    rgb15(18,28, 8), // yoshi 1
    rgb15(27,30,28), // yoshi 0

    // rgb15(), // kirby 0
    // rgb15(), // kirby 1
    // rgb15(), // kirby 2
    // rgb15(), // kirby 3

    // aesthetic colors
    rgb15(31,28,17), // lemonade
    rgb15(31,28,25), // peach
};
static const int colorsCount = sizeof(colors) / sizeof(colors[0]); // 2 bytes in short

// player properties
int x = 0;
int y = 0;

int currentColorID = 0;
unsigned short colorOnSpaceBefore = bgColor; 

// ------------------------ // FUNCTIONS
void plotPixel(color) {
    int xx = x*pixelSize;
    int yy = y*pixelSize;

    colorOnSpaceBefore = reg_vram[bmpCoord(xx, yy)];

    for (int i = 0; i < pixelSize; i++) {
        for (int ii = 0; ii < pixelSize; ii++) {
            reg_vram[bmpCoord(xx+ii, yy+i)] = color;
        }
    }
}

void checkOOB() {
    unsigned char width = (gba_screenw/pixelSize)-1;
    unsigned char height = (gba_screenh/pixelSize)-1;

    if (x < 0) x = width;
    else if (x > width) x = 0;

    if (y < 0) y = height;
    else if (y > height) y = 0;
}

void switchColor(signed char amt) {
    currentColorID += amt;

    if (currentColorID < 0) currentColorID = colorsCount-1;
    else if (currentColorID > colorsCount-1) currentColorID = 0;

    unsigned short preColorBefore = colorOnSpaceBefore;
    plotPixel(colors[currentColorID]);
    colorOnSpaceBefore = preColorBefore;
}

void reset() {
    for (int i = 0; i < gba_screenw*gba_screenh; i++)
        reg_vram[i] = bgColor;

    x = gba_screenw/pixelSize/2;
    y = gba_screenh/pixelSize/2;
    //colorOnSpaceBefore = bgColor;

    plotPixel(colors[currentColorID]);
}

// ------------------------ // MAIN
int main() {

    // setup everything
    reg_vidmode = val_vidmode_reset;
    // *turn off fast blank
    reg_vidmode &= ~0x0080; 

    reset();

    // player properties
    char movedX = 0;
    char movedY = 0;

    char drawed = 0;
    char miscPressed = 0;

    while (1) {
        while (reg_vcount > gba_screenh);
        while (reg_vcount < gba_screenh);

        volatile unsigned short joypad = reg_joypad;

        // moving
        if (getBit(joypad, 4) == 0) {
            if (!movedX) {
                plotPixel(colorOnSpaceBefore);
                x++;
                checkOOB();
                plotPixel(colors[currentColorID]);

                movedX = 1;
            }
        }
        else if (getBit(joypad, 5) == 0) {
            if (!movedX) {
                plotPixel(colorOnSpaceBefore);
                x--;
                checkOOB();
                plotPixel(colors[currentColorID]);

                movedX = 1;
            }
        }
        else movedX = 0;

        if (getBit(joypad, 6) == 0) {
            if (!movedY) {
                plotPixel(colorOnSpaceBefore);
                y--;
                checkOOB();
                plotPixel(colors[currentColorID]);

                movedY = 1;
            }
        }
        else if (getBit(joypad, 7) == 0) {
            if (!movedY) {
                plotPixel(colorOnSpaceBefore);
                y++;
                checkOOB();
                plotPixel(colors[currentColorID]);

                movedY = 1;
            }
        }
        else movedY = 0;

        // placing
        if (getBit(joypad, 0) == 0) {
            if (!drawed || movedX || movedY) {
                colorOnSpaceBefore = colors[currentColorID];

                drawed = 1;
            }
        }
        // erasing
        if (getBit(joypad, 1) == 0) {
            if (!drawed || movedX || movedY) {
                colorOnSpaceBefore = bgColor;

                drawed = 1;
            }
        }
        else drawed = 0;

        // switch color right
        if (getBit(joypad, 8) == 0) {
            if (!miscPressed) {
                switchColor(1);
                miscPressed = 1;
            }
        }
        // switch color left
        else if (getBit(joypad, 9) == 0) {
            if (!miscPressed) {
                switchColor(-1);
                miscPressed = 1;
            }
        }
        // reset
        else if (getBit(joypad, 2) == 0) {
            if (!miscPressed) {
                reset();
                miscPressed = 1;
            }
        }
        else miscPressed = 0;

    }

    return 0;
}