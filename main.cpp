#include <iostream>
#include <vector>
#include <stdint.h>
#include "EasyBMP/EasyBMP.h"
#include <fstream>

// OneBit
// 2015.06.03
// by Brian Puthuff

// This program accepts, as input, a .BMP image file and exports
// a Black and White .BMP image. The program renders output as
// black and white pixels only and uses dithering to simulate
// shades. There is no gray scale.

// The program also generates an ASCII variant of the dithered
// image as a text file.


// The pattern class holds a dither pattern.
class Pattern
{
    public:
    Pattern(uint8_t r0, uint8_t r1, uint8_t r2 , uint8_t r3, uint8_t r4, uint8_t r5, uint8_t r6, uint8_t r7)
    {
        bits[0] = r0;
        bits[1] = r1;
        bits[2] = r2;
        bits[3] = r3;
        bits[4] = r4;
        bits[5] = r5;
        bits[6] = r6;
        bits[7] = r7;

        row = 0;
        col = 0xF0;
        index = 7;
    };

    // retrieve the current bit in the pattern
    uint8_t getBit(void)
    {
        uint8_t temp = bits[row] & col;
        temp = temp >> index;
        return temp;
    };

    // advance to the next bit to the right
    void nextCol()
    {
        col = col >> 1;
        index--;
        if(index < 0)
        {
            // reset columns
            index = 7;
            col = 0xF0;
        }
    };

    // advance to the next row below
    void nextRow()
    {
        row++;
        if(row > 7)
        {
            // reset rows
            row = 0;
        }
        // reset columns
        index = 7;
        col = 0xF0;
    };
    private:
    uint8_t bits[8];
    uint8_t row;
    uint8_t col;
    int8_t index;
};

// The pattern manager class holds a collection of patterns
// and serves as an interface to them.
class PatternManager
{
    public:
    PatternManager(void)
    {};

    ~PatternManager(void)
    {
        patterns.clear();
    };

    void addPattern(Pattern p)
    {
        patterns.push_back(p);
    };

    uint8_t getBit(uint8_t value)
    {
        uint8_t bit = 0;
        if(value < patterns.size())
        {
            bit = patterns[value].getBit();
            for(uint8_t index = 0; index < patterns.size(); index++)
            {
                // advance all patterns to next column bit position
                patterns[index].nextCol();
            }

        }
        return bit;
    };

    void nextRow(void)
    {
        for(uint8_t index = 0; index < patterns.size(); index++)
        {
            // advance all patterns to next row
            patterns[index].nextRow();
        }
    };

    private:
    std::vector<Pattern> patterns;
};

// function prototype
uint8_t reducePixelColor(uint8_t p);
char returnScaleCharacter(uint8_t p);

int main(int argc, char** argv)
{
    PatternManager pm;
    pm.addPattern(Pattern(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF)); //  0% BLACK
    pm.addPattern(Pattern(0xFF, 0xFF, 0xFF, 0xF7, 0xFF, 0xFF, 0xFF, 0x7F)); //  10%
    pm.addPattern(Pattern(0xEF, 0xFF, 0xFE, 0xFF, 0xEF, 0xFF, 0xFE, 0xFF)); //  20%
    pm.addPattern(Pattern(0x77, 0xDD, 0x77, 0xDD, 0x77, 0xDD, 0x77, 0xDD)); //  30%
    pm.addPattern(Pattern(0xEE, 0x55, 0xBB, 0x55, 0xEE, 0x55, 0xBB, 0x55)); //  40%
    pm.addPattern(Pattern(0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55)); //  50%
    pm.addPattern(Pattern(0xAA, 0x55, 0xAA, 0x51, 0xAA, 0x55, 0xAA, 0x15)); //  60%
    pm.addPattern(Pattern(0xAA, 0x44, 0xAA, 0x11, 0xAA, 0x44, 0xAA, 0x11)); //  70%
    pm.addPattern(Pattern(0x88, 0x00, 0x22, 0x00, 0x88, 0x00, 0x22, 0x00)); //  80%
    pm.addPattern(Pattern(0x80, 0x00, 0x08, 0x00, 0x80, 0x00, 0x08, 0x00)); //  90%
    pm.addPattern(Pattern(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)); // 100% WHITE

    BMP infile; //input file
    BMP outfile; // output file

    std::string gasname = argv[1];
    gasname.replace(gasname.length()-4, gasname.length(), "_GS.txt");
    std::ofstream outgas;
    outgas.open(gasname.c_str());


    infile.ReadFromFile(argv[1]);
    int width = infile.TellWidth();
    int height = infile.TellHeight();

    outfile.SetBitDepth(1);
    outfile.SetSize(width, height);

    RGBApixel pixel;
    RGBApixel black = {0, 0, 0};
    RGBApixel white = {255, 255, 255};

    // main conversion loop
    for(int r = 0; r < height; r++)
    {
        for(int c = 0; c < width; c++)
        {
            pixel = infile.GetPixel(c, r);
            uint8_t color = (uint8_t) (((float) pixel.Red * .3) + ((float) pixel.Green * .59) + ((float) pixel.Blue * .11));
            outgas << returnScaleCharacter(color);
            if(pm.getBit(reducePixelColor(color)) == 1)
                outfile.SetPixel(c, r, black);
            else
                outfile.SetPixel(c, r, white);
        }
        pm.nextRow();
        outgas << '\n';
    }

    std::string outname = argv[1];
    outname.insert(outname.length()-4, "_BW");
    outfile.WriteToFile(outname.c_str());
    outgas.close();

    return 0;
}

uint8_t reducePixelColor(uint8_t p)
{
    if(p < 12)
        return 0;
    else if(p < 38)
        return 1;
    else if(p < 64)
        return 2;
    else if(p < 90)
        return 3;
    else if(p < 116)
        return 4;
    else if(p < 140)
        return 5;
    else if(p < 166)
        return 6;
    else if(p < 192)
        return 7;
    else if(p < 218)
        return 8;
    else if(p < 244)
        return 9;
    else
        return 10;
};

char returnScaleCharacter(uint8_t p)
{
    if(p < 12)
        return '@';
    else if(p < 38)
        return '#';
    else if(p < 64)
        return '&';
    else if(p < 90)
        return '$';
    else if(p < 116)
        return '%';
    else if(p < 140)
        return 'Z';
    else if(p < 166)
        return '?';
    else if(p < 192)
        return '+';
    else if(p < 218)
        return '=';
    else if(p < 244)
        return '.';
    else
        return ' ';
}
