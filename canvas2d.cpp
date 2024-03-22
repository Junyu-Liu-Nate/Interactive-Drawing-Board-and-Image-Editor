#include "canvas2d.h"
#include <QPainter>
#include <QMessageBox>
#include <QFileDialog>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include "settings.h"

/**
 * @brief Initializes new 500x500 canvas
 */
void Canvas2D::init() {
    setMouseTracking(true);
    m_width = 500;
    m_height = 500;
    clearCanvas();

    setMask(); // Initialize the mask

    displayImage();
}

/**
 * @brief Canvas2D::clearCanvas sets all canvas pixels to blank white
 */
void Canvas2D::clearCanvas() {
    m_data.assign(m_width * m_height, RGBA{255, 255, 255, 255});
    settings.imagePath = "";
    displayImage();
}

/**
 * @brief Stores the image specified from the input file in this class's
 * `std::vector<RGBA> m_image`.
 * Also saves the image width and height to canvas width and height respectively.
 * @param file: file path to an image
 * @return True if successfully loads image, False otherwise.
 */
bool Canvas2D::loadImageFromFile(const QString &file) {
    QImage myImage;
    if (!myImage.load(file)) {
        std::cout<<"Failed to load in image"<<std::endl;
        return false;
    }
    myImage = myImage.convertToFormat(QImage::Format_RGBX8888);
    m_width = myImage.width();
    m_height = myImage.height();
    QByteArray arr = QByteArray::fromRawData((const char*) myImage.bits(), myImage.sizeInBytes());

    m_data.clear();
    m_data.reserve(m_width * m_height);
    for (int i = 0; i < arr.size() / 4.f; i++){
        m_data.push_back(RGBA{(std::uint8_t) arr[4*i], (std::uint8_t) arr[4*i+1], (std::uint8_t) arr[4*i+2], (std::uint8_t) arr[4*i+3]});
    }
    displayImage();
    return true;
}

/**
 * @brief Saves the current canvas image to the specified file path.
 * @param file: file path to save image to
 * @return True if successfully saves image, False otherwise.
 */
bool Canvas2D::saveImageToFile(const QString &file) {
    QImage myImage = QImage(m_width, m_height, QImage::Format_RGBX8888);
    for (int i = 0; i < m_data.size(); i++){
        myImage.setPixelColor(i % m_width, i / m_width, QColor(m_data[i].r, m_data[i].g, m_data[i].b, m_data[i].a));
    }
    if (!myImage.save(file)) {
        std::cout<<"Failed to save image"<<std::endl;
        return false;
    }
    return true;
}


/**
 * @brief Get Canvas2D's image data and display this to the GUI
 */
void Canvas2D::displayImage() {
    QByteArray* img = new QByteArray(reinterpret_cast<const char*>(m_data.data()), 4*m_data.size());
    QImage now = QImage((const uchar*)img->data(), m_width, m_height, QImage::Format_RGBX8888); // Changed from RGBX to RGBA!
    setPixmap(QPixmap::fromImage(now));
    setFixedSize(m_width, m_height);
    update();
}

/**
 * @brief Canvas2D::resize resizes canvas to new width and height
 * @param w
 * @param h
 */
void Canvas2D::resize(int w, int h) {
    m_width = w;
    m_height = h;
    m_data.resize(w * h);
    displayImage();
}

/**
 * @brief Called when the filter button is pressed in the UI
 */
void Canvas2D::filterImage() {
    // Filter TODO: apply the currently selected filter to the loaded image
    std::vector<float> kernel;
    std::vector<float> kernel1;
    std::vector<float> kernel2;
    switch (settings.filterType) {
        case FILTER_BLUR:
            kernel = filterBlur(settings.blurRadius);
            applyBlurFilter(kernel);
            break;

        case FILTER_EDGE_DETECT:
            filterGray();
            kernel1 = solbelFilter1();
            kernel2 = solbelFilter2();
            applySolbelFilter(kernel1, kernel2);
            break;

        case FILTER_SCALE:
            scale();
            break;

        case FILTER_MEDIAN:
            applyMedianFilter();
            break;

        case FILTER_MAPPING:
            if (!settings.nonLinearMap) {
                mappingLinear();
            }
            else {
                mappingNonLinear();
            }

            break;

        case FILTER_CHROMATIC:
            chromaticShift();
            break;

        case FILTER_ROTATION:
            rotate();
            break;

        case FILTER_BILATERAL:
            applyBilateralFilter();
            break;

        default:
            break;
    }
}

/*************************************************
 ****************** Helper Functions *************
 *************************************************/
std::uint8_t Canvas2D::floatToUint8(float x) {
    return round(x * 255.f);
}

std::uint8_t Canvas2D::rgbaToGray(const RGBA &pixel) {
    std::uint8_t gray = 0.299 * pixel.r + 0.587 * pixel.g + 0.114 * pixel.b;
    return gray;
}

float Canvas2D::clampValue(float value, float lowerBound, float upperBound) {
    if (value < lowerBound) {
        return 0.0;
    }
    if (value > upperBound) {
        return 255.0;
    }
    return value;
}

RGBA Canvas2D::rgbToYcbcr(RGBA color) {
    float y = 0.299 * color.r + 0.587 * color.g + 0.114 * color.b;
    float cb = -0.169 * color.r - 0.331 * color.g + 0.5 * color.b + 128;
    float cr = 0.5 * color.r - 0.419 * color.g - 0.081 * color.b + 128;

    y = clampValue(y, 0, 255);
    cb = clampValue(cb, 0, 255);
    cr = clampValue(cr, 0, 255);

    RGBA colorYcbcr = {floatToUint8(y / 255.0), floatToUint8(cb / 255.0), floatToUint8(cr / 255.0)};

    return colorYcbcr;
}

