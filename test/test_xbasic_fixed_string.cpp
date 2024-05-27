/***************************************************************************
* Copyright (c) 2016, Sylvain Corlay and Johan Mabille                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "doctest/doctest.h"
#include "xeus/xbasic_fixed_string.hpp"

namespace xeus
{
    using string_type = xbasic_fixed_string<char, 16, buffer | store_size, string_policy::throwing_error>;
    using numpy_string = xbasic_fixed_string<char, 16, buffer, string_policy::throwing_error>;
    using size_type = string_type::size_type;

    TEST_SUITE("xfixed_string") {
    TEST_CASE("constructors")
    {
        // Default constructor
        {
            string_type s;
            REQUIRE_EQ(s.size(), size_type(0));
            REQUIRE_EQ(s.c_str(), "");
        }

        // ch repeated n times
        {
            string_type s(size_type(4), 'a');
            REQUIRE_EQ(s.size(), size_type(4));
            REQUIRE_EQ(s.c_str(), "aaaa");
        }

        // From C string
        {
            const char* cstr = "constructor";
            string_type s(cstr);
            REQUIRE_EQ(s.size(), size_type(11));
            REQUIRE_EQ(s.c_str(), cstr);
        }

        // From const char* with \0 inside
        {
            const char* cstr = "construc\0tor";
            string_type s(cstr, 12);
            REQUIRE_EQ(s.size(), size_type(12));
            REQUIRE_EQ(s.c_str(), "construc");
        }

        // From substring
        {
            string_type s1("constructor");
            string_type s2(s1, size_type(1), size_type(4));
            REQUIRE_EQ(s2.size(), size_type(4));
            REQUIRE_EQ(s2.c_str(), "onst");
            string_type s3(s1, size_type(2), size_type(45));
            REQUIRE_EQ(s3.size(), size_type(9));
            REQUIRE_EQ(s3.c_str(), "nstructor");
        }

        // From initializer list
        {
            string_type s = { 'c', 'o', 'n', 's', 't' };
            REQUIRE_EQ(s.size(), size_type(5));
            REQUIRE_EQ(s.c_str(), "const");
        }

        // From iterators pair
        {
            std::string s1 = "constructor";
            string_type s2(s1.cbegin(), s1.cend());
            REQUIRE_EQ(s2.size(), size_type(11));
            REQUIRE_EQ(s2.c_str(), s1.c_str());
        }

        // From string
        {
            std::string s1 = "constructor";
            string_type s2(s1);
            REQUIRE_EQ(s2.size(), size_type(11));
            REQUIRE_EQ(s2.c_str(), s1.c_str());

            string_type s3(s1, size_type(1), size_type(4));
            REQUIRE_EQ(s3.size(), size_type(4));
            REQUIRE_EQ(s3.c_str(), "onst");

            std::string s4 = s1;
            REQUIRE_EQ(s4.size(), size_type(11));
            REQUIRE_EQ(s4.c_str(), s4.c_str());
        }
    }

    TEST_CASE("copy_semantic")
    {
        string_type ref("constructor");
        string_type ref2("operator=");
        string_type s(ref);
        REQUIRE_EQ(s.size(), ref.size());
        REQUIRE_EQ(s.c_str(), ref.c_str());
        s = ref2;
        REQUIRE_EQ(s.size(), ref2.size());
        REQUIRE_EQ(s.c_str(), ref2.c_str());
    }

    TEST_CASE("move_semantic")
    {
        string_type ref("constructor");
        string_type ref_tmp(ref);
        string_type ref2("operator=");
        string_type ref2_tmp(ref2);
        string_type s(std::move(ref_tmp));
        REQUIRE_EQ(s.size(), ref.size());
        REQUIRE_EQ(s.c_str(), ref.c_str());
        s = std::move(ref2_tmp);
        REQUIRE_EQ(s.size(), ref2.size());
        REQUIRE_EQ(s.c_str(), ref2.c_str());
    }

    TEST_CASE("assign_operator")
    {
        string_type ref("reference");

        // From const char*
        {
            const char* cstr = "constructor";
            string_type s(ref);
            s = cstr;
            REQUIRE_EQ(s.size(), size_type(11));
            REQUIRE_EQ(s.c_str(), cstr);
        }

        // From char
        {
            string_type s(ref);
            s = 'b';
            REQUIRE_EQ(s.size(), size_type(1));
            REQUIRE_EQ(s.c_str(), "b");
        }

        // From initializer_list
        {
            string_type s(ref);
            s = { 'c', 'o', 'n', 's', 't' };
            REQUIRE_EQ(s.size(), size_type(5));
            REQUIRE_EQ(s.c_str(), "const");
        }

        // From std::string
        {
            string_type s(ref);
            std::string a("const");
            s = a;
            REQUIRE_EQ(s.size(), size_type(5));
            REQUIRE_EQ(s.c_str(), "const");
        }
    }

    TEST_CASE("assign")
    {
        string_type ref("reference");

        // ch repeated n times
        {
            string_type s(ref);
            s.assign(size_type(4), 'a');
            REQUIRE_EQ(s.size(), size_type(4));
            REQUIRE_EQ(s.c_str(), "aaaa");
            REQUIRE_THROWS_AS(s.assign(size_type(17), 'a'), std::length_error);
        }

        // From substring
        {
            string_type to_assign("constructor");
            string_type s(ref);
            s.assign(to_assign, 1, size_type(4));
            REQUIRE_EQ(s.size(), size_type(4));
            REQUIRE_EQ(s.c_str(), "onst");
        }

        // From const char* with \0 inside
        {
            const char* cstr = "construc\0tor";
            string_type s(ref);
            s.assign(cstr, size_type(12));
            REQUIRE_EQ(s.size(), size_type(12));
            REQUIRE_EQ(s.c_str(), "construc");
            const char* ctothrow = "abcdefghij\0klmnopq";
            REQUIRE_THROWS_AS(s.assign(ctothrow, size_type(18)), std::length_error);
        }

        // From const char*
        {
            const char* cstr = "constructor";
            string_type s(ref);
            s.assign(cstr);
            REQUIRE_EQ(s.size(), size_type(11));
            REQUIRE_EQ(s.c_str(), cstr);
            const char* ctothrow = "abcdefghijklmnopq";
            REQUIRE_THROWS_AS(s.assign(ctothrow, size_type(17)), std::length_error);
        }

        // From initializer_list
        {
            string_type s(ref);
            s.assign({ 'c', 'o', 'n', 's', 't' });
            REQUIRE_EQ(s.size(), size_type(5));
            REQUIRE_EQ(s.c_str(), "const");
#ifdef __GNUC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wstringop-overflow="
#endif
            REQUIRE_THROWS_AS(s.assign({ 'a','b','c','a','b','c','a','b','c','a','b','c','a','b','c','a','b','c' }), std::length_error);
#ifdef __GNUC__
#   pragma GCC diagnostic pop
#endif
        }

        // From iterators pair
        {
            string_type s(ref);
            std::string s1 = "constructor";
            s.assign(s1.cbegin(), s1.cend());
            REQUIRE_EQ(s.size(), size_type(11));
            REQUIRE_EQ(s.c_str(), s1.c_str());
            std::string ctothrow = "abcdefghijklmnopq";
            REQUIRE_THROWS_AS(s.assign(ctothrow.cbegin(), ctothrow.cend()), std::length_error);
        }

        // From lvalue reference
        {
            string_type s(ref);
            string_type s1 = "constructor";
            s.assign(s1);
            REQUIRE_EQ(s.size(), size_type(11));
            REQUIRE_EQ(s.c_str(), s1.c_str());
        }

        // From rvalue reference
        {
            string_type s(ref);
            string_type s1 = "constructor";
            string_type s1_tmp(s1);
            s.assign(std::move(s1_tmp));
            REQUIRE_EQ(s.size(), size_type(11));
            REQUIRE_EQ(s.c_str(), s1.c_str());
        }

        // From string
        {
            string_type s(ref);
            std::string s1 = "constructor";
            s.assign(s1);
            REQUIRE_EQ(s.size(), size_type(11));
            REQUIRE_EQ(s.c_str(), s1.c_str());
            s.assign(s1, 1, size_type(4));
            REQUIRE_EQ(s.size(), size_type(4));
            REQUIRE_EQ(s.c_str(), "onst");
        }
    }

    TEST_CASE("element_access")
    {
        string_type s = "element_access";
        s[2] = 'E';
        REQUIRE_EQ(s[2], 'E');
        REQUIRE_EQ(s.c_str(), "elEment_access");

        s.at(3) = 'M';
        REQUIRE_EQ(s.at(3), 'M');
        REQUIRE_EQ(s.c_str(), "elEMent_access");

        s.front() = 'E';
        REQUIRE_EQ(s.front(), 'E');
        REQUIRE_EQ(s.c_str(), "ElEMent_access");

        s.back() = 'S';
        REQUIRE_EQ(s.back(), 'S');
        REQUIRE_EQ(s.c_str(), "ElEMent_accesS");

        REQUIRE_THROWS_AS(s.at(15), std::out_of_range);

        REQUIRE_EQ(s.data(), s.c_str());
    }

    TEST_CASE("iterator")
    {
        string_type s = "iterator";
        *(s.begin()) = 'I';
        REQUIRE_EQ(*(s.begin()), 'I');
        REQUIRE_EQ(*(s.cbegin()), 'I');

        auto iter = s.begin();
        auto citer = s.cbegin();
        for (size_type count = 0; count < s.size(); ++iter, ++citer, ++count) {}
        REQUIRE_EQ(iter, s.end());
        REQUIRE_EQ(citer, s.cend());

        *(s.rbegin()) = 'R';
        REQUIRE_EQ(*(s.rbegin()), 'R');
        REQUIRE_EQ(*(s.crbegin()), 'R');

        auto riter = s.rbegin();
        auto criter = s.crbegin();
        for (size_type count = 0; count < s.size(); ++riter, ++criter, ++count) {}
        REQUIRE_EQ(riter, s.rend());
        REQUIRE_EQ(criter, s.crend());
    }

    TEST_CASE("capacity")
    {
        string_type s = "capacity";
        REQUIRE_EQ(s.size(), s.length());
        REQUIRE_FALSE(s.empty());
        REQUIRE_EQ(s.max_size(), size_type(16));
    }

    TEST_CASE("clear")
    {
        string_type s = "capacity";
        s.clear();
        REQUIRE_EQ(s.size(), size_type(0));
        REQUIRE_EQ(s.c_str(), "");
    }

    TEST_CASE("push_back")
    {
        string_type s = "operation";
        s.push_back('s');
        REQUIRE_EQ(s.size(), size_type(10));
        REQUIRE_EQ(s.c_str(), "operations");
    }

    TEST_CASE("pop_back")
    {
        string_type s = "operation";
        s.push_back('s');
        REQUIRE_EQ(s.size(), size_type(10));
        REQUIRE_EQ(s.c_str(), "operations");
    }

    TEST_CASE("substr")
    {
        string_type ref = "operation";
        string_type s1 = ref.substr(2, 5);
        REQUIRE_EQ(s1.c_str(), "erati");
        string_type s2 = ref.substr(2, 45);
        REQUIRE_EQ(s2.c_str(), "eration");
        REQUIRE_THROWS_AS(ref.substr(15, 4), std::out_of_range);
    }

    TEST_CASE("copy")
    {
        string_type ref = "operation";
        string_type dst1 = "aaaa";
        string_type dst2 = "bbbbbb";

        size_type cp1 = ref.copy(dst1.data(), 4, 2);
        REQUIRE_EQ(cp1, size_type(4));
        REQUIRE_EQ(dst1.c_str(), "erat");

        size_type cp2 = ref.copy(dst2.data(), 45, 3);
        REQUIRE_EQ(cp2, size_type(6));
        REQUIRE_EQ(dst2.c_str(), "ration");

        REQUIRE_THROWS_AS(ref.copy(dst2.data(), 4, 15), std::out_of_range);
    }

    TEST_CASE("resize")
    {
        string_type s1 = "operation";
        string_type s2 = s1;
        REQUIRE_EQ(s1.size(), size_type(9));
        s1.resize(11);
        REQUIRE_EQ(s1.size(), size_type(11));
        s2.resize(11, 'a');
        REQUIRE_EQ(s2.size(), size_type(11));
        REQUIRE_EQ(s2.c_str(), "operationaa");
    }

    TEST_CASE("swap")
    {
        string_type s1 = "operation";
        string_type s2 = "swap";

        s1.swap(s2);
        REQUIRE_EQ(s1.c_str(), "swap");
        REQUIRE_EQ(s2.c_str(), "operation");

        using std::swap;
        swap(s1, s2);
        REQUIRE_EQ(s1.c_str(), "operation");
        REQUIRE_EQ(s2.c_str(), "swap");
    }

    TEST_CASE("insert")
    {
        string_type ref = "operation";
        {
            string_type s = ref;
            s.insert(3, 2, 'a');
            REQUIRE_EQ(s.c_str(), "opeaaration");
            REQUIRE_THROWS_AS(s.insert(45, 2, 'b'), std::out_of_range);
        }

        {
            string_type s = ref;
            s.insert(3, "bb");
            REQUIRE_EQ(s.c_str(), "opebbration");
            REQUIRE_THROWS_AS(s.insert(45, "bb"), std::out_of_range);
        }

        {
            string_type s = ref;
            s.insert(3, "b\0b", 3);
            REQUIRE_EQ(s.size(), size_type(12));
            REQUIRE_EQ(s.c_str(), "opeb");
            REQUIRE_THROWS_AS(s.insert(45, "b\0b", 3), std::out_of_range);
        }

        {
            string_type s = ref;
            string_type ins = "aa";
            s.insert(3, ins);
            REQUIRE_EQ(s.c_str(), "opeaaration");
            REQUIRE_THROWS_AS(s.insert(45, ins), std::out_of_range);
        }

        {
            string_type s = ref;
            string_type ins = "abcdefgh";
            s.insert(3, ins, 2, 2);
            REQUIRE_EQ(s.c_str(), "opecdration");
            s.insert(3, ins, 5, 15);
            REQUIRE_EQ(s.c_str(), "opefghcdration");
            REQUIRE_THROWS_AS(s.insert(45, ins, 2, 2), std::out_of_range);
        }

        {
            string_type s = ref;
            std::string ins = "aa";
            s.insert(3, ins);
            REQUIRE_EQ(s.c_str(), "opeaaration");
            REQUIRE_THROWS_AS(s.insert(45, ins), std::out_of_range);
        }

        {
            string_type s = ref;
            std::string ins = "abcdefgh";
            s.insert(3, ins, 2, 2);
            REQUIRE_EQ(s.c_str(), "opecdration");
            s.insert(3, ins, 5, 15);
            REQUIRE_EQ(s.c_str(), "opefghcdration");
            REQUIRE_THROWS_AS(s.insert(45, ins, 2, 2), std::out_of_range);
        }

        {
            string_type s = ref;
            s.insert(s.begin() + 3, 'b');
            REQUIRE_EQ(s.c_str(), "opebration");
        }

        {
            string_type s = ref;
            s.insert(s.begin() + 3, 2, 'b');
            REQUIRE_EQ(s.c_str(), "opebbration");
        }

        {
            string_type s = ref;
            s.insert(s.begin() + 3, { 'a', 'b', 'c' });
            REQUIRE_EQ(s.c_str(), "opeabcration");
        }

        {
            string_type s = ref;
            string_type ins = "insert";
            s.insert(s.begin() + 3, ins.cbegin() + 2, ins.cbegin() + 4);;
            REQUIRE_EQ(s.c_str(), "opeseration");
        }
    }

    TEST_CASE("erase")
    {
        string_type ref = "operation";

        {
            string_type s = ref;
            s.erase(2, 2);
            REQUIRE_EQ(s.c_str(), "opation");
            s.erase();
            REQUIRE_EQ(s.c_str(), "");
        }

        {
            string_type s = ref;
            s.erase(s.cbegin() + 2);
            REQUIRE_EQ(s.c_str(), "opration");
        }

        {
            string_type s = ref;
            s.erase(s.cbegin() + 2, s.cbegin() + 4);
            REQUIRE_EQ(s.c_str(), "opation");
        }
    }

    TEST_CASE("append")
    {
        string_type ref = "operation";

        {
            string_type s = ref;
            s.append(2, 'a');
            REQUIRE_EQ(s.c_str(), "operationaa");
            REQUIRE_THROWS_AS(s.append(15, 'a'), std::length_error);
        }

        {
            string_type s = ref;
            string_type ap = "abc";
            s.append(ap);
            REQUIRE_EQ(s.c_str(), "operationabc");
            REQUIRE_THROWS_AS(s.append(string_type("operation")), std::length_error);
        }

        {
            string_type s = ref;
            string_type ap = "abc";
            s.append(ap, 1, 2);
            REQUIRE_EQ(s.c_str(), "operationbc");
            s.append(ap, 1, 15);
            REQUIRE_EQ(s.c_str(), "operationbcbc");
        }

        {
            string_type s = ref;
            std::string ap = "abc";
            s.append(ap);
            REQUIRE_EQ(s.c_str(), "operationabc");
            REQUIRE_THROWS_AS(s.append(string_type("operation")), std::length_error);
        }

        {
            string_type s = ref;
            std::string ap = "abc";
            s.append(ap, 1, 2);
            REQUIRE_EQ(s.c_str(), "operationbc");
            s.append(ap, 1, 15);
            REQUIRE_EQ(s.c_str(), "operationbcbc");
        }

        {
            string_type s = ref;
            const char* ap = "ab\0cde";
            s.append(ap, 6);
            REQUIRE_EQ(s.size(), size_type(15));
            REQUIRE_EQ(s.c_str(), "operationab");
        }

        {
            string_type s = ref;
            const char* ap = "ab\0cde";
            s.append(ap);
            REQUIRE_EQ(s.size(), size_type(11));
            REQUIRE_EQ(s.c_str(), "operationab");
        }

        {
            string_type s = ref;
            s.append({ 'a', 'b', 'c' });
            REQUIRE_EQ(s.c_str(), "operationabc");
        }

        {
            string_type s = ref;
            std::string ap = "append";
            s.append(ap.cbegin() + 1, ap.cbegin() + 5);
            REQUIRE_EQ(s.c_str(), "operationppen");
        }
    }

    TEST_CASE("operator_append")
    {
        string_type ref = "operation";

        {
            string_type s = ref;
            string_type ap = "abc";
            s += (ap);
            REQUIRE_EQ(s.c_str(), "operationabc");
        }

        {
            string_type s = ref;
            std::string ap = "abc";
            s += (ap);
            REQUIRE_EQ(s.c_str(), "operationabc");
        }

        {
            string_type s = ref;
            s += 'a';
            REQUIRE_EQ(s.c_str(), "operationa");
        }

        {
            string_type s = ref;
            const char* ap = "ab";
            s += (ap);
            REQUIRE_EQ(s.size(), size_type(11));
            REQUIRE_EQ(s.c_str(), "operationab");
        }

        {
            string_type s = ref;
            s += { 'a', 'b', 'c' };
            REQUIRE_EQ(s.c_str(), "operationabc");
        }
    }

    TEST_CASE("compare")
    {
        string_type s1 = "aabcdef";
        string_type s2 = "abcdefg";
        string_type s3 = "aabcd";

        {
            int r1 = s1.compare(s2);
            REQUIRE_LT(r1, 0);
            int r2 = s1.compare(s1);
            REQUIRE_EQ(r2, 0);
            int r3 = s2.compare(s1);
            REQUIRE_GT(r3, 0);
            int r4 = s1.compare(s3);
            REQUIRE_GT(r4, 0);
            int r5 = s3.compare(s1);
            REQUIRE_LT(r5, 0);
        }

        {
            int r1 = s1.compare(s2.c_str());
            REQUIRE_LT(r1, 0);
            int r2 = s1.compare(s1.c_str());
            REQUIRE_EQ(r2, 0);
            int r3 = s2.compare(s1.c_str());
            REQUIRE_GT(r3, 0);
            int r4 = s1.compare(s3.c_str());
            REQUIRE_GT(r4, 0);
            int r5 = s3.compare(s1.c_str());
            REQUIRE_LT(r5, 0);
        }

        {
            int r1 = s2.compare(0, 5, s3);
            REQUIRE_GT(r1, 0);
            int r2 = s1.compare(0, 5, s3);
            REQUIRE_EQ(r2, 0);
        }

        {
            int r1 = s2.compare(0, 5, s3.c_str());
            REQUIRE_GT(r1, 0);
            int r2 = s1.compare(0, 5, s3.c_str());
            REQUIRE_EQ(r2, 0);
        }

        {
            int r1 = s1.compare(2, 3, s2, 2, 3);
            REQUIRE_LT(r1, 0);
            int r2 = s1.compare(2, 3, s1, 2, 3);
            REQUIRE_EQ(r2, 0);
            int r3 = s2.compare(2, 3, s1, 2, 3);
            REQUIRE_GT(r3, 0);
            int r4 = s1.compare(2, 3, s3, 1, 2);
            REQUIRE_GT(r4, 0);
            int r5 = s3.compare(1, 2, s1, 2, 3);
            REQUIRE_LT(r5, 0);
        }

        {
            int r1 = s1.compare(2, 3, s2.c_str() + 2, 3);
            REQUIRE_LT(r1, 0);
            int r2 = s1.compare(2, 3, s1.c_str() + 2, 3);
            REQUIRE_EQ(r2, 0);
            int r3 = s2.compare(2, 3, s1.c_str() + 2, 3);
            REQUIRE_GT(r3, 0);
            int r4 = s1.compare(2, 3, s3.c_str() + 1, 2);
            REQUIRE_GT(r4, 0);
            int r5 = s3.compare(1, 2, s1.c_str() + 2, 3);
            REQUIRE_LT(r5, 0);
        }

        {
            std::string str1(s1.cbegin(), s1.cend());
            std::string str2(s2.cbegin(), s2.cend());
            std::string str3(s3.cbegin(), s3.cend());

            int r1 = s1.compare(str2);
            REQUIRE_LT(r1, 0);
            int r2 = s1.compare(str1);
            REQUIRE_EQ(r2, 0);
            int r3 = s2.compare(str1);
            REQUIRE_GT(r3, 0);
            int r4 = s1.compare(str3);
            REQUIRE_GT(r4, 0);
            int r5 = s3.compare(str1);
            REQUIRE_LT(r5, 0);
        }
    }

    TEST_CASE("replace")
    {
        string_type ref = "replace";
        string_type s = ref;

        string_type rep1 = "abc";
        s.replace(1, 4, rep1);
        REQUIRE_EQ(s.c_str(), "rabcce");

        string_type rep2 = "epla";
        s.replace(s.cbegin() + 1, s.cbegin() + 4, rep2);
        REQUIRE_EQ(s.c_str(), ref.c_str());

        string_type rep3 = "operation";
        s.replace(2, 2, rep3, 3, 4);
        REQUIRE_EQ(s.c_str(), "reratiace");

        const char* rep4 = "plabc";
        s.replace(2, 4, rep4, 2);
        REQUIRE_EQ(s.c_str(), ref.c_str());

        s.replace(s.cbegin() + 2, s.cbegin() + 4, rep3.c_str() + 3, 4);
        REQUIRE_EQ(s.c_str(), "reratiace");

        const char* rep5 = "pl";
        s.replace(2, 4, rep5);
        REQUIRE_EQ(s.c_str(), ref.c_str());

        s.replace(s.cbegin() + 2, s.cbegin() + 4, rep4);
        REQUIRE_EQ(s.c_str(), "replabcace");

        s.replace(2, 5, 3, 'c');
        REQUIRE_EQ(s.c_str(), "recccace");

        s.replace(s.cbegin() + 2, s.cbegin() + 5, 2, 'b');
        REQUIRE_EQ(s.c_str(), "rebbace");

        s.replace(s.cbegin() + 2, s.cbegin() + 4, { 'p', 'l' });
        REQUIRE_EQ(s.c_str(), ref.c_str());

        s.replace(s.cbegin() + 2, s.cbegin() + 4, rep4 + 2, rep4 + 4);
        REQUIRE_EQ(s.c_str(), "reabace");

        std::string rep6 = "pl";
        s.replace(2, 2, rep6);
        REQUIRE_EQ(s.c_str(), ref.c_str());

        std::string rep7 = "zyx";
        s.replace(s.cbegin() + 2, s.cbegin() + 4, rep7);
        REQUIRE_EQ(s.c_str(), "rezyxace");

        std::string rep8 = "epla";
        s.replace(2, 3, rep8, 1, 2);
        REQUIRE_EQ(s.c_str(), ref.c_str());
    }

    TEST_CASE("find")
    {
        string_type ref = "operationftionf";
        string_type sub = "tionf";

        size_type r1 = ref.find(sub, 2);
        REQUIRE_EQ(r1, size_type(5));
        size_type r2 = ref.find(sub, 11);
        REQUIRE_EQ(r2, string_type::npos);

        size_type r3 = ref.find(sub.c_str(), 2, 3);
        REQUIRE_EQ(r3, size_type(5));
        size_type r4 = ref.find(sub.c_str(), 11, 3);
        REQUIRE_EQ(r4, string_type::npos);

        size_type r5 = ref.find(sub.c_str(), 2);
        REQUIRE_EQ(r5, size_type(5));
        size_type r6 = ref.find(sub.c_str(), 11);
        REQUIRE_EQ(r6, string_type::npos);

        size_type r7 = ref.find('a', 2);
        REQUIRE_EQ(r7, size_type(4));
        size_type r8 = ref.find('a', 8);
        REQUIRE_EQ(r8, string_type::npos);

        std::string ssub(sub.cbegin(), sub.cend());
        size_type r9 = ref.find(ssub, 2);
        REQUIRE_EQ(r9, size_type(5));
        size_type r10 = ref.find(ssub, 11);
        REQUIRE_EQ(r10, string_type::npos);
    }

    TEST_CASE("rfind")
    {
        string_type ref = "operateration";
        string_type sub = "erat";

        size_type r1 = ref.rfind(sub, 12);
        REQUIRE_EQ(r1, size_type(6));
        size_type r2 = ref.rfind(sub, 1);
        REQUIRE_EQ(r2, string_type::npos);

        size_type r3 = ref.rfind(sub.c_str(), 12, 3);
        REQUIRE_EQ(r3, size_type(6));
        size_type r4 = ref.rfind(sub.c_str(), 1, 3);
        REQUIRE_EQ(r4, string_type::npos);

        size_type r5 = ref.rfind(sub.c_str(), 12);
        REQUIRE_EQ(r5, size_type(6));
        size_type r6 = ref.rfind(sub.c_str(), 1);
        REQUIRE_EQ(r6, string_type::npos);

        size_type r7 = ref.rfind('a');
        REQUIRE_EQ(r7, size_type(8));
        size_type r8 = ref.rfind('a', 2);
        REQUIRE_EQ(r8, string_type::npos);

        std::string ssub(sub.cbegin(), sub.cend());
        size_type r9 = ref.rfind(ssub, 12);
        REQUIRE_EQ(r9, size_type(6));
        size_type r10 = ref.rfind(ssub, 1);
        REQUIRE_EQ(r10, string_type::npos);
    }

    TEST_CASE("find_first_of")
    {
        string_type ref = "Hello World!";
        string_type sub = "Good Bye!";
        string_type sub2 = "eo";

        size_type r1 = ref.find_first_of(sub, 1);
        REQUIRE_EQ(r1, size_type(1));
        size_type r2 = ref.find_first_of(sub, 3);
        REQUIRE_EQ(r2, size_type(4));
        size_type r3 = ref.find_first_of(sub2, 8);
        REQUIRE_EQ(r3, string_type::npos);

        size_type r4 = ref.find_first_of(sub.c_str(), 1);
        REQUIRE_EQ(r4, size_type(1));
        size_type r5 = ref.find_first_of(sub.c_str(), 3);
        REQUIRE_EQ(r5, size_type(4));
        size_type r6 = ref.find_first_of(sub2.c_str(), 8);
        REQUIRE_EQ(r6, string_type::npos);

        size_type r7 = ref.find_first_of(sub.c_str(), 1, 4);
        REQUIRE_EQ(r7, size_type(4));
        size_type r8 = ref.find_first_of(sub.c_str(), 5, 4);
        REQUIRE_EQ(r8, size_type(7));
        size_type r9 = ref.find_first_of(sub2.c_str(), 8, 2);
        REQUIRE_EQ(r9, string_type::npos);

        size_type r10 = ref.find_first_of('o', 2);
        REQUIRE_EQ(r10, size_type(4));
        size_type r11 = ref.find_first_of('o', 5);
        REQUIRE_EQ(r11, size_type(7));
        size_type r12 = ref.find_first_of('o', 8);
        REQUIRE_EQ(r12, string_type::npos);

        std::string ssub = "Good Bye!";
        std::string ssub2 = "eo";
        size_type r13 = ref.find_first_of(ssub, 1);
        REQUIRE_EQ(r13, size_type(1));
        size_type r14 = ref.find_first_of(ssub, 3);
        REQUIRE_EQ(r14, size_type(4));
        size_type r15 = ref.find_first_of(ssub2, 8);
        REQUIRE_EQ(r15, string_type::npos);
    }

    TEST_CASE("find_first_not_of")
    {
        string_type ref = "Hello World!";
        string_type sub = "eo";

        size_type r1 = ref.find_first_not_of(sub, 1);
        REQUIRE_EQ(r1, size_type(2));
        size_type r2 = ref.find_first_not_of(sub, 4);
        REQUIRE_EQ(r2, size_type(5));
        size_type r3 = ref.find_first_not_of(ref, 4);
        REQUIRE_EQ(r3, string_type::npos);

        size_type r4 = ref.find_first_not_of(sub.c_str(), 1);
        REQUIRE_EQ(r4, size_type(2));
        size_type r5 = ref.find_first_not_of(sub.c_str(), 4);
        REQUIRE_EQ(r5, size_type(5));
        size_type r6 = ref.find_first_not_of(ref.c_str(), 4);
        REQUIRE_EQ(r6, string_type::npos);

        size_type r7 = ref.find_first_not_of(sub.c_str(), 1, 2);
        REQUIRE_EQ(r7, size_type(2));
        size_type r8 = ref.find_first_not_of(sub.c_str(), 4, 2);
        REQUIRE_EQ(r8, size_type(5));
        size_type r9 = ref.find_first_not_of(ref.c_str(), 4, 12);
        REQUIRE_EQ(r9, string_type::npos);

        size_type r10 = ref.find_first_not_of('l', 2);
        REQUIRE_EQ(r10, size_type(4));

        std::string ssub = "eo";
        size_type r11 = ref.find_first_not_of(ssub, 1);
        REQUIRE_EQ(r11, size_type(2));
        size_type r12 = ref.find_first_not_of(ssub, 4);
        REQUIRE_EQ(r12, size_type(5));
    }

    TEST_CASE("find_last_of")
    {
        string_type ref = "path/to/file";
        string_type sub = "/f";

        size_type r1 = ref.find_last_of(sub, 11);
        REQUIRE_EQ(r1, size_type(8));
        size_type r2 = ref.find_last_of(sub, 6);
        REQUIRE_EQ(r2, size_type(4));
        size_type r3 = ref.find_last_of(sub, 3);
        REQUIRE_EQ(r3, string_type::npos);

        size_type r4 = ref.find_last_of(sub.c_str(), 11);
        REQUIRE_EQ(r4, size_type(8));
        size_type r5 = ref.find_last_of(sub.c_str(), 6);
        REQUIRE_EQ(r5, size_type(4));
        size_type r6 = ref.find_last_of(sub.c_str(), 3);
        REQUIRE_EQ(r6, string_type::npos);

        size_type r7 = ref.find_last_of(sub.c_str(), 11, 2);
        REQUIRE_EQ(r7, size_type(8));
        size_type r8 = ref.find_last_of(sub.c_str(), 6, 2);
        REQUIRE_EQ(r8, size_type(4));
        size_type r9 = ref.find_last_of(sub.c_str(), 3, 2);
        REQUIRE_EQ(r9, string_type::npos);

        size_type r10 = ref.find_last_of('/', 11);
        REQUIRE_EQ(r10, size_type(7));
        size_type r11 = ref.find_last_of('/', 6);
        REQUIRE_EQ(r11, size_type(4));
        size_type r12 = ref.find_last_of('/', 3);
        REQUIRE_EQ(r12, string_type::npos);

        std::string ssub = "/f";
        size_type r13 = ref.find_last_of(ssub, 11);
        REQUIRE_EQ(r13, size_type(8));
        size_type r14 = ref.find_last_of(ssub, 6);
        REQUIRE_EQ(r14, size_type(4));
        size_type r15 = ref.find_last_of(ssub, 3);
        REQUIRE_EQ(r15, string_type::npos);
    }

    TEST_CASE("find_last_not_of")
    {
        string_type ref = "path/to/file";
        string_type sub = "/f";

        size_type r1 = ref.find_last_not_of(sub, 11);
        REQUIRE_EQ(r1, size_type(11));
        size_type r2 = ref.find_last_not_of(sub, 4);
        REQUIRE_EQ(r2, size_type(3));

        size_type r3 = ref.find_last_not_of(sub.c_str(), 11);
        REQUIRE_EQ(r3, size_type(11));
        size_type r4 = ref.find_last_not_of(sub.c_str(), 4);
        REQUIRE_EQ(r4, size_type(3));

        size_type r5 = ref.find_last_not_of(sub.c_str(), 11, 2);
        REQUIRE_EQ(r5, size_type(11));
        size_type r6 = ref.find_last_not_of(sub.c_str(), 4, 2);
        REQUIRE_EQ(r6, size_type(3));

        size_type r10 = ref.find_last_not_of('/', 11);
        REQUIRE_EQ(r10, size_type(11));
        size_type r11 = ref.find_last_not_of('/', 4);
        REQUIRE_EQ(r11, size_type(3));

        std::string ssub = "/f";
        size_type r12 = ref.find_last_not_of(ssub, 11);
        REQUIRE_EQ(r12, size_type(11));
        size_type r13 = ref.find_last_not_of(ssub, 4);
        REQUIRE_EQ(r13, size_type(3));
    }

    TEST_CASE("concatenation")
    {
        string_type s1 = "opera";
        string_type s2 = "tions";
        string_type ref = "operations";

        string_type res1 = s1 + s2;
        REQUIRE_EQ(res1.c_str(), ref.c_str());
        string_type res2 = s1 + s2.c_str();
        REQUIRE_EQ(res2.c_str(), ref.c_str());
        string_type res3 = s1 + 'b';
        REQUIRE_EQ(res3.c_str(), "operab");
        string_type res4 = s1.c_str() + s2;
        REQUIRE_EQ(res4.c_str(), ref.c_str());
        string_type res5 = 'v' + s1;
        REQUIRE_EQ(res5.c_str(), "vopera");

        string_type s1bu = s1;
        string_type res6 = std::move(s1bu) + s2;
        REQUIRE_EQ(res6.c_str(), ref.c_str());

        string_type s2bu = s2;
        string_type res7 = s1 + std::move(s2bu);
        REQUIRE_EQ(res7.c_str(), ref.c_str());

        s1bu = s1;
        s2bu = s2;
        string_type res8 = std::move(s1bu) + std::move(s2bu);
        REQUIRE_EQ(res8.c_str(), ref.c_str());

        s1bu = s1;
        string_type res9 = std::move(s1bu) + s2.c_str();
        REQUIRE_EQ(res9.c_str(), ref.c_str());

        s1bu = s1;
        string_type res10 = std::move(s1bu) + 'b';
        REQUIRE_EQ(res10.c_str(), "operab");

        s2bu = s2;
        string_type res11 = s1.c_str() + std::move(s2bu);
        REQUIRE_EQ(res11.c_str(), ref.c_str());

        s2bu = s2;
        string_type res12 = 'b' + std::move(s2bu);
        REQUIRE_EQ(res12.c_str(), "btions");
    }

    TEST_CASE("comparison_operators")
    {
        string_type s1 = "aabcdef";
        string_type s2 = "abcdefg";
        string_type s3 = "aabcd";

        REQUIRE(s1 < s2);
        REQUIRE(s1 <= s2);
        REQUIRE(s1 <= s1);
        REQUIRE(s1 == s1);
        REQUIRE(s1 >= s1);
        REQUIRE(s2 > s1);
        REQUIRE(s2 >= s1);

        REQUIRE(s1.c_str() < s2);
        REQUIRE(s1.c_str() <= s2);
        REQUIRE(s1.c_str() <= s1);
        REQUIRE(s1.c_str() == s1);
        REQUIRE(s1.c_str() >= s1);
        REQUIRE(s2.c_str() > s1);
        REQUIRE(s2.c_str() >= s1);

        REQUIRE(s1 < s2.c_str());
        REQUIRE(s1 <= s2.c_str());
        REQUIRE(s1 <= s1.c_str());
        REQUIRE(s1 == s1.c_str());
        REQUIRE(s1 >= s1.c_str());
        REQUIRE(s2 > s1.c_str());
        REQUIRE(s2 >= s1.c_str());

        std::string ss1 = "aabcdef";
        std::string ss2 = "abcdefg";
        std::string ss3 = "aabcd";

        REQUIRE(s1 < ss2);
        REQUIRE(s1 <= ss2);
        REQUIRE(s1 <= ss1);
        REQUIRE(s1 == ss1);
        REQUIRE(s1 >= ss1);
        REQUIRE(s2 > ss1);
        REQUIRE(s2 >= ss1);
    }

    TEST_CASE("input_output")
    {
        std::string s("input_output");
        string_type ref = "input_output";
        string_type res;

        std::istringstream iss(s);
        iss >> res;
        REQUIRE_EQ(ref.c_str(), res.c_str());

        std::ostringstream oss;
        oss << ref;
        std::string res2 = oss.str();
        REQUIRE_EQ(res2.c_str(), ref.c_str());
    }

    TEST_CASE("hash")
    {
        std::hash<string_type> h;
        std::size_t res = h("test");
        REQUIRE(res != std::size_t(0));
    }

    TEST_CASE("limit")
    {
      using string_type = xbasic_fixed_string<char, 255, buffer | store_size, string_policy::throwing_error>;
      string_type s1 = "hello";
      static_assert(sizeof(s1) == 256 * sizeof(char), "minimal storage");
    }
    }

    TEST_SUITE("numpy_string") {
    TEST_CASE("constructor")
    {
        std::string s = "thisisatest";
        char buf[16];
        strcpy(buf, s.c_str());
        numpy_string* x = reinterpret_cast<numpy_string*>(buf);

        REQUIRE_EQ(*x, s);
        (*x)[4] = '!';
        REQUIRE_EQ(buf[4], '!');
        REQUIRE_EQ(buf, *x);
    }

    TEST_CASE("element_access")
    {
        char buf[16] = "element_access";
        numpy_string& s = *reinterpret_cast<numpy_string*>(buf);
        s[2] = 'E';
        REQUIRE_EQ(s[2], 'E');
        REQUIRE_EQ(s.c_str(), "elEment_access");

        s.at(3) = 'M';
        REQUIRE_EQ(s.at(3), 'M');
        REQUIRE_EQ(s.c_str(), "elEMent_access");

        s.front() = 'E';
        REQUIRE_EQ(s.front(), 'E');
        REQUIRE_EQ(s.c_str(), "ElEMent_access");

        s.back() = 'S';
        REQUIRE_EQ(s.back(), 'S');
        REQUIRE_EQ(s.c_str(), "ElEMent_accesS");

        REQUIRE_THROWS_AS(s.at(15), std::out_of_range);

        REQUIRE_EQ(s.data(), s.c_str());
        REQUIRE_EQ(s.data(), buf);
    }

    TEST_CASE("iterator")
    {
        numpy_string s("iterator");
        *(s.begin()) = 'I';
        REQUIRE_EQ(*(s.begin()), 'I');
        REQUIRE_EQ(*(s.cbegin()), 'I');

        auto iter = s.begin();
        auto citer = s.cbegin();
        for (size_type count = 0; count < s.size(); ++iter, ++citer, ++count) {}
        REQUIRE_EQ(iter, s.end());
        REQUIRE_EQ(citer, s.cend());

        *(s.rbegin()) = 'R';
        REQUIRE_EQ(*(s.rbegin()), 'R');
        REQUIRE_EQ(*(s.crbegin()), 'R');

        auto riter = s.rbegin();
        auto criter = s.crbegin();
        for (size_type count = 0; count < s.size(); ++riter, ++criter, ++count) {}
        REQUIRE_EQ(riter, s.rend());
        REQUIRE_EQ(criter, s.crend());
    }

    TEST_CASE("find")
    {
        numpy_string ref("operationftionf");
        numpy_string sub("tionf");

        size_type r1 = ref.find(sub, 2);
        REQUIRE_EQ(r1, size_type(5));
        size_type r2 = ref.find(sub, 11);
        REQUIRE_EQ(r2, string_type::npos);

        size_type r3 = ref.find(sub.c_str(), 2, 3);
        REQUIRE_EQ(r3, size_type(5));
        size_type r4 = ref.find(sub.c_str(), 11, 3);
        REQUIRE_EQ(r4, string_type::npos);

        size_type r5 = ref.find(sub.c_str(), 2);
        REQUIRE_EQ(r5, size_type(5));
        size_type r6 = ref.find(sub.c_str(), 11);
        REQUIRE_EQ(r6, string_type::npos);

        size_type r7 = ref.find('a', 2);
        REQUIRE_EQ(r7, size_type(4));
        size_type r8 = ref.find('a', 8);
        REQUIRE_EQ(r8, string_type::npos);

        std::string ssub(sub.cbegin(), sub.cend());
        size_type r9 = ref.find(ssub, 2);
        REQUIRE_EQ(r9, size_type(5));
        size_type r10 = ref.find(ssub, 11);
        REQUIRE_EQ(r10, string_type::npos);
    }
    }


} // xeus
