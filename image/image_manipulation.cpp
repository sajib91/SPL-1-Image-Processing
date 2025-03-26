
#include <iostream>
#include <vector>
#include <graphics.h>
#include <fstream>
#include <cmath>

using namespace std;

struct Color {
    float b, g, r;
};

struct Image {
    int width;
    int height;
    vector<Color> colors;
    vector<Color> colors_modified;
};

Color create_color(float b, float g, float r) {
    Color color;
    color.b = b;
    color.g = g;
    color.r = r;
    return color;
}

Image* create_image(int width, int height) {
    Image* img = new Image;
    img->width = width;
    img->height = height;
    img->colors = vector<Color>(width * height);
    img->colors_modified = vector<Color>(width * height);
    return img;
}

void destroy_image(Image* img) {
    delete img;
}

Color get_color(const Image* img, int x, int y) {
    return img->colors_modified[y * img->width + x];
}

void read_image(Image* img);
void print_menu();
void negative(Image* img);
void grayscale(Image* img);
void brightening(Image* img);
void flip(Image* img);
void sharpen(Image* img);
void smoothing(Image* img);
void darken(Image* img);
void gaussian_blur(Image* img);
void sobel_edge_detection(Image* img);
void angle_calculation(Image* img);
void histogram(Image* img);
void show_image(const Image* img, const char* path);
void export_image(const Image* img, const char* path);

void read_image(Image* img) {
    string path;
    cout << "Enter Image: ";
    cin >> path;

    ifstream f;
    f.open(path, std::ios::in | std::ios::binary);

    if (!f.is_open()) {
        cout << "File can't be opened!" << endl;
        return;
    }

    const int fileHeaderSize = 14;
    const int infoHeaderSize = 40;

    unsigned char fileHeader[fileHeaderSize];
    f.read(reinterpret_cast<char*>(fileHeader), fileHeaderSize);

    unsigned char infoHeader[infoHeaderSize];
    f.read(reinterpret_cast<char*>(infoHeader), infoHeaderSize);

    const int fileSize = fileHeader[2] + (fileHeader[3] << 8) + (fileHeader[4] << 16) + (fileHeader[5] << 24);
    img->width = infoHeader[4] + (infoHeader[5] << 8) + (infoHeader[6] << 16) + (infoHeader[7] << 24);
    img->height = infoHeader[8] + (infoHeader[9] << 8) + (infoHeader[10] << 16) + (infoHeader[11] << 24);

    if (fileHeader[0] != 'B' && fileHeader[1] != 'M') {
        cout << "Invalid BMP file format!" << endl;
        return;
    }

    const int bytesPerPixel = 3;
    int paddingAmount = (4 - ((img->width * bytesPerPixel) % 4)) % 4;

    img->colors.resize(img->width * img->height);

    for (int y = 0; y < img->height; ++y) {
        for (int x = 0; x < img->width; ++x) {
            unsigned char color[3];

            f.read(reinterpret_cast<char*>(color), 3);

            img->colors[y * img->width + x].r = static_cast<float>(color[2]);
            img->colors[y * img->width + x].g = static_cast<float>(color[1]);
            img->colors[y * img->width + x].b = static_cast<float>(color[0]);
        }
        f.ignore(paddingAmount);
    }
    f.close();

    cout << "\n\n\n" << endl;
    cout << "\t\t\t!!----Image Reading Complete---!!" << endl;

}

void show_image(const char* path) {
    int heightOfScreen = GetSystemMetrics(SM_CXSCREEN);
    int widthOfScreen = GetSystemMetrics(SM_CYSCREEN);
    char nameOfImage[100];
    snprintf(nameOfImage, sizeof(nameOfImage), "%s", path);
    initwindow(800, 700, nameOfImage, 0, 0);
    readimagefile(nameOfImage, 200, 200, 600, 600);

    delay(3000);
    closegraph();
}

void negative(Image* img) {
    img->colors_modified = vector<Color>(img->height * img->width);

    for (int y = 0; y < img->height; ++y) {
        for (int x = 0; x < img->width; ++x) {
            float r = 0, g = 0, b = 0;

            b = abs(255 - img->colors[y * img->width + x].b);
            g = abs(255 - img->colors[y * img->width + x].g);
            r = abs(255 - img->colors[y * img->width + x].r);

            img->colors_modified[y * img->width + x].b = b;
            img->colors_modified[y * img->width + x].g = g;
            img->colors_modified[y * img->width + x].r = r;
        }
    }

    cout << "\n" << endl;
    cout << "\t\t\t!!----Image Negative Complete---!!" << endl;
    cout << "\n" << endl;

    cout << "\tCreating Negative Image......" << endl;
    cout << "\tNegative Image Created named \"NegativeImage.bmp\"......" << endl;
    export_image(img, "NegativeImage.bmp");
}

void grayscale(Image* img) {
    img->colors_modified = vector<Color>(img->height * img->width);

    float avg = 0;
    for (int y = 0; y < img->height; ++y) {
        for (int x = 0; x < img->width; ++x) {
            float r = 0, g = 0, b = 0;

            b = img->colors[y * img->width + x].b;
            g = img->colors[y * img->width + x].g;
            r = img->colors[y * img->width + x].r;

            avg = (r + g + b) / 3;

            img->colors_modified[y * img->width + x].b = avg;
            img->colors_modified[y * img->width + x].g = avg;
            img->colors_modified[y * img->width + x].r = avg;
        }
    }

    cout << "\n" << endl;
    cout << "\t\t\t!!----Image Grayscale Complete---!!" << endl;
    cout << "\n" << endl;

    cout << "\tCreating Grayscale Image......" << endl;
    cout << "\tGrayscale Image Created named \"GrayscaleImage.bmp\"......" << endl;
    export_image(img, "GrayscaleImage.bmp");
}