RGBA Canvas2D::ycbcrToRgb(RGBA color) {
    float r = color.r + 1.402 * (color.b - 128);
    float g = color.r - 0.344136 * (color.g - 128) - 0.714136 * (color.b - 128);
    float b = color.r + 1.772 * (color.g - 128);

    r = clampValue(r, 0, 255);
    g = clampValue(g, 0, 255);
    b = clampValue(b, 0, 255);

    RGBA colorRGB = {floatToUint8(r / 255.0), floatToUint8(g / 255.0), floatToUint8(b / 255.0)};

    return colorRGB;
}

// Repeats the pixel on the edge of the image such that A,B,C,D looks like ...A,A,A,B,C,D,D,D...
RGBA Canvas2D::getPixelRepeated(std::vector<RGBA> &data, int width, int height, int x, int y) {
    int newX = (x < 0) ? 0 : std::min(x, width  - 1);
    int newY = (y < 0) ? 0 : std::min(y, height - 1);
    return data[width * newY + newX];
}

// Flips the edge of the image such that A,B,C,D looks like ...C,B,A,B,C,D,C,B...
RGBA Canvas2D::getPixelReflected(std::vector<RGBA> &data, int width, int height, int x, int y) {
    int newX = (x < 0) ? -x : std::min(x, 2*width  - x);
    int newY = (y < 0) ? -y : std::min(y, 2*height  - y);
    return data[width * newY + newX];
}

// Wraps the image such that A,B,C,D looks like ...C,D,A,B,C,D,A,B...
RGBA Canvas2D::getPixelWrapped(std::vector<RGBA> &data, int width, int height, int x, int y) {
    int newX = (x < 0) ? x + width  : x % width;
    int newY = (y < 0) ? y + height : y % height;
    return data[width * newY + newX];
}


/*************************************************
 ****************** Convolution ******************
 *************************************************/
std::vector<RGBA> Canvas2D::convolve1DHorizontal(std::vector<RGBA> &data, int width, int height, const std::vector<float> &kernel, std::vector<int> channels) {
    std::vector<RGBA> result;
    result.assign(m_width * m_height, RGBA{255, 255, 255, 255});

    int dimension = kernel.size();

    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            size_t centerIndex = r * width + c;

            float redAcc = 0.0;
            float greenAcc = 0.0;
            float blueAcc = 0.0;

            int startX = c - dimension / 2;

            for (int filterIdx = 0; filterIdx < dimension; filterIdx++) {
                float weight = kernel.at(filterIdx);
                RGBA pixel = getPixelWrapped(data, width, height, startX + filterIdx, r);
                redAcc += pixel.r * weight;
                greenAcc += pixel.g * weight;
                blueAcc += pixel.b * weight;
            }

            if (channels.at(0) == 0) {
                redAcc = data.at(r * width + c).r;
            }
            if (channels.at(1) == 0) {
                greenAcc = data.at(r * width + c).g;
            }
            if (channels.at(2) == 0) {
                blueAcc = data.at(r * width + c).b;
            }

            redAcc = clampValue(redAcc, 0, 255);
            greenAcc = clampValue(greenAcc, 0, 255);
            blueAcc = clampValue(blueAcc, 0, 255);

            RGBA filteredColor = {floatToUint8(redAcc / 255.0), floatToUint8(greenAcc / 255.0), floatToUint8(blueAcc / 255.0)};
            result.at(centerIndex) = filteredColor;
        }
    }

    return result;
}


std::vector<RGBA> Canvas2D::convolve1DVertical(std::vector<RGBA> &data, int width, int height, const std::vector<float> &kernel, std::vector<int> channels) {
    std::vector<RGBA> result;
    result.assign(m_width * m_height, RGBA{255, 255, 255, 255});

    int dimension = kernel.size();

    for (int c = 0; c < width; c++) {
        for (int r = 0; r < height; r++) {
            size_t centerIndex = r * width + c;

            float redAcc = 0.0;
            float greenAcc = 0.0;
            float blueAcc = 0.0;

            int startY = r - dimension / 2;

            for (int filterIdx = 0; filterIdx < dimension; filterIdx++) {
                float weight = kernel.at(filterIdx);
                RGBA pixel = getPixelWrapped(data, width, height, c, startY + filterIdx);
                redAcc += pixel.r * weight;
                greenAcc += pixel.g * weight;
                blueAcc += pixel.b * weight;
            }

            if (channels.at(0) == 0) {
                redAcc = data.at(r * width + c).r;
            }
            if (channels.at(1) == 0) {
                greenAcc = data.at(r * width + c).g;
            }
            if (channels.at(2) == 0) {
                blueAcc = data.at(r * width + c).b;
            }

            redAcc = clampValue(redAcc, 0, 255);
            greenAcc = clampValue(greenAcc, 0, 255);
            blueAcc = clampValue(blueAcc, 0, 255);

            RGBA filteredColor = {floatToUint8(redAcc / 255.0), floatToUint8(greenAcc / 255.0), floatToUint8(blueAcc / 255.0)};
            result.at(centerIndex) = filteredColor;
        }
    }

    return result;
}


