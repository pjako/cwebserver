#include "base.h"
#include <stdlib.h>

#ifndef _BASE_C_
#define _BASE_C_

///////////////////////////////////////////
// BASE STR ///////////////////////////////
///////////////////////////////////////////

S8 str_makeSized(Arena* arena, u8* arr, u32 size) {
    S8 str;
    str.size = size;
    str.content = mem_arenaPushArray(arena, u8, size);
    mem_copy(str.content, arr, str.size);
    return str;
 }
 
 i64  str_find(S8 str, S8 findExp) {
     for (u64 idx = 0; (idx + findExp.size)  < str.size; idx++) {
         for (u64 fi = 0; str.content[idx + fi] != str.content[fi]; fi++) {
             continue;
         }
         return idx;
     }
     return -1;
 }
 
 i64  str_findChar(S8 str, char c) {
     for (u64 idx = 0; idx < str.size; idx++) {
         if (str.content[idx] == c) {
             return idx;
         }
     }
     return -1;
 }
 API i64 str_lastIndexOfChar(S8 str, char c) {
    for (i64 idx = i64_cast(str.size) - 1; idx >= 0; idx--) {
       if (str.content[idx] == c) {
          return idx;
       }
    }
    return -1;
 }
 
 bool str_startsWithChar(S8 str, char startChar) {
     if (str.size == 0 || str.content[0] != startChar) return false;
     return true;
 }
 
 bx str_hasPrefix(S8 str, S8 prefix) {
    if (prefix.size > str.size) {
       return false;
    }
 
    for (u64 idx = 0; prefix.size > idx; idx++) {
       if (str.content[idx] != prefix.content[idx]) return false;
    }
 
    return true;
 }
 
 bx str_hasSuffix(S8 str, S8 endsWith) {
    if (str.size < endsWith.size) return false;
    u8* l = str.content + (str.size - endsWith.size);
    u8* r = endsWith.content;
 
    for (u64 idx = 0; endsWith.size > idx; idx++) {
       if (l[idx] != r[idx]) return false;
    }
 
    return true;
 }
 
 S8 str_toLowerAscii(S8 str) {
     for (u64 idx = 0; str.size > idx; idx++) {
         char c = str.content[idx];
         if (c >= 'A' && c <= 'Z') {
             str.content[idx] = 'a' + (c - 'A');
         }
     }
     return str;
 }
 
 S8 str_toUpperAscii(S8 str) {
     for (u64 idx = 0; str.size > idx; idx++) {
         char c = str.content[idx];
         if (c >= 'a' && c <= 'Z') {
             str.content[idx] = 'A' + (c - 'a');
         }
     }
     return str;
 }
 
 S8 str_subStr(S8 str, u64 start, u64 size) {
     if (str.size == 0) {
         return str;
     }
     if (str.size <= start) {
         return STR_NULL;
     }
 
     S8 subStr;
     subStr.content = str.content + start;
     subStr.size = minVal(str.size - start, size);
     return subStr;
 }
 
 API S8 str_from(S8 str, u64 from) {
    if (from >= str.size) {
       //ASSERT(!"start out of str range");
       return STR_NULL;
    }
    u64 start = minVal(str.size - 1, from);
    S8 result;
    result.content = &str.content[start];
    result.size = str.size - start;
    return result;
 }
 
 API S8 str_to(S8 str, u64 to) {
    str.size = minVal(to, str.size);
    return str;
 }