void brightening(Image* img) {
    img->colors_modified = vector<Color>(img->height * img->width);

    cout << "---------------------Brightening--------------------\n\n";
    cout << "Input the brightness level between -255 to +255: ";
    double level;
    cin >> level;

    if (level > 255 || level < -255) {
        cout << "Invalid brightness level";
        return;
    } else {
        for (int y = 0; y < img->height; y++) {
            for (int x = 0; x < img->width; x++) {
                double r = 0, g = 0, b = 0;

                r = img->colors[y * img->width + x].r;
                r = r + min(level, (255 - r));
                img->colors_modified[y * img->width + x].r = max(0.0, r);

                g = img->colors[y * img->width + x].g;
                g = g + min(level, (255 - g));
                img->colors_modified[y * img->width + x].g = max(0.0, g);

                b = img->colors[y * img->width + x].b;
                b = b + min(level, (255 - b));
                img->colors_modified[y * img->width + x].b = max(0.0, b);
            }
        }
    }

    cout << "\n" << endl;
    cout << "\t\t\t!!----Image Brightening Complete---!!" << endl;
    cout << "\n" << endl;

    cout << "\tCreating Brightening Image......" << endl;
    cout << "\tBrightening Image Created named \"BrighteningImage.bmp\"......" << endl;
    export_image(img, "BrighteningImage.bmp");
}

void darken(Image* img) {
    img->colors_modified = vector<Color>(img->height * img->width);

    cout << "Input the darkness level between -255 to +255: ";
    double level;
    cin >> level;

    if (level > 255 || level < -255) {
        cout << "Invalid darkness level";
        return;
    } else {
        for (int y = 0; y < img->height; y++) {
            for (int x = 0; x < img->width; x++) {
                double r = 0, g = 0, b = 0;

                r = img->colors[y * img->width + x].r;
                r = r - level;
                img->colors_modified[y * img->width + x].r = max(0.0, r);

                g = img->colors[y * img->width + x].g;
                g = g - level;
                img->colors_modified[y * img->width + x].g = max(0.0, g);

                b = img->colors[y * img->width + x].b;
                b = b - level;
                img->colors_modified[y * img->width + x].b = max(0.0, b);
            }
        }
    }

    cout << "\n" << endl;
    cout << "\t\t\t!!----Image Darkening Complete---!!" << endl;
    cout << "\n" << endl;

    cout << "\tCreating Darken Image......" << endl;
    cout << "\tDarken Image Created named \"DarkenImage.bmp\"......" << endl;
    export_image(img, "DarkenImage.bmp");
}

void flip(Image* img) {
    img->colors_modified = vector<Color>(img->height * img->width);

    cout << "\t\t\t\t!!----------Flipping Image----------!!" << endl;
    cout << "1-> Horizontal Flip" << endl;
    cout << "2-> Vertical Flip " << endl;

    int choice;
    cout << endl;
    cout << "Enter Choice: ";
    cin >> choice;

    if (choice == 1) {
        cout << "          Horizontal Flip       " << endl;

        for (int y = 0; y < img->height; ++y) {
            for (int x = 0; x < img->width / 2; ++x) {
                img->colors_modified[y * img->width + (img->width - x - 1)].r = img->colors[y * img->width + x].r;
                img->colors_modified[y * img->width + x].r = img->colors[y * img->width + (img->width - x - 1)].r;

                img->colors_modified[y * img->width + (img->width - x - 1)].g = img->colors[y * img->width + x].g;
                img->colors_modified[y * img->width + x].g = img->colors[y * img->width + (img->width - x - 1)].g;

                img->colors_modified[y * img->width + (img->width - x - 1)].b = img->colors[y * img->width + x].b;
                img->colors_modified[y * img->width + x].b = img->colors[y * img->width + (img->width - x - 1)].b;
            }
        }
    } else if (choice == 2) {
        cout << "          Vertical Flip " << endl;

        for (int y = 0; y < img->height / 2; ++y) {
            for (int x = 0; x < img->width; ++x) {
                img->colors_modified[(img->height - y - 1) * img->width + x].r = img->colors[y * img->width + x].r;
                img->colors_modified[y * img->width + x].r = img->colors[(img->height - y - 1) * img->width + x].r;

                img->colors_modified[(img->height - y - 1) * img->width + x].g = img->colors[y * img->width + x].g;
                img->colors_modified[y * img->width + x].g = img->colors[(img->height - y - 1) * img->width + x].g;

                img->colors_modified[(img->height - y - 1) * img->width + x].b = img->colors[y * img->width + x].b;
                img->colors_modified[y * img->width + x].b = img->colors[(img->height - y - 1) * img->width + x].b;
            }
        }
    }

    cout << "\n" << endl;
    cout << "\t\t\t!!----Image Flip Complete---!!" << endl;
    cout << "\n" << endl;

    cout << "\tCreating Flip Image......" << endl;
    cout << "\tFlip Image Created named \"FlipImage.bmp\"......" << endl;

     export_image(img, "FlipImage.bmp");
}


void smoothing(Image* img) {
    img->colors_modified = vector<Color>(img->height * img->width);

    float filter[3][3] = {
        {1, 1, 1},
        {1, 1, 1},
        {1, 1, 1}
    };


    for (int y = 1; y < img->height - 1; ++y) {
        for (int x = 1; x < img->width - 1; ++x) {
            float r = 0, g = 0, b = 0;

       for (int i = -1; i <= 1; ++i) {
    for (int j = -1; j <= 1; ++j) {
        r += img->colors[(y + i) * img->width + (x + j)].r;
        g += img->colors[(y + i) * img->width + (x + j)].g;
        b += img->colors[(y + i) * img->width + (x + j)].b;
    }
}
r /= 9;
g /= 9;
b /= 9;


            img->colors_modified[y * img->width + x].r = r;
            img->colors_modified[y * img->width + x].g = g;
            img->colors_modified[y * img->width + x].b = b;
        }
    }

    cout << "\n" << endl;
    cout << "\t\t\t!!----Image Smoothing Complete---!!" << endl;
    cout << "\n" << endl;

    cout << "\tCreating Smooth Image......" << endl;
    cout << "\tSmooth Image Created named \"SmoothImage.bmp\"......" << endl;

     export_image(img, "SmoothImage.bmp");
}


