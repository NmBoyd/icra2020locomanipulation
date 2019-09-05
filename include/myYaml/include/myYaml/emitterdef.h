#ifndef EMITTERDEF_H_62B23520_7C8E_11DE_8A39_0800200C9A66_ASD
#define EMITTERDEF_H_62B23520_7C8E_11DE_8A39_0800200C9A66_ASD

#if defined(_MSC_VER) ||                                            \
    (defined(__GNUC__) && (__GNUC__ == 3 && __GNUC_MINOR__ >= 4) || \
     (__GNUC__ >= 4))  // GCC supports "pragma once" correctly since 3.4
#pragma once
#endif

namespace myYAML {
struct EmitterNodeType {
  enum value { NoType, Property, Scalar, FlowSeq, BlockSeq, FlowMap, BlockMap };
};
}

#endif  // EMITTERDEF_H_62B23520_7C8E_11DE_8A39_0800200C9A66