API S8 str_skipWhiteSpace(S8 str) {
   for (; str.size > 0 && (str_isSpacingChar(str.content[0]) || str_isEndOfLineChar(str.content[0])); (str.size -=1, str.content = &str.content[1]));
   return str;
}
 
 u64 str_length(S8 str) {
    char* s = (char*) str.content;
    const char* t = (const char*) str.content;
    u64 length = 0;
 
    while ((u64)(s - t) < str.size && '\0' != *s) {
       if (0xf0 == (0xf8 & *s)) {
          /* 4-byte utf8 code point (began with 0b11110xxx) */
          s += 4;
       } else if (0xe0 == (0xf0 & *s)) {
          /* 3-byte utf8 code point (began with 0b1110xxxx) */
          s += 3;
       } else if (0xc0 == (0xe0 & *s)) {
          /* 2-byte utf8 code point (began with 0b110xxxxx) */
          s += 2;
       } else { /* if (0x00 == (0x80 & *s)) { */
          /* 1-byte ascii (began with 0b0xxxxxxx) */
          s += 1;
       }
 
       /* no matter the bytes we marched s forward by, it was
       * only 1 utf8 codepoint */
       length++;
    }
 
    if ((u64)(s - t) > str.size) {
       length--;
    }
    return length;
 }
 
 bool str_isEqual(S8 left, S8 right) {
     if (left.size != right.size) return false;
     for (u64 idx = 0; idx < left.size; idx++) {
       if (left.content[idx] != right.content[idx]) {
          return false;
       }
     }
     return true;
 }
 
 bool str_isSpacingChar(char c) {
     return ((c == ' ') || (c == '\t') || (c == '\v') || (c == '\f'));
 }
 
 bool str_isNumberChar(char c) {
     return ((c >= '0') && (c <= '9'));
 }
 
 bool str_isEndOfLineChar(char c) {
     return ((c == '\n') || (c == '\r'));
 }
 
 bool str_isAlphaChar(char c) {
     return (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')));
 }
 
 f32  str_parseF32 (S8 str) {
    f32 val = 0.f;
    str_parseF32N(str, &val);
    return val;
 }
 
 u64  str_parseF32N(S8 str, f32* f) {
    i64 left = 0;
    u64 offset = str_parseS64N(str, &left);
    if (offset == 0) {
       *f = 0;
       return 0;
    }
    *f = f32_cast(left);
    if (str.size > (offset + 2) && str.content[offset + 1] == '.') {
       S8 sub = str_subStr(str, offset + 1, str.size);
       i64 right = 0;
       u64 offsetRight = str_parseS64N(sub, &right);
       if (offsetRight != 0 && right > 0) {
          *f = (left > 0 ? 1.f : -1.f) * f32_cast(right) / f32_cast(offsetRight);
          offset += offsetRight;
       }
    }
    return offset;
 }
 
 i64  str_parseS64 (S8 str) {
    i64 val = 0;
    str_parseS64N(str, &val);
    return val;
 }
 
 u64  str_parseS64N(S8 str, i64* s) {
    u64 out = 0;
    u64 idx = 1;
    if (str.size == 0) {
       return 0;
    }
    i64 mult = str.content[0] == '-' ? -1 : 1;
 
    for (; str.size > idx; idx++) {
       char numChar = str.content[idx];
       if (numChar < '0' || numChar > '9') {
          *s = out * mult;
          return idx;
       }
       out *= 10;
       out += numChar - '0';
       if (out >= i64_max) {
          *s = mult * i64_max;
          return idx;
       }
    }
    *s = out * mult;
    return idx;
 
 }
 
 u32  str_parseU32 (S8 str) {
    u32 u = 0;
    str_parseU32N(str, &u);
    return u;
 }
 
 u64 str_parseU32N(S8 str, u32* u) {
    u64 out = 0;
    u64 idx = 0;
    for (; str.size > idx; idx++) {
       char numChar = str.content[idx];
       if (numChar < '0' || numChar > '9') {
          *u = out;
          return idx;
       }
       out *= 10;
       out += numChar - '0';
       if (out >= u32_max) {
          *u = u32_max;
          return idx;
       }
    }
    *u = out;
    return idx;
 }
 
 S8 str_copyNullTerminated(Arena* arena, S8 str) {
    mms size = str_isNullTerminated(str) ? str.size : str.size + 1;
    S8 result = {
       .size = size,
       .content = (u8*) mem_arenaPush(arena, size)
    };
    mem_copy(result.content, str.content, str.size);
    result.content[result.size - 1] = '\0';
 
    return result;
 }
 
 S8 str_alloc(Arena* arena, mms size) {
    S8 result = {
       .size = size,
       .content = (u8*) mem_arenaPush(arena, size)
    };
 
    return result;
 }
 
 S8 str_copy(Arena* arena, S8 sourceStr) {
    if (sourceStr.size == 0) {
       return STR_NULL;
    }
    S8 result = str_alloc(arena, sourceStr.size);
    mem_copy(result.content, sourceStr.content, sourceStr.size);
 
    return result;
 }
 
 u64 str_findFirst(S8 str, S8 findStr, u64 offset) {
    u64 i = 0;
    if (findStr.size > 0) {
       i = str.size;
       if (str.size >= findStr.size) {
          i = offset;
          i8 c = findStr.content[0];
          u64 onePastLast = str.size - findStr.size + 1;
          for (; i < onePastLast; i++) {
             if (str.content[i] == c) {
                if ((str.size - i) >= findStr.size) {
                   S8 sub = {.content = str.content + i, .size = findStr.size};
                   if (str_isEqual(sub, findStr)) {
                      break;
                   }
                }
             }
          }
          if (i == onePastLast) {
             i = str.size;
          }
       }
    }
    return i;
 }
 
 u64 str_findLast(S8 str, S8 findStr, u64 offset) {
    if ((findStr.size + offset) > str.size) {
       return str.size;
    }
    u64 notFoundReturn = str.size;
    str = str_from(str, offset);
    for (u32 idx = str.size - (findStr.size); idx != 0; idx--) {
       if (str_isEqual(str_subStr(str, idx, findStr.size), findStr)) {
          return idx;
       }
    }
    return notFoundReturn;
 }
 
 u64 str_containsSubStringCount(S8 str, S8 findStr) {
    u64 ct = 0;
    u64 idx = 0;
    while (true) {
       idx = str_findFirst(str, findStr, idx);
       if (idx == str.size) {
          break;
       }
 
       ct++;
       idx++;
    }
    return ct;
 }
 
 S8 str_replaceAll(Arena* arena, S8 str, S8 replaceStr, S8 replacement) {
    if (replaceStr.size == 0) return str;
    u64 replaceable = str_containsSubStringCount(str, replaceStr);
    if (replaceable == 0) return str;
 
    u64 new_size = (str.size - replaceable * replaceStr.size) + (replaceable * replacement.size);
    S8 ret = str_alloc(arena, new_size);
 
    b8 replaced;
    u64 o = 0;
    for (u64 i = 0; i < str.size;) {
       replaced = false;
       if (str.content[i] == replaceStr.content[0]) {
          if ((str.size - i) >= replaceStr.size) {
                S8 sub = { .content = str.content + i, .size = replaceStr.size };
                if (str_isEqual(sub, replaceStr)) {
                   // replace this one
                   memmove(ret.content + o, replacement.content, replacement.size);
                   replaced = true;
                }
          }
       }
       
       if (replaced) {
          o += replacement.size;
          i += replaceStr.size;
          continue;
       }
       
       ret.content[o] = str.content[i];
       o++; i++;
    }
 
    return ret;
 }
 
 ///////////////////////////////////////
 // UTF-8 functions
 
 u64 str_utf8Count(S8 str) {
     u64 length = 0;
     u64 idx = 0;
     while (idx < str.size) {
         if (0xf0 == (0xf8 & str.content[idx])) {
             // 4-byte utf8 code point (began with 0b11110xxx)
             idx += 4;
         } else if (0xe0 == (0xf0 & str.content[idx])) {
             // 3-byte utf8 code point (began with 0b1110xxxx)
             idx += 3;
         } else if (0xc0 == (0xe0 & str.content[idx])) {
             // 2-byte utf8 code point (began with 0b110xxxxx)
             idx += 2;
         } else {
             // 1-byte ascii (began with 0b0xxxxxxx)
             idx += 1;
         }
 
         // it is always only one rune independent of its size
         length++;
     }
 
     if (idx > str.size) {
         // rune size goes beyond content buffer so we go back on that one
         length--;
     }
     return length;
 }
 
 ////////////////////////////
 // NOTE(pjako): hash
 
 API u32 str_hash32(S8 str) {
     u32 hash = 5381;
     i32 c;
     for (u64 i = 0; i < str.size; i++) {
         hash = ((hash << 5) + hash) + str.content[i]; /* hash * 33 + c */
     }
     return hash;
 }
 
 API u64 str_hash64(S8 str) {
     u32 hash1 = 5381;
     u32 hash2 = 52711;
     u64 i = str.size;
     while (i--) {
         u8 c = str.content[i];
         hash1 = (hash1 * 33) ^ c;
         hash2 = (hash2 * 33) ^ c;
     }
 
     return (hash1 >> 0) * 4096 + (hash2 >> 0);
 }
 
 S8 str_fromCharPtr(u8* charArr, u64 size) {
    S8 str;
    str.content = charArr;
    str.size = size;
 
    return str;
 }
 
 S8 str_fromNullTerminatedCharPtr(char* charArr) {
    S8 str;
    str.content = (u8*) charArr;
    str.size = 0;
 
    for (;charArr[str.size] != '\0'; str.size++);
 
    return str;
 }
 
 #if 0
 static str_Decode str_decodeUtf8(u8* str, u64 cap) {
    str_Decode result = {'#', 1};
    u16 x = str[0];
    if (x < 0xD800 || 0xDFFF < x) {
       result.codepoint = x;
    } else if (cap >= 2) {
       u16 y = str[1];
       if (0xD800 <= x && x < 0xDC00 && 0xDC00 <= y && y < 0xE000) {
          u16 xj = x - 0xD800;
          u16 yj = y - 0xDc00;
          u32 xy = (xj << 10) | yj;
          result.codepoint = xy + 0x10000;
          result.size = 2;
       }
    }
    return result;
 }
 #endif
 
 str_StringDecode str_decodeUtf8(u8* str, u64 cap) {
    str_StringDecode result = {'#', 1};
    u16 x = str[0];
    if (x < 0xD800 || 0xDFFF < x){
       result.codepoint = x;
    } else if (cap >= 2){
       u16 y = str[1];
       if (0xD800 <= x && x < 0xDC00 &&
          0xDC00 <= y && y < 0xE000){
          u16 xj = x - 0xD800;
          u16 yj = y - 0xDc00;
          u32 xy = (xj << 10) | yj;
          result.codepoint = xy + 0x10000;
          result.size = 2;
       }
    }
    return(result);
 }
 
 static u32 str_encodeUtf16(u16* dst, u32 codepoint) {
    u32 size = 0;
    if (codepoint < 0x10000) {
       dst[0] = codepoint;
       #if OS_WIN == 1
       size = 1;
       #else
       size = 2;
       #endif
    } else {
       u32 cpj = codepoint - 0x10000;
       dst[0] = (cpj >> 10 ) + 0xD800;
       dst[1] = (cpj & 0x3FF) + 0xDC00;
       #if OS_WIN == 1
       size = 2;
       #else
       size = 4;
       #endif
    }
    return size;
 }
 
 str_StringDecode str_decodeUtf16(u16 *str, u32 cap) {
     str_StringDecode result = {'#', 1};
     u16 x = str[0];
     if (x < 0xD800 || 0xDFFF < x){
         result.codepoint = x;
     } else if (cap >= 2) {
         u16 y = str[1];
         if (0xD800 <= x && x < 0xDC00 &&
             0xDC00 <= y && y < 0xE000) {
             u16 xj = x - 0xD800;
             u16 yj = y - 0xDc00;
             u32 xy = (xj << 10) | yj;
             result.codepoint = xy + 0x10000;
             result.size = 2;
         }
     }
     return(result);
 }
 
 u32 str_encodeUtf8(u8 *dst, u32 codepoint){
     u32 size = 0;
     if (codepoint < (1 << 8)) {
         dst[0] = codepoint;
         size = 1;
     } else if (codepoint < (1 << 11)) {
         dst[0] = 0xC0 | (codepoint >> 6);
         dst[1] = 0x80 | (codepoint & 0x3F);
         size = 2;
     } else if (codepoint < (1 << 16)) {
         dst[0] = 0xE0 | (codepoint >> 12);
         dst[1] = 0x80 | ((codepoint >> 6) & 0x3F);
         dst[2] = 0x80 | (codepoint & 0x3F);
         size = 3;
     } else if (codepoint < (1 << 21)) {
         dst[0] = 0xF0 | (codepoint >> 18);
         dst[1] = 0x80 | ((codepoint >> 12) & 0x3F);
         dst[2] = 0x80 | ((codepoint >> 6) & 0x3F);
         dst[3] = 0x80 | (codepoint & 0x3F);
         size = 4;
     } else {
         dst[0] = '#';
         size = 1;
     }
     return(size);
 }
 
 S8 str_fromS16(Arena* arena, S16 str) {
    u8 *memory = mem_arenaPushArrayZero(arena, u8, str.size * 3 + 1);
 
    u8  *dptr = memory;
    u16 *ptr  = str.content;
    u16 *opl  = str.content + str.size;
    for (; ptr < opl;) {
       str_StringDecode decode = str_decodeUtf16(ptr, (u64)(opl - ptr));
       u16 enc_size = str_encodeUtf8(dptr, decode.codepoint);
       ptr += decode.size;
       dptr += enc_size;
    }
 
    dptr[0] = 0;
    //dptr[3] = 0;
 
    u64 allocCount = str.size * 3 + 1;
    u64 stringCount = (u64)(dptr - memory);
    u64 unusedCount = allocCount - stringCount - 1;
    //mem_arenaPopAmount(arena, unusedCount * sizeof(*memory));
 
    S8 result = {memory, stringCount};
    return(result);
 }
 
 S16 str_toS16(Arena *arena, S8 str) {
    u64 allocSize = str.size * 4 + 2;
    u16 *memory = mem_arenaPushArray(arena, u16, allocSize);
 
    u16 *dptr = memory;
    u8 *ptr = str.content;
    u8 *opl = str.content + str.size;
    u16* wstr = (u16*) memory;
 
    for (; ptr < opl;) {
       str_StringDecode decode = str_decodeUtf8(ptr, (u64)(opl - ptr));
       u32 encSize = str_encodeUtf16(dptr, decode.codepoint);
       ptr  = ptr + decode.size;
       dptr = &dptr[encSize];
    }
 
    *dptr = 0;
    dptr += 1;
    *dptr = 0;
 
    u64 stringCount = (u64)(dptr - memory);
    u64 unusedCount = allocSize - stringCount - 1;
    //mem_arenaPopAmount(arena, unusedCount);
 
    S16 result = {memory, stringCount};
    return result;
 }
 
 S32 str_toS32(Arena *arena, S8 str) {
    u64 allocSize = str.size * 4 + 2;
    u32 *memory = mem_arenaPushArray(arena, u32, allocSize);
 
    u32 *dptr = memory;
    u8 *ptr = str.content;
    u8 *opl = str.content + str.size;
    u32* wstr = (u32*) memory;
 
    for (; ptr < opl;) {
       str_StringDecode decode = str_decodeUtf8(ptr, (u64)(opl - ptr));
       ptr  = ptr + decode.size;
       *dptr = decode.codepoint;
       dptr = dptr + 1;
    }
 
    *dptr = 0;
    dptr += 1;
    *dptr = 0;
 
    u64 stringCount = (u64)(dptr - memory);
    u64 unusedCount = allocSize - stringCount - 1;
    //mem_arenaPopAmount(arena, unusedCount);
 
    S32 result = {memory, stringCount};
    return result;
 }
 
 ////////////////////////////
 // NOTE(pjako): fomat/join
 
 LOCAL void str_recordStr(Arena* arena, S8 str) {
    mem_copy(mem_arenaPush(arena, str.size), str.content, str.size);
 }
 
 LOCAL void str_recordCustom(Arena* arena, str_CustomVal* customVal, S8 format) {
    customVal->buildStrFn(arena, format, customVal->usrPtr);
 }
 
 LOCAL i32 str__f32ToCharArr(char const** start, u32 *len, char* out, i32* decimal_pos, f64 value, i32 frac_digits);
 
 LOCAL void str_recordf32_cast(Arena* arena, f32 floatVal) {
    u32 fracDigits = 10;
    const char* startChar = NULL;
    char str[512];
    u32 length = 0;
    i32 decPos = 0;
    i32 start = str__f32ToCharArr(&startChar, &length, str, &decPos, floatVal, fracDigits);
    if (decPos == 0) {
       mem_copy(mem_arenaPush(arena, 2), "0.", 2);
    }
    if (decPos < length) {
       mem_copy(mem_arenaPush(arena, decPos), startChar, decPos);
       mem_copy(mem_arenaPush(arena, 1),  ".", 1);
       mem_copy(mem_arenaPush(arena, length - decPos), &startChar[decPos], length - decPos);
    } else {
       mem_copy(mem_arenaPush(arena, length), &startChar[0], length);
    }
 }
 
 LOCAL void str_recordF64(Arena* arena, f64 floatVal) {
    u32 fracDigits = 15;
    const char* startChar = NULL;
    char str[512];
    u32 length = 0;
    i32 decPos = 0;
    i32 start = str__f32ToCharArr(&startChar, &length, &str[0], &decPos, floatVal, fracDigits);
    if (decPos == 0) {
       mem_copy(mem_arenaPush(arena, length), "0.", 2);
    }
    mem_copy(mem_arenaPush(arena, decPos), startChar, decPos);
    mem_copy(mem_arenaPush(arena, 1),  ".", 1);
    mem_copy(mem_arenaPush(arena, length - decPos), &startChar[decPos], length - decPos);
 }
 // TODO: consider fast coversation alternative: https://github.com/fmtlib/format-benchmark/blob/master/src/u2985907.h
 LOCAL void str_recordU64(Arena* arena, u64 value, bx isNegativ, S8 format) {
    if (isNegativ) {
       u8* ptr = mem_arenaPush(arena, 1);
       ptr[0] = '-';
    }
    if (value == 0) {
       u8* ptr = mem_arenaPush(arena, 1);
       ptr[0] = '0';
       return;
    }
    u8 intStr[21];
    uint32_t pos = 20;
    for (uint64_t tmpVal = value; tmpVal > 0;tmpVal /= 10) {
       uint64_t digit = tmpVal - ((tmpVal / 10) * 10);
       intStr[pos--] = '0' + digit;
    }
    pos += 1;
    u32 length = 21 - pos;
    mem_copy(mem_arenaPush(arena, length), intStr + pos, length);
 }
 
 LOCAL i32 str__ufast_utoa10(u32 value, char* str);
 
 LOCAL void str_recordU32(Arena* arena, u32 value, bx isNegativ, S8 format) {
    if (isNegativ) {
       u8* ptr = mem_arenaPush(arena, 1);
       ptr[0] = '-';
    }
    if (value == 0) {
       u8* ptr = mem_arenaPush(arena, 1);
       ptr[0] = '0';
       return;
    }
    
    int popBy = 10 - str__ufast_utoa10(value, (char*) mem_arenaPush(arena, 10));
    if (popBy > 0) {
       mem_arenaPopTo(arena, arena->pos - popBy);
    }
 }
 
 LOCAL void str_recordChar(Arena* arena, char value) {
    char* ptr = mem_arenaPush(arena, 1);
    ptr[0] = value;
 }
 
 API S8 str_joinVargs(Arena* arena, u32 argCount, va_list inValist) {
    va_list valist;
    va_copy(valist, inValist);
    //va_start(valist, argCount);
 
    S8 strOut;
    str_record(strOut, arena) {
       for (u32 idx = 0; idx < argCount; idx++) {
          str_Value arg = va_arg(valist, str_Value);
          switch (arg.type) {
             case str_argType_custom:   str_recordCustom(arena, &arg.customVal, (S8) {0, 0}); break;
             case str_argType_char:     str_recordChar(arena, arg.charVal); break;
             case str_argType_str:      str_recordStr(arena, arg.strVal); break;
             case str_argType_f32:      str_recordf32_cast(arena, arg.f32Val); break;
             case str_argType_f64:      str_recordF64(arena, arg.f64Val); break;
             case str_argType_u32:      str_recordU32(arena, arg.u32Val, false, (S8){0, 0}); break;
             case str_argType_u64:      str_recordU64(arena, arg.u64Val, false, (S8){0, 0}); break;
             case str_argType_i32:      str_recordU32(arena, absVal(arg.i32Val), arg.i32Val >= 0 ? false : true, (S8){0, 0}); break;
             case str_argType_i64:      str_recordU64(arena, absVal(arg.i64Val), arg.i64Val >= 0 ? false : true, (S8){0, 0}); break;
          }
       }
    }
    va_end(valist);
    return strOut;
 }
 
 S8 str_joinRaw(Arena* arena, u32 argCount, ...) {
    va_list valist;
    va_start(valist, argCount);
    return str_joinVargs(arena, argCount, valist);
 }
 
 S8 str_fmtRaw(Arena* arena, S8 fmt, u32 argCount, ...) {
    va_list valist;
    va_start(valist, argCount);
    return str_fmtVargs(arena, fmt, argCount, valist);
 }
 
 bx str__insertValue(Arena* arena, u32 insertCount, S8 valueFormat, u32 argCount, va_list inValist) {
    va_list valist;
    va_copy(valist, inValist);
    i32 insertIndex = -1;
    i32 fmtIdx = 0;
    if (valueFormat.size == 0 || valueFormat.content[0] == ':') {
       insertIndex = insertCount;
    } else if (valueFormat.size >= 1 && str_isNumberChar(valueFormat.content[0])) {
       u32 number = 0;
       u64 offset = str_parseU32N(valueFormat, &number);
       if (offset != 0) {
          insertIndex = number;
          fmtIdx = offset;
       }
    }
    str_KeyValue arg;
    if (insertIndex != -1) {
       // Find value at index
       if (insertIndex >= argCount) {
          ASSERT(!"Index out of arguments bounds.");
          va_end(valist);
          return false;
       }
       for (u32 idx = 0; idx < argCount; idx++) {
          arg = va_arg(valist, str_KeyValue);
          if (idx == insertIndex) {
             break;
          }
       }
    } else {
       // figure out S8 key and search for it
       u32 count = 0;
       for (; valueFormat.content[count] != ':' && valueFormat.content[count] != '}'; count++);
       S8 key;
       if (count > 1) {
          key.content = valueFormat.content;
          key.size = count;
          fmtIdx = count;
       } else {
          ASSERT(!"Key argument invalid.");
          va_end(valist);
          return false;
       }
       bx keyFound = false;
 
       for (u32 idx = 0; idx < argCount; idx++) {
          arg = va_arg(valist, str_KeyValue);
          if (str_isEqual(arg.key, key)) {
             keyFound = true;
             break;
          }
       }
 
       if (!keyFound) {
          ASSERT(!"Key was not found in arguments");
          va_end(valist);
          return false;
       }
    }
    va_end(valist);
 
    S8 format;
    format.content = NULL;
    format.size = 0;
 
    if (valueFormat.content[fmtIdx] == ':') {
       u8 * f = valueFormat.content + (fmtIdx + 1);
       u32 size = valueFormat.size - (fmtIdx + 1);
       u32 count = 0;
       for (; f[count] != '}' && count < size; count++);
       if (count > 0) {
          format.content = f;
          format.size = count;
       }
    }
 
    switch (arg.value.type) {
       case str_argType_custom:   str_recordCustom(arena, &arg.value.customVal, format); break;
       case str_argType_char:     str_recordChar(arena, arg.value.charVal); break;
       case str_argType_str:      str_recordStr(arena, arg.value.strVal); break;
       case str_argType_f32:      str_recordf32_cast(arena, arg.value.f32Val); break;
       case str_argType_f64:      str_recordF64(arena, arg.value.f64Val); break;
       case str_argType_u32:      str_recordU32(arena, arg.value.u32Val, false, format); break;
       case str_argType_u64:      str_recordU64(arena, arg.value.u64Val, false, format); break;
       case str_argType_i32:      str_recordU32(arena, absVal(arg.value.i32Val), arg.value.i32Val >= 0 ? false : true, format); break;
       case str_argType_i64:      str_recordU64(arena, absVal(arg.value.i64Val), arg.value.i64Val >= 0 ? false : true, format); break;
    }
 
    return true;
 }
 
 S8 str_fmtVargs(Arena* arena, S8 fmt, u32 argCount, va_list list) {
    S8 strOut;
    u32 storedAligmnet = arena->alignment;
    arena->alignment = 1;
    str_record(strOut, arena) {
       char lastChar = 0;
       char currentChar = 0;
       i32 insertCount = 0;
       i32 insertStartIdx = -1;
       i32 textStartIdx = 0;
 
       for (i32 idx = 0; idx < fmt.size; (lastChar = fmt.content[idx], idx++)) {
          currentChar = fmt.content[idx];
          if (insertStartIdx != -1) {
             if (currentChar == '}' && lastChar != '\\') {
                S8 fmtStr;
                fmtStr.content = fmt.content + insertStartIdx + 1;
                fmtStr.size = idx - (insertStartIdx + 1);
                if (str__insertValue(arena, insertCount, fmtStr, argCount, list)) {
                   textStartIdx = idx + 1;
                } else {
                   textStartIdx = insertStartIdx;
                }
                insertStartIdx = -1;
                insertCount += 1;
             }
          } else if (currentChar == '{') {
             if (!(idx < fmt.size)) {
                ASSERT(!"Encountered single '{' at the end of the string!");
                arena->alignment = storedAligmnet;
                return str_lit("");
             }
             char nextChar = fmt.content[idx + 1];
             if (nextChar == '{') {
                idx += 1;
             }
             i32 textSize = idx - textStartIdx;
             if (textSize > 0) {
                // copy text from template
                u8* mem = (u8*) mem_arenaPush(arena, textSize * sizeof(u8));
                mem_copy(mem, (fmt.content + textStartIdx), textSize * sizeof(u8));
             }
             if (nextChar != '{') {
                insertStartIdx = idx;
             }
             textStartIdx = idx + 1;
          } else if (currentChar == '}') {
             if (!(idx < fmt.size)) {
                ASSERT(!"Encountered single '}' at the end of the string!");
                arena->alignment = storedAligmnet;
                return str_lit("");
             }
             char nextChar = fmt.content[idx + 1];
             if (nextChar == '}') {
                idx += 1;
             } else {
                ASSERT(!"Encountered single '}' ");
                arena->alignment = storedAligmnet;
                return str_lit("");
             }
             i32 textSize = idx - textStartIdx;
             if (textSize > 0) {
                // copy text from template
                u8* mem = (u8*) mem_arenaPush(arena, textSize * sizeof(u8));
                mem_copy(mem, (fmt.content + textStartIdx), textSize * sizeof(u8));
             }
             textStartIdx = idx + 1;
          }
       }
       // copy over the remaining text
       u32 size =  fmt.size - textStartIdx;
       if (size > 0) {
          // copy text from template
          u8* mem = (u8*) mem_arenaPush(arena, size * sizeof(u8));
          mem_copy(mem, (fmt.content + textStartIdx), size * sizeof(u8));
       }
    }
 
    arena->alignment = storedAligmnet;
    return strOut;
 }
 
 void str_builderJoinRaw(str_Builder* builder, u32 argCount, ...) {
    ASSERT(builder && "Builder is NULL");
    u32 storedAlignment = builder->arena->alignment;
    str__BuilderBlock* builderBlock = NULL;
    if (builder->lastBlock != NULL && builder->arena->commitPos == builder->arenaLastOffset) {
    } else {
       builderBlock = (str__BuilderBlock*) mem_arenaPush(builder->arena, sizeOf(*builder->lastBlock));
       if (builder->firstBlock == NULL) {
          builder->firstBlock = builderBlock;
          builder->lastBlock = builderBlock;
       } else {
          builder->lastBlock->next = builderBlock;
          builderBlock->prev = builder->lastBlock;
          builder->lastBlock = builderBlock;
       }
    }
 
    va_list valist;
    va_start(valist, argCount);
    str_joinVargs(builder->arena, argCount, valist);
    builder->arenaLastOffset = builder->arena->commitPos;
    builder->arena->alignment = storedAlignment;
 }
 void str_builderFmtRaw(str_Builder* builder, S8 fmt, u32 argCount, ...) {
    ASSERT(builder && "Builder is NULL");
    u32 storedAlignment = builder->arena->alignment;
    str__BuilderBlock* builderBlock = NULL;
    if (builder->lastBlock != NULL && builder->arena->commitPos == builder->arenaLastOffset) {
    } else {
       builderBlock = (str__BuilderBlock*) mem_arenaPush(builder->arena, sizeOf(*builder->lastBlock));
       if (builder->firstBlock == NULL) {
          builder->firstBlock = builderBlock;
          builder->lastBlock = builderBlock;
       } else {
          builder->lastBlock->next = builderBlock;
          builderBlock->prev = builder->lastBlock;
          builder->lastBlock = builderBlock;
       }
    }
 
    va_list valist;
    va_start(valist, argCount);
    str_fmtVargs(builder->arena, fmt, argCount, valist);
    builder->arenaLastOffset = builder->arena->commitPos;
    builder->arena->alignment = storedAlignment;
 }
 
 
 /////////////////
 // Parse u32 to str
 
 i32 str__ufast_utoa10(u32 value, char* str) {
    #define JOIN(N)                                                      \
    N "0", N "1", N "2", N "3", N "4", N "5", N "6", N "7", N "8", N "9" \
 
    #define JOIN2(N)                                                     \
    JOIN(N "0"), JOIN(N "1"), JOIN(N "2"), JOIN(N "3"), JOIN(N "4"),     \
    JOIN(N "5"), JOIN(N "6"), JOIN(N "7"), JOIN(N "8"), JOIN(N "9")      \
 
    #define JOIN3(N)                                                      \
    JOIN2(N "0"), JOIN2(N "1"), JOIN2(N "2"), JOIN2(N "3"), JOIN2(N "4"), \
    JOIN2(N "5"), JOIN2(N "6"), JOIN2(N "7"), JOIN2(N "8"), JOIN2(N "9")  \
 
    #define JOIN4                                               \
    JOIN3("0"), JOIN3("1"), JOIN3("2"), JOIN3("3"), JOIN3("4"), \
    JOIN3("5"), JOIN3("6"), JOIN3("7"), JOIN3("8"), JOIN3("9")  \
 
    #define JOIN5(N)                                                \
    JOIN(N), JOIN(N "1"), JOIN(N "2"), JOIN(N "3"), JOIN(N "4"),    \
    JOIN(N "5"), JOIN(N "6"), JOIN(N "7"), JOIN(N "8"), JOIN(N "9") \
 
    #define JOIN6                                              \
    JOIN5(""), JOIN2("1"), JOIN2("2"), JOIN2("3"), JOIN2("4"), \
    JOIN2("5"), JOIN2("6"), JOIN2("7"), JOIN2("8"), JOIN2("9") \
 
    #define F(N)  ((N) >= 100 ? 3 : (N) >= 10 ? 2 : 1)
 
    #define F10(N)                                   \
    F(N), F(N + 1), F(N + 2), F(N + 3), F(N + 4),    \
    F(N + 5), F(N + 6), F(N + 7), F(N + 8), F(N + 9) \
 
    #define F100(N)                                    \
    F10(N), F10(N + 10), F10(N + 20), F10(N + 30),     \
    F10(N + 40), F10(N + 50), F10(N + 60), F10(N + 70),\
    F10(N + 80), F10(N + 90)                           \
 
    static const short offsets[] = {
                                     F100(  0), F100(100), F100(200), F100(300), F100(400),
                                     F100(500), F100(600), F100(700), F100(800), F100(900)
                                     };
 
    static const char table1[][4] = { JOIN ("") };
    static const char table2[][4] = { JOIN2("") };
    static const char table3[][4] = { JOIN3("") };
    static const char table4[][8] = { JOIN4 };
    static const char table5[][4] = { JOIN6 };
 
    #undef JOIN
    #undef JOIN2
    #undef JOIN3
    #undef JOIN4
    #undef F
    #undef F10
    #undef F100
 
    char *wstr;
    #if (_WIN64 || __x86_64__ || __ppc64__)
       uint64_t remains[2];
    #else
       uint32_t remains[2];
    #endif
    unsigned int v2;
 
    if (value >= 100000000)
    {
       #if (_WIN64 || __x86_64__ || __ppc64__)
          remains[0] = (((uint64_t)value * (uint64_t)3518437209) >> 45);
          remains[1] = (((uint64_t)value * (uint64_t)2882303762) >> 58);
       #else
          remains[0] = value / 10000;
          remains[1] = value / 100000000;
       #endif
       v2 = remains[1];
       remains[1] = remains[0] - remains[1] * 10000;
       remains[0] = value - remains[0] * 10000;
       if (v2 >= 10)
       {
          memcpy(str,table5[v2],2);
          str += 2;
          memcpy(str,table4[remains[1]],4);
          str += 4;
          memcpy(str,table4[remains[0]],4);
          return 10;
       }
       else
       {
          *(char *) str = v2 + '0';
          str += 1;
          memcpy(str,table4[remains[1]],4);
          str += 4;
          memcpy(str,table4[remains[0]],4);
          return 9;
       }
    }
    else if (value >= 10000)
    {
       #if (_WIN64 || __x86_64__ || __ppc64__)
          v2 = (((uint64_t)value * (uint64_t)3518437209 ) >> 45);
       #else
          v2 = value / 10000;
       #endif
       remains[0] = value - v2 * 10000;
       if (v2 >= 1000)
       {
          memcpy(str,table4[v2],4);
          str += 4;
          memcpy(str,table4[remains[0]],4);
          return 8;
       }
       else
       {
          wstr = str;
          memcpy(wstr,table5[v2],4);
          wstr += offsets[v2];
          memcpy(wstr,table4[remains[0]],4);
          wstr += 4;
          return (wstr - str);
       }
    }
    else
    {
       if (value >= 1000)
       {
          memcpy(str,table4[value],4);
          return 4;
       }
       else if (value >= 100)
       {
          memcpy(str,table3[value],3);
          return 3;
       }
       else if (value >= 10)
       {
          memcpy(str,table2[value],2);
          return 2;
       }
       else
       {
          *(char *) str = *(char *) table1[value];
          return 1;
       }
    }
 }
 
 LOCAL i32 str__ufast_itoa10(i32 value, char* str) {
   if (value < 0) { *(str++) = '-'; 
     return str__ufast_utoa10(-value, str) + 1; 
   }
   else return str__ufast_utoa10(value, str);
 }
 
 /////////////////
 // Parse float in str
 // this could be an alternative: https://github.com/charlesnicholson/nanoprintf/blob/bb60443d4821b86482e3c70329fc10280f1e6aa4/nanoprintf.h#L602
 
 // copies d to bits w/ strict aliasing (this compiles to nothing on /Ox)
 #define STR__COPYFP(dest, src)                     \
    {                                               \
       int cn;                                      \
       for (cn = 0; cn < 8; cn++)                   \
          ((char *)&dest)[cn] = ((char *)&src)[cn]; \
    }
 
 // get float info
 static int32_t str__real_to_parts(int64_t *bits, int32_t *expo, double value)
 {
    double d;
    int64_t b = 0;
 
    // load value and round at the frac_digits
    d = value;
 
    STR__COPYFP(b, d);
 
    *bits = b & ((((uint64_t)1) << 52) - 1);
    *expo = (int32_t)(((b >> 52) & 2047) - 1023);
 
    return (int32_t)((uint64_t) b >> 63);
 }
 
 #define STR__SPECIAL 0x7000
 
 #if defined(_MSC_VER) && (_MSC_VER <= 1200)
 static uint64_t const str__powten[20] = {
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
    1000000000,
    10000000000,
    100000000000,
    1000000000000,
    10000000000000,
    100000000000000,
    1000000000000000,
    10000000000000000,
    100000000000000000,
    1000000000000000000,
    10000000000000000000U
 };
 #define str__tento19th ((uint64_t)1000000000000000000)
 #else
 static uint64_t const str__powten[20] = {
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
    1000000000,
    10000000000ULL,
    100000000000ULL,
    1000000000000ULL,
    10000000000000ULL,
    100000000000000ULL,
    1000000000000000ULL,
    10000000000000000ULL,
    100000000000000000ULL,
    1000000000000000000ULL,
    10000000000000000000ULL
 };
 #define str__tento19th (1000000000000000000ULL)
 #endif
 
 #define str__ddmulthi(oh, ol, xh, yh)                            \
    {                                                               \
       double ahi = 0, alo, bhi = 0, blo;                           \
       int64_t bt;                                             \
       oh = xh * yh;                                                \
       STR__COPYFP(bt, xh);                                       \
       bt &= ((~(uint64_t)0) << 27);                           \
       STR__COPYFP(ahi, bt);                                      \
       alo = xh - ahi;                                              \
       STR__COPYFP(bt, yh);                                       \
       bt &= ((~(uint64_t)0) << 27);                           \
       STR__COPYFP(bhi, bt);                                      \
       blo = yh - bhi;                                              \
       ol = ((ahi * bhi - oh) + ahi * blo + alo * bhi) + alo * blo; \
    }
 
 #define str__ddtoS64(ob, xh, xl)          \
    {                                        \
       double ahi = 0, alo, vh, t;           \
       ob = (int64_t)ph;                \
       vh = (double)ob;                      \
       ahi = (xh - vh);                      \
       t = (ahi - xh);                       \
       alo = (xh - (ahi - t)) - (vh + t);    \
       ob += (int64_t)(ahi + alo + xl); \
    }
 
 #define str__ddrenorm(oh, ol) \
    {                            \
       double s;                 \
       s = oh + ol;              \
       ol = ol - (s - oh);       \
       oh = s;                   \
    }
 
 #define str__ddmultlo(oh, ol, xh, xl, yh, yl) ol = ol + (xh * yl + xl * yh);
 
 #define str__ddmultlos(oh, ol, xh, yl) ol = ol + (xh * yl);
 
 static void str__raiseToPower10(double *ohi, double *olo, double d, int32_t power) { // power can be -323 to +350
     static double const str__bot[23] = {
         1e+000, 1e+001, 1e+002, 1e+003, 1e+004, 1e+005, 1e+006, 1e+007, 1e+008, 1e+009, 1e+010, 1e+011,
         1e+012, 1e+013, 1e+014, 1e+015, 1e+016, 1e+017, 1e+018, 1e+019, 1e+020, 1e+021, 1e+022
     };
     static double const str__negbot[22] = {
         1e-001, 1e-002, 1e-003, 1e-004, 1e-005, 1e-006, 1e-007, 1e-008, 1e-009, 1e-010, 1e-011,
         1e-012, 1e-013, 1e-014, 1e-015, 1e-016, 1e-017, 1e-018, 1e-019, 1e-020, 1e-021, 1e-022
     };
     static double const str__negboterr[22] = {
         -5.551115123125783e-018,  -2.0816681711721684e-019, -2.0816681711721686e-020, -4.7921736023859299e-021, -8.1803053914031305e-022, 4.5251888174113741e-023,
         4.5251888174113739e-024,  -2.0922560830128471e-025, -6.2281591457779853e-026, -3.6432197315497743e-027, 6.0503030718060191e-028,  2.0113352370744385e-029,
         -3.0373745563400371e-030, 1.1806906454401013e-032,  -7.7705399876661076e-032, 2.0902213275965398e-033,  -7.1542424054621921e-034, -7.1542424054621926e-035,
         2.4754073164739869e-036,  5.4846728545790429e-037,  9.2462547772103625e-038,  -4.8596774326570872e-039
     };
     static double const str__top[13] = {
         1e+023, 1e+046, 1e+069, 1e+092, 1e+115, 1e+138, 1e+161, 1e+184, 1e+207, 1e+230, 1e+253, 1e+276, 1e+299
     };
     static double const str__negtop[13] = {
         1e-046, 1e-069, 1e-092, 1e-115, 1e-138, 1e-161, 1e-184, 1e-207, 1e-230, 1e-253, 1e-276, 1e-299
     };
     static double const str__toperr[13] = {
         8388608,
         6.8601809640529717e+028,
         -7.253143638152921e+052,
         -4.3377296974619174e+075,
         -1.5559416129466825e+098,
         -3.2841562489204913e+121,
         -3.7745893248228135e+144,
         -1.7356668416969134e+167,
         -3.8893577551088374e+190,
         -9.9566444326005119e+213,
         6.3641293062232429e+236,
         -5.2069140800249813e+259,
         -5.2504760255204387e+282
     };
     static double const str__negtoperr[13] = {
         3.9565301985100693e-040,  -2.299904345391321e-063,  3.6506201437945798e-086,  1.1875228833981544e-109,
         -5.0644902316928607e-132, -6.7156837247865426e-155, -2.812077463003139e-178,  -5.7778912386589953e-201,
         7.4997100559334532e-224,  -4.6439668915134491e-247, -6.3691100762962136e-270, -9.436808465446358e-293,
         8.0970921678014997e-317
     };
 
    double ph, pl;
    if ((power >= 0) && (power <= 22)) {
       str__ddmulthi(ph, pl, d, str__bot[power]);
    } else {
       int32_t e, et, eb;
       double p2h, p2l;
 
       e = power;
       if (power < 0)
          e = -e;
       et = (e * 0x2c9) >> 14; /* %23 */
       if (et > 13)
          et = 13;
       eb = e - (et * 23);
 
       ph = d;
       pl = 0.0;
       if (power < 0) {
          if (eb) {
             --eb;
             str__ddmulthi(ph, pl, d, str__negbot[eb]);
             str__ddmultlos(ph, pl, d, str__negboterr[eb]);
          }
          if (et) {
             str__ddrenorm(ph, pl);
             --et;
             str__ddmulthi(p2h, p2l, ph, str__negtop[et]);
             str__ddmultlo(p2h, p2l, ph, pl, str__negtop[et], str__negtoperr[et]);
             ph = p2h;
             pl = p2l;
          }
       } else {
          if (eb) {
             e = eb;
             if (eb > 22)
                eb = 22;
             e -= eb;
             str__ddmulthi(ph, pl, d, str__bot[eb]);
             if (e) {
                str__ddrenorm(ph, pl);
                str__ddmulthi(p2h, p2l, ph, str__bot[e]);
                str__ddmultlos(p2h, p2l, str__bot[e], pl);
                ph = p2h;
                pl = p2l;
             }
          }
          if (et) {
             str__ddrenorm(ph, pl);
             --et;
             str__ddmulthi(p2h, p2l, ph, str__top[et]);
             str__ddmultlo(p2h, p2l, ph, pl, str__top[et], str__toperr[et]);
             ph = p2h;
             pl = p2l;
          }
       }
    }
    str__ddrenorm(ph, pl);
    *ohi = ph;
    *olo = pl;
 }
 
 LOCAL i32 str__f32ToCharArr(char const** start, u32 *len, char* out, i32* decimal_pos, f64 value, i32 frac_digits) {
    f64 d;
    i64 bits = 0;
    i32 expo, e, ng, tens;
 
    d = value;
    STR__COPYFP(bits, d);
    expo = (int32_t)((bits >> 52) & 2047);
    ng = (int32_t)((uint64_t) bits >> 63);
    if (ng)
       d = -d;
 
    if (expo == 2047) // is nan or inf?
    {
       *start = (bits & ((((uint64_t)1) << 52) - 1)) ? "NaN" : "Inf";
       *decimal_pos = STR__SPECIAL;
       *len = 3;
       return ng;
    }
 
    if (expo == 0) // is zero or denormal
    {
       if (((uint64_t) bits << 1) == 0) // do zero
       {
          *decimal_pos = 1;
          *start = out;
          out[0] = '0';
          *len = 1;
          return ng;
       }
       // find the right expo for denormals
       {
          int64_t v = ((uint64_t)1) << 51;
          while ((bits & v) == 0) {
             --expo;
             v >>= 1;
          }
       }
    }
 
    // find the decimal exponent as well as the decimal bits of the value
    {
       double ph, pl;
 
       // log10 estimate - very specifically tweaked to hit or undershoot by no more than 1 of log10 of all expos 1..2046
       tens = expo - 1023;
       tens = (tens < 0) ? ((tens * 617) / 2048) : (((tens * 1233) / 4096) + 1);
 
       // move the significant bits into position and stick them into an int
       str__raiseToPower10(&ph, &pl, d, 18 - tens);
 
       // get full as much precision from double-double as possible
       str__ddtoS64(bits, ph, pl);
 
       // check if we undershot
       if (((uint64_t)bits) >= str__tento19th)
          ++tens;
    }
 
    // now do the rounding in integer land
    frac_digits = (frac_digits & 0x80000000) ? ((frac_digits & 0x7ffffff) + 1) : (tens + frac_digits);
    if ((frac_digits < 24)) {
       uint32_t dg = 1;
       if ((uint64_t)bits >= str__powten[9])
          dg = 10;
       while ((uint64_t)bits >= str__powten[dg]) {
          ++dg;
          if (dg == 20)
             goto noround;
       }
       if (frac_digits < dg) {
          uint64_t r;
          // add 0.5 at the right position and round
          e = dg - frac_digits;
          if ((uint32_t)e >= 24)
             goto noround;
          r = str__powten[e];
          bits = bits + (r / 2);
          if ((uint64_t)bits >= str__powten[dg])
             ++tens;
          bits /= r;
       }
    noround:;
    }
 
    // kill long trailing runs of zeros
    if (bits) {
       uint32_t n;
       for (;;) {
          if (bits <= 0xffffffff)
             break;
          if (bits % 1000)
             goto donez;
          bits /= 1000;
       }
       n = (uint32_t)bits;
       while ((n % 1000) == 0)
          n /= 1000;
       bits = n;
    donez:;
    }
    static const struct {
      short temp; // force next field to be 2-byte aligned
      char pair[201];
    } str__digitpair = {
      0,
      "00010203040506070809101112131415161718192021222324"
      "25262728293031323334353637383940414243444546474849"
      "50515253545556575859606162636465666768697071727374"
      "75767778798081828384858687888990919293949596979899"
    };
 
    // convert to string
    out += 64;
    e = 0;
    for (;;) {
       uint32_t n;
       char *o = out - 8;
       // do the conversion in chunks of U32s (avoid most 64-bit divides, worth it, constant denomiators be damned)
       if (bits >= 100000000) {
          n = (uint32_t)(bits % 100000000);
          bits /= 100000000;
       } else {
          n = (uint32_t)bits;
          bits = 0;
       }
       while (n) {
          out -= 2;
          *(uint16_t *)out = *(uint16_t *)&str__digitpair.pair[(n % 100) * 2];
          n /= 100;
          e += 2;
       }
       if (bits == 0) {
          if ((e) && (out[0] == '0')) {
             ++out;
             --e;
          }
          break;
       }
       while (out != o) {
          *--out = '0';
          ++e;
       }
    }
 
    *decimal_pos = tens;
    *start = out;
    *len = e;
    return ng;
 }
 
 S8 str_floatToStr(f64 value, S8 storeStr, i32* decimalPos, i32 fracDigits) {
    fracDigits = fracDigits >= 0 ? fracDigits : 15;
    fracDigits = minVal(fracDigits, 15);
    const char* startChar = NULL;
    u8 str[15 * 2];
    u32 length = 0;
    i32 decPos = 0;
    // (char const** start, u32 *len, char* out, i32* decimal_pos, f64 value, i32 frac_digits)
    i32 start = str__f32ToCharArr(&startChar, &length, (char*) storeStr.content, decimalPos, value, fracDigits);
    S8 outStr;
    outStr.content = length == 0 ? NULL : storeStr.content + start;
    outStr.size = length;
    return outStr;
 }
 
 S8 str_u32ToHex(Arena* arena, u32 value) {
    static const u8 digits[] = "0123456789ABCDEF";
    static const u32 hexLen = 4 << 1;
    u8 strTmp[4 << 1];
    strTmp[0] = digits[0];
    u32 i = 0;
    for (u32 j = (hexLen - 1) * 4, f = 0; f < hexLen;j -= 4, f += 1) {
       u32 digitIdx = value >> j;
       if (i == 0 && digitIdx == 0) {
          continue;
       }
       strTmp[i] = digits[(value >> j) & 0x0f];
       i += 1;
    }
    S8 raw;
    raw.content = &strTmp[0];
    raw.size = i == 0 ? 1 : i;
 
    return str_join(arena, s8("0x"), raw);
 }

///////////////////////////////////////////
// BASE ARGS //////////////////////////////
///////////////////////////////////////////

arg_Ctx arg_makeCtx(arg_Opt* opts, u32 optsCount, char** inputOpts, u32 inputOptsCount) {
    arg_Ctx ctx = {opts, optsCount, inputOpts, inputOptsCount};
    return ctx;
}

S8 arg_createHelpText(Arena* arena, arg_Ctx* ctx) {
    for (u64 idx = 0; idx < ctx->optCount; idx++) {
        arg_Opt* opt = ctx->opts + idx;
        str_join(arena, s8("argument:"), opt->name, s8(" or "), opt->shortName, s8("\n"));
    }
    return str8("");
}

i32 arg_nextOpt(arg_Ctx* ctx) {
    while (ctx->nextIdx < ctx->inputOptsCount) {
        char* charOpt = ctx->inputOpts[ctx->nextIdx];
        S8 key = str_fromNullTerminatedCharPtr(charOpt);
        //i64 firstSpace = str_findChar(opt, ' ');
        //S8 key;
        bx longName = false;
        if (str_hasPrefix(key, str8("--"))) {
            longName = true;
            key = str_subStr(key, 2, key.size);
            if (key.size == 0) {
                ctx->nextIdx++;
                continue;
            }
        } else if (str_hasPrefix(key, str8("-")) && key.size > 1) {
            key = str_subStr(key, 1, key.size);
        } else {
            // skip arguments that don't start with "-" or "--"
            ctx->nextIdx++;
            continue;
        }
        S8 value = STR_EMPTY;
        if ((ctx->nextIdx + 1) < ctx->inputOptsCount) {
            char* charOpt = ctx->inputOpts[ctx->nextIdx + 1];
            if (charOpt[0] != '-') {
                value = str_fromNullTerminatedCharPtr(charOpt);
                ctx->nextIdx++;
            }
        }
        
        for (u64 idx = 0; idx < ctx->optCount; idx++) {
            arg_Opt* opt = ctx->opts + idx;
            if (longName ? str_isEqual(key, opt->name) : (key.size == 1 && key.content[0] == opt->shortName)) {
                ctx->foundKey   = opt->name;
                ctx->foundValue = value.size > 0 ? value : opt->defaultValue;
                ctx->foundi32Value  = 0;
                ctx->foundF32Value  = 0;
                ctx->foundFlagsValue = 0;
                ctx->currentOpt = opt;
                ctx->nextIdx++;

                // check for duplicate
                u64 flag = 1ull << idx;
                bx existAlready = (ctx->redArgsBitField & flag) == flag;
                if (existAlready && !(opt->flags & arg_flag_duplicatesAllowed)) {
                    // duplicated argument
                    return arg_error_duplicate;
                }
                ctx->redArgsBitField |= flag;
                if (opt->flags & arg_flag_requiredValue) {
                    if (value.size == 0) {
                        // missing required value
                        return arg_error_missingValue;
                    }
                }
                switch (opt->type) {
                    case arg_optType_string: break;
                    case arg_optType_f32: {
                        u64 offset = str_parseF32N(value, &ctx->foundF32Value);
                        if (offset != (value.size - 1)) {
                            return arg_error_invalidValue;
                        }
                    } break;
                    case arg_optType_i32: {
                        i64 outVal = 0;
                        u64 offset = str_parseS64N(value, &outVal);
                        if (offset != (value.size - 1)) {
                            return arg_error_invalidValue;
                        }
                    } break;
                    case arg_optType_flags: {
                        u64 offset = str_parseU32N(value, &ctx->foundFlagsValue);
                        if (offset != (value.size - 1)) {
                            return arg_error_invalidValue;
                        }
                    } break;
                    default: break;
                }
                return opt->shortName;
            }
        }
        ctx->nextIdx++;
    }

    // search for missing arguments
    for (u32 idx = ctx->nextIdx - ctx->inputOptsCount; (ctx->nextIdx - ctx->inputOptsCount) < ctx->optCount; ctx->inputOptsCount++) {
        arg_Opt* opt = ctx->opts + (ctx->nextIdx - ctx->inputOptsCount);
            if (opt->flags & arg_flag_required) {
            u64 flag = idx << 1ull;
            bx existAlready = (ctx->redArgsBitField & flag) == flag;
            if (!existAlready) {
                // duplicated argument
                return arg_error_missingArg;
            }
        }
    }

    ctx->foundKey        = str8("");
    ctx->foundValue      = str8("");
    ctx->foundi32Value   = 0;
    ctx->foundF32Value   = 0;
    ctx->foundFlagsValue = 0;
    ctx->currentOpt = NULL;
    return arg_end;
}
///////////////////////////////////////////
// BASE MEM ///////////////////////////////
///////////////////////////////////////////


void* mem__reserve(void* ctx, u64 size) {
    unused(ctx);
    return malloc(size);
}

void mem__commit(void* ctx, void* ptr, u64 size) {
    unusedVars(ctx, size);
}

void mem__decommit(void* ctx, void* ptr, u64 size) {
    unusedVars(ctx, ptr, size);
}

void mem__release(void* ctx, void* ptr, u64 size) {
    unusedVars(ctx, ptr, size);
}

BaseMemory mem_getMallocBaseMem(void) {
    BaseMemory baseMem;
    baseMem.ctx = NULL;
    baseMem.pageSize = OS_DEFAULT_PAGE_SIZE;
    baseMem.reserve = mem__reserve;
    baseMem.commit = mem__commit;
    baseMem.decommit = mem__decommit;
    baseMem.release = mem__release;

    return baseMem;
}


u64 mem_getArenaMemOffsetPos(Arena* arena) {
    u64 offset = ((u64) &arena->memory[0]) - ((u64) arena);
    return arena->pos - offset;
}

void* mem_arenaPush(Arena* arena, u64 size) {
    ASSERT(arena);
    void* result = NULL;
    if (!arena->unsafeRecord && arena->alignment > 1) {
        size = alignUp(size, arena->alignment);
    }
    if (arena->pos + size <= arena->cap) {
        result = arena->memory + (arena->pos - (u64_cast(&arena->memory[0]) - u64_cast(arena)));
        arena->pos += size;

        u64 p = arena->pos;
        u64 commitP = arena->commitPos;
        if (p > commitP) {
            u64 pAlign      = alignUp(p, arena->base.pageSize);
            u64 nextCommitP = clampTop(pAlign, arena->cap);
            u64 commitSize  = nextCommitP - commitP;
            arena->base.commit(arena->base.ctx, ((u8*) arena) + arena->commitPos, commitSize);
            arena->commitPos = nextCommitP;
        }
    }
    return result;
}

void mem_arenaPopTo(Arena* arena, u64 pos) {
    ASSERT(arena);
    pos = maxVal(sizeof(Arena), pos);
    if (pos < arena->pos) {
        arena->pos      = pos;

        u64 p           = arena->pos;
        u64 pAlign      = alignUp(p, arena->base.pageSize);
        u64 nextCommitP = clampTop(pAlign, arena->cap);
        u64 commitP     = arena->commitPos;
        if (nextCommitP < commitP) {
            u64 decommitSize = commitP - nextCommitP;
            arena->base.decommit(arena->base.ctx, ((u8*) arena) + nextCommitP, decommitSize);
            arena->commitPos = nextCommitP;
        }
    }
}
void mem_arenaPopAmount(Arena* arena, u64 amount) {
    mem_arenaPopTo(arena, arena->pos - amount);
}

mem_Scratch mem_scratchStart(Arena* arena) {
    mem_Scratch scratch;
    scratch.arena = arena;
    scratch.start = arena->pos;
    return scratch;
}

void mem_scratchEnd(mem_Scratch* scratch) {
    mem_arenaPopTo(scratch->arena, scratch->start);
}

typedef struct mem__MallocEntry {
    struct mem__MallocEntry* prev;
    struct mem__MallocEntry* next;
    bx      active : 1;
    u64     size;
    u8      mem[0];
} mem__MallocEntry;

typedef struct ManagedAlloc {
    Arena* arena;
    mem__MallocEntry* first;
    mem__MallocEntry* last;
    mem__MallocEntry* firstHole;
    u64 largestHole;
    u8 mem[0];
} ManagedAlloc;

ManagedAlloc* mem_makeManagedAlloc(Arena* arena) {
    ManagedAlloc* mallocCtx = mem_arenaPush(arena, sizeof(ManagedAlloc));
    mem_setZero(mallocCtx, sizeof(ManagedAlloc));
    mallocCtx->arena = arena;
    return mallocCtx;
}

void* mem_managedAlloc(ManagedAlloc* malloc, u64 size) {
    if (malloc->firstHole) {
        for (mem__MallocEntry* allocation = malloc->firstHole; allocation != NULL; allocation = allocation->next) {
            if (allocation->active) {
                // freeSize = 0;
                // prevFree = NULL;
                continue;
            }
            if (allocation->size >= size) {
                // todo: divide up the allocation when the unused mem is too big
                allocation->active = true;
                if (malloc->firstHole->active) {
                    mem__MallocEntry* firstHole = malloc->firstHole;
                    malloc->firstHole = NULL;
                    for (;firstHole;firstHole = firstHole->next) {
                        if (!firstHole->active) {
                            malloc->firstHole = firstHole;
                            break;
                        }
                    }
                }
                return &allocation->mem[0];
            }
        }
    }

    mem__MallocEntry* allocation = (mem__MallocEntry*) mem_arenaPush(malloc->arena, sizeof(mem__MallocEntry) + size);
    allocation->prev = NULL;
    if (!malloc->first) {
        malloc->first = allocation;
    }
    allocation->prev = malloc->last;
    if (!malloc->last) {
        malloc->last = allocation;
    }
    malloc->last = allocation;
    return &allocation->mem[0];
}


LOCAL void* arena__allocFn(u64 size, void* userPtr) {
    Arena* arena = (Arena*) userPtr;
    return mem_arenaPush(arena, size);
}

LOCAL void* arena__reallocFn(u64 size, void* oldPtr, u64 oldSize, void* userPtr) {
    Arena* arena = (Arena*) userPtr;
    void* newMem = mem_arenaPush(arena, size);

    mem_copy(newMem, oldPtr, oldSize);

    return newMem;
}

LOCAL void arena__freeFn(void* ptr, void* userPtr) {
    unusedVars(ptr, userPtr);
}

Arena* mem_makeArena(BaseMemory* baseMem, u64 cap) {
    return mem_makeArenaAligned(baseMem, cap, 16);
}

Arena* mem_makeArenaAligned(BaseMemory* baseMem, u64 cap, u64 aligment) {
    u32 arr = sizeOf(Arena);
    ASSERT(baseMem);
    ASSERT(baseMem->reserve);
    ASSERT(baseMem->commit);
    ASSERT(baseMem->decommit);
    ASSERT(baseMem->release);

    u64 p            = sizeof(Arena);
    u64 pAlign       = alignUp(p, baseMem->pageSize);
    u64 commitSize   = clampTop(pAlign, cap);

    void* mem = baseMem->reserve(NULL, cap);
    baseMem->commit(NULL, mem, commitSize);
    Arena* arena = (Arena*) mem;
    mem_structSetZero(arena);
    arena->allocator.alloc = arena__allocFn;
    arena->allocator.realloc = arena__reallocFn;
    arena->allocator.free = arena__freeFn;
    arena->allocator.allocator = arena;
    arena->base = *baseMem;
    arena->cap = cap;
    arena->commitPos = commitSize;
    arena->alignment = aligment;

    arena->pos = ((u64) &arena->memory[0]) - ((u64) arena);
    return arena;
}

void mem_destroyArena(Arena* arena) {
    ASSERT(arena);
    ASSERT(arena->base.release);
    ASSERT(arena->cap > 0);

    mms cap = arena->cap;
    arena->commitPos = 0;
    arena->cap = 0;
    arena->base.release(arena->base.ctx, (void*) arena, cap);
}


u64 mem_arenaStartUnsafeRecord(Arena* arena) {
    ASSERT(arena);
    arena->unsafeRecord += 1;
    return mem_getArenaMemOffsetPos(arena);
}
void mem_arenaStopUnsafeRecord(Arena* arena) {
    ASSERT(arena);
    ASSERT(arena->unsafeRecord > 0);
    arena->unsafeRecord -= 1;

    if (arena->unsafeRecord == 0) {
        arena->pos = alignUp(arena->pos, arena->alignment);
    }
}

LOCAL void* mem__reservePre(void* ctx, u64 size) {
    unusedVars(ctx, size);
    return ctx;
}

LOCAL void mem__commitPre(void* ctx, void* ptr, u64 size) {
    unusedVars(ctx, size, ptr);
}

LOCAL void mem__decommitPre(void* ctx, void* ptr, u64 size) {
    unusedVars(ctx, size, ptr);
}

LOCAL void mem__releasePre(void* ctx, void* ptr, u64 size) {
    unusedVars(ctx, size, ptr);
}

Arena* mem_makeArenaPreAllocated(void* mem, u64 size) {
    Arena* arena = (Arena*) mem;
    mem_setZero(arena, sizeof(Arena));
    arena->base.ctx = mem;
    arena->base.pageSize = size;
    arena->base.reserve = mem__reserve;
    arena->base.commit = mem__commitPre;
    arena->base.decommit = mem__decommitPre;
    arena->base.release = mem__releasePre;

    arena->cap = size;
    arena->unsafeRecord = 0;
    arena->alignment = 16;
    arena->commitPos = size;
    arena->pos = ((u64) &arena->memory[0]) - ((u64) arena);

    return arena;
}

///////////////////////////////////////////
// BASE TIME //////////////////////////////
///////////////////////////////////////////


#if OS_WIN
#else
#include "time.h"

DateTime tm_nowGm(void) {
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    struct tm tt;
    struct tm* res = gmtime_r(&spec.tv_sec, &tt);
    ASSERT(res != NULL);
    DateTime dateTime = {0};
    dateTime.year  = res->tm_year;
    dateTime.month   = res->tm_mon;
    dateTime.day   = res->tm_mday;
    dateTime.hour  = res->tm_hour;
    dateTime.second   = res->tm_sec;
    dateTime.milliSecond  = roundVal(spec.tv_nsec, 1000000);

    return dateTime;
}

DateTime tm_nowLocal(void) {
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    struct tm tt;
    struct tm* res = localtime_r(&spec.tv_sec, &tt);
    ASSERT(res != NULL);
    DateTime dateTime = {0};
    dateTime.year  = res->tm_year;
    dateTime.month   = res->tm_mon;
    dateTime.day   = res->tm_mday;
    dateTime.hour  = res->tm_hour;
    dateTime.second   = res->tm_sec;
    dateTime.milliSecond  = roundVal(spec.tv_nsec, 1000000);

    return dateTime;
}

#endif // OS_WIN


DenseTime tm_toDenseTime(DateTime in) {
    u32 yearEncode = u32_cast(i32_cast(in.year+ 0x8000));
    u64 result = 0;
    result += yearEncode;
    result *= 12;
    result += in.month;
    result *= 31;
    result += in.day;
    result *= 24;
    result += in.hour;
    result *= 60;
    result += in.minute;
    result *= 61;
    result += in.second;
    result *= 1000;
    result += in.milliSecond;
    DenseTime denseTime = {result};
    return denseTime;
}

DateTime tm_fromDenseTime(DenseTime denseTime) {
    u64 in = denseTime.value;
    DateTime result;
    result.milliSecond = in % 1000;
    in /= 1000;
    result.second = in % 61;
    in /= 61;
    result.minute = in % 60;
    in /= 60;
    result.hour = in % 24;
    in /= 24;
    result.day = in % 31;
    in /= 31;
    result.month = in % 12;
    in /= 12;
    i32 yearEncoded = i32_cast(in);
    result.year = (yearEncoded - 0x8000);
    return result;
}

#if OS_WIN
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif // WIN32_LEAN_AND_MEAN
#elif OS_ANDROID
#include <time.h>
#elif OS_EMSCRIPTEN
#include <emscripten.h>
#elif OS_APPLE
#include <mach/mach_time.h>
#else
#include <sys/time.h>
#endif 

u64 tm_currentCount(void) {
    u64 counter = u64_max;
#if OS_WIN
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    counter = li.QuadPart;
#elif OS_ANDROID
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    counter = now.tv_sec*INT64_C(1000000000) + now.tv_nsec;
#elif OS_EMSCRIPTEN
    counter = i64_cast(1000.0f * emscripten_get_now());
#elif OS_APPLE
    counter = mach_absolute_time();
#else
    struct timeval now;
    gettimeofday(&now, 0);
    counter = now.tv_sec * i64_cast(1000000) + now.tv_usec;
#endif
    return counter;
}

tm_FrequencyInfo tm_getPerformanceFrequency(void) {
    tm_FrequencyInfo freq;
    freq.frequency = i64_val(1000000000);
#if  OS_WIN
    LARGE_INTEGER li;
    QueryPerformanceFrequency(&li);
	freq.frequency = li.QuadPart;
#elif OS_ANDROID
    freq.frequency = i64_val(1000000000);
#elif OS_EMSCRIPTEN
	freq.frequency = i64_val(1000000);
#elif OS_APPLE
    mach_timebase_info_data_t info = {0};
    kern_return_t result = mach_timebase_info(&info);
    if (result == KERN_SUCCESS) {
        freq.numer = info.numer;
        freq.denom = info.denom;
    }
#endif
    return freq;
}

INLINE i32 tm__i64Muldiv(i64 value, i64 numer, i64 denom) {
    i64 q = value / denom;
    i64 r = value % denom;
    return q * numer + r * numer / denom;
}

u64 tm_countToNanoseconds( tm_FrequencyInfo info, i64 count) {
    u64 now = 0;
#if OS_WIN
    now = (u64) tm__i64Muldiv(count, 1000000000, info.frequency);
#elif OS_APPLE
    now = u64_cast(((count * i64_cast(info.numer))) / i64_cast(info.denom));
#elif OS_EMSCRIPTEN
    f64 js_now = count;
    now = u64_cast(count * info.frequency) / 1000;
#else
    now = count * info.frequency;
#endif
    return now;
}

static const u64 tm__refreshRates[][2] = {
    { 16666667, 1000000 },  //  60 Hz: 16.6667 +- 1ms
    { 13888889,  250000 },  //  72 Hz: 13.8889 +- 0.25ms
    { 13333333,  250000 },  //  75 Hz: 13.3333 +- 0.25ms
    { 11764706,  250000 },  //  85 Hz: 11.7647 +- 0.25
    { 11111111,  250000 },  //  90 Hz: 11.1111 +- 0.25ms
    { 10000000,  500000 },  // 100 Hz: 10.0000 +- 0.5ms
    {  8333333,  500000 },  // 120 Hz:  8.3333 +- 0.5ms
    {  6944445,  500000 },  // 144 Hz:  6.9445 +- 0.5ms
    {  4166667, 1000000 },  // 240 Hz:  4.1666 +- 1ms
};

u64 tm_roundToCommonRefreshRate(u64 count) {
    i32 i = 0;
    for (i32 i = 0; countOf(tm__refreshRates) > i; i++) {
        u64 ns = tm__refreshRates[i][1];
        uint64_t tol = tm__refreshRates[i][1];
        if ((count > (ns - tol)) && (count < (ns + tol))) {
            return ns;
        }
    }
    return count;
}



///////////////////////////////////////////
// OS /////////////////////////////////////
///////////////////////////////////////////


#if OS_APPLE || OS_ANDROID || OS_UNIX || OS_LINUX
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>
#include <mach/mach.h>
#include <stdio.h>
#include <dispatch/dispatch.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <sys/sysctl.h>

#if OS_APPLE
#include <sys/types.h>
#include <pwd.h>
#endif


#ifdef OS_OSX
#define DLL_EXTENSION ".dylib"
#else 
#define DLL_EXTENSION ".so"
#endif

u32 os_coreCount(void) {
#if defined(_MSC_VER) || defined(_WIN32) || defined(_WIN64) // Windows
   SYSTEM_INFO sysinfo;
   GetSystemInfo(&sysinfo);
   return (u32)sysinfo.dwNumberOfProcessors;
#elif defined(__APPLE__) // macOS and iOS
   u32 count;
   size_t count_len = sizeof(count);
   sysctlbyname("hw.ncpu", &count, &count_len, NULL, 0);
   return count;
#elif defined(__ANDROID__) || defined(__linux__) // Android and Linux
   // sysconf is POSIX and available on both Linux and Android
   long count = sysconf(_SC_NPROCESSORS_ONLN);
   return (count > 0) ? (u32)count : 1; // Fallback to 1 if error
#elif defined(__EMSCRIPTEN__) // Emscripten
   // Use JavaScript's navigator.hardwareConcurrency via Emscripten
   return (u32)emscripten_run_script_int("navigator.hardwareConcurrency || 1");
#else
   // Fallback for unsupported platforms (shouldn't reach here due to #error)
   return 1;
#endif
}



os_Dl* os_dlOpen(S8 filePath) {
    u8 fileName[1024];
    u32 size = minVal(countOf(fileName) - 1, filePath.size);
    mem_copy(fileName, filePath.content, size);
    S8 extension = str_lit(DLL_EXTENSION);
    if (!str_hasSuffix(filePath, extension)) {
        ASSERT(sizeOf(fileName) > extension.size + size);
        mem_copy(&fileName[size], extension.content, extension.size);
        size += extension.size;

    }
    ASSERT(sizeOf(fileName) > size + 1);
    fileName[size] = '\0';

    void* handle = dlopen((const char *) fileName, RTLD_NOW);

    return (os_Dl*) handle;
}

void os_dlClose(os_Dl* handle) {
    dlclose((void*)handle);
}

void*  os_dlSym(os_Dl* handle, const char* symbol) {
    return (void*) dlsym((void*) handle, symbol);
}

// See: https://github.com/jemalloc/jemalloc/blob/12cd13cd418512d9e7596921ccdb62e25a103f87/src/pages.c
// See: https://web.archive.org/web/20150730125201/http://blog.nervus.org/managing-virtual-address-spaces-with-mmap/
void* os_memoryReserve(u64 size) {
    void* ptr = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0);
    int mres = msync(ptr, size, MS_SYNC | MS_INVALIDATE);
    ASSERT(mres != -1);
    return ptr;
}