void sharpen(Image* img) {
    img->colors_modified = vector<Color>(img->height * img->width);

    int kernel[4][9] = {
        {0, 1, 0, 1, -4, 1, 0, 1, 0},
        {0, -1, 0, -1, 4, -1, 0, -1, 0},
        {1, 1, 1, 1, -8, 1, 1, 1, 1},
        {-1, -1, -1, -1, 8, -1, -1, -1, -1},
    };

    cout << "\n\nChoose Filter: " << endl;
    cout << "\n1 -> Laplacian filter.\n";
    cout << "2 -> Strong Laplacian filter.\n";
    cout << "3 -> High Boost Filter.\n";
    cout << "4 -> Strong High Boost Filter.\n";

    float filter[3][3];
    int choice;
    int index = 0;

    cout << "choice: ";
    cin >> choice;
    choice--;

    cout << "\n\n\n" << endl;

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            filter[i][j] = kernel[choice][index];
            index++;
        }
    }

    for (int y = 1; y < img->height - 1; ++y) {
        for (int x = 1; x < img->width - 1; ++x) {
            double r = 0, g = 0, b = 0;

            r += img->colors[(y - 1) * img->width + (x - 1)].r * filter[0][0] + img->colors[(y - 1) * img->width + x].r * filter[0][1] + img->colors[(y - 1) * img->width + (x + 1)].r * filter[0][2];
            g += img->colors[(y - 1) * img->width + (x - 1)].g * filter[0][0] + img->colors[(y - 1) * img->width + x].g * filter[0][1] + img->colors[(y - 1) * img->width + (x + 1)].g * filter[0][2];
            b += img->colors[(y - 1) * img->width + (x - 1)].b * filter[0][0] + img->colors[(y - 1) * img->width + x].b * filter[0][1] + img->colors[(y - 1) * img->width + (x + 1)].b * filter[0][2];

            r += img->colors[y * img->width + (x - 1)].r * filter[1][0] + img->colors[y * img->width + x].r * filter[1][1] + img->colors[y * img->width + (x + 1)].r * filter[1][2];
            g += img->colors[y * img->width + (x - 1)].g * filter[1][0] + img->colors[y * img->width + x].g * filter[1][1] + img->colors[y * img->width + (x + 1)].g * filter[1][2];
            b += img->colors[y * img->width + (x - 1)].b * filter[1][0] + img->colors[y * img->width + x].b * filter[1][1] + img->colors[y * img->width + (x + 1)].b * filter[1][2];

            r += img->colors[(y + 1) * img->width + (x - 1)].r * filter[2][0] + img->colors[(y + 1) * img->width + x].r * filter[2][1] + img->colors[(y + 1) * img->width + (x + 1)].r * filter[2][2];
            g += img->colors[(y + 1) * img->width + (x - 1)].g * filter[2][0] + img->colors[(y + 1) * img->width + x].g * filter[2][1] + img->colors[(y + 1) * img->width + (x + 1)].g * filter[2][2];
            b += img->colors[(y + 1) * img->width + (x - 1)].b * filter[2][0] + img->colors[(y + 1) * img->width + x].b * filter[2][1] + img->colors[(y + 1) * img->width + (x + 1)].b * filter[2][2];

            r = min(255.0, r);
            r = max(0.0, r);

            g = min(255.0, g);
            g = max(0.0, g);

            b = min(255.0, b);
            b = max(0.0, b);

            img->colors_modified[y * img->width + x].r = r;
            img->colors_modified[y * img->width + x].g = g;
            img->colors_modified[y * img->width + x].b = b;
        }
    }

    cout << "\n" << endl;
    cout << "\t\t\t!!----Image Sharpen Complete---!!" << endl;
    cout << "\n" << endl;

    cout << "\tCreating Sharpen Image......" << endl;
    cout << "\tSharpen Image Created named \"SharpenImage.bmp\"......" << endl;

     export_image(img, "SharpenImage.bmp");
}

