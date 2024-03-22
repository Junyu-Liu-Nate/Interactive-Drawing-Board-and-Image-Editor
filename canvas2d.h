#ifndef CANVAS2D_H
#define CANVAS2D_H

#include <QLabel>
#include <QMouseEvent>
#include <array>
#include "rgba.h"

class Canvas2D : public QLabel {
    Q_OBJECT
public:
    int m_width = 0;
    int m_height = 0;

    void init();
    void clearCanvas();
    bool loadImageFromFile(const QString &file);
    bool saveImageToFile(const QString &file);
    void displayImage();
    void resize(int w, int h);

    // This will be called when the settings have changed
    void settingsChanged();

    // Filter TODO: implement
    void filterImage();

private:
    std::vector<RGBA> m_data;

    void mouseDown(int x, int y);
    void mouseDragged(int x, int y);
    void mouseUp(int x, int y);

    // These are functions overriden from QWidget that we've provided
    // to prevent you from having to interact with Qt's mouse events.
    // These will pass the mouse coordinates to the above mouse functions
    // that you will have to fill in.
    virtual void mousePressEvent(QMouseEvent* event) override {
        auto [x, y] = std::array{ event->position().x(), event->position().y() };
        mouseDown(static_cast<int>(x), static_cast<int>(y));
    }
    virtual void mouseMoveEvent(QMouseEvent* event) override {
        auto [x, y] = std::array{ event->position().x(), event->position().y() };
        mouseDragged(static_cast<int>(x), static_cast<int>(y));
    }
    virtual void mouseReleaseEvent(QMouseEvent* event) override {
        auto [x, y] = std::array{ event->position().x(), event->position().y() };
        mouseUp(static_cast<int>(x), static_cast<int>(y));
    }

    /********************** Functions for Filter*************************/
    std::uint8_t floatToUint8(float x);
    std::uint8_t rgbaToGray(const RGBA &pixel);
    float clampValue(float value, float lowerBound, float upperBound);
    RGBA rgbToYcbcr(RGBA color);
    RGBA ycbcrToRgb(RGBA color);
    RGBA getPixelRepeated(std::vector<RGBA> &data, int width, int height, int x, int y);
    RGBA getPixelReflected(std::vector<RGBA> &data, int width, int height, int x, int y);
    RGBA getPixelWrapped(std::vector<RGBA> &data, int width, int height, int x, int y);

    std::vector<RGBA> convolve1DHorizontal(std::vector<RGBA> &data, int width, int height, const std::vector<float> &kernel, std::vector<int> channels);
    std::vector<RGBA> convolve1DVertical(std::vector<RGBA> &data, int width, int height, const std::vector<float> &kernel, std::vector<int> channels);
//    std::vector<int> convolve1DHorizontalGray(std::vector<RGBA> &data, int width, int height, const std::vector<float> &kernel);
//    std::vector<int> convolve1DVerticalGray(std::vector<RGBA> &data, int width, int height, const std::vector<float> &kernel)
    std::vector<RGBA> median2D(std::vector<RGBA> &data, int width, int height, int kernelSize);
    std::vector<RGBA> bilateral2D(std::vector<RGBA> &data, int width, int height, int kernelSize);

    std::vector<float> normalizeFilter(std::vector<float> kernel);
    void upDateCanvas(std::vector<RGBA> result);
    void applyBlurFilter(std::vector<float> kernel);
    void applySolbelFilter(std::vector<float> kernel1, std::vector<float> kernel2);
    void scale();
    void applyMedianFilter();
    void mappingLinear();
    void mappingNonLinear();
    void chromaticShift();
    void rotate();
    void applyBilateralFilter();

    void filterGray();
    std::vector<float> filterBlur(int radius);
    std::vector<float> solbelFilter1();
    std::vector<float> solbelFilter2();
    double triFilter(double x, double a);
    std::vector<float> filterMedian(int radius);
    std::vector<float> filterShift(int radius);
    float filterSpatialGaussian(int x, int y, double sigma);
    float filterRangeGaussian(double intensityDifference, double sigma);

    /********************** Functions for Mask*************************/
    void applyMask(std::vector<float> mask, int x, int y, int radius, RGBA brushColor);
    void applySmudgeMask(std::vector<float> mask, int x, int y, int radius);
    RGBA blendColor(RGBA currentColor, RGBA brushColor, float maskValue);
    RGBA blendSmudgeColor(RGBA currentColor, RGBA brushColor, float maskValue);
    void fill(int x, int y);
    bool checkNeighborFill(int x, int y, RGBA brushColor);

    bool m_isDown;
    bool setting_isChanged;

    std::vector<float> mask;
    std::vector<RGBA> pickupColor;
    std::array<int, 2> storePosition;
    std::vector<float> brushTrack;
    void setMask();
    std::vector<float> constMask(int radius);
    std::vector<float> linearMask(int radius);
    std::vector<float> quadraticMask(int radius);
    std::vector<float> smudgeMask(int radius);
    std::vector<float> sprayMask(int radius);
    std::vector<float> speedMask(int radius);

    float calDistance(std::array<int, 2> point1, std::array<int, 2> point2);
    void storeColor(int x, int y, int radius);
    bool compareRGBA(RGBA colorA, RGBA colorB);
    bool checkEmpty(RGBA sampleColor);

    int canvasToMask(std::array<int, 2> canvasCoord, std::array<int, 2> filterCenter, int radius);
    int posToIndex(int x, int y, int width);
    std::array<int, 2> indexToPos(int index);
};

#endif // CANVAS2D_H
