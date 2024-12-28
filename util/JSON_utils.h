/*
 * Copyright (c) 2024, edgecraft. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#pragma once


#include <iostream>
#include <fstream>

#if defined(USE_EXPERIMENTAL_FS)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#if defined(__APPLE__)
#include <unistd.h>
#endif
#endif

#define JSON_USE_IMPLICIT_CONVERSIONS 0
#include "json.hpp"

#ifdef USE_ORDERED_JSON
using njson = nlohmann::ordered_json;
#else
using njson = nlohmann::json;
#endif

// #define NLOHMANN_DEFINE_TYPE_INTRUSIVE(Type, ...)  \
//     friend void to_json(nlohmann::json& nlohmann_json_j, const Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__)) } \
//     friend void from_json(const nlohmann::json& nlohmann_json_j, Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, __VA_ARGS__)) }
#define NLOHMANN_DEFINE_TYPE_INTRUSIVE_ORDERED(Type, ...)  \
    friend void to_json(nlohmann::ordered_json& nlohmann_json_j, const Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__)) } \
    friend void from_json(const nlohmann::ordered_json& nlohmann_json_j, Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, __VA_ARGS__)) }
#define NLOHMANN_DEFINE_TYPE_INTRUSIVE_HYBRID(Type, ...)  \
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Type, __VA_ARGS__)  \
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_ORDERED(Type, __VA_ARGS__)

// #define NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Type, ...)  \
//     inline void to_json(nlohmann::json& nlohmann_json_j, const Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__)) } \
//     inline void from_json(const nlohmann::json& nlohmann_json_j, Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, __VA_ARGS__)) }
#define NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_ORDERED(Type, ...)  \
    inline void to_json(nlohmann::ordered_json& nlohmann_json_j, const Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__)) } \
    inline void from_json(const nlohmann::ordered_json& nlohmann_json_j, Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, __VA_ARGS__)) }
#define NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_HYBRID(Type, ...)  \
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Type, __VA_ARGS__)  \
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_ORDERED(Type, __VA_ARGS__)

namespace {

// // JSON. Specializing enum conversion.
// NLOHMANN_JSON_SERIALIZE_ENUM( em_COLLO_lens_spec, {
// 	{ em_COLLO_lens_spec::NORMAL, "NORMAL" },
// 	{ em_COLLO_lens_spec::FISHEYE_EQUIDISTANT, "FISHEYE_EQUIDISTANT" },
// 	{ em_COLLO_lens_spec::FISHEYE_EQUISOLID_ANGLE, "FISHEYE_EQUISOLID_ANGLE" },
// 	{ em_COLLO_lens_spec::FISHEYE_ORTHOGRAPHIC, "FISHEYE_ORTHOGRAPHIC" },
// 	{ em_COLLO_lens_spec::FISHEYE_STEREOGRAPHIC, "FISHEYE_STEREOGRAPHIC" },
// })
// NLOHMANN_JSON_SERIALIZE_ENUM( OutputViewType, {
// 	{ IMG_BLEND, "IMG_BLEND" },
// 	{ IMG_MASK, "IMG_MASK" },
// 	{ IMG_INPUT, "IMG_INPUT" },
// 	{ IMG_INPUT_PANORAMA, "IMG_INPUT_PANORAMA" },
// 	{ IMG_BGR, "IMG_BGR" },
// 	{ IMG_FISHEYE, "IMG_FISHEYE" },
// 	{ IMG_FLASH, "IMG_FLASH" },
// 	{ IMG_INPUT_ADJ_COLOR, "IMG_INPUT_ADJ_COLOR" },
// })


// get value from json.
inline auto json_get_val = [](const njson &j, const std::string &key, auto &val) -> void {
    auto it = j.find(key);
    if (it != j.end()) {
        njson j_tmp = it.value();
        val = j_tmp.template get<std::remove_reference_t<decltype(val)>>();
    }
};

// get arrayed value from json.
inline auto json_get_array_val = [](const njson &j, const std::string &key, auto &ary) -> void {
    njson j_sub;
    json_get_val(j, key, j_sub);
    auto i = 0;
    for (auto &e : j_sub) {
        if (i >= std::size(ary)) break;
        ary[i] = j_sub.at(i).template get<std::remove_reference_t<decltype(ary[0])>>();
        i++;
    }
};

// get vector value from json.
inline auto json_get_vector_val = [](const njson &j, const std::string &key, auto &vec) -> void {
    njson j_sub;
    json_get_val(j, key, j_sub);
    vec.clear();
    for (auto &e : j_sub) vec.push_back(e.template get<std::remove_reference_t<decltype(vec[0])>>());
};

// convert enum -> std::string.
inline auto json_conv_enum2str = [](auto val) -> std::string {
    njson json = val;
    std::string str = json.template get<std::string>();
    return str;
};

// 拡張子の取り出し
#ifdef USE_STD_FILESYSTEM
inline auto get_extname = [](const std::string &str, bool lower = false) -> std::string {
    auto p = fs::path{str};
    auto ext = p.extension();
    auto ext_str = ext.generic_string();
    if (lower) std::transform(ext_str.cbegin(), ext_str.cend(), ext_str.begin(), ::tolower);

    return ext_str;
};
#else
inline auto get_extname = [](const std::string &str, bool lower = false) -> auto {
    auto ext_i = str.find_last_of(".");
    if (ext_i == std::string::npos) return std::string{""};

    std::string extname = str.substr(ext_i, str.size() - ext_i);
    return extname;
};
#endif

njson read_json_file(const std::string &filename, bool force_float32 = false)
{
    njson json = {};

    auto ext_str = get_extname(filename);

    if (ext_str == ".json") {
        std::ifstream ifs(filename);

        njson::parser_callback_t cb = [](int depth, njson::parse_event_t event, njson & parsed) -> bool {
            if (event == njson::parse_event_t::value and parsed.is_number_float()) {
                auto val = parsed.get<float>();
                parsed = val;
            }

            return true;
        };

        if (!ifs.is_open()) {
            std::cout << "ERROR! can't open JSON file to read : (" << filename << ")" << std::endl;
            return {};
        }
        if (force_float32) {
            json = njson::parse(ifs, cb);
        } else {
            ifs >> json;
        }

    } else if (ext_str == ".dat" || ext_str == ".cbor") {
        std::ifstream ifs(filename, std::ios::binary);
        if (!ifs.is_open()) {
            std::cout << "ERROR!! can't open DAT file to read : (" << filename << ")" << std::endl;
            return {};
        }
        auto p = fs::path{filename};
        auto sz = fs::file_size(p);
        std::vector<uint8_t> cbor(sz);
        ifs.read(reinterpret_cast<char *>(cbor.data()), sz);
        json = njson::from_cbor(cbor);

    } else {
        std::cout << "ERROR! not support file type to read: " << ext_str << "." << std::endl;
        return {};
    }

    return json;
}

template<typename T> T read_json_file(const std::string &filename, bool force_float32 = false)
{
    njson json = read_json_file(filename, force_float32);

    T data = json.template get<T>();

    return data;
}

void write_json_file(const std::string &filename, const njson &json)
{
    auto ext_str = get_extname(filename);

    if (ext_str == ".json") {
        std::ofstream ofs(filename);
        if (!ofs.is_open()) {
            std::cout << "ERROR! can't open JSON file to write : (" << filename << ")" << std::endl;
            return;
        }
        ofs << std::setw(4) << json << std::endl;

    } else if (ext_str == ".dat" || ext_str == ".cbor") {
        std::ofstream ofs(filename, std::ios::binary);
        if (!ofs.is_open()) {
            std::cout << "ERROR!! can't open DAT file to write : (" << filename << ")" << std::endl;
            return;
        }
        auto cbor = njson::to_cbor(json);
        ofs.write(reinterpret_cast<char *>(cbor.data()), cbor.size());

    } else {
        std::cout << "ERROR! not support file type to write : " << ext_str << "." << std::endl;
        return;
    }
}

template<typename T> void write_json_file(const std::string &filename, const T &data)
{
    njson json = {};
    json = data;

    write_json_file(filename, json);
}

}