void gaussian_blur(Image* img) {
    img->colors_modified = vector<Color>(img->height * img->width);

    cout << "\t\t\t\t!!----------Gaussian Blur----------!!" << endl;
    cout << "1-> (3 x 3) Gaussian Blur" << endl;
    cout << "2-> (5 x 5) Gaussian Blur" << endl;

    int choice;
    cout << endl;
    cout << "Enter Choice: ";
    cin >> choice;

    if (choice == 1) {
        int filter[3][3] = {
            {1, 2, 1},
            {2, 4, 2},
            {1, 2, 1},
        };

        for (int y = 1; y < img->height - 1; ++y) {
            for (int x = 1; x < img->width - 1; ++x) {
                float r = 0, g = 0, b = 0;

                r += img->colors[(y - 1) * img->width + (x - 1)].r * filter[0][0] + img->colors[(y - 1) * img->width + x].r * filter[0][1] + img->colors[(y - 1) * img->width + (x + 1)].r * filter[0][2];
                g += img->colors[(y - 1) * img->width + (x - 1)].g * filter[0][0] + img->colors[(y - 1) * img->width + x].g * filter[0][1] + img->colors[(y - 1) * img->width + (x + 1)].g * filter[0][2];
                b += img->colors[(y - 1) * img->width + (x - 1)].b * filter[0][0] + img->colors[(y - 1) * img->width + x].b * filter[0][1] + img->colors[(y - 1) * img->width + (x + 1)].b * filter[0][2];

                r += img->colors[y * img->width + (x - 1)].r * filter[1][0] + img->colors[y * img->width + x].r * filter[1][1] + img->colors[y * img->width + (x + 1)].r * filter[1][2];
                g += img->colors[y * img->width + (x - 1)].g * filter[1][0] + img->colors[y * img->width + x].g * filter[1][1] + img->colors[y * img->width + (x + 1)].g * filter[1][2];
                b += img->colors[y * img->width + (x - 1)].b * filter[1][0] + img->colors[y * img->width + x].b * filter[1][1] + img->colors[y * img->width + (x + 1)].b * filter[1][2];

                r += img->colors[(y + 1) * img->width + (x - 1)].r * filter[2][0] + img->colors[(y + 1) * img->width + x].r * filter[2][1] + img->colors[(y + 1) * img->width + (x + 1)].r * filter[2][2];
                g += img->colors[(y + 1) * img->width + (x - 1)].g * filter[2][0] + img->colors[(y + 1) * img->width + x].g * filter[2][1] + img->colors[(y + 1) * img->width + (x + 1)].g * filter[2][2];
                b += img->colors[(y + 1) * img->width + (x - 1)].b * filter[2][0] + img->colors[(y + 1) * img->width + x].b * filter[2][1] + img->colors[(y + 1) * img->width + (x + 1)].b * filter[2][2];

                r /= 16;
                g /= 16;
                b /= 16;

                img->colors_modified[y * img->width + x].r = r;
                img->colors_modified[y * img->width + x].g = g;
                img->colors_modified[y * img->width + x].b = b;
            }
        }
    } else if (choice == 2) {
        int filter[5][5] = {
            {1, 4, 6, 4, 1},
            {4, 16, 24, 16, 4},
            {6, 24, 36, 24, 6},
            {4, 16, 24, 16, 4},
            {1, 4, 6, 4, 1},
        };

        for (int y = 2; y < img->height - 2; ++y) {
            for (int x = 2; x < img->width - 2; ++x) {
                float r = 0, g = 0, b = 0;

                r += img->colors[(y - 2) * img->width + (x - 2)].r * filter[0][0] + img->colors[(y - 2) * img->width + (x - 1)].r * filter[0][1] + img->colors[(y - 2) * img->width + x].r * filter[0][2] + img->colors[(y - 2) * img->width + (x + 1)].r * filter[0][3] + img->colors[(y - 2) * img->width + (x + 2)].r * filter[0][4];
                g += img->colors[(y - 2) * img->width + (x - 2)].g * filter[0][0] + img->colors[(y - 2) * img->width + (x - 1)].g * filter[0][1] + img->colors[(y - 2) * img->width + x].g * filter[0][2] + img->colors[(y - 2) * img->width + (x + 1)].g * filter[0][3] + img->colors[(y - 2) * img->width + (x + 2)].g * filter[0][4];
                b += img->colors[(y - 2) * img->width + (x - 2)].b * filter[0][0] + img->colors[(y - 2) * img->width + (x - 1)].b * filter[0][1] + img->colors[(y - 2) * img->width + x].b * filter[0][2] + img->colors[(y - 2) * img->width + (x + 1)].b * filter[0][3] + img->colors[(y - 2) * img->width + (x + 2)].b * filter[0][4];

                r += img->colors[(y - 1) * img->width + (x - 2)].r * filter[1][0] + img->colors[(y - 1) * img->width + (x - 1)].r * filter[1][1] + img->colors[(y - 1) * img->width + x].r * filter[1][2] + img->colors[(y - 1) * img->width + (x + 1)].r * filter[1][3] + img->colors[(y - 1) * img->width + (x + 2)].r * filter[1][4];
                g += img->colors[(y - 1) * img->width + (x - 2)].g * filter[1][0] + img->colors[(y - 1) * img->width + (x - 1)].g * filter[1][1] + img->colors[(y - 1) * img->width + x].g * filter[1][2] + img->colors[(y - 1) * img->width + (x + 1)].g * filter[1][3] + img->colors[(y - 1) * img->width + (x + 2)].g * filter[1][4];
                b += img->colors[(y - 1) * img->width + (x - 2)].b * filter[1][0] + img->colors[(y - 1) * img->width + (x - 1)].b * filter[1][1] + img->colors[(y - 1) * img->width + x].b * filter[1][2] + img->colors[(y - 1) * img->width + (x + 1)].b * filter[1][3] + img->colors[(y - 1) * img->width + (x + 2)].b * filter[1][4];

                r += img->colors[y * img->width + (x - 2)].r * filter[2][0] + img->colors[y * img->width + (x - 1)].r * filter[2][1] + img->colors[y * img->width + x].r * filter[2][2] + img->colors[y * img->width + (x + 1)].r * filter[2][3] + img->colors[y * img->width + (x + 2)].r * filter[2][4];
                g += img->colors[y * img->width + (x - 2)].g * filter[2][0] + img->colors[y * img->width + (x - 1)].g * filter[2][1] + img->colors[y * img->width + x].g * filter[2][2] + img->colors[y * img->width + (x + 1)].g * filter[2][3] + img->colors[y * img->width + (x + 2)].g * filter[2][4];
                b += img->colors[y * img->width + (x - 2)].b * filter[2][0] + img->colors[y * img->width + (x - 1)].b * filter[2][1] + img->colors[y * img->width + x].b * filter[2][2] + img->colors[y * img->width + (x + 1)].b * filter[2][3] + img->colors[y * img->width + (x + 2)].b * filter[2][4];

                r += img->colors[(y + 1) * img->width + (x - 2)].r * filter[3][0] + img->colors[(y + 1) * img->width + (x - 1)].r * filter[3][1] + img->colors[(y + 1) * img->width + x].r * filter[3][2] + img->colors[(y + 1) * img->width + (x + 1)].r * filter[3][3] + img->colors[(y + 1) * img->width + (x + 2)].r * filter[3][4];
                g += img->colors[(y + 1) * img->width + (x - 2)].g * filter[3][0] + img->colors[(y + 1) * img->width + (x - 1)].g * filter[3][1] + img->colors[(y + 1) * img->width + x].g * filter[3][2] + img->colors[(y + 1) * img->width + (x + 1)].g * filter[3][3] + img->colors[(y + 1) * img->width + (x + 2)].g * filter[3][4];
                b += img->colors[(y + 1) * img->width + (x - 2)].b * filter[3][0] + img->colors[(y + 1) * img->width + (x - 1)].b * filter[3][1] + img->colors[(y + 1) * img->width + x].b * filter[3][2] + img->colors[(y + 1) * img->width + (x + 1)].b * filter[3][3] + img->colors[(y + 1) * img->width + (x + 2)].b * filter[3][4];

                r += img->colors[(y + 2) * img->width + (x - 2)].r * filter[4][0] + img->colors[(y + 2) * img->width + (x - 1)].r * filter[4][1] + img->colors[(y + 2) * img->width + x].r * filter[4][2] + img->colors[(y + 2) * img->width + (x + 1)].r * filter[4][3] + img->colors[(y + 2) * img->width + (x + 2)].r * filter[4][4];
                g += img->colors[(y + 2) * img->width + (x - 2)].g * filter[4][0] + img->colors[(y + 2) * img->width + (x - 1)].g * filter[4][1] + img->colors[(y + 2) * img->width + x].g * filter[4][2] + img->colors[(y + 2) * img->width + (x + 1)].g * filter[4][3] + img->colors[(y + 2) * img->width + (x + 2)].g * filter[4][4];
                b += img->colors[(y + 2) * img->width + (x - 2)].b * filter[4][0] + img->colors[(y + 2) * img->width + (x - 1)].b * filter[4][1] + img->colors[(y + 2) * img->width + x].b * filter[4][2] + img->colors[(y + 2) * img->width + (x + 1)].b * filter[4][3] + img->colors[(y + 2) * img->width + (x + 2)].b * filter[4][4];

                r /= 256;
                g /= 256;
                b /= 256;

                img->colors_modified[y * img->width + x].r = r;
                img->colors_modified[y * img->width + x].g = g;
                img->colors_modified[y * img->width + x].b = b;
            }
        }
    } else {
        cout << "Invalid Input!!" << endl;
        cout << "\tTry Again." << endl;
        gaussian_blur(img);
    }

    cout << "\n" << endl;
    cout << "\t\t\t!!----Gaussian Blur Complete---!!" << endl;
    cout << "\n" << endl;

    cout << "\tCreating Blur Image......" << endl;
    cout << "\tBlur Image Created named \"GaussianBlurImage.bmp\"......" << endl;

     export_image(img, "GaussianBlurImage.bmp");
}