std::vector<RGBA> Canvas2D::median2D(std::vector<RGBA> &data, int width, int height, int kernelSize) {
    std::vector<RGBA> result;
    result.assign(m_width * m_height, RGBA{0, 0, 0, 255});

    int dimension = int(std::sqrt(kernelSize));

    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            size_t centerIndex = r * width + c;

            std::vector<int> rValues;
            rValues.assign(kernelSize, 0);
            std::vector<int> gValues;
            gValues.assign(kernelSize, 0);
            std::vector<int> bValues;
            bValues.assign(kernelSize, 0);

            int startX = c - dimension / 2 - 1;
            int startY = r - dimension / 2 - 1;

            for (int rFilter = dimension - 1; rFilter >= 0; rFilter--) {
                for (int cFilter = dimension - 1; cFilter >= 0; cFilter--) {
                    RGBA pixel = getPixelWrapped(data, width, height, startX + dimension - cFilter, startY + dimension - rFilter);
                    rValues.at(cFilter + rFilter * dimension) = pixel.r;
                    gValues.at(cFilter + rFilter * dimension) = pixel.g;
                    bValues.at(cFilter + rFilter * dimension) = pixel.b;
                }
            }

            std::sort(rValues.begin(), rValues.end());
            std::sort(gValues.begin(), gValues.end());
            std::sort(bValues.begin(), bValues.end());
            int rMedian = rValues.at(kernelSize / 2);
            int gMedian = gValues.at(kernelSize / 2);
            int bMedian = bValues.at(kernelSize / 2);

            RGBA filteredColor = {floatToUint8(rMedian / 255.0), floatToUint8(gMedian / 255.0), floatToUint8(bMedian / 255.0)};
            result.at(centerIndex) = filteredColor;
        }
    }

    return result;
}

std::vector<RGBA> Canvas2D::bilateral2D(std::vector<RGBA> &data, int width, int height, int kernelSize) {
    std::vector<RGBA> result;
    result.assign(m_width * m_height, RGBA{0, 0, 0, 255});

    int dimension = int(std::sqrt(kernelSize));

    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            size_t centerIndex = r * width + c;

            RGBA centerPixel = m_data.at(r * width + c);

            double weightSum = 0;

            float redAcc = 0.0;
            float greenAcc = 0.0;
            float blueAcc = 0.0;

            int startX = c - dimension / 2;
            int startY = r - dimension / 2;

            for (int rFilter = 0; rFilter < dimension; rFilter++) {
                for (int cFilter = 0; cFilter < dimension; cFilter++) {
                    RGBA pixel = getPixelWrapped(data, width, height, startX + cFilter, startY + rFilter);

                    int xDifference = startX + cFilter - c;
                    int yDifference = startY + rFilter - r;
                    double spatialGaussianWeight = filterSpatialGaussian(xDifference, yDifference, 3.0);

                    int rDifference = pixel.r - centerPixel.r;
                    int gDifference = pixel.g - centerPixel.g;
                    int bDifference = pixel.b - centerPixel.b;
                    double intensityDifference = sqrt(rDifference * rDifference + gDifference * gDifference + bDifference * bDifference);
                    double intensityGaussianWeight = filterRangeGaussian(intensityDifference, 30.0);

                    double weight = spatialGaussianWeight * intensityGaussianWeight;
                    weightSum += weight;

                    redAcc += pixel.r * weight;
                    greenAcc += pixel.g * weight;
                    blueAcc += pixel.b * weight;
                }
            }

            redAcc = redAcc / weightSum;
            greenAcc = greenAcc / weightSum;
            blueAcc = blueAcc / weightSum;

            RGBA filteredColor = {floatToUint8(redAcc / 255.0), floatToUint8(greenAcc / 255.0), floatToUint8(blueAcc / 255.0)};
            result.at(centerIndex) = filteredColor;
        }
    }

    return result;
}

/*************************************************
 ****************** Apply Filters ****************
 *************************************************/
void Canvas2D::upDateCanvas(std::vector<RGBA> result) {
    for (int i = 0; i < m_width*m_height; i++) {
        m_data.at(i) = result.at(i);
    }
}

std::vector<float> Canvas2D::normalizeFilter(std::vector<float> kernel) {
    float sum = 0.0;
    for (int i = 0; i < kernel.size(); i++) {
        sum = sum + kernel.at(i);
    }
    for (int i = 0; i < kernel.size(); i++) {
        kernel.at(i) = kernel.at(i) / sum;
    }
    return kernel;
}

void Canvas2D::applyBlurFilter(std::vector<float> kernel) {
    std::vector<int> channels = {1, 1, 1};
    std::vector<RGBA> resultH = convolve1DHorizontal(m_data, m_width, m_height, kernel, channels);
    std::vector<RGBA> resultV = convolve1DVertical(resultH, m_width, m_height, kernel, channels);
    upDateCanvas(resultV);
    displayImage();
}

void Canvas2D::applySolbelFilter(std::vector<float> kernel1, std::vector<float> kernel2) {
    filterGray();

    std::vector<float> kernel1r = {1, 2, 1};
    std::vector<float> kernel2r = {1, 0, -1};

    std::vector<int> channels = {1, 1, 1};
    std::vector<RGBA> result1H = convolve1DHorizontal(m_data, m_width, m_height, kernel1, channels);
    std::vector<RGBA> result1V = convolve1DVertical(result1H, m_width, m_height, kernel2, channels);

    std::vector<RGBA> result1Hr = convolve1DHorizontal(m_data, m_width, m_height, kernel1r, channels);
    std::vector<RGBA> result1Vr = convolve1DVertical(result1H, m_width, m_height, kernel2r, channels);

    std::vector<RGBA> result2H = convolve1DHorizontal(m_data, m_width, m_height, kernel2, channels);
    std::vector<RGBA> result2V = convolve1DVertical(result2H, m_width, m_height, kernel1, channels);

    std::vector<RGBA> result2Hr = convolve1DHorizontal(m_data, m_width, m_height, kernel2r, channels);
    std::vector<RGBA> result2Vr = convolve1DVertical(result2Hr, m_width, m_height, kernel1r, channels);

    std::vector<RGBA> result;
    RGBA initialColor = {255, 255, 255};
    result.assign(m_width * m_height, initialColor);

    float maxIntensity = 0;
    for (int i = 0; i < m_width * m_height; i++) {
        int intensity1 = result1V.at(i).r;
        int intensity1r = result1Vr.at(i).r;
        int intensity2 = result2V.at(i).r;
        int intensity2r = result2Vr.at(i).r;

        float intensityResult = settings.edgeDetectSensitivity * sqrt(intensity1*intensity1 + intensity1r*intensity1r + intensity2*intensity2 + intensity2r*intensity2r);
//        float intensityResult = settings.edgeDetectSensitivity * sqrt(intensity1*intensity1 + intensity1r*intensity1r);
//        float intensityResult = settings.edgeDetectSensitivity * sqrt(intensity2*intensity2 + intensity2r*intensity2r);


        if (intensityResult < 0) {
            intensityResult = 0;
        }
        if (intensityResult > 255) {
            intensityResult = 255;
        }

        if (intensityResult > maxIntensity) {
            maxIntensity = intensityResult;
        }

        RGBA filteredColor = {floatToUint8(intensityResult / 255.0), floatToUint8(intensityResult / 255.0), floatToUint8(intensityResult / 255.0)};
        result.at(i) = filteredColor;
    }


//    for (RGBA& color: result) {
//        color.r *= (255.0f / maxIntensity);
//        color.g *= (255.0f / maxIntensity);
//        color.b *= (255.0f / maxIntensity);
//    }

    upDateCanvas(result);
    displayImage();
}