void os_memoryCommit(void* ptr, u64 size) {
    void* res = mmap(ptr, size, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED | MAP_ANON, -1, 0);
    ASSERT(res != MAP_FAILED);
    ASSERT(res == ptr);
    int mres = msync(res, size, MS_SYNC | MS_INVALIDATE);
    ASSERT(mres != -1);
}

API void os_memorydecommit(void* ptr, u64 size) {
    // instead of unmapping the address, we're just gonna trick 
    // the TLB to mark this as a new mapped area which, due to 
    // demand paging, will not be committed until used.
    void* res = mmap(ptr, size, PROT_NONE, MAP_FIXED | MAP_PRIVATE | MAP_ANON, -1, 0);
    int mres = msync(ptr, size, MS_SYNC | MS_INVALIDATE);
    ASSERT(mres != -1);
}

void os_memoryRelease(void* ptr, u64 size) {
    int mres = msync(ptr, size, MS_SYNC);
    ASSERT(mres != -1);
    munmap(ptr, size);
}

LOCAL void* os__reserve(void* ctx, u64 size) {
    unusedVars(ctx);
    ASSERT(size > 0);
    return os_memoryReserve(size);
}

LOCAL void os__commit(void* ctx, void* ptr, u64 size) {
    unusedVars(ctx);
    ASSERT(size > 0);
    os_memoryCommit(ptr, size);
}