void sobel_edge_detection(Image* img) {
    img->colors_modified = vector<Color>(img->height * img->width);
    vector<Color> colors_avg = vector<Color>(img->height * img->width);

    float avg = 0;
    for (int y = 0; y < img->height; ++y) {
        for (int x = 0; x < img->width; ++x) {
            float r = 0, g = 0, b = 0;

            b = img->colors[y * img->width + x].b;
            g = img->colors[y * img->width + x].g;
            r = img->colors[y * img->width + x].r;

            avg = (r + g + b) / 3;

            colors_avg[y * img->width + x].b = avg;
            colors_avg[y * img->width + x].g = avg;
            colors_avg[y * img->width + x].r = avg;
        }
    }

    int gx[3][3] = {{1, 0, -1}, {2, 0, -2}, {1, 0, -1}};
    int gy[3][3] = {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};

    int upThreshold;
    cout << "Enter Threshold value for Edge Detection: ";
    cin >> upThreshold;

    for (int y = 0; y < img->height; ++y) {
        for (int x = 0; x < img->width; ++x) {
            if (y != 0 && y != img->height - 1 && x != 0 && x != img->width - 1) {
                float Rx = 0, Gx = 0, Bx = 0;

                Rx += colors_avg[(y - 1) * img->width + (x - 1)].r * gx[0][0] + colors_avg[(y - 1) * img->width + x].r * gx[0][1] + colors_avg[(y - 1) * img->width + (x + 1)].r * gx[0][2];
                Gx += colors_avg[(y - 1) * img->width + (x - 1)].g * gx[0][0] + colors_avg[(y - 1) * img->width + x].g * gx[0][1] + colors_avg[(y - 1) * img->width + (x + 1)].g * gx[0][2];
                Bx += colors_avg[(y - 1) * img->width + (x - 1)].b * gx[0][0] + colors_avg[(y - 1) * img->width + x].b * gx[0][1] + colors_avg[(y - 1) * img->width + (x + 1)].b * gx[0][2];

                Rx += colors_avg[y * img->width + (x - 1)].r * gx[1][0] + colors_avg[y * img->width + x].r * gx[1][1] + colors_avg[y * img->width + (x + 1)].r * gx[1][2];
                Gx += colors_avg[y * img->width + (x - 1)].g * gx[1][0] + colors_avg[y * img->width + x].g * gx[1][1] + colors_avg[y * img->width + (x + 1)].g * gx[1][2];
                Bx += colors_avg[y * img->width + (x - 1)].b * gx[1][0] + colors_avg[y * img->width + x].b * gx[1][1] + colors_avg[y * img->width + (x + 1)].b * gx[1][2];

                Rx += colors_avg[(y + 1) * img->width + (x - 1)].r * gx[2][0] + colors_avg[(y + 1) * img->width + x].r * gx[2][1] + colors_avg[(y + 1) * img->width + (x + 1)].r * gx[2][2];
                Gx += colors_avg[(y + 1) * img->width + (x - 1)].g * gx[2][0] + colors_avg[(y + 1) * img->width + x].g * gx[2][1] + colors_avg[(y + 1) * img->width + (x + 1)].g * gx[2][2];
                Bx += colors_avg[(y + 1) * img->width + (x - 1)].b * gx[2][0] + colors_avg[(y + 1) * img->width + x].b * gx[2][1] + colors_avg[(y + 1) * img->width + (x + 1)].b * gx[2][2];

                double Ry = 0, Gy = 0, By = 0;

                Ry += colors_avg[(y - 1) * img->width + (x - 1)].r * gy[0][0] + colors_avg[(y - 1) * img->width + x].r * gy[0][1] + colors_avg[(y - 1) * img->width + (x + 1)].r * gy[0][2];
                Gy += colors_avg[(y - 1) * img->width + (x - 1)].g * gy[0][0] + colors_avg[(y - 1) * img->width + x].g * gy[0][1] + colors_avg[(y - 1) * img->width + (x + 1)].g * gy[0][2];
                By += colors_avg[(y - 1) * img->width + (x - 1)].b * gy[0][0] + colors_avg[(y - 1) * img->width + x].b * gy[0][1] + colors_avg[(y - 1) * img->width + (x + 1)].b * gy[0][2];

                Ry += colors_avg[y * img->width + (x - 1)].r * gy[1][0] + colors_avg[y * img->width + x].r * gy[1][1] + colors_avg[y * img->width + (x + 1)].r * gy[1][2];
                Gy += colors_avg[y * img->width + (x - 1)].g * gy[1][0] + colors_avg[y * img->width + x].g * gy[1][1] + colors_avg[y * img->width + (x + 1)].g * gy[1][2];
                By += colors_avg[y * img->width + (x - 1)].b * gy[1][0] + colors_avg[y * img->width + x].b * gy[1][1] + colors_avg[y * img->width + (x + 1)].b * gy[1][2];

                Ry += colors_avg[(y + 1) * img->width + (x - 1)].r * gy[2][0] + colors_avg[(y + 1) * img->width + x].r * gy[2][1] + colors_avg[(y + 1) * img->width + (x + 1)].r * gy[2][2];
                Gy += colors_avg[(y + 1) * img->width + (x - 1)].g * gy[2][0] + colors_avg[(y + 1) * img->width + x].g * gy[2][1] + colors_avg[(y + 1) * img->width + (x + 1)].g * gy[2][2];
                By += colors_avg[(y + 1) * img->width + (x - 1)].b * gy[2][0] + colors_avg[(y + 1) * img->width + x].b * gy[2][1] + colors_avg[(y + 1) * img->width + (x + 1)].b * gy[2][2];

                double sqrtBlue = sqrt(Bx * Bx + By * By);
                double sqrtGreen = sqrt(Gx * Gx + Gy * Gy);
                double sqrtRed = sqrt(Rx * Rx + Ry * Ry);

                if (sqrtBlue > upThreshold) {
                    sqrtBlue = 255;
                } else {
                    sqrtBlue = 0;
                }

                if (sqrtGreen > upThreshold) {
                    sqrtGreen = 255;
                } else {
                    sqrtGreen = 0;
                }

                if (sqrtRed > upThreshold) {
                    sqrtRed = 255;
                } else {
                    sqrtRed = 0;
                }

                img->colors_modified[y * img->width + x].b = sqrtBlue;
                img->colors_modified[y * img->width + x].g = sqrtGreen;
                img->colors_modified[y * img->width + x].r = sqrtRed;
            } else {
                img->colors_modified[y * img->width + x].r = 0;
                img->colors_modified[y * img->width + x].g = 0;
                img->colors_modified[y * img->width + x].b = 0;
            }
        }
    }

    cout << "\n" << endl;
    cout << "\t\t\t!!----Sobel EdgeDetection Complete---!!" << endl;
    cout << "\n" << endl;

    cout << "\tCreating Edge Detection Image......" << endl;
    cout << "\tEdge Detection Image Created named \"SobelImage.bmp\"......" << endl;
    // Assuming export_image is defined elsewhere
    export_image(img, "SobelImage.bmp");
}

