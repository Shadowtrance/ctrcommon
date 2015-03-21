#ifndef __CTRCOMMON_GPU_HPP__
#define __CTRCOMMON_GPU_HPP__

#include "ctrcommon/screen.hpp"
#include "ctrcommon/types.hpp"

// TODO: error reporting system

#define ATTRIBUTE(i, n, f) (((((n)-1)<<2)|((f)&3))<<((i)*4))

#define TEXENV_SOURCES(a,b,c) (((a))|((b)<<4)|((c)<<8))
#define TEXENV_OPERANDS(a,b,c) (((a))|((b)<<4)|((c)<<8))

#define TEXTURE_MAG_FILTER(v) (((v)&0x1)<<1)
#define TEXTURE_MIN_FILTER(v) (((v)&0x1)<<2)
#define TEXTURE_WRAP_S(v) (((v)&0x3)<<8)
#define TEXTURE_WRAP_T(v) (((v)&0x3)<<12)

typedef enum {
    CULL_NONE = 0x0,
    CULL_FRONT_CCW = 0x1,
    CULL_BACK_CCW = 0x2
} CullMode;

typedef enum {
    TEST_NEVER = 0x0,
    TEST_ALWAYS = 0x1,
    TEST_EQUAL = 0x2,
    TEST_NOTEQUAL = 0x3,
    TEST_LESS = 0x4,
    TEST_LEQUAL = 0x5,
    TEST_GREATER = 0x6,
    TEST_GEQUAL = 0x7
} TestFunc;

typedef enum {
    OP_KEEP = 0x0,
    OP_AND_NOT = 0x1,
    OP_XOR = 0x5
} StencilOp;

typedef enum {
    BLEND_ADD = 0x0,
    BLEND_SUBTRACT = 0x1,
    BLEND_REVERSE_SUBTRACT = 0x2,
    BLEND_MIN = 0x3,
    BLEND_MAX = 0x4
} BlendEquation;

typedef enum {
    FACTOR_ZERO = 0x0,
    FACTOR_ONE = 0x1,
    FACTOR_SRC_COLOR = 0x2,
    FACTOR_ONE_MINUS_SRC_COLOR = 0x3,
    FACTOR_DST_COLOR = 0x4,
    FACTOR_ONE_MINUS_DST_COLOR = 0x5,
    FACTOR_SRC_ALPHA = 0x6,
    FACTOR_ONE_MINUS_SRC_ALPHA = 0x7,
    FACTOR_DST_ALPHA = 0x8,
    FACTOR_ONE_MINUS_DST_ALPHA = 0x9,
    FACTOR_CONSTANT_COLOR = 0xA,
    FACTOR_ONE_MINUS_CONSTANT_COLOR = 0xB,
    FACTOR_CONSTANT_ALPHA = 0xC,
    FACTOR_ONE_MINUS_CONSTANT_ALPHA = 0xD,
    FACTOR_SRC_ALPHA_SATURATE = 0xE
} BlendFactor;

typedef enum {
    VERTEX_SHADER = 0x0,
    GEOMETRY_SHADER = 0x1
} ShaderType;

typedef enum {
    PRIM_TRIANGLES = 0x0000,
    PRIM_TRIANGLE_STRIP = 0x0100,
    PRIM_TRIANGLE_FAN = 0x0200,
    PRIM_UNKPRIM = 0x0300
} Primitive;

typedef enum {
    ATTR_BYTE = 0x0,
    ATTR_UNSIGNED_BYTE = 0x1,
    ATTR_SHORT = 0x2,
    ATTR_FLOAT = 0x3
} AttributeType;

typedef enum{
    SOURCE_PRIMARY_COLOR = 0x0,
    SOURCE_TEXTURE0 = 0x3,
    SOURCE_TEXTURE1 = 0x4,
    SOURCE_TEXTURE2 = 0x5,
    SOURCE_TEXTURE3 = 0x6,
    SOURCE_CONSTANT = 0xE,
    SOURCE_PREVIOUS = 0xF
} TexEnvSource;

