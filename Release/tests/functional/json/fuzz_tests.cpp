/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
* fuzz_tests.cpp
*
* Fuzz tests for the JSON library.
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/

#include "stdafx.h"
#include "cpprest/filestream.h"
#include "cpprest/containerstream.h"

using namespace web;

namespace tests { namespace functional { namespace json_tests {

#ifdef _WIN32

SUITE(json_fuzz_tests)
{
std::string get_fuzzed_file_path()
{
    std::string ipfile;

    if(UnitTest::GlobalSettings::Has("fuzzedinputfile"))
    {
        ipfile = UnitTest::GlobalSettings::Get("fuzzedinputfile");
    }

    return ipfile;
}

TEST(fuzz_json_parser, "Requires", "fuzzedinputfile")
{
    typedef std::basic_string<char32_t> utf32string;

    utility::string_t ipfile = utility::conversions::to_string_t(get_fuzzed_file_path());
    if (true == ipfile.empty())
    {
        VERIFY_IS_TRUE(false, "Input file is empty");
        return;
    }

    auto fs = Concurrency::streams::file_stream<uint8_t>::open_istream(ipfile).get();
    concurrency::streams::container_buffer<utf8string> cbuf;
    fs.read_to_end(cbuf).get();
    fs.close().get();

    utf8string uf8_json_str = cbuf.collection();
    utf16string utf16_json_str(reinterpret_cast<utf16char*>(&uf8_json_str[0]),
                               reinterpret_cast<utf16char*>(&uf8_json_str[0] + uf8_json_str.size()));
    utf32string utf32_json_str(reinterpret_cast<char32_t*>(&uf8_json_str[0]),
                               reinterpret_cast<char32_t*>(&uf8_json_str[0] + uf8_json_str.size()));

    utility::string_t json_str;

    if ((uint8_t)uf8_json_str[0] == 0xEF ||
        (uint8_t)uf8_json_str[1] == 0xBB ||
        (uint8_t)uf8_json_str[2] == 0xBF)
    {
        // UTF-8, remove the BOM
        uf8_json_str.erase(0, 3);
        json_str = utility::conversions::to_string_t(uf8_json_str);
    }
    else if (utf16_json_str.front() == 0xFEFF && utf32_json_str.front() != 0x0000FEFF)
    {
        // UTF-16, remove the BOM
        utf16_json_str.erase(0, 1);
        json_str = utility::conversions::to_string_t(utf16_json_str);
    }
    else
    {
        // either UTF-32, or we can't detect
        VERIFY_IS_TRUE(false, "Input file encoding is not UTF-8 or UTF-16. Test will not parse the file.");
        return;
    }

    try
    {
        json::value::parse(std::move(json_str));
        std::cout << "Input file parsed successfully.";
    }
    catch(const json::json_exception& ex)
    {
        std::cout << "json exception:" << ex.what();
    }
}
}

#endif
}}}