void angle_calculation(Image* img) {
    img->colors_modified = vector<Color>(img->height * img->width);

    int gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int gy[3][3] = {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};

    int upThreshold;
    cout << "Enter Threshold upper value for Angle Detection: ";
    cin >> upThreshold;

    int downThreshold;
    cout << "Enter Threshold lower value for Angle Detection: ";
    cin >> downThreshold;

    for (int y = 1; y < img->height - 1; ++y) {
        for (int x = 1; x < img->width - 1; ++x) {
            float Rx = 0, Gx = 0, Bx = 0;

            Rx += img->colors[(y - 1) * img->width + (x - 1)].r * gx[0][0] + img->colors[(y - 1) * img->width + x].r * gx[0][1] + img->colors[(y - 1) * img->width + (x + 1)].r * gx[0][2];
            Gx += img->colors[(y - 1) * img->width + (x - 1)].g * gx[0][0] + img->colors[(y - 1) * img->width + x].g * gx[0][1] + img->colors[(y - 1) * img->width + (x + 1)].g * gx[0][2];
            Bx += img->colors[(y - 1) * img->width + (x - 1)].b * gx[0][0] + img->colors[(y - 1) * img->width + x].b * gx[0][1] + img->colors[(y - 1) * img->width + (x + 1)].b * gx[0][2];

            Rx += img->colors[y * img->width + (x - 1)].r * gx[1][0] + img->colors[y * img->width + x].r * gx[1][1] + img->colors[y * img->width + (x + 1)].r * gx[1][2];
            Gx += img->colors[y * img->width + (x - 1)].g * gx[1][0] + img->colors[y * img->width + x].g * gx[1][1] + img->colors[y * img->width + (x + 1)].g * gx[1][2];
            Bx += img->colors[y * img->width + (x - 1)].b * gx[1][0] + img->colors[y * img->width + x].b * gx[1][1] + img->colors[y * img->width + (x + 1)].b * gx[1][2];

            Rx += img->colors[(y + 1) * img->width + (x - 1)].r * gx[2][0] + img->colors[(y + 1) * img->width + x].r * gx[2][1] + img->colors[(y + 1) * img->width + (x + 1)].r * gx[2][2];
            Gx += img->colors[(y + 1) * img->width + (x - 1)].g * gx[2][0] + img->colors[(y + 1) * img->width + x].g * gx[2][1] + img->colors[(y + 1) * img->width + (x + 1)].g * gx[2][2];
            Bx += img->colors[(y + 1) * img->width + (x - 1)].b * gx[2][0] + img->colors[(y + 1) * img->width + x].b * gx[2][1] + img->colors[(y + 1) * img->width + (x + 1)].b * gx[2][2];

            double Ry = 0, Gy = 0, By = 0;

            Ry += img->colors[(y - 1) * img->width + (x - 1)].r * gy[0][0] + img->colors[(y - 1) * img->width + x].r * gy[0][1] + img->colors[(y - 1) * img->width + (x + 1)].r * gy[0][2];
            Gy += img->colors[(y - 1) * img->width + (x - 1)].g * gy[0][0] + img->colors[(y - 1) * img->width + x].g * gy[0][1] + img->colors[(y - 1) * img->width + (x + 1)].g * gy[0][2];
            By += img->colors[(y - 1) * img->width + (x - 1)].b * gy[0][0] + img->colors[(y - 1) * img->width + x].b * gy[0][1] + img->colors[(y - 1) * img->width + (x + 1)].b * gy[0][2];

            Ry += img->colors[y * img->width + (x - 1)].r * gy[1][0] + img->colors[y * img->width + x].r * gy[1][1] + img->colors[y * img->width + (x + 1)].r * gy[1][2];
            Gy += img->colors[y * img->width + (x - 1)].g * gy[1][0] + img->colors[y * img->width + x].g * gy[1][1] + img->colors[y * img->width + (x + 1)].g * gy[1][2];
            By += img->colors[y * img->width + (x - 1)].b * gy[1][0] + img->colors[y * img->width + x].b * gy[1][1] + img->colors[y * img->width + (x + 1)].b * gy[1][2];

            Ry += img->colors[(y + 1) * img->width + (x - 1)].r * gy[2][0] + img->colors[(y + 1) * img->width + x].r * gy[2][1] + img->colors[(y + 1) * img->width + (x + 1)].r * gy[2][2];
            Gy += img->colors[(y + 1) * img->width + (x - 1)].g * gy[2][0] + img->colors[(y + 1) * img->width + x].g * gy[2][1] + img->colors[(y + 1) * img->width + (x + 1)].g * gy[2][2];
            By += img->colors[(y + 1) * img->width + (x - 1)].b * gy[2][0] + img->colors[(y + 1) * img->width + x].b * gy[2][1] + img->colors[(y + 1) * img->width + (x + 1)].b * gy[2][2];

            float resultR, AngleR;
            if (Rx == 0) {
                AngleR = 90;
            } else {
                resultR = (Ry / Rx);
                resultR = atan(resultR);
                AngleR = (resultR * 180) / 3.1415;
            }

            float resultG, AngleG;
            if (Gx == 0) {
                AngleG = 90;
            } else {
                resultG = (Gy / Gx);
                resultG = atan(resultG);
                AngleG = (resultG * 180) / 3.1415;
            }

            float resultB, AngleB;
            if (Bx == 0) {
                AngleB = 90;
                continue;
            } else {
                resultB = (By / Bx);
                resultB = atan(resultB);
                AngleB = (resultB * 180) / 3.1415;
            }

            if (AngleR > upThreshold) {
                AngleR = 255;
            } else {
                AngleR = 0;
            }
            if (AngleG > upThreshold) {
                AngleG = 255;
            } else {
                AngleG = 0;
            }
            if (AngleB > upThreshold) {
                AngleB = 255;
            } else {
                AngleB = 0;
            }

            img->colors_modified[y * img->width + x].r = AngleR;
            img->colors_modified[y * img->width + x].g = AngleG;
            img->colors_modified[y * img->width + x].b = AngleB;
        }
    }

    cout << "---Angle---" << endl;

    cout << "\n" << endl;
    cout << "\t\t\t!!----Angle Calculation Complete---!!" << endl;
    cout << "\n" << endl;

    cout << "\tCreating Image......" << endl;
    cout << "\tImage Created named \"AngleImage.bmp\"......" << endl;
    // Assuming export_image is defined elsewhere
    export_image(img, "AngleImage.bmp");
}