void Canvas2D::scale() {
    int newWidth = int(m_width * settings.scaleX);
    int newHeight = int(m_height * settings.scaleY);

    // Horizontally adjust the width, fix height
    std::vector<RGBA> resultWidth;
    RGBA initialColor = {255, 255, 255};
    resultWidth.assign(newWidth * m_height, initialColor);

    double a = settings.scaleX;
    for (int r = 0; r < m_height; r++) {
        for (int c = 0; c < newWidth; c++) {
            float center = c / a + (1 - a) / (2 * a);

            double radius;
            if (a < 1) {
                radius = 1.0 / a;
            }
            else {
                radius = 1.0;
            }

            int left = ceil(center - radius);
            int right = floor(center + radius);

            double sumR = 0;
            double sumG = 0;
            double sumB = 0;
            double weightSum = 0;
            for (int i = left; i <= right; i++) {
                double weight = triFilter(i - center, a);
                RGBA sourcePixel = getPixelWrapped(m_data, m_width, m_height, i, r);
                sumR += weight * sourcePixel.r;
                sumG += weight * sourcePixel.g;
                sumB += weight * sourcePixel.b;
                weightSum += weight;
            }

            double newR = sumR / weightSum;
            double newG = sumG / weightSum;
            double newB = sumB / weightSum;
            RGBA newColor = {floatToUint8(newR / 255.0), floatToUint8(newG / 255.0), floatToUint8(newB / 255.0)};

            resultWidth.at(r * newWidth + c) = newColor;
        }
    }

    // Vertically adjust the height, fix width
    std::vector<RGBA> resultHeight;
    resultHeight.assign(newWidth * newHeight, initialColor);

    a = settings.scaleY;
    for (int c = 0; c < newWidth; c++) {
        for (int r = 0; r < newHeight; r++) {
            float center = r / a + (1 - a) / (2 * a);

            double radius;
            if (a < 1) {
                radius = 1.0 / a;
            }
            else {
                radius = 1.0;
            }

            int left = ceil(center - radius);
            int right = floor(center + radius);

            double sumR = 0;
            double sumG = 0;
            double sumB = 0;
            double weightSum = 0;
            for (int i = left; i <= right; i++) {
                double weight = triFilter(i - center, a);
                RGBA sourcePixel = getPixelWrapped(resultWidth, newWidth, m_height, c, i);
                sumR += weight * sourcePixel.r;
                sumG += weight * sourcePixel.g;
                sumB += weight * sourcePixel.b;
                weightSum += weight;
            }

            double newR = sumR / weightSum;
            double newG = sumG / weightSum;
            double newB = sumB / weightSum;
            RGBA newColor = {floatToUint8(newR / 255.0), floatToUint8(newG / 255.0), floatToUint8(newB / 255.0)};

            resultHeight.at(r * newWidth + c) = newColor;
        }
    }

    m_width = newWidth;
    m_height = newHeight;
    m_data.assign(m_width * m_height, initialColor);
    for (int i = 0; i < m_width * m_height; i++) {
        m_data.at(i) = resultHeight.at(i);
    }
    displayImage();
}

void Canvas2D::applyMedianFilter() {
    std::vector<RGBA> result;
    RGBA initialColor = {255, 255, 255};
    result.assign(m_width * m_height, initialColor);

    int kernelSize = (settings.medianRadius * 2 + 1) * (settings.medianRadius * 2 + 1);
    result = median2D(m_data, m_width, m_height, kernelSize);

    upDateCanvas(result);
    displayImage();
}

void Canvas2D::mappingLinear() {
    std::vector<RGBA> m_colorYcbcr;
    RGBA initialColor = {255, 255, 255};
    m_colorYcbcr.assign(m_width * m_height, initialColor);

    int yMin = 255;
    int yMax = 0;
    for (int i = 0; i < m_width * m_height; i++) {
        RGBA colorYcbcr = rgbToYcbcr(m_data.at(i));
        m_colorYcbcr.at(i) = colorYcbcr;
        if (int(colorYcbcr.r) > yMax) {
            yMax = int(colorYcbcr.r);
        }
        if (int(colorYcbcr.r) < yMin) {
            yMin = int(colorYcbcr.r);
        }
    }

    std::vector<RGBA> result;
    result.assign(m_width * m_height, initialColor);

    for (int i = 0; i < m_width * m_height; i++) {
        float y = m_colorYcbcr.at(i).r;
        float yMapped = (y - yMin) / (yMax - yMin) * 255;
        m_colorYcbcr.at(i).r = floatToUint8(yMapped / 255.0);
        RGBA colorRGB =  ycbcrToRgb(m_colorYcbcr.at(i));
        result.at(i) = colorRGB;
    }

    upDateCanvas(result);
    displayImage();
}

