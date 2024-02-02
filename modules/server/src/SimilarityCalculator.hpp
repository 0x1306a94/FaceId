//
//  SimilarityCalculation.hpp
//  seeta_face_feature_database
//
//  Created by king on 2024/1/17.
//

#ifndef SimilarityCalculation_hpp
#define SimilarityCalculation_hpp

#include <cstddef>

namespace face {

namespace algorithm {
struct SeetaDefault {};
};  // namespace algorithm

template <typename AlgorithmTag>
struct SimilarityCalculator {
    static float calculate(const float *lhs, const float *rhs, std::size_t size);
};

template struct SimilarityCalculator<algorithm::SeetaDefault>;
};     // namespace face
#endif /* SimilarityCalculation_hpp */
