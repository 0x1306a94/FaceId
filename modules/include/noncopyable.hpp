//
//  noncopyable.hpp
//  face_id
//
//  Created by king on 2024/2/2.
//

#ifndef face_id_noncopyable_hpp
#define face_id_noncopyable_hpp

/* FACE_MAKE_NONCOPYABLE */

#ifndef FACE_MAKE_NONCOPYABLE

#define FACE_MAKE_NONCOPYABLE(ClassName)   \
    ClassName(const ClassName &) = delete; \
    ClassName &operator=(const ClassName &) = delete;

#endif

#endif /* face_id_noncopyable_hpp */