void Canvas2D::mappingNonLinear() {
    std::vector<RGBA> result;
    RGBA initialColor = {255, 255, 255};
    result.assign(m_width * m_height, initialColor);

    float gamma = settings.gamma;
    for (int i = 0; i < m_width * m_height; i++) {
        float rNew = 255 * std::pow(m_data.at(i).r / 255.0, gamma);
        float gNew = 255 * std::pow(m_data.at(i).g / 255.0, gamma);
        float bNew = 255 * std::pow(m_data.at(i).b / 255.0, gamma);
        rNew = clampValue(rNew, 0, 255);
        gNew = clampValue(gNew, 0, 255);
        bNew = clampValue(bNew, 0, 255);

        RGBA newColor = {floatToUint8(rNew / 255.0), floatToUint8(gNew / 255.0), floatToUint8(bNew / 255.0)};
        result.at(i) = newColor;
    }

    upDateCanvas(result);
    displayImage();
}

void Canvas2D::chromaticShift() {
    RGBA initialColor = {255, 255, 255};

    std::vector<RGBA> resultR;
    resultR.assign(m_width * m_height, initialColor);
    std::vector<int> channelsR = {1, 1, 1};
    std::vector<float> kernelR = filterShift(settings.rShift);
    resultR = convolve1DHorizontal(m_data, m_width, m_height, kernelR, channelsR);
    resultR = convolve1DVertical(resultR, m_width, m_height, kernelR, channelsR);

    std::vector<RGBA> resultG;
    resultG.assign(m_width * m_height, initialColor);
    std::vector<int> channelsG = {1, 1, 1};
    std::vector<float> kernelG = filterShift(settings.gShift);
    resultG = convolve1DHorizontal(m_data, m_width, m_height, kernelG, channelsG);
    resultG = convolve1DVertical(resultG, m_width, m_height, kernelG, channelsG);

    std::vector<RGBA> resultB;
    resultB.assign(m_width * m_height, initialColor);
    std::vector<int> channelsB = {1, 1, 1};
    std::vector<float> kernelB = filterShift(settings.bShift);
    resultB = convolve1DHorizontal(m_data, m_width, m_height, kernelB, channelsB);
    resultB = convolve1DVertical(resultB, m_width, m_height, kernelB, channelsB);

    std::vector<RGBA> result;
    result.assign(m_width * m_height, initialColor);
    for (int i = 0; i < m_width * m_height; i++) {
        RGBA shiftedColor = {resultR.at(i).r, resultG.at(i).g, resultB.at(i).b};
        result.at(i) = shiftedColor;
    }

    upDateCanvas(result);
    displayImage();
}

void Canvas2D::rotate() {
    float angle = settings.rotationAngle * (M_PI / 180.0f);

    int widthNew = std::abs(m_width * cos(angle)) + std::abs(m_height * sin(angle));
    int heightNew = std::abs(m_width * sin(angle)) + std::abs(m_height * cos(angle));

    std::vector<RGBA> result;
    RGBA initialColor = {0, 0, 0};
    result.assign(widthNew * heightNew, initialColor);

    for (int y = 0; y < heightNew; y++) {
        for (int x = 0; x < widthNew; x++) {
            int xTranslate = x - widthNew / 2;
            int yTranslate = y - heightNew / 2;

//            int xSourceTranslate = xTranslate * cos(angle) + yTranslate * sin(angle);
//            int ySourceTranslate = -1 * xTranslate * sin(angle) + yTranslate * cos(angle);

            int xSourceTranslate = xTranslate * cos(angle) - yTranslate * sin(angle);
            int ySourceTranslate = xTranslate * sin(angle) + yTranslate * cos(angle);

            int xSource = xSourceTranslate + m_width / 2;
            int ySource = ySourceTranslate + m_height / 2;

            if (xSource < 0 || xSource >= m_width || ySource < 0 || ySource >= m_height) {
                continue;
            }
            else {
                result.at(y * widthNew + x) = m_data.at(ySource * m_width + xSource);
            }

        }
    }

    m_width = widthNew;
    m_height = heightNew;
    m_data.assign(m_width * m_height, initialColor);
    for (int i = 0; i < m_width * m_height; i++) {
        m_data.at(i) = result.at(i);
    }
    displayImage();
}

void Canvas2D::applyBilateralFilter() {
    std::vector<RGBA> result;
    RGBA initialColor = {255, 255, 255};
    result.assign(m_width * m_height, initialColor);

    int kernelSize = (settings.bilateralRadius * 2 + 1) * (settings.bilateralRadius * 2 + 1);
    result = bilateral2D(m_data, m_width, m_height, kernelSize);

    upDateCanvas(result);
    displayImage();
}

/********************************************
 ****************** Filters *****************
 ********************************************/
void Canvas2D::filterGray() {
    for (int row = 0; row <m_height; ++row) {
        for (int col = 0; col < m_width; ++col) {
            size_t currentIndex = m_width * row + col;
            RGBA &currentPixel = m_data[currentIndex];

            std::uint8_t temp = rgbaToGray(currentPixel);

            currentPixel.r = temp;
            currentPixel.g = temp;
            currentPixel.b = temp;
        }
    }
}

std::vector<float> Canvas2D::filterBlur(int radius) {
    float filterHeight = 1.0 / (radius + 1);

    std::vector<float> kernel;
    kernel.assign(2*radius + 1, 0.0);

    for (int i = 0; i < radius; i++) {
        kernel.at(i) = 1.0 * (i + 1) / ((radius + 1) * (radius + 1));
    }
    kernel.at(radius) = filterHeight;
    for (int i = radius + 1; i < 2*radius + 1; i++) {
        kernel.at(i) = filterHeight - 1.0 * (i-radius) / ((radius + 1) * (radius + 1));
    }

    kernel = normalizeFilter(kernel);

    return kernel;
}

