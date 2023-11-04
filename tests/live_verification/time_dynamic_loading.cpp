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

#include <vulkan/vulkan.h>

#include <chrono>
#include <iostream>
#include <vector>

int main() {
    uint32_t iterations = 20;
    std::vector<std::chrono::milliseconds> samples;
    samples.resize(iterations);
    for (uint32_t i = 0; i < iterations; i++) {
        auto t1 = std::chrono::system_clock::now();
        uint32_t count = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
        // vkEnumerateInstanceLayerProperties(&count, nullptr);
        // vkEnumerateInstanceVersion(&count);
        auto t2 = std::chrono::system_clock::now();
        samples[i] = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    }
    std::chrono::milliseconds total_time{};
    for (uint32_t i = 0; i < iterations; i++) {
        total_time += samples[i];
    }
    std::cout << "average time " << total_time.count() / iterations << "ms\n";
    std::cout << "first call time " << samples[0].count() << "ms\n";
    std::cout << "second call time " << samples[1].count() << "ms\n";
    std::cout << "last call time " << samples[iterations - 1].count() << "ms\n";
}