LOCAL void os__decommit(void* ctx, void* ptr, u64 size) {
    unusedVars(ctx);
    ASSERT(size > 0);
    os_memorydecommit(ptr, size);
}

LOCAL void os__release(void* ctx, void* ptr, u64 size) {
    unusedVars(ctx);
    ASSERT(size > 0);
    os_memoryRelease(ptr, size);
}

BaseMemory os_getBaseMemory(void) {
    BaseMemory mem;
    mem.ctx = NULL;
    mem.pageSize = os_memoryPageSize();
    mem.reserve = os__reserve;
    mem.commit = os__commit;
    mem.decommit = os__decommit;
    mem.release = os__release;
    return mem;
}


// Time

LOCAL DateTime os__posixDateTimeFromSystemTime(struct tm* in, u16 ms) {
	DateTime result = {0};
	result.year   = in->tm_year;
	result.month    = in->tm_mon;
	result.day    = in->tm_mday;
	result.hour   = in->tm_hour;
	result.minute    = in->tm_min;
	result.second    = in->tm_sec;
	result.milliSecond   = ms;
	return result;
}

LOCAL struct timespec os__posixLocalSystemTimeFromDateTime(DateTime* in) {
	struct tm result_tm = {0};
	result_tm.tm_year = in->year;
	result_tm.tm_mon  = in->month;
	result_tm.tm_mday = in->day;
	result_tm.tm_hour = in->hour;
	result_tm.tm_min  = in->minute;
	result_tm.tm_sec  = in->second;
	long ms = in->milliSecond;
	time_t result_tt = timelocal(&result_tm);
	struct timespec result = { .tv_sec = result_tt, .tv_nsec = ms * 1000000 };
	return result;
}