void histogram(Image* img) {
    img->colors_modified = vector<Color>(img->height * img->width);

    for (int y = 0; y < img->height; ++y) {
        for (int x = 0; x < img->width; ++x) {
            float r = img->colors[y * img->width + x].r;
            float g = img->colors[y * img->width + x].g;
            float b = img->colors[y * img->width + x].b;

            int avg = (r + g + b) / 3;

            img->colors_modified[y * img->width + x].r = avg;
            img->colors_modified[y * img->width + x].g = avg;
            img->colors_modified[y * img->width + x].b = avg;
        }
    }

    int Hist[256] = {0};

    for (int y = 0; y < img->height; ++y) {
        for (int x = 0; x < img->width; ++x) {
            int val = img->colors_modified[y * img->width + x].r;
            Hist[val] += 1;
        }
    }

    long long int cdf[256] = {0};
    cdf[0] = Hist[0];

    for (int i = 1; i < 256; ++i) {
        cdf[i] = cdf[i - 1] + Hist[i];
    }

    int cdfMin = cdf[0];
    for (int i = 1; i < 256; ++i) {
        if (cdf[i] < cdfMin)
            cdfMin = cdf[i];
    }

    int histogram[256] = {0};

    for (int i = 0; i < 256; ++i) {
        histogram[i] = round((cdf[i] - cdfMin) * 255) / ((img->height * img->width) - cdfMin);
    }

    // Printing histogram
    printf("Histogram : \n\n");
    for (int i = 0; i < 256; i++) {
        if (i < 10) {
            printf("[%d]   | ", i);
            for (int j = 0; j <= (Hist[i] % 20); j++)
                printf("*");
            printf("-->(%d)", Hist[i]);
            printf("\t\t\t\t\t\t\t   ||Equalized value {%d}", histogram[i]);
            printf("\n");
        } else if (i < 100) {
            printf("[%d]  | ", i);
            for (int j = 0; j <= (Hist[i] % 20); j++)
                printf("*");
            printf("-->(%d)", Hist[i]);
            printf("\t\t\t\t\t\t\t   ||Equalized value {%d}", histogram[i]);
            printf("\n");
        } else if (i < 256) {
            printf("[%d] | ", i);
            for (int j = 0; j <= (Hist[i] % 20); j++)
                printf("*");
            printf("-->(%d)", Hist[i]);
            printf("\t\t\t\t\t\t\t   ||Equalized value {%d}", histogram[i]);
            printf("\n");
        }
    }
}

