//
//  feature_file_header.h
//  face_id
//
//  Created by king on 2024/2/5.
//

#ifndef face_id_storage_feature_file_header_h
#define face_id_storage_feature_file_header_h

#define FACE_FEATURE_LENGHT 1024
#define FACE_FEATURE_FILE_MAX_ITEM 100000

#define FACE_FEATURE_FILE_MAGIC 0x46414345 /* the mach magic number */
#define FACE_FEATURE_FILE_CIGAM 0x45434146 /* swap(FACE_FEATURE_FILE_MAGIC) */

#pragma pack(push, 1)
struct FeatureItem {
    float value[FACE_FEATURE_LENGHT];
};

struct FeatureFileHeader {
    uint32_t magic;
    char appid[32] = {'\0'};
    uint16_t index;
    uint16_t reserved;
    uint32_t count;
    FeatureItem features[0];
};
#pragma pack(pop)

#define FACE_FEATURE_FILE_SIZE (sizeof(struct FeatureFileHeader) + sizeof(struct FeatureItem) * FACE_FEATURE_FILE_MAX_ITEM)

#endif /* face_id_storage_feature_file_header_h */