LOCAL struct timespec os__posixUniversalSystemTimeFromDateTime(DateTime* in) {
	struct tm result_tm = {0};
	result_tm.tm_year = in->year;
	result_tm.tm_mon  = in->month;
	result_tm.tm_mday = in->day;
	result_tm.tm_hour = in->hour;
	result_tm.tm_min  = in->minute;
	result_tm.tm_sec  = in->second;
	long ms = in->milliSecond;
	time_t result_tt = timegm(&result_tm);
	struct timespec result = { .tv_sec = result_tt, .tv_nsec = ms * 1000000 };
	return result;
}

DateTime os_timeUniversalNow(void) {
	struct timespec spec;
	clock_gettime(CLOCK_REALTIME, &spec);
	struct tm *the_tm = localtime(&spec.tv_sec);
	return os__posixDateTimeFromSystemTime(the_tm, (u16)(spec.tv_nsec / 1000000));
}

DateTime os_timeLocalFromUniversal(DateTime* date_time) {
	struct timespec local = os__posixLocalSystemTimeFromDateTime(date_time);
	struct tm *the_tm = localtime(&local.tv_sec);
	return os__posixDateTimeFromSystemTime(the_tm, (u16)(local.tv_nsec / 1000000));
}

DateTime os_timeUniversalFromLocal(DateTime* date_time) {
	struct timespec univ = os__posixUniversalSystemTimeFromDateTime(date_time);
	struct tm *the_tm = localtime(&univ.tv_sec);
	return os__posixDateTimeFromSystemTime(the_tm, (u16)(univ.tv_nsec / 1000000));
}

u64 os_timeMicrosecondsNow(void) {
	struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	u64 us = ((u64)ts.tv_sec * 1000000) + ((u64)ts.tv_nsec / 1000000);
    return us;
}


/// File/Dir

S8 os_fileRead(Arena* arena, S8 fileName) {
    os_FileProperties fileProps = os_fileProperties(fileName);
    if (fileProps.size == 0) {
        return str_lit("");
    }
    u8 path[255 + 4096 + 1];
    ASSERT(sizeof(path) > (fileName.size + 1));
    ASSERT(fileName.content);
    ASSERT(fileName.size > 0);
    mem_copy(path, fileName.content, fileName.size);
    path[fileName.size] = '\0';

    i32 fileHandle = open((const char*) path, O_RDONLY);
    if (fileHandle == -1) {
        return str_lit("");
    }
    void* mem = mem_arenaPush(arena, fileProps.size);
    bool result = read(fileHandle, mem, fileProps.size) == fileProps.size;
    close(fileHandle);
    return result ? str_fromCharPtr((u8*) mem, fileProps.size) : str_lit("");
}

bx os_fileWrite(S8 fileName, S8 data) {
    // max file length + max file path length + '\0'
    u8 path[255 + 4096 + 1];
    ASSERT(sizeof(path) > (fileName.size + 1));
    ASSERT(fileName.content);
    ASSERT(fileName.size > 0);
    mem_copy(path, fileName.content, fileName.size);
    path[fileName.size] = '\0';
    // overwrite existing file and don't leave any of its existing file content
    i32 fileHandle = open((const char*) path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR + S_IWUSR + S_IRGRP + S_IROTH);
    if (fileHandle == -1) {
        return false;
    }
    bool result = write(fileHandle, data.content, data.size) == data.size;
    close(fileHandle);
    return result;
}

bx os_fileDelete(S8 fileName) {
    u8 path[255 + 4096 + 1];
    ASSERT(sizeof(path) > (fileName.size + 1));
    ASSERT(fileName.content);
    ASSERT(fileName.size > 0);
    mem_copy(path, fileName.content, fileName.size);
    path[fileName.size] = '\0';

    return remove((const char*) path) == 0;
}

bx os_fileExists(S8 fileName) {
    u8 path[255 + 4096 + 1];
    ASSERT(sizeof(path) > (fileName.size + 1));
    ASSERT(fileName.content);
    ASSERT(fileName.size > 0);
    mem_copy(path, fileName.content, fileName.size);
    path[fileName.size] = '\0';
    return access((const char*) path, F_OK) == 0;
}




bx os_dirCreate(S8 dirname) {
    mem_defineMakeStackArena(tmpMem, 1024 * sizeof(u32) + 1);
    if (!str_isNullTerminated(dirname)) {
        dirname = str_copyNullTerminated(tmpMem, dirname);
    }
	b32 result = true;
	// NOTE(voxel): Not sure what mode is actually a good default...
	size_t o = mkdir((const char*) dirname.content, S_IRUSR | S_IRGRP | S_IROTH | S_IWOTH | S_IRGRP | S_IWGRP);
	if (o == -1) result = false;
	return o == -1 ? false : true;
}