void export_image(const Image* img, const char* path) {
    ofstream f;
    f.open(path, std::ios::out | std::ios::binary);

    if (!f.is_open()) {
        cout << "File can't be opened!" << endl;
        return;
    }

    unsigned char bmpPad[3] = {0, 0, 0};
    const int fileHeaderSize = 14;
    const int infoHeaderSize = 40;
    const int bytesPerPixel = 3;
    int paddingAmount = (4 - ((img->width * bytesPerPixel) % 4)) % 4;
    const uint32_t fileSize = fileHeaderSize + infoHeaderSize + (img->width * img->height * bytesPerPixel) + paddingAmount * img->height;

    unsigned char fileHeader[fileHeaderSize];

    // file type
    fileHeader[0] = 'B';
    fileHeader[1] = 'M';

    // file size
    fileHeader[2] = fileSize;
    fileHeader[3] = fileSize >> 8;
    fileHeader[4] = fileSize >> 16;
    fileHeader[5] = fileSize >> 24;

    // reserved 1 (Not used)
    fileHeader[6] = 0;
    fileHeader[7] = 0;

    // reserved 2 (Not used)
    fileHeader[8] = 0;
    fileHeader[9] = 0;

    // pixel data offset
    fileHeader[10] = fileHeaderSize + infoHeaderSize;
    fileHeader[11] = 0;
    fileHeader[12] = 0;
    fileHeader[13] = 0;

    unsigned char infoHeader[infoHeaderSize];

    // Header size
    infoHeader[0] = infoHeaderSize;
    infoHeader[1] = 0;
    infoHeader[2] = 0;
    infoHeader[3] = 0;

    // image width
    infoHeader[4] = img->width;
    infoHeader[5] = img->width >> 8;
    infoHeader[6] = img->width >> 16;
    infoHeader[7] = img->width >> 24;

    // image height
    infoHeader[8] = img->height;
    infoHeader[9] = img->height >> 8;
    infoHeader[10] = img->height >> 16;
    infoHeader[11] = img->height >> 24;

    // planes
    infoHeader[12] = 1;
    infoHeader[13] = 0;

    // bits per pixel (RGB)
    infoHeader[14] = 24;
    infoHeader[15] = 0;

    // compression (No compression)
    infoHeader[16] = 0;
    infoHeader[17] = 0;
    infoHeader[18] = 0;
    infoHeader[19] = 0;

    // image size (no compression)
    infoHeader[20] = 0;
    infoHeader[21] = 0;
    infoHeader[22] = 0;
    infoHeader[23] = 0;

    // X pixels per meter (not specified)
    infoHeader[24] = 0;
    infoHeader[25] = 0;
    infoHeader[26] = 0;
    infoHeader[27] = 0;

    // Y pixels per meter (not specified)
    infoHeader[28] = 0;
    infoHeader[29] = 0;
    infoHeader[30] = 0;
    infoHeader[31] = 0;

    // total colors (color palette not used)
    infoHeader[32] = 0;
    infoHeader[33] = 0;
    infoHeader[34] = 0;
    infoHeader[35] = 0;

    // important colors (generally ignored)
    infoHeader[36] = 0;
    infoHeader[37] = 0;
    infoHeader[38] = 0;
    infoHeader[39] = 0;

    f.write(reinterpret_cast<char*>(fileHeader), fileHeaderSize);
    f.write(reinterpret_cast<char*>(infoHeader), infoHeaderSize);

    for (int y = 0; y < img->height; ++y) {
        for (int x = 0; x < img->width; ++x) {
            unsigned char r = static_cast<unsigned char>(get_color(img, x, y).r);
            unsigned char g = static_cast<unsigned char>(get_color(img, x, y).g);
            unsigned char b = static_cast<unsigned char>(get_color(img, x, y).b);

            unsigned char color[] = {b, g, r};
            f.write(reinterpret_cast<char*>(color), 3);
        }
        f.write(reinterpret_cast<char*>(bmpPad), paddingAmount);
    }

    f.close();
    cout << "File created" << endl;

    show_image(path);
}

void print_menu() {
    cout << "\n";
    cout << "\t\t\t";
    cout << "!!!----------Basic Image  Manipulation ----------!!!" << endl;

    cout << endl;
    cout << "1->Read Image" << endl;
    cout << "2->Negative" << endl;
    cout << "3->Grayscale" << endl;
    cout << "4->Brightening" << endl;
    cout << "5->Flip Image" << endl;
    cout << "6->Sharpen Image" << endl;
    cout << "7->Smoothing Image" << endl;
    cout << "8->Darkening" << endl;
    cout << "9->Gaussian Blur" << endl;
    cout << "10->Sobel Edge Detection" << endl;
    cout << "11->Angle Detection" << endl;
    cout << "12->Histogram" << endl;
    cout << "13->Export Image" << endl;
    cout << "0->Exit" << endl;
}



int main() {
    print_menu();

    bool flag = true;
    Image* img = create_image(0, 0);

    while (flag) {
        int choice;
        cout << endl;
        cout << "choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                read_image(img);
                break;
            case 2:
                negative(img);
                break;
            case 3:
                grayscale(img);
                break;
            case 4:
                brightening(img);
                break;
            case 5:
                flip(img);
                break;
            case 6:
                sharpen(img);
                break;
            case 7:
                smoothing(img);
                break;
            case 8:
                darken(img);
                break;
            case 9:
                gaussian_blur(img);
                break;
            case 10:
                sobel_edge_detection(img);
                break;
            case 11:
                angle_calculation(img);
                break;
            case 12:
                histogram(img);
                break;
            case 13:
                export_image(img, "TestImage.bmp");
                break;
            case 0:
                cout << "\t\t\t\t...Thank You for using Image Processing Tools..." << endl;
                flag = false;
                break;
            default:
                cout << "Invalid Input!" << endl;
                cout << "Try Again." << endl;
                flag = false;
        }
    }

    destroy_image(img);
    return 0;
}