typedef enum {
    COMBINE_REPLACE = 0x0,
    COMBINE_MODULATE = 0x1,
    COMBINE_ADD = 0x2,
    COMBINE_ADD_SIGNED = 0x3,
    COMBINE_INTERPOLATE = 0x4,
    COMBINE_SUBTRACT = 0x5,
    COMBINE_DOT3_RGB = 0x6
} CombineFunc;

typedef enum {
    TEXUNIT0 = 0x1,
    TEXUNIT1 = 0x2,
    TEXUNIT2 = 0x4
} TexUnit;

typedef enum {
    FILTER_NEAREST = 0x0,
    FILTER_LINEAR = 0x1
} TextureFilter;

typedef enum {
    WRAP_CLAMP_TO_EDGE = 0x0,
    WRAP_REPEAT = 0x2
} TextureWrap;

typedef enum {
    PIXEL_RGBA8 = 0x0,
    PIXEL_RGB8 = 0x1,
    PIXEL_RGBA5551 = 0x2,
    PIXEL_RGB565 = 0x3,
    PIXEL_RGBA4 = 0x4,
    PIXEL_LA8 = 0x5,
    PIXEL_HILO8 = 0x6,
    PIXEL_L8 = 0x7,
    PIXEL_A8 = 0x8,
    PIXEL_LA4 = 0x9,
    PIXEL_L4 = 0xA,
    PIXEL_ETC1 = 0xB,
    PIXEL_ETC1A4 = 0xC
} PixelFormat;

typedef enum {
    SCISSOR_DISABLE = 0x0,
    SCISSOR_INVERT = 0x1,
    SCISSOR_NORMAL = 0x3
} ScissorMode;

void gpuInit();

void gpuFlush();

void gpuClear();

void gpuClearColor(u8 red, u8 green, u8 blue, u8 alpha);
void gpuClearDepth(u32 depth);

void gpuViewport(Screen screen, u32 x, u32 y, u32 width, u32 height);
void gpuScissorTest(ScissorMode mode, u32 x, u32 y, u32 width, u32 height);
void gpuDepthMap(float near, float far);

void gpuCullMode(CullMode mode);

void gpuStencilFunc(bool enable, TestFunc func, u8 ref, u8 mask, u8 replace);
void gpuStencilOp(StencilOp fail, StencilOp zfail, StencilOp zpass);

void gpuBlendColor(u8 red, u8 green, u8 blue, u8 alpha);
void gpuBlendFunc(BlendEquation colorEquation, BlendEquation alphaEquation, BlendFactor colorSrc, BlendFactor colorDst, BlendFactor alphaSrc, BlendFactor alphaDst);

void gpuAlphaFunc(bool enable, TestFunc func, u8 ref);

void gpuDepthFunc(bool enable, TestFunc func);

void gpuColorMask(bool red, bool green, bool blue, bool alpha);
void gpuDepthMask(bool depth);

void gpuCreateShader(u32* shader);
void gpuFreeShader(u32 shader);
void gpuLoadShader(u32 shader, const void* data, u32 size, u8 geometryStride = 0);
void gpuUseShader(u32 shader);
void gpuGetUniformBool(u32 shader, ShaderType type, int id, bool* value);
void gpuSetUniformBool(u32 shader, ShaderType type, int id, bool value);
void gpuSetUniform(u32 shader, ShaderType type, const char* name, const void* data, u32 elements);

void gpuCreateVbo(u32* vbo);
void gpuFreeVbo(u32 vbo);
void gpuVboData(u32 vbo, const void* data, u32 size, u32 numVertices, Primitive primitive);
void gpuVboIndices(u32 vbo, const void* data, u32 size);
void gpuVboAttributes(u32 vbo, u64 attributes, u8 attributeCount);
void gpuDrawVbo(u32 vbo);

void gpuTexEnv(u32 env, u16 rgbSources, u16 alphaSources, u16 rgbOperands, u16 alphaOperands, CombineFunc rgbCombine, CombineFunc alphaCombine, u32 constantColor);
void gpuCreateTexture(u32* texture);
void gpuFreeTexture(u32 texture);
void gpuTextureData(u32 texture, const void* data, u32 inWidth, u32 inHeight, PixelFormat inFormat, u32 outWidth, u32 outHeight, PixelFormat outFormat, u32 params);
void gpuBindTexture(TexUnit unit, u32 texture);

#endif