bx os_dirDelete(S8 dirname) {
    mem_defineMakeStackArena(tmpMem, 1024 * sizeof(u32) + 1);
    if (!str_isNullTerminated(dirname)) {
        dirname = str_copyNullTerminated(tmpMem, dirname);
    }
	size_t o = rmdir((const char*) dirname.content);

	return o == -1 ? false : true;
}

S8 os_filepath(Arena* arena, os_systemPath path) {
	S8 result = {0};
	
	switch (path) {
		case os_systemPath_currentDir: {
            mms pushAmount = 1024 * sizeOf(u32);
            u8* mem = mem_arenaPush(arena, pushAmount);
			getcwd((char*)mem, pushAmount);
			result.size = strlen((const char*)mem) - 1;
            result.content = mem;
            mem_arenaPopAmount(arena, pushAmount - result.size);
		} break;
		case os_systemPath_binary: {
            mms pushAmount = 1024 * sizeOf(u32);
            u8* mem = mem_arenaPush(arena, pushAmount);
            #if OS_APPLE
            i32 size = 0;
            _NSGetExecutablePath(NULL, &size);
            i32 error = _NSGetExecutablePath((char*)mem, &size);
            if (error != 0) {
                return str_lit("");
            }
            #else
			i32 size = readlink("/proc/self/exe", (char*)mem, pushAmount);
            if (size == -1) {
                return str_lit("");
            }
            #endif
            result.size = size;
            result.content = mem;
			u64 end = str_lastIndexOfChar(result, '/');
			result.size = end - 1;
            mem_arenaPopAmount(arena, pushAmount - result.size);
		} break;
		case os_systemPath_userData: {
			char* buffer = getenv("HOME");

            if (!buffer) {
                struct passwd* pwd = getpwuid(getuid());
                if (pwd) {
                    buffer = pwd->pw_dir;
                }
            }
            if (buffer) {
                result.content = (u8*)buffer;
                result.size = strlen((const char*)buffer);
            }
		} break;
		case os_systemPath_tempData: {
			return str_lit("/tmp");
		} break;
	}
	
	return result;
}

os_FileProperties os_fileProperties(S8 fileName) {
    // max file length + max file path length + '\0'
    u8 path[255 + 4096 + 1];
    ASSERT(sizeof(path) > (fileName.size + 1));
    ASSERT(fileName.content);
    ASSERT(fileName.size > 0);
    mem_copy(path, fileName.content, fileName.size);
    path[fileName.size] = '\0';
    struct stat stats;

    i32 result = stat((const char*)&path[0], &stats);
    ASSERT(result == 0);
    os_FileProperties fileProps = {0};
    fileProps.size = stats.st_size;
    // https://c-for-dummies.com/blog/?p=4101
    if (S_ISDIR(stats.st_mode)) {
        fileProps.flags |= os_filePropertyFlag_isFolder;
    }
    // S_ISDIR, true when the file is a directory
    // S_ISLNK, true when the file is a symbolic link
    // S_ISREG, true when the file is a plain ol’ “regular” file

    if (stats.st_mode & S_IRUSR) {
        fileProps.access |= os_dataAccessFlag_read;
    }

    if (stats.st_mode & S_IWUSR) {
        fileProps.access |= os_dataAccessFlag_write;
    }

    if (stats.st_mode & S_IXUSR) {
        fileProps.access |= os_dataAccessFlag_execute;
    }
    
    struct tm  time;
    // gmtime_r
    struct tm *tt = gmtime_r(&stats.st_birthtimespec.tv_sec, &time);

    DateTime dateTime;
    dateTime.year  = tt->tm_year;
    dateTime.month   = tt->tm_mon;
    dateTime.day   = tt->tm_mday;
    dateTime.hour  = tt->tm_hour;
    dateTime.second   = tt->tm_sec;
    dateTime.milliSecond  = 0; //roundVal(stats.st_ctimespec , 1000000);
    fileProps.creationTime = tm_toDenseTime(dateTime);

    tt = gmtime_r(&stats.st_ctimespec.tv_sec, &time);
    //DateTime dateTime;
    dateTime.year  = tt->tm_year;
    dateTime.month   = tt->tm_mon;
    dateTime.day   = tt->tm_mday;
    dateTime.hour  = tt->tm_hour;
    dateTime.second   = tt->tm_sec;
    dateTime.milliSecond  = 0; //roundVal(stats.st_ctimespec , 1000000);
    fileProps.lastChangeTime = tm_toDenseTime(dateTime);

    return fileProps;
}


void* os_execute(Arena* tmpArena, S8 execPath, S8* args, u32 argCount) {
    pid_t pid = fork();
    if (pid != 0) {
        return (void*)((uintptr_t)pid);
    }
    mem_scoped(scratch, tmpArena) {
        S8 exec = str_join(scratch.arena, execPath, '\0');
        S8 argsUnix;
        str_record(argsUnix, scratch.arena) {
            for (u32 idx = 0; idx < argCount; idx++) {
                str_join(scratch.arena, args[idx], '\0');
            }
        }
        char** argsArr = (char**) mem_arenaPush(scratch.arena, sizeof(char*) * (argCount + 1));
        uint32_t offset = 0;
        for (u32 idx = 0; idx < argCount; idx++) {
            argsArr[idx] = (char*) argsUnix.content + offset;
            offset = args[idx].size + 1;
        }
        argsArr[argCount] = NULL;

        int result = execvp((char*) exec.content, argsArr);
    }
    return NULL;
}

void os_sleep(u32 ms) {
    struct timespec req = { (time_t)ms/1000, (long)( (ms%1000)*1000000) };
    struct timespec rem = { 0, 0 };
    nanosleep(&req, &rem);
}

void os_yield(void) {
#ifdef __MACH__
    sched_yield();
#else
    pthread_yield();
#endif
}

void os_mutexLock(os_Mutex* mutex) {
    pthread_mutex_lock((pthread_mutex_t*) mutex->internal);
}

bx os_mutexTryLock(os_Mutex* mutex) {
    return pthread_mutex_trylock((pthread_mutex_t*) mutex->internal) == 0;
}

void os_mutexUnlock(os_Mutex* mutex) {
    pthread_mutex_unlock((pthread_mutex_t*) mutex->internal);
}

void os_mutexInit(os_Mutex* mutex) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init((pthread_mutex_t*) mutex->internal, &attr);
}

void os_mutexDestroy(os_Mutex* mutex) {
    pthread_mutex_destroy((pthread_mutex_t*) mutex->internal);
}

u32 os__mutexLockRet(os_Mutex* mutex) {
    os_mutexLock(mutex);
    return 0;
}

void os_log(S8 msg) {
    if (msg.content == NULL || msg.size == 0) {
        return;
    }
    //os_mutexScoped(&os__ctx.logLock) {
#if OS_ANDROID
        __android_log_print(ANDROID_LOG_UNKNOWN, "os", "%.*s\n", msg.size, msg.content);
#else
        fwrite(msg.content, msg.size, 1, stdout);
        fwrite("\n", 1, 1, stdout);
#endif // OS_ANDROD 
    //}
}

typedef struct os__ThreadInternal {
    pthread_t handle;
    char* tmpThreadName;
} os__ThreadInternal;

static void* os_threadEntry(void* arg) {
    os_Thread* thread = (os_Thread*) arg;
    union {
        void* ptr;
        i32 i;
    } cast;
    if (((os__ThreadInternal*) &thread->internal)->tmpThreadName) {
#if OS_APPLE
        pthread_setname_np(((os__ThreadInternal*) &thread->internal)->tmpThreadName);
#else
        pthread_setname_np(((os__ThreadInternal*) &thread->internal)->handle, ((os__ThreadInternal*) &thread->internal)->tmpThreadName);
#endif
        ((os__ThreadInternal*) &thread->internal)->tmpThreadName = NULL;
    }
    os_semaphorePost(&thread->sem, 1);
    cast.i = thread->entryFunc(thread, thread->userData);
    return cast.ptr;
}

bx os_threadCreate(os_Thread* thread, os_threadFunc threadFunc, void* userData, u32 stackSize, S8 name) {
    os__ThreadInternal* ti = (os__ThreadInternal*) thread->internal;

    int result;

    pthread_attr_t attr;
    result = pthread_attr_init(&attr);
    if (0 != result) {
        return false;
    }
    // See:
    // https://man7.org/linux/man-pages/man3/pthread_attr_setstacksize.3.html
    // PTHREAD_STACK_MIN (16384)
    stackSize = maxVal(stackSize, 16384);

    if (stackSize != 0) {
        result = pthread_attr_setstacksize(&attr, stackSize);

        if (result != 0) {
            return false;
        }
    }
    
    
    char buff[1024];
    if (name.size > 0) {
        u64 length = minVal(name.size, countOf(buff) - 1);
        mem_copy(buff, name.content, length);
        buff[length] = '\0';
        ti->tmpThreadName = &buff[0];
    } else {
        ti->tmpThreadName = NULL;
    }
    
    
    thread->entryFunc = threadFunc;
    thread->userData = userData;

    os_semaphoreInit(&thread->sem);
    result = pthread_create(&ti->handle, &attr, os_threadEntry, thread);
    if (result != 0) {
        return false;
    }
    
    thread->running = true;
    os_semaphoreWait(&thread->sem, 1);

    return true;
}

void os_threadShutdown(os_Thread* thread) {
    os__ThreadInternal* internal = (os__ThreadInternal*) thread->internal;
    union {
        void* ptr;
        i32 i;
    } cast;
    pthread_join(internal->handle, &cast.ptr);
    thread->exitCode = cast.i;
    internal->handle = 0;
    thread->running = false;
    os_semaphoreDestroy(&thread->sem);
}

S8 os_workingPath(Arena* arena) {
    S8 str;
    u32 maxSize = PATH_MAX * sizeof(char);
    char* cwd = mem_arenaPushArray(arena, char, PATH_MAX);
    if (getcwd(cwd, maxSize) != 0) {
        str.content = (u8*) cwd;
        str.size = strlen(cwd);
    } else {
        str.content = NULL;
        str.size = 0;
    }

    mem_arenaPopAmount(arena, maxSize - str.size);

    return str;
}
#if 0
S8 os_execPath(Arena* arena) {
    i32 length, dirnameLength;
    char* path = NULL;

    length = wai_getExecutablePath(NULL, 0, &dirnameLength);
    if (length > 0) {
        path = mem_arenaPushArray(arena, char, length);
        wai_getExecutablePath(path, length, &dirnameLength);
    }
    S8 str;
    str.content = (u8*) path;
    str.size = dirnameLength;

    return str;
}
#endif

u32 os_memoryPageSize(void) {
    return getpagesize();
}

umm os_getProcessMemoryUsed(void) {
#ifdef MACH_TASK_BASIC_INFO
    struct mach_task_basic_info info;
    mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;

    i32 const result = task_info(mach_task_self() , MACH_TASK_BASIC_INFO , (task_info_t)&info , &infoCount);
#else
    task_basic_info info;
    mach_msg_type_number_t infoCount = TASK_BASIC_INFO_COUNT;

    i32 const result = task_info(mach_task_self() , TASK_BASIC_INFO , (task_info_t)&info , &infoCount);

#endif // MACH_TASK_BASIC_INFO
    if (result != KERN_SUCCESS) {
        return 0;
    }

    return info.resident_size;
}

#if OS_APPLE 
typedef struct os__SemaphoreInternal {
    dispatch_semaphore_t handle;
} os__SemaphoreInternal;

void os_semaphoreInit(os_Semaphore* sem) {
    STATIC_ASSERT(sizeof(os_Semaphore) >= sizeof(os__SemaphoreInternal));
    os__SemaphoreInternal* si = (os__SemaphoreInternal*) sem->internal;
    si->handle = dispatch_semaphore_create(0);
    ASSERT(si->handle != NULL && "dispatch_semaphore_create failed.");
}

void os_semaphorePost(os_Semaphore* sem, u32 count) {
    os__SemaphoreInternal* si = (os__SemaphoreInternal*) sem->internal;

    for (u32 ii = 0; ii < count; ++ii) {
        dispatch_semaphore_signal(si->handle);
    }
}

bool os_semaphoreWait(os_Semaphore* sem, i32 count) {
    os__SemaphoreInternal* si = (os__SemaphoreInternal*) sem->internal;

    dispatch_time_t dt = 0 > count ? DISPATCH_TIME_FOREVER : dispatch_time(DISPATCH_TIME_NOW, i64_cast(count)*1000000);
    return !dispatch_semaphore_wait(si->handle, dt);
}

void os_semaphoreDestroy(os_Semaphore* sem) {
    os__SemaphoreInternal* si = (os__SemaphoreInternal*) sem->internal;
    dispatch_release(si->handle);
}

#else

typedef struct os__SemaphoreInternal {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    i32 count;
} os__SemaphoreInternal;

void os_semaphoreInit(os_Semaphore* sem) {
    STATIC_ASSERT(sizeof(os_Semaphore) >= sizeof(os__SemaphoreInternal));

    os__SemaphoreInternal* si = (os__SemaphoreInternal*) sem->internal;
    si->count = 0;

    int result;

    result = pthread_mutex_init(&si->mutex, NULL);
    ASSERT(result == 0);

    result = pthread_cond_init(&si->cond, NULL);
    ASSERT(result == 0);
}

void os_semaphorePost(os_Semaphore* sem, u32 count) {
    os__SemaphoreInternal* si = (os__SemaphoreInternal*) sem->internal;

    int result = pthread_mutex_lock(&si->mutex);
    ASSERT(result == 0);

    for (u32 ii = 0; ii < count; ++ii) {
        result = pthread_cond_signal(&si->cond);
        ASSERT(0 == result);
    }

    si->count += count;

    result = pthread_mutex_unlock(&si->mutex);
    ASSERT(0 == result);

    UNUSED(result);
}

INLINE u64 os__timeSpecToNs(const struct timespec ts) {
    return ts.tv_sec * u64_val(1000000000) + ts.tv_nsec;
}

INLINE struct timespec os__toTimespecNs(struct timespec ts, uint64_t _nsecs) {
    ts.tv_sec  = _nsecs / u64_val(1000000000);
    ts.tv_nsec = _nsecs % u64_val(1000000000);
    return ts;
}

INLINE struct timespec os__timespecAdd(struct timespec ts, u64 msecs) {
    u64 ns = os__timeSpecToNs(ts);
    return os__toTimespecNs(ts, ns + msecs * u64_val(1000000));
}

bool os_semaphoreWait(os_Semaphore* sem, i32 count) {
    os__SemaphoreInternal* si = (os__SemaphoreInternal*) sem->internal;

    i32 result = pthread_mutex_lock(&si->mutex);
    ASSERT(result == 0);

    if (count == -1) {
        while (0 == result && 0 >= si->count) {
            result = pthread_cond_wait(&si->cond, &si->mutex);
        }
    } else {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts = os__timespecAdd(ts, (u64) count);

        while (0 == result && 0 >= si->count) {
            result = pthread_cond_timedwait(&si->cond, &si->mutex, &ts);
        }
    }

    bx ok = result == 0;

    if (ok) {
        --si->count;
    }

    result = pthread_mutex_unlock(&si->mutex);
    ASSERT(result == 0);

    UNUSED(result);

    return ok;
}

void os_semaphoreDestroy(os_Semaphore* sem) {
    os__SemaphoreInternal* si = (os__SemaphoreInternal*) sem->internal;

    int result;
    result = pthread_cond_destroy(&si->cond);
    ASSERT(result == 0);

    result = pthread_mutex_destroy(&si->mutex);
    ASSERT(result == 0);

    UNUSED(result);
}
#endif // OS_APPLE || OS_ANDROID || OS_UNIX || OS_LINUX

#elif OS_WIN
#define PATH_MAX (1024)

#pragma comment(lib, "userenv.lib")
#pragma comment(lib, "ws2_32.lib")

#include <stdio.h>
#include <Windows.h>
#include <psapi.h>
#include <Synchapi.h>
#include <processthreadsapi.h>
#ifndef alloca
#define alloca(S) _alloca(S)
#endif

u32 os_memoryPageSize(void) {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwPageSize;
}

umm os_getProcessMemoryUsed(void) {
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    return pmc.WorkingSetSize;
}

void* os_memoryReserve(u64 size) {
    void* result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
    return result;
}

void os_memoryCommit(void* ptr, u64 size) {
    VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
}

void os_memorydecommit(void* ptr, u64 size) {
    VirtualFree(ptr, size, MEM_DECOMMIT);
}

