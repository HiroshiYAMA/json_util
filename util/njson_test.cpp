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

// g++ -std=c++17 -D USE_STD_FILESYSTEM njson_test.cpp
// ./a.out

#include <iostream>
#include <optional>

#include "JSON_utils.h"

struct st_AAA
{
    int i = 0;
    std::string s = {};

    bool operator==(const st_AAA &rhs) const
    {
        return (i == rhs.i)
            && (s == rhs.s);
    }

    bool operator!=(const st_AAA &rhs) const
    {
        return !(*this == rhs);
    }

public:
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_HYBRID(
        st_AAA,
        i,
        s
    )
};

struct st_BBB
{
    std::optional<int> i = std::nullopt;
    std::optional<std::string> s = std::nullopt;

    bool operator==(const st_BBB &rhs) const
    {
        return (i == rhs.i)
            && (s == rhs.s);
    }

    bool operator!=(const st_BBB &rhs) const
    {
        return !(*this == rhs);
    }
};
void to_json(njson& json, const st_BBB& val)
{
    if (val.i) json["i"] = *(val.i);
    if (val.s) json["s"] = *(val.s);
}
void from_json(const njson& json, st_BBB& val)
{
    json_get_val(json, "i", *(val.i));
    json_get_val(json, "s", *(val.s));
}

int main(int ac, char *av[])
{
    st_AAA aaa = {};
    aaa.i = 100;
    aaa.s = "AAA";

    njson j = aaa;
    std::cout << std::setw(4) << j << std::endl;
    auto aaa2 = j.template get<st_AAA>();

    write_json_file("json_j.json", j);
    write_json_file("json_aaa.json", aaa2);
    auto jj = read_json_file("json_j.json");
    auto aaa22 = read_json_file<st_AAA>("json_aaa.json");
    assert(j == jj);
    assert(aaa2 == aaa22);

    st_BBB bbb = {};
    bbb.i = 200;
    // bbb.s = "BBB";

    njson jb = bbb;
    // jb.erase("s");
    std::cout << std::setw(4) << jb << std::endl;
    auto bbb2 = jb.template get<st_BBB>();

    write_json_file("json_jb.json", jb);
    write_json_file("json_bbb.json", bbb2);
    auto jbjb = read_json_file("json_jb.json");
    auto bbb22 = read_json_file<st_BBB>("json_bbb.json");
    assert(jb == jbjb);
    assert(bbb2 == bbb22);

    return EXIT_SUCCESS;
}
