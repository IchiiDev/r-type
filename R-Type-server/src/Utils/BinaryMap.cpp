#include "ServerApp.hpp"

#include "Rte/Common.hpp"
#include <cstdlib>
#include <stack>


std::vector<int> convertToBinary(const Rte::u8* image, const Rte::Vec2<Rte::u16>& size) {
    std::vector<int> binaryImage;
    for (int i = 1; i < size.x * size.y + 1; i++) {
        if (image[i * 4 - 1] == 0)
            binaryImage.push_back(0);
        else
            binaryImage.push_back(1);
    }

    return binaryImage;
}

std::vector<std::vector<int>> connectedComponentLabeling(const std::vector<int>& binaryPixels, const Rte::Vec2<Rte::u16>& size) {   // NOLINT(readability-function-cognitive-complexity)
    const int width = size.x;
    const int height = size.y;

    std::vector<std::vector<int>> labels(height, std::vector<int>(width, 0));
    int currentLabel = 1;

    auto getIndex = [width](int x, int y) {
        return y * width + x;
    };

    auto isValid = [width, height](int x, int y) {
        return x >= 0 && x < width && y >= 0 && y < height;
    };

    const std::vector<std::pair<int, int>> directions = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (binaryPixels.at(getIndex(x, y)) == 1 && labels.at(y).at(x) == 0) {
                std::stack<std::pair<int, int>> stack;
                stack.emplace(x, y);
                labels.at(y).at(x) = currentLabel;

                while (!stack.empty()) {
                    auto [cx, cy] = stack.top();
                    stack.pop();

                    for (const auto& [dx, dy] : directions) {
                        const int nx = cx + dx;
                        const int ny = cy + dy;

                        if (isValid(nx, ny) && binaryPixels.at(getIndex(nx, ny)) == 1 && labels.at(ny).at(nx) == 0) {
                            stack.emplace(nx, ny);
                            labels.at(ny).at(nx) = currentLabel;
                        }
                    }
                }

                ++currentLabel;
            }
        }
    }

    std::vector<std::vector<int>> labeledImages(currentLabel - 1, std::vector<int>(binaryPixels.size(), 0));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const int label = labels.at(y).at(x);
            if (label > 0)
                labeledImages.at(label - 1).at(getIndex(x, y)) = 1;
        }
    }

    return labeledImages;
}

std::vector<Rte::u8> andImage(const Rte::u8* image, const std::vector<int>& binaryImage, Rte::Vec2<Rte::u16> size) {
    std::vector<Rte::u8> newImage(static_cast<size_t>(size.x * size.y * 4), 0);
    for (int i = 0; i < size.x * size.y; i++) {
        if (binaryImage.at(i) == 1) {
            newImage.at(static_cast<long long>(static_cast<long>(i)) * 4) = image[static_cast<ptrdiff_t>(i * 4)];
            newImage.at(i * 4 + 1) = image[i * 4 + 1];
            newImage.at(i * 4 + 2) = image[i * 4 + 2];
            newImage.at(i * 4 + 3) = image[i * 4 + 3];
        }
    }

    return newImage;
}