void os_memoryRelease(void* ptr, u64 size) {
    VirtualFree(ptr, 0, MEM_RELEASE);
}

DateTime os_win32DateTimeFromSystemTime(SYSTEMTIME *in){
    DateTime result;
    result.year = in->wYear;
    result.month  = (u8) in->wMonth;
    result.day  = in->wDay;
    result.hour = in->wHour;
    result.minute  = in->wMinute;
    result.second  = in->wSecond;
    result.milliSecond = in->wMilliseconds;
    return result;
}

DenseTime os_win32DenseTimeFromFileTime(FILETIME *fileTime){
    SYSTEMTIME systemTime = {0};
    FileTimeToSystemTime(fileTime, &systemTime);
    DateTime dateTime = os_win32DateTimeFromSystemTime(&systemTime);
    DenseTime result = tm_toDenseTime(dateTime);
    return result;
}

S8 os_fileRead(Arena* arena, S8 fileName) {
    S8 result;

    HANDLE file = INVALID_HANDLE_VALUE;
    mem_scoped(scratch, arena) {
        S16 fileName16 = str_toS16(scratch.arena, fileName);
        file = CreateFileW((WCHAR*) fileName16.content, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    }

    
    if (file != INVALID_HANDLE_VALUE) {
        DWORD hiSize = 0;
        DWORD loSize = GetFileSize(file, &hiSize);
        u64 totalSize = (u64_cast(hiSize) << 32) | u64_cast(loSize);
        mem_scoped(scratch, arena) {
            u8* buffer = mem_arenaPush(scratch.arena, totalSize);
            u8* ptr = buffer;
            bool success = true;
            
            for (u64 totalToRead = 0;totalToRead < totalSize;) {
                DWORD toRead = (DWORD) totalSize;
                if (totalToRead > u32_max) {
                    toRead = u32_max;
                }
                DWORD  actualRead = 0;
                if (!ReadFile(file, ptr, toRead, &actualRead, 0)) {
                    success = false;
                    break;
                }
                ptr += actualRead;
                totalToRead += actualRead;
            }
            CloseHandle(file);
            if (success) {
                result.content = buffer;
                result.size = totalSize;
                return result;
            }
        }
    }
    result.content = NULL;
    result.size = 0;

    return result;
}

bool os_fileWrite(S8 fileName, S8 data) {
    // max file length + max file path length + '\0'
    mem_defineMakeStackArena(arena, 1024 * sizeOf(u32));
    
    bx result = false;
    S16 fileMame16 = str_toS16(arena, fileName);
    HANDLE file = CreateFileW((WCHAR*)fileMame16.content, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (file != INVALID_HANDLE_VALUE) {
        result = true;
        DWORD actualWrite = 0;
        if (!WriteFile(file, data.content, data.size, &actualWrite, 0)) {
            result = false;
        } else {
            result = actualWrite == data.size;
        }
        
        CloseHandle(file);
    }

    return result;
}


bx os_dirCreate(S8 dirname) {
    mem_defineMakeStackArena(tmpMem, 1024 * sizeof(u32) + 1);
    S16 dirname16 = str_toS16(tmpMem, dirname);
	b32 result = CreateDirectoryW((WCHAR*) dirname16.content, 0);
	return result;
}

bx os_dirDelete(S8 dirname) {
    mem_defineMakeStackArena(tmpMem, 1024 * sizeof(u32) + 1);
    S16 dirname16 = str_toS16(tmpMem, dirname);
	b32 result = RemoveDirectoryW((WCHAR*) dirname16.content);
	return result;
}

static DateTime w32_date_time_from_system_time(SYSTEMTIME* in) {
	DateTime result = {0};
	result.year   = in->wYear;
	result.month  = (u8)in->wMonth;
	result.day    = in->wDay;
	result.hour   = in->wHour;
	result.minute = in->wMinute;
	result.second    = in->wSecond;
	result.milliSecond     = in->wMilliseconds;
	return result;
}

static SYSTEMTIME w32_system_time_from_date_time(DateTime* in) {
	SYSTEMTIME result    = {0};
	result.wYear         = in->year;
	result.wMonth        = in->month;
	result.wDay          = in->day;
	result.wHour         = in->hour;
	result.wMinute       = in->minute;
	result.wSecond       = in->second;
	result.wMilliseconds = in->milliSecond;
	return result;
}

#if 0
static u64 w32_dense_time_from_file_time(FILETIME *file_time) {
	SYSTEMTIME system_time;
	FileTimeToSystemTime(file_time, &system_time);
	U_DateTime date_time = w32_date_time_from_system_time(&system_time);
	U_DenseTime result = U_DenseTimeFromDateTime(&date_time);
	return result;
}

static OS_FilePropertyFlags w32_prop_flags_from_attribs(DWORD attribs) {
	OS_FilePropertyFlags result = 0;
	if (attribs & FILE_ATTRIBUTE_DIRECTORY){
		result |= FileProperty_IsFolder;
	}
	return result;
}

static OS_DataAccessFlags w32_access_from_attributes(DWORD attribs) {
	OS_DataAccessFlags result = DataAccess_Read | DataAccess_Exec;
	if (!(attribs & FILE_ATTRIBUTE_READONLY)){
		result |= DataAccess_Write;
	}
	return result;
}
#endif

os_FileProperties os_fileProperties(S8 fileName) {
    // max file length + max file path length + '\0'
    u8 path[255 + 4096 + 1];
    ASSERT(sizeof(path) > (fileName.size + 1));
    ASSERT(fileName.content);
    ASSERT(fileName.size > 0);
    mem_copy(path, fileName.content, fileName.size);
    path[fileName.size] = '\0';
    #if 0
	M_Scratch scratch = scratch_get();
	string_utf16 filename16 = S16_from_str8(&scratch.arena, filename);
	OS_FileProperties result = {0};
	WIN32_FILE_ATTRIBUTE_DATA attribs = {0};
	if (GetFileAttributesExW((WCHAR*)filename16.str, GetFileExInfoStandard,
							 &attribs)) {
		result.size = ((u64)attribs.nFileSizeHigh << 32) | (u64)attribs.nFileSizeLow;
		result.flags = w32_prop_flags_from_attribs(attribs.dwFileAttributes);
		result.create_time = w32_dense_time_from_file_time(&attribs.ftCreationTime);
		result.modify_time = w32_dense_time_from_file_time(&attribs.ftLastWriteTime);
		result.access = w32_access_from_attributes(attribs.dwFileAttributes);
	}
	return result;
    #endif

    #if 0
    M_ArenaTemp scratch = m_get_scratch(0, 0);
    String16 file_name16 = S16_from_str8(scratch.arena, file_name);
    
    // get attribs and convert to properties
    FileProperties result = {};
    WIN32_FILE_ATTRIBUTE_DATA attribs = {};
    if (GetFileAttributesExW((WCHAR*)file_name16.str, GetFileExInfoStandard,
                             &attribs)){
        result.size = ((U64)attribs.nFileSizeHigh << 32) | (U64)attribs.nFileSizeLow;
        result.flags = w32_prop_flags_from_attribs(attribs.dwFileAttributes);
        result.create_time = w32_dense_time_from_file_time(&attribs.ftCreationTime);
        result.modify_time = w32_dense_time_from_file_time(&attribs.ftLastWriteTime);
        result.access = w32_access_from_attributes(attribs.dwFileAttributes);
    }


    struct stat stats;

    i32 result = stat((const char*)&path[0], &stats);
    ASSERT(result == 0);
    os_FileProperties fileProps = {0};
    fileProps.size = stats.st_size;
    // https://c-for-dummies.com/blog/?p=4101
    if (S_ISDIR(stats.st_mode)) {
        fileProps.flags |= os_filePropertyFlag_isFolder;
    }
    // S_ISDIR, true when the file is a directory
    // S_ISLNK, true when the file is a symbolic link
    // S_ISREG, true when the file is a plain ol’ “regular” file

    if (stats.st_mode & S_IRUSR) {
        fileProps.access |= os_dataAccessFlag_read;
    }

    if (stats.st_mode & S_IWUSR) {
        fileProps.access |= os_dataAccessFlag_write;
    }

    if (stats.st_mode & S_IXUSR) {
        fileProps.access |= os_dataAccessFlag_execute;
    }
    
    struct tm  time;
    // gmtime_r
    struct tm *tt = gmtime_r(&stats.st_birthtimespec.tv_sec, &time);

    DateTime dateTime;
    dateTime.year  = tt->tm_year;
    dateTime.mon   = tt->tm_mon;
    dateTime.day   = tt->tm_mday;
    dateTime.hour  = tt->tm_hour;
    dateTime.sec   = tt->tm_sec;
    dateTime.msec  = 0; //roundVal(stats.st_ctimespec , 1000000);
    fileProps.creationTime = denseTime_fromDateTime(dateTime);

    tt = gmtime_r(&stats.st_ctimespec.tv_sec, &time);
    //DateTime dateTime;
    dateTime.year  = tt->tm_year;
    dateTime.mon   = tt->tm_mon;
    dateTime.day   = tt->tm_mday;
    dateTime.hour  = tt->tm_hour;
    dateTime.sec   = tt->tm_sec;
    dateTime.msec  = 0; //roundVal(stats.st_ctimespec , 1000000);
    fileProps.lastChangeTime = denseTime_fromDateTime(dateTime);

    return fileProps;
    #endif
}

S8 os_filepath(Arena* arena, os_systemPath path) {
	S8 result = {0};

    DWORD tmpName[2048 * 2];
    DWORD tmpCount = countOf(tmpName);
    S16 path16 = {0};
    path16.content = (u16*) tmpName;

    mem_defineMakeStackArena(tmpArena, 2048 * sizeOf(u32));

	switch (path) {
		case os_systemPath_currentDir: {
            mms pushAmount = 1024 * sizeOf(u32);
            u8* mem = mem_arenaPush(arena, pushAmount);
			DWORD size = GetCurrentDirectoryW(tmpCount, (WCHAR*) tmpName);
            ASSERT((size >= tmpCount) && "Increase tmpName size");
            path16.content = (u16*) mem;
            path16.size = size;
            result = str_replaceAll(arena, str_fromS16(tmpArena, path16), s8("\\"), s8("/"));
        } break;
        case os_systemPath_binary: {
            mms pushAmount = 2 * 1024 * sizeOf(u32);
            u8* mem = mem_arenaPush(arena, pushAmount);
            DWORD preError = GetLastError();
            DWORD size = GetModuleFileNameW(0, (WCHAR*)mem, pushAmount);
            DWORD error = GetLastError();
            ASSERT(size == tmpCount && error == ERROR_INSUFFICIENT_BUFFER && "Increase tmpName size");
            path16.content = (u16*) mem;
            path16.size = size;
            S8 fullPath = str_replaceAll(arena, str_fromS16(tmpArena, path16), s8("\\"), s8("/"));
			S8 binaryPath = os_getDirectoryFromFilepath(fullPath);
            result = binaryPath;
		} break;
		case os_systemPath_userData: {
            mms pushAmount = 1024 * sizeOf(u32);
            u8* mem = mem_arenaPush(arena, pushAmount);
			HANDLE token = GetCurrentProcessToken();
            bx success = GetUserProfileDirectoryW(token, (WCHAR*)tmpName, &tmpCount);
            ASSERT(success && "Increase tmpName size");
            path16.content = (u16*) mem;
            path16.size = tmpCount;
            result = str_replaceAll(arena, str_fromS16(tmpArena, path16), s8("\\"), s8("/"));
		} break;
		case os_systemPath_tempData: {
            mms pushAmount = 1024 * sizeOf(u32);
            u8* mem = mem_arenaPush(arena, pushAmount);
			DWORD size = GetTempPathW(pushAmount, (WCHAR*)mem);
            ASSERT(size >= tmpCount && "Increase tmpName size");
            path16.content = (u16*) mem;
            path16.size = size;
            result = str_replaceAll(arena, str_fromS16(tmpArena, path16), s8("\\"), s8("/"));
		} break;
	}

    result = str_replaceAll(arena, result, str_lit("\\"), str_lit("/"));
	
	return result;
}

#if 0
S8 os_filepath(Arena* arena, os_systemPath path) {
	S8 result = STR_NULL;
	switch (path) {
		case SystemPath_CurrentDir: {
			M_Scratch scratch = scratch_get();
			DWORD cap = 2048;
			u16* buffer = arena_alloc_array(&scratch.arena, u16, cap);
			DWORD size = GetCurrentDirectoryW(cap, (WCHAR*) buffer);
			if (size >= cap) {
				scratch_reset(&scratch);
				buffer = arena_alloc_array(&scratch.arena, u16, size + 1);
				size = GetCurrentDirectoryW(size + 1, (WCHAR*) buffer);
			}
			result = str8_from_S16(&scratch.arena, (string_utf16) { buffer, size });
			result = str_replace_all(arena, result, str_lit("\\"), str_lit("/"));
			
			scratch_return(&scratch);
		} break;
		
		case SystemPath_Binary: {
			M_Scratch scratch = scratch_get();
			
			DWORD cap = 2048;
			u16 *buffer = 0;
			DWORD size = 0;
			for (u64 r = 0; r < 4; r += 1, cap *= 4){
				u16* try_buffer = arena_alloc_array(&scratch.arena, u16, cap);
				DWORD try_size = GetModuleFileNameW(0, (WCHAR*)try_buffer, cap);
				
				if (try_size == cap && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
					scratch_reset(&scratch);
				} else {
					buffer = try_buffer;
					size = try_size;
					break;
				}
			}
			
			string full_path = str8_from_S16(&scratch.arena, (string_utf16) { buffer, size });
			string binary_path = U_GetDirectoryFromFilepath(full_path);
			result = str_replace_all(arena, binary_path, str_lit("\\"), str_lit("/"));
			
			scratch_return(&scratch);
		} break;
		
		case SystemPath_UserData: {
			M_Scratch scratch = scratch_get();
			
			HANDLE token = GetCurrentProcessToken();
			DWORD cap = 2048;
			u16 *buffer = arena_alloc_array(&scratch.arena, u16, cap);
			if (!GetUserProfileDirectoryW(token, (WCHAR*)buffer, &cap)) {
				scratch_reset(&scratch);
				buffer = arena_alloc_array(&scratch.arena, u16, cap + 1);
				if (GetUserProfileDirectoryW(token, (WCHAR*)buffer, &cap)) {
					buffer = 0;
				}
			}
			
			if (buffer) {
				result = str8_from_S16(&scratch.arena, S16_cstring(buffer));
				result = str_replace_all(arena, result, str_lit("\\"), str_lit("/"));
			}
			
			scratch_return(&scratch);
		} break;
		
		case SystemPath_TempData: {
			M_Scratch scratch = scratch_get();
			DWORD cap = 2048;
			u16 *buffer = arena_alloc_array(&scratch.arena, u16, cap);
			DWORD size = GetTempPathW(cap, (WCHAR*)buffer);
			if (size >= cap){
				scratch_reset(&scratch);
				buffer = arena_alloc_array(&scratch.arena, u16, size + 1);
				size = GetTempPathW(size + 1, (WCHAR*)buffer);
			}
			result = str8_from_S16(&scratch.arena, (string_utf16) { buffer, size - 1 });
			result = str_replace_all(arena, result, str_lit("\\"), str_lit("/"));
			
			scratch_return(&scratch);
		} break;
	}
	
	return result;
}
#endif


void* os_execute(Arena* tmpArena, S8 execPath, S8* args, u32 argCount) {
    STARTUPINFOA si;
    mem_structSetZero(&si);
    si.cb = sizeof(STARTUPINFOA);

    PROCESS_INFORMATION pi;
    mem_structSetZero(&pi);
    bx ok = false;
    mem_scoped(scratch, tmpArena) {
        S8 exec = str_join(scratch.arena, execPath, '\0');
        S8 argsWin;
        str_record(argsWin, scratch.arena) {
            for (u32 idx = 0; idx < argCount; idx++) {
                str_join(scratch.arena, args[idx], ' ');
            }
            str_join(scratch.arena, '\0');
        }

        ok = !!CreateProcessA(exec.content
            , argsWin.content
            , NULL
            , NULL
            , false
            , 0
            , NULL
            , NULL
            , &si
            , &pi
        );
    }

    return ok ? pi.hProcess : NULL;
}

void os_mutexLock(os_Mutex* mutex) {
    EnterCriticalSection((LPCRITICAL_SECTION) &mutex->internal[0]);
}

u32 os__mutexLockRet(os_Mutex* mutex) {
    os_mutexLock(mutex);
    return 0;
}

void os_mutexUnlock(os_Mutex* mutex) {
    LeaveCriticalSection((LPCRITICAL_SECTION) &mutex->internal[0]);
}

void os_mutexInit(os_Mutex* mutex) {
    InitializeCriticalSection((LPCRITICAL_SECTION) &mutex->internal[0]);
}

void os_mutexDestroy(os_Mutex* mutex) {
    DeleteCriticalSection((LPCRITICAL_SECTION) &mutex->internal[0]);
}

void os_log(S8 msg) {
    if (msg.content == NULL || msg.size == 0) {
        return;
    }
   // os_mutexScoped(&os__ctx.logLock) {
#if OS_ANDROID
        __android_log_print(ANDROID_LOG_UNKNOWN, "os", "%.*s", msg.size, msg.content);
#else
        fwrite(msg.content, msg.size, 1, stdout);
#if OS_WIN

#if 0
    // Fast/correct debug log for windows
    // LPCSTR lpOutputString
    size_t a_len = strlen(lpOutputString);

    int len = MultiByteToWideChar(CP_UTF8, 0, lpOutputString, (int) a_len, nullptr, 0);
    wchar_t* w_str = (wchar_t*) _malloca(len * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, lpOutputString, (int)a_len, w_str, len);

    ULONG_PTR args[4] = { (ULONG_PTR)len + 1, (ULONG_PTR)w_str, a_len + 1, (ULONG_PTR)lpOutputString };
    RaiseException(0x4001000A, 0, 4, args); // DBG_PRINTEXCEPTION_WIDE_C
#endif
    if (msg.content[msg.size - 1] == '\0') {
        OutputDebugStringA((char*) msg.content);
    } else {
        ASSERT(!"Not implemented");
        #if 0
        mem_scoped(scratch, os_tempMemory()) {
            u8* buff = (u8*) mem_arenaPush(scratch.arena, msg.size + 1);
            mem_copy(buff, msg.content, msg.size);
            buff[msg.size] = '\0';
            OutputDebugStringA((char*) buff);
        }
        #endif
    }
#endif // OS_WIN
#endif // OS_ANDROD 
    //}
}

typedef struct os__SemaphoreInternal {
    HANDLE handle;
} os__SemaphoreInternal;

void os_semaphoreInit(os_Semaphore* sem) {
    os__SemaphoreInternal* si = (os__SemaphoreInternal*) sem->internal;
    si->handle = NULL;
    si->handle = CreateSemaphoreA(NULL, 0, LONG_MAX, NULL);
    ASSERT(si->handle != NULL);
}

void os_semaphorePost(os_Semaphore* sem, u32 count) {
    os__SemaphoreInternal* si = (os__SemaphoreInternal*) sem->internal;
    ReleaseSemaphore(si->handle, count, NULL);
}

bool os_semaphoreWait(os_Semaphore* sem, i32 count) {
    os__SemaphoreInternal* si = (os__SemaphoreInternal*) sem->internal;
    DWORD milliseconds = (0 > count) ? INFINITE : count;
    return WaitForSingleObject(si->handle, milliseconds) == WAIT_OBJECT_0;
}

void os_semaphoreDestroy(os_Semaphore* sem) {
    os__SemaphoreInternal* si = (os__SemaphoreInternal*) sem->internal;
    CloseHandle(si->handle);
}

typedef struct os__ThreadInternal {
    HANDLE handle;
    DWORD  threadId;
} os__ThreadInternal;

static DWORD WINAPI os__threadEntry(LPVOID arg) {
    os_Thread* thread = (os_Thread*) arg;
    os__ThreadInternal* internal = (os__ThreadInternal*) thread->internal;
    internal->threadId = GetCurrentThreadId();
    os_semaphorePost(&thread->sem, 1);
    i32 result = thread->entryFunc(thread, thread->userData);
    return result;
}

bool os_threadCreate(os_Thread* thread, os_threadFunc threadFunc, void* userData, u32 stackSize, S8 name) {
    os__ThreadInternal* internal = (os__ThreadInternal*) thread->internal;

    os_semaphoreInit(&thread->sem);
    internal->handle = CreateThread(NULL, stackSize, (LPTHREAD_START_ROUTINE)threadFunc, thread, 0, NULL);
    if (internal->handle == NULL) {
        return false;
    }
    thread->running = true;
    os_semaphoreWait(&thread->sem, 1);

    if (name.size > 0) {
        os_threadSetName(thread, name);
    }

    return true;
}

void os_threadShutdown(os_Thread* thread) {
    os__ThreadInternal* internal = (os__ThreadInternal*) thread->internal;
    WaitForSingleObject(internal->handle, INFINITE);
    GetExitCodeThread(internal->handle, (DWORD*)&thread->exitCode);
    CloseHandle(internal->handle);
    internal->handle = INVALID_HANDLE_VALUE;
    thread->running = false;
}

bool os_threadIsRunning(os_Thread* thread) {
    return thread->running;
}

i32 os_threadGetExitCode(os_Thread* thread) {
    return thread->exitCode;
}

void os_threadSetName(os_Thread* thread, S8 name) {
    os__ThreadInternal* internal = (os__ThreadInternal*) thread->internal;
    // Try to use the new thread naming API from Win10 Creators update onwards if we have it
    typedef HRESULT (WINAPI SetThreadDescriptionProc)(HANDLE, PCWSTR);
    SetThreadDescriptionProc* setThreadDescription = (SetThreadDescriptionProc*) os_dlSym((os_Dl*)GetModuleHandleA("Kernel32.dll"), "SetThreadDescription");
    u32 totalSize = name.size + 1;
    char* nameBuffer = alloca(totalSize);
    mem_copy(nameBuffer, name.content, name.size);
    nameBuffer[name.size] = '\0';

    if (SetThreadDescription != NULL) {
        wchar_t* buff = alloca(totalSize);
        u32 size = totalSize * sizeof(wchar_t);
        mbstowcs(buff, name.content, size/* - 2*/);
        setThreadDescription(internal->handle, buff);
        return;
    }

#if COMPILER_MSVC
#pragma pack(push, 8)
    typedef struct ThreadName {
        DWORD  type;
        LPCSTR name;
        DWORD  id;
        DWORD  flags;
    } ThreadName;
#pragma pack(pop)
    ThreadName tn;
    tn.type  = 0x1000;
    tn.name  = nameBuffer;
    tn.id    = internal->threadId;
    tn.flags = 0;

    __try
    {
        RaiseException(0x406d1388, 0, sizeof(tn)/4, (const ULONG_PTR*)(&tn));
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
    }
#endif // COMPILER_MSVC
}

os_Dl* os_dlOpen(S8 filePath) {
    S8 dllExtension = s8(".dll");
    if (!str_hasSuffix(filePath, dllExtension)) {
        dllExtension = s8("");
    }

    mem_defineMakeStackArena(tmpArena, 2048 * sizeOf(u32));

    S8 dllPath = str_join(tmpArena, filePath, dllExtension, s8("\0"));

    HINSTANCE hInst = LoadLibrary((const char *) dllPath.content);

    return (os_Dl*) hInst;
}

DateTime os_timeUniversalNow(void) {
	SYSTEMTIME system_time;
	GetSystemTime(&system_time);
	DateTime result = w32_date_time_from_system_time(&system_time);
	return result;
}

DateTime os_timeLocalFromUniversal(DateTime* date_time) {
	SYSTEMTIME univ_system_time = w32_system_time_from_date_time(date_time);
	FILETIME univ_file_time;
	SystemTimeToFileTime(&univ_system_time, &univ_file_time);
	FILETIME local_file_time;
	FileTimeToLocalFileTime(&univ_file_time, &local_file_time);
	SYSTEMTIME local_system_time;
	FileTimeToSystemTime(&local_file_time, &local_system_time);
	DateTime result = w32_date_time_from_system_time(&local_system_time);
	return result;
}

DateTime os_timeUniversalFromLocal(DateTime* date_time) {
	SYSTEMTIME local_system_time = w32_system_time_from_date_time(date_time);
	FILETIME local_file_time;
	SystemTimeToFileTime(&local_system_time, &local_file_time);
	FILETIME univ_file_time;
	LocalFileTimeToFileTime(&local_file_time, &univ_file_time);
	SYSTEMTIME univ_system_time;
	FileTimeToSystemTime(&univ_file_time, &univ_system_time);
	DateTime result = w32_date_time_from_system_time(&univ_system_time);
	return result;
}

u64 os_timeMicrosecondsNow(void) {
	u64 result = 0;
	LARGE_INTEGER perfCounter = {0};
    LARGE_INTEGER ticksPerSecond = {0};

	if (QueryPerformanceCounter(&perfCounter) && QueryPerformanceFrequency(&ticksPerSecond)) {
		u64 ticks = ((u64)perfCounter.HighPart << 32) | perfCounter.LowPart;
		u64 uTicksPerSecond = ((u64)ticksPerSecond.HighPart << 32) | ticksPerSecond.LowPart;
		result = ticks * 1000000 / uTicksPerSecond;
	}
	return result;
}

// DLL

void os_dlClose(os_Dl* handle) {
    //BOOL ok;
    int rc= 0;

    //ok = FreeLibrary((HINSTANCE)handle);
    FreeLibrary((HINSTANCE)handle);
#if 0
    if (! ok) {
        var.lasterror = GetLastError ();
        var.err_rutin = "dlclose";
        rc= -1;
    }
#endif
}

void*  os_dlSym(os_Dl* handle, const char* symbol) {
    FARPROC fp;

    fp= GetProcAddress((HINSTANCE)handle, symbol);
#if 0
    if (!fp) {
        var.lasterror = GetLastError ();
        var.err_rutin = "dlsym";
    }
#endif
    return (void *)(intptr_t)fp;
}

void os_sleep(u32 ms) {
    Sleep(ms);
}

void os_yield(void) {
    YieldProcessor();
}

LOCAL void* os__reserve(void* ctx, u64 size) {
    unusedVars(ctx);
    ASSERT(size > 0);
    return os_memoryReserve(size);
}

LOCAL void os__commit(void* ctx, void* ptr, u64 size) {
    unusedVars(ctx);
    ASSERT(size > 0);
    os_memoryCommit(ptr, size);
}

LOCAL void os__decommit(void* ctx, void* ptr, u64 size) {
    unusedVars(ctx);
    ASSERT(size > 0);
    os_memorydecommit(ptr, size);
}

LOCAL void os__release(void* ctx, void* ptr, u64 size) {
    unusedVars(ctx);
    ASSERT(size > 0);
    os_memoryRelease(ptr, size);
}

BaseMemory os_getBaseMemory(void) {
    BaseMemory mem;
    mem.ctx = NULL;
    mem.pageSize = os_memoryPageSize();
    mem.reserve = os__reserve;
    mem.commit = os__commit;
    mem.decommit = os__decommit;
    mem.release = os__release;
    return mem;
}

#else
#error "Unknown OS"
#endif

///////////////////////////////////////////
// URL PARSER /////////////////////////////
///////////////////////////////////////////

LOCAL u32 url__defaultPortForScheme(S8 scheme) {
	if (scheme.size == 0) return 0;
	if (str_isEqual(scheme, str_lit("http"))   == 0) return 80;
	if (str_isEqual(scheme, str_lit("https"))  == 0) return 443;
	if (str_isEqual(scheme, str_lit("ftp"))    == 0) return 21;
	if (str_isEqual(scheme, str_lit("ssh"))    == 0) return 22;
	if (str_isEqual(scheme, str_lit("telnet")) == 0) return 23;
	return 0;
}

LOCAL S8 url__parseScheme(S8 url, Url* out) {
   i64 start = str_findChar(url, ':');
   if (start < 0) return url;
   S8 schemesep = str_from(url, start + 1);
   

   // ... is this the user part of a user/pass pair or the separator host:port? ...
   if (!str_hasPrefix(schemesep, s8("//"))) {
      out->error = url_errorCode_scheme;
      return s8("");
   }
   out->scheme = str_subStr(url, 0, start);
   if (out->scheme.size == 0) {
      out->error = url_errorCode_scheme;
      return s8("");
   }
   return str_from(schemesep, 2);
}

LOCAL S8 url__parseUserPass(S8 url, Url* out) {
    i64 start = str_findChar(url, '@');
	i64 atpos = str_findChar(str_subStr(url, start, 0), '@');
	if (start >= 0) {
		// ... check for a : before the @ ...
        i64 passsep = str_findChar(str_subStr(url, 0, atpos), ':');
		if (passsep < 0) {
            out->pass = str_lit("");
            out->user = str_subStr(url, 0, atpos);
		} else {
            out->user = str_subStr(url, 0, passsep);
            out->pass = str_subStr(url, passsep + 1, atpos - passsep - 1);
		}

		if(out->user.size == 0 || out->pass.size == 0) {
            out->error = url_errorCode_userOrPass;
            return str_lit("");
        }

		return str_subStr(url, atpos + 1, 0);
	} else {
       out->pass = str_lit("");
       out->user = str_lit(""); 
    }

	return url;
}

LOCAL S8 url__parseHostPort(S8 url, Url* out) {
	out->port = url__defaultPortForScheme(out->scheme);

	i64 portsep = str_findChar(url, ':');
	i64 pathsep = str_findChar(url, '/');

	u64 hostlen = 0;

	if (portsep < 0) {
		pathsep = str_findChar(url, '/');
		hostlen = pathsep == 0x0 ? url.size : pathsep - url.size;
	} else {
		if(pathsep && portsep && (pathsep < portsep)) {
			// ... path separator was before port-separator, i.e. the : was not a port-separator! ...
			hostlen = pathsep;
		} else {
         S8 strPort = str_subStr(url, portsep + 1, pathsep - portsep - 1);
			out->port = str_parseU32(strPort);
			hostlen = portsep;
			//pathsep = str_findChar(str_subStr(url, portsep, 0), '/');
		}
	}

	if ( hostlen > 0 ) {
		out->host = str_subStr(url, 0, hostlen);
		if (out->host.size == 0) return str_lit("");
	}

	// ... parse path ... TODO: extract to own function.
	if (pathsep >= 0) {
		// ... check if there are any query or fragment to parse ...
		i64 pathEnd = str_find(str_subStr(url, pathsep, 0), str_lit("?#"));

		u64 reslen = 0;
		if (pathEnd >= 0) {
            reslen = pathEnd;
        } else {
            reslen = url.size;
        }

        out->path = str_subStr(url, pathsep, reslen);
		if (out->path.size == 0) {
            out->error = url_errorCode_hostOrPort;
            return str_lit("");
        }
		return str_from(url, reslen);
	}

	return s8("");
}

LOCAL S8 url__parseQuery(S8 url, Url* out) {
	// ... do we have a query? ...
	if (!str_startsWithChar(url, '?')) return url;
		

	// ... skip '?' ...
    url = str_from(url, 1);

	// ... find the end of the query ...
	u64 queryLen = 0;

	i64 fragmentStart = str_startsWithChar(url, '#');
	if (fragmentStart < 0) {
        queryLen = 0;
    }

    out->query = str_subStr(url, 0, queryLen);

    if (out->fragment.size == 0) {
        out->error = url_errorCode_query;
        return str_lit(""); 
    }

    return str_subStr(url, queryLen, 0);
}

LOCAL void url__parseFragment(S8 url, Url* out) {
	// ... do we have a fragment? ...
	if (!str_startsWithChar(url, '#')) return;

	// ... skip '#' ...
    url = str_subStr(url, 1, 0);

    out->fragment = url;

    if (out->fragment.size == 0) {
        out->error = url_errorCode_fragment;
    }
}

Url url_fromStr(S8 url) {
	Url out = {0};
   out.error = url_errorCode_ok;
	out.path = str_lit("/");

	url = url__parseScheme(url, &out);
    if (out.error != url_errorCode_ok) return out;
	url = url__parseUserPass(url, &out);
    if (out.error != url_errorCode_ok) return out;
	url = url__parseHostPort(url, &out);
    if (out.error != url_errorCode_ok) return out;
	url = url__parseQuery(url, &out);
    if (out.error != url_errorCode_ok) return out;
	url__parseFragment(url, &out);

    if (out.error == url_errorCode_ok) {
        if (out.host.size == 0) {
	        out.host = str_lit("localhost");
        }
        if (out.path.size == 0) {
	        out.path = str_lit("/");
        }
    }

	return out;
}

#endif // _BASE_C_