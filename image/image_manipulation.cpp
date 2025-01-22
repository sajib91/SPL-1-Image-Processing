
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
   export_image(img, "GrayscaleImage.bmp");
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

}

void smoothing(Image* img) {

}

void sharpen(Image* img) {

}

void gaussian_blur(Image* img) {

}

void sobel_edge_detection(Image* img) {

}

void angle_calculation(Image* img) {

}

void histogram(Image* img) {

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
