/* MIT License
 *
 *  Copyright (c) 2022 edgecraft.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>

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

#include "json.hpp"
using njson = nlohmann::json;

static bool opt_force_float32 = false;

bool json2dat(const fs::path &filename, bool force_float32 = false)
{
    constexpr auto ext_dat = "dat";
    fs::path fn_dat = filename;
    fn_dat.replace_extension(ext_dat);

    njson::parser_callback_t cb = [](int depth, njson::parse_event_t event, njson & parsed) -> bool {
        if (event == njson::parse_event_t::value and parsed.is_number_float()) {
            auto val = parsed.get<float>();
            parsed = val;
        }

        return true;
    };

    // read JSON file.
    std::ifstream ifs(filename.c_str());
    if (!ifs.is_open()) {
        std::cout << "ERROR!! can't open JSON file(" << filename << ")." << std::endl;
        return false;
    }
    njson json_list = {};
    if (force_float32) {
        json_list = njson::parse(ifs, cb);
    } else {
        ifs >> json_list;
    }

    // write dat(CBOR) file.
    std::ofstream ofs(fn_dat, std::ios::binary);
    if (!ofs.is_open()) {
        std::cout << "ERROR!! can't open DAT file(" << fn_dat << ")." << std::endl;
        return false;
    }
    auto cbor_list = njson::to_cbor(json_list);
    ofs.write(reinterpret_cast<char *>(cbor_list.data()), cbor_list.size());

    return true;
}

bool dat2json(const fs::path &filename)
{
    constexpr auto ext_json = "json";
    fs::path fn_json = filename;
    fn_json.replace_extension(ext_json);

    // read dat(CBOR) file.
    std::ifstream ifs(filename.c_str(), std::ios::binary);
    if (!ifs.is_open()) {
        std::cout << "ERROR!! can't open DAT file(" << filename << ")." << std::endl;
        return false;
    }
    auto sz = fs::file_size(filename);
    std::vector<uint8_t> cbor_list(sz);
    ifs.read(reinterpret_cast<char *>(cbor_list.data()), sz);

    // write JSON file.
    std::ofstream ofs(fn_json);
    if (!ofs.is_open()) {
        std::cout << "ERROR!! can't open JSON file(" << fn_json << ")." << std::endl;
        return false;
    }
    njson json_list = njson::from_cbor(cbor_list);
    ofs << std::setw(4) << json_list << std::endl;

    return true;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cout << "usage: json_util [option] <hogehoge.json | fugafuga.dat>" << std::endl;
        std::cout << "    Convert fileformat json <---> dat." << std::endl;
        std::cout << "    option:" << std::endl;
        std::cout << "    ---" << std::endl;
        std::cout << "    -f: [json -> dat] using float32 to convert from JSON to binary." << std::endl;
        exit(EXIT_FAILURE);
    }

    fs::path filename;
    if (argc == 3) {
        if (std::string{argv[1]} == "-f") opt_force_float32 = true;
        filename = fs::path{argv[2]};
    } else {
        filename = fs::path{argv[1]};
    }

    std::error_code ec;
    if (!fs::is_regular_file(filename, ec)) {
        std::cout << "ERROR!! file not found(" << filename << ")." << std::endl;
        exit(EXIT_FAILURE);
    }

    auto ext = filename.extension();
    auto ext_str = ext.generic_string();
    std::transform(ext_str.cbegin(), ext_str.cend(), ext_str.begin(), ::tolower);

    if (ext_str == ".json") {
        if (!json2dat(filename, opt_force_float32)) {
            std::cout << "ERROR!! can't convert JSON -> DAT." << std::endl;
            exit(EXIT_FAILURE);
        }
    } else if (ext_str == ".dat") {
        if (!dat2json(filename)) {
            std::cout << "ERROR!! can't convert DAT -> JSON." << std::endl;
            exit(EXIT_FAILURE);
        }
    } else {
        std::cout << "ERROR!! not support file type." << std::endl;
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
