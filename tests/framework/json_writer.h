/*
 * Copyright (c) 2023 The Khronos Group Inc.
 * Copyright (c) 2023 Valve Corporation
 * Copyright (c) 2023 LunarG, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and/or associated documentation files (the "Materials"), to
 * deal in the Materials without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Materials, and to permit persons to whom the Materials are
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice(s) and this permission notice shall be included in
 * all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE MATERIALS OR THE
 * USE OR OTHER DEALINGS IN THE MATERIALS.
 *
 * Author: Charles Giessen <charles@lunarg.com>
 */

#pragma once

#include <stack>
#include <string>
#include <utility>

// Utility class to simplify the writing of JSON manifest files

struct JsonWriter {
    std::string output;

    // the bool represents whether an object has been written & a comma is needed
    std::stack<bool> stack;

    void StartObject() {
        CommaAndNewLine();
        Indent();
        output += "{";
        stack.push(false);
    }
    void StartKeyedObject(std::string const& key) {
        CommaAndNewLine();
        Indent();
        output += "\"" + key + "\": {";
        stack.push(false);
    }
    void EndObject() {
        stack.pop();
        output += "\n";
        Indent();
        output += "}";
    }
    void StartKeyedArray(std::string const& key) {
        CommaAndNewLine();
        Indent();
        output += "\"" + key + "\": [";
        stack.push(false);
    }
    void StartArray() {
        CommaAndNewLine();
        Indent();
        output += "[";
        stack.push(false);
    }
    void EndArray() {
        stack.pop();
        output += "\n";
        Indent();
        output += "]";
    }

    void AddKeyedString(std::string const& key, std::string const& value) {
        CommaAndNewLine();
        Indent();
        output += "\"" + key + "\": \"" + value + "\"";
    }
    void AddString(std::string const& value) {
        CommaAndNewLine();
        Indent();
        output += "\"" + value + "\"";
    }

    void AddKeyedBool(std::string const& key, bool value) {
        CommaAndNewLine();
        Indent();
        output += "\"" + key + "\": " + (value ? "true" : "false");
    }
    void AddBool(bool value) {
        CommaAndNewLine();
        Indent();
        output += std::string(value ? "true" : "false");
    }

   private:
    void CommaAndNewLine() {
        if (stack.size() > 0) {
            if (stack.top() == false) {
                stack.top() = true;
            } else {
                output += ",";
            }
            output += "\n";
        }
    }
    void Indent() {
        for (uint32_t i = 0; i < stack.size(); i++) {
            output += '\t';
        }
    }
};