std::vector<float> Canvas2D::solbelFilter1() {
    std::vector<float> kernel = {1, 2, 1};
//    kernel = normalizeFilter(kernel);

    return kernel;
}

std::vector<float> Canvas2D::solbelFilter2() {
    std::vector<float> kernel = {-1, 0, 1};

    return kernel;
}

double Canvas2D::triFilter(double x, double a) {
    double radius;
    if (a < 1) {
        radius = 1.0 / a;
    }
    else {
        radius = 1.0;
    }

    if (x < -radius || x > radius) {
        return 0;
    }
    else {
        return (1 - fabs(x) / radius) / radius;
    }
}

std::vector<float> Canvas2D::filterMedian(int radius) {
    float filterHeight = 1.0 / (radius + 1);

    std::vector<float> kernel;
    kernel.assign(2*radius + 1, 0.0);

    for (int i = 0; i < 2*radius + 1; i++) {
        kernel.at(i) = filterHeight;
    }

    kernel = normalizeFilter(kernel);

    return kernel;
}

std::vector<float> Canvas2D::filterShift(int radius) {
    std::vector<float> kernel;

    if (radius >= 0) {
        kernel.assign(2*radius + 1, 0.0);

        for (int i = 0; i < 2*radius; i++) {
            kernel.at(i) = 0;
        }
        kernel.at(2*radius) = 1;
    }
    else {
        radius = -radius;

        kernel.assign(2*radius + 1, 0.0);

        kernel.at(0) = 1;
        for (int i = 1; i < 2*radius + 1; i++) {
            kernel.at(i) = 0;
        }
    }


//    kernel = normalizeFilter(kernel);

    return kernel;
}

float Canvas2D::filterSpatialGaussian(int x, int y, double sigma) {
    float weight = exp(-((x * x + y * y) / (2 * sigma * sigma)));
    return weight;
}

float Canvas2D::filterRangeGaussian(double intensityDifference, double sigma) {
    float weight = exp(-(intensityDifference * intensityDifference) / (2 * sigma * sigma));
    return weight;
}


/************************* Below are code for Brush Project ************************/

/**
 * @brief Called when any of the parameters in the UI are modified.
 */
void Canvas2D::settingsChanged() {
    // this saves your UI settings locally to load next time you run the program
    settings.saveSettings();

    setting_isChanged = true;
}

void Canvas2D::setMask() {
    switch (settings.brushType) {
        case BRUSH_CONSTANT:
                mask = constMask(settings.brushRadius);
                break;

        case BRUSH_LINEAR:
                mask = linearMask(settings.brushRadius);
                break;

        case BRUSH_QUADRATIC:
                mask = quadraticMask(settings.brushRadius);
                break;

        case BRUSH_SMUDGE:
                mask = quadraticMask(settings.brushRadius);
                break;

        case BRUSH_SPRAY:
                mask = sprayMask(settings.brushRadius);
                break;

        case BRUSH_SPEED:
                break;

        case BRUSH_FILL:
                break;

        case BRUSH_CUSTOM:
                break;

        default:
                mask = constMask(settings.brushRadius);
                break;
    }
}

/**
 * @brief These functions are called when the mouse is clicked and dragged on the canvas
 */
void Canvas2D::mouseDown(int x, int y) {
    // Only when mouse is down, change the mask
    if (setting_isChanged) {
        setMask();
        setting_isChanged = false; // Freeze the mask until the settings are changed again
    }

    m_isDown = true;
    RGBA sampleColor;
    storeColor(x, y, settings.brushRadius);

    brushTrack.assign(m_width * m_height, 0.0); // For each new stroke, start a new track

    switch (settings.brushType) {
        case BRUSH_SMUDGE:
            applySmudgeMask(mask, x, y, settings.brushRadius);
            break;

        case BRUSH_SPEED:
            storePosition = std::array<int, 2>{x, y};
            mask = speedMask(settings.brushRadius); // The initial radius don't change
            applyMask(mask, x, y, settings.brushRadius, settings.brushColor);
            break;

        case BRUSH_FILL:
            sampleColor = m_data[0];
            if (checkEmpty(sampleColor)) {
                m_data.assign(m_width * m_height, settings.brushColor);
                return;
            }
            else {
                fill(x, y);
            }
            break;

        default:
            applyMask(mask, x, y, settings.brushRadius, settings.brushColor);
            break;
    }

    displayImage();
}

void Canvas2D::mouseDragged(int x, int y) {
    if (m_isDown) {
        float unitSpeed;
        int newRadius;
        float scale;

        switch (settings.brushType) {
            case BRUSH_SMUDGE:
                applySmudgeMask(mask, x, y, settings.brushRadius);
                storeColor(x, y, settings.brushRadius);
                break;

            case BRUSH_SPEED:
                unitSpeed = sqrt((x-storePosition[0])*(x-storePosition[0]) + (y-storePosition[1])*(y-storePosition[1]));
                scale = 0.6;

                newRadius = static_cast<int>((1.0 - scale) * settings.brushRadius + scale * settings.brushRadius / (unitSpeed + 1.0));
                mask = speedMask(newRadius);
                applyMask(mask, x, y, newRadius, settings.brushColor);
                storePosition = std::array<int, 2>{x, y};
                break;

            default:
                applyMask(mask, x, y, settings.brushRadius, settings.brushColor);
                break;
        }

        displayImage();
    }
}

