#pragma once

#include "ServerApp.hpp"

#include "Rte/Common.hpp"
#include <cstdlib>
#include <stack>

std::vector<int> convertToBinary(const Rte::u8* image, const Rte::Vec2<Rte::u16>& size);

std::vector<std::vector<int>> connectedComponentLabeling(const std::vector<int>& binaryPixels, const Rte::Vec2<Rte::u16>& size);

std::vector<Rte::u8> andImage(const Rte::u8* image, const std::vector<int>& binaryImage, Rte::Vec2<Rte::u16> size);
