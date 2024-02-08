//
//  SimilarityCalculator.cpp
//  seeta_face_feature_database
//
//  Created by king on 2024/1/17.
//

#include "SimilarityCalculator.hpp"

#include "../common/src/byte_order.hpp"

#include <algorithm>

namespace face {
template <>
float SimilarityCalculator<algorithm::SeetaDefault>::calculate(const float *lhs, const float *rhs, std::size_t size, bool swapByteOrder) {
    float sum = 0;
    for (std::size_t i = 0; i < size; ++i) {
        if (swapByteOrder) {
            sum += common::SwapFloatLittleToHost(*lhs) * common::SwapFloatLittleToHost(*rhs);
        } else {
            sum += *lhs * *rhs;
        }
        ++lhs;
        ++rhs;
    }
    sum = std::max<float>(sum, 0);
    return sum;
}
};  // namespace face