void Canvas2D::mouseUp(int x, int y) {
    m_isDown = false;

    brushTrack.assign(m_width * m_height, 0.0); // For each new stroke, start a new track

    displayImage();
}

/**************************************************************************************
 * General function to apply masks and blend
**************************************************************************************/
void Canvas2D::applyMask(std::vector<float> mask, int x, int y, int radius, RGBA brushColor) {
    int mMin = x - radius;
    int mMax = x + radius;
    int nMin = y - radius;
    int nMax = y + radius;

    if (mMin < 0) {
        mMin = 0;
    }
    if (mMax >= m_width) {
        mMax = m_width - 1;
    }
    if (nMin < 0) {
        nMin = 0;
    }
    if (nMax >= m_height) {
        nMax = m_height - 1;
    }

    for (int m = mMin; m < mMax; m++) {
        for (int n = nMin; n < nMax; n++) {
            int maskIdx = canvasToMask(std::array<int, 2>{m, n}, std::array<int, 2>{x, y}, radius);
            RGBA currentColor =  m_data.at(posToIndex(m, n, m_width));
            float maskValue = mask.at(maskIdx);

            // When Fixed alpha blending is activated
            float alpha = static_cast<float>(brushColor.a) / static_cast<float>(255);
            if (settings.fixAlphaBlending & (brushTrack.at(posToIndex(m, n, m_width)) >= alpha)) {
                maskValue = 0.0;
            }

            RGBA newColor = blendColor(currentColor, brushColor, maskValue);
            m_data.at(posToIndex(m, n, m_width)) = newColor;

            // When Fixed alpha blending is activated
            if (settings.fixAlphaBlending & (brushTrack.at(posToIndex(m, n, m_width)) < alpha)) {
                brushTrack.at(posToIndex(m, n, m_width)) += maskValue * alpha;
            }
            // TODO: Still show some strange affect, not perfect yet!!!
        }
    }
}

RGBA Canvas2D::blendColor(RGBA currentColor, RGBA brushColor, float maskValue) {
    float alpha = static_cast<float>(brushColor.a) / static_cast<float>(255);
    std::uint8_t blendR = brushColor.r * maskValue * alpha + currentColor.r * (1 - maskValue * alpha) + 0.5f;
    std::uint8_t blendG = brushColor.g * maskValue * alpha + currentColor.g * (1 - maskValue * alpha) + 0.5f;
    std::uint8_t blendB = brushColor.b * maskValue * alpha + currentColor.b * (1 - maskValue * alpha) + 0.5f;

    RGBA blendColor = {blendR, blendG, blendB};
    return blendColor;
}

/**************************************************************************************
 * Special functions for Smudge Brush
**************************************************************************************/
RGBA Canvas2D::blendSmudgeColor(RGBA currentColor, RGBA brushColor, float maskValue) {
    std::uint8_t blendR = brushColor.r * maskValue + currentColor.r * (1 - maskValue) + 0.5f;
    std::uint8_t blendG = brushColor.g * maskValue + currentColor.g * (1 - maskValue) + 0.5f;
    std::uint8_t blendB = brushColor.b * maskValue + currentColor.b * (1 - maskValue) + 0.5f;

    RGBA blendColor = {blendR, blendG, blendB};
    return blendColor;
}

void Canvas2D::applySmudgeMask(std::vector<float> mask, int x, int y, int radius) {
    int mMin = x - radius;
    int mMax = x + radius;
    int nMin = y - radius;
    int nMax = y + radius;

    if (mMin < 0) {
        mMin = 0;
    }
    if (mMax >= m_width) {
        mMax = m_width - 1;
    }
    if (nMin < 0) {
        nMin = 0;
    }
    if (nMax >= m_height) {
        nMax = m_height - 1;
    }

    for (int m = mMin; m < mMax; m++) {
        for (int n = nMin; n < nMax; n++) {
            int maskIdx = canvasToMask(std::array<int, 2>{m, n}, std::array<int, 2>{x, y}, radius);
            RGBA currentColor =  m_data.at(posToIndex(m, n, m_width));
            float maskValue = mask.at(maskIdx);
            RGBA newColor = blendSmudgeColor(currentColor, pickupColor.at(maskIdx), maskValue);
            m_data.at(posToIndex(m, n, m_width)) = newColor;
        }
    }
}

/**************************************************************************************
 * Special functions for Fill
**************************************************************************************/
void Canvas2D::fill(int x, int y) {
    RGBA brushColor = settings.brushColor;

    if (checkNeighborFill(x, y, brushColor) or x == 0 or x == m_width - 1 or y == 0 or y == m_height - 1) {
        return;
    }
    else {
        fill(x - 1, y - 1);
        fill(x + 1, y - 1);
        fill(x - 1, y + 1);
        fill(x + 1, y + 1);
    }
    return;
}

bool Canvas2D::checkNeighborFill(int x, int y, RGBA brushColor) {
    bool isFullyFill = true;

    int mMin = x - 1;
    int mMax = x + 1;
    int nMin = y - 1;
    int nMax = y + 1;

    if (mMin < 0) {
        mMin = 0;
    }
    if (mMax >= m_width) {
        mMax = m_width - 1;
    }
    if (nMin < 0) {
        nMin = 0;
    }
    if (nMax >= m_height) {
        nMax = m_height - 1;
    }

    for (int m = mMin; m < mMax; m++) {
        for (int n = nMin; n < nMax; n++) {
            RGBA currentColor =  m_data.at(posToIndex(m, n, m_width));
            if (!compareRGBA(brushColor, currentColor)) {
                isFullyFill = false;
                m_data.at(posToIndex(m, n, m_width)) = brushColor;
            }
        }
    }

    return isFullyFill;
}


/************************************************************************************
 * Specification of different masks
************************************************************************************/
std::vector<float> Canvas2D::constMask(int radius) {
    int width = 2*radius + 1;
    std::vector<float> filter;
    filter.assign(width*width, 0);

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < width; y++) {
            if (calDistance(std::array<int, 2>{x,y}, std::array<int, 2>{radius,radius}) <= radius) {
                filter.at(posToIndex(x, y, width)) = 1;
            }
        }
    }

    return filter;
}

std::vector<float> Canvas2D::linearMask(int radius) {
    int width = 2*radius + 1;
    std::vector<float> filter;
    filter.assign(width*width, 0);

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < width; y++) {
            if (x == radius & y == radius) {
                filter.at(posToIndex(x, y, width)) = 1;
                continue;
            }
            float d = calDistance(std::array<int, 2>{x,y}, std::array<int, 2>{radius,radius});
            if (d <= radius) {
                filter.at(posToIndex(x, y, width)) = 1 - d / radius;
            }
        }
    }

    return filter;
}

std::vector<float> Canvas2D::quadraticMask(int radius) {
    int width = 2*radius + 1;
    std::vector<float> filter;
    filter.assign(width*width, 0);

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < width; y++) {
            if (x == radius & y == radius) {
                filter.at(posToIndex(x, y, width)) = 1;
                continue;
            }
            float d = calDistance(std::array<int, 2>{x,y}, std::array<int, 2>{radius,radius});
            if (d <= radius) {
                filter.at(posToIndex(x, y, width)) = 1.0/(radius*radius)*d*d - 2.0/radius*d + 1.0;
            }
        }
    }

    return filter;
}

std::vector<float> Canvas2D::smudgeMask(int radius) {
    int width = 2*radius + 1;
    std::vector<float> filter;
    filter.assign(width*width, 0);

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < width; y++) {
            if (x == radius & y == radius) {
                filter.at(posToIndex(x, y, width)) = 1;
                continue;
            }
            float d = calDistance(std::array<int, 2>{x,y}, std::array<int, 2>{radius,radius});
            if (d <= radius) {
                filter.at(posToIndex(x, y, width)) = 1.0/(radius*radius)*d*d - 2.0/radius*d + 1.0;
            }
        }
    }

    return filter;
}

std::vector<float> Canvas2D::sprayMask(int radius) {
    int width = 2*radius + 1;
    std::vector<float> filter;
    filter.assign(width*width, 0);

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < width; y++) {
            float d = calDistance(std::array<int, 2>{x,y}, std::array<int, 2>{radius,radius});
            if (d <= radius) {
                int randomSeed = arc4random_uniform(100);
                if (randomSeed < settings.brushDensity) {
                    filter.at(posToIndex(x, y, width)) = 1;
                }
            }
        }
    }

    return filter;
}

std::vector<float> Canvas2D::speedMask(int radius) {
    int width = 2*radius + 1;
    std::vector<float> filter;
    filter.assign(width*width, 0);

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < width; y++) {
            if (calDistance(std::array<int, 2>{x,y}, std::array<int, 2>{radius,radius}) <= radius) {
                filter.at(posToIndex(x, y, width)) = 1;
            }
        }
    }

    return filter;
}


/************************************************************************************
 * Helper functions
*************************************************************************************/
float Canvas2D::calDistance(std::array<int, 2> point1, std::array<int, 2> point2) {
    float distance = sqrt((point1[0] - point2[0]) * (point1[0] - point2[0]) + (point1[1] - point2[1]) * (point1[1] - point2[1]));
    return distance;
}

void Canvas2D::storeColor(int x, int y, int radius) {
    RGBA initialRGBA = {255,255,255,255};
    int width = 2*radius + 1;
    pickupColor.assign(width*width, initialRGBA);

    int mMin = x - radius;
    int mMax = x + radius;
    int nMin = y - radius;
    int nMax = y + radius;

    if (mMin < 0) {
        mMin = 0;
    }
    if (mMax >= m_width) {
        mMax = m_width - 1;
    }
    if (nMin < 0) {
        nMin = 0;
    }
    if (nMax >= m_height) {
        nMax = m_height - 1;
    }

    for (int m = mMin; m < mMax; m++) {
        for (int n = nMin; n < nMax; n++) {
            int maskIdx = canvasToMask(std::array<int, 2>{m, n}, std::array<int, 2>{x, y}, radius);
            pickupColor.at(maskIdx) = m_data.at(posToIndex(m, n, m_width));
        }
    }
}

bool Canvas2D::compareRGBA(RGBA colorA, RGBA colorB) {
    if (colorA.r == colorB.r & colorA.g == colorB.g & colorA.b == colorB.b) {
        return true;
    }
    else {
        return false;
    }
}

bool Canvas2D::checkEmpty(RGBA sampleColor) {
    RGBA initialRGBA = {.r = 255, .g = 255, .b = 255, .a = 255};
    for (int i = 0; i < m_width; i++) {
        for (int j = 0; j < m_height; j++) {
            RGBA currentColor =  m_data.at(posToIndex(i, j, m_width));

            if (!compareRGBA(sampleColor, currentColor)) {
                return false;
            }
        }
    }
    return true;
}


/**************************************************************************************
 * Conversion functions
**************************************************************************************/
int Canvas2D::canvasToMask(std::array<int, 2> canvasCoord, std::array<int, 2> filterCenter, int radius) {
    int filterIdx = 2*radius*radius + 2*radius - ((filterCenter[0] - canvasCoord[0]) + (filterCenter[1] - canvasCoord[1]) * (2*radius + 1));
    return filterIdx;
}

int Canvas2D::posToIndex(int x, int y, int width) {
    int index = x + y * width;
    return index;
}

std::array<int, 2> Canvas2D::indexToPos(int index) {
    int x = index % m_width;
    int y = index / m_width;
    return std::array<int, 2>{x, y};
}

