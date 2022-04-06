/*
 * Copyright (c) 2021 The Khronos Group Inc.
 * Copyright (c) 2021 Valve Corporation
 * Copyright (c) 2021 LunarG, Inc.
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

#include "test_environment.h"

#include <mutex>

struct MemoryTrackerSettings {
    MemoryTrackerSettings() = default;
    MemoryTrackerSettings(bool should_fail_on_allocation, size_t fail_after_allocations, bool should_fail_after_set_number_of_calls,
                          size_t fail_after_calls)
        : should_fail_on_allocation(should_fail_on_allocation),
          fail_after_allocations(fail_after_allocations),
          should_fail_after_set_number_of_calls(should_fail_after_set_number_of_calls),
          fail_after_calls(fail_after_calls) {}
    bool should_fail_on_allocation = false;
    size_t fail_after_allocations = 0;  // fail after this number of allocations in total
    bool should_fail_after_set_number_of_calls = false;
    size_t fail_after_calls = 0;  // fail after this number of calls to alloc or realloc
};

class MemoryTracker {
    std::mutex main_mutex;
    MemoryTrackerSettings settings{};
    VkAllocationCallbacks callbacks{};
    // Implementation internals
    struct AllocationDetails {
        size_t requested_size_bytes;
        size_t actual_size_bytes;
        VkSystemAllocationScope alloc_scope;
    };
    const static size_t UNKNOWN_ALLOCATION = std::numeric_limits<size_t>::max();
    size_t allocation_count = 0;
    size_t call_count = 0;
    std::vector<std::unique_ptr<char[]>> allocations;
    std::vector<void*> allocations_aligned;
    std::vector<AllocationDetails> allocation_details;
    void add_element(std::unique_ptr<char[]>&& alloc, void* aligned_alloc, AllocationDetails detail) {
        allocations.push_back(std::move(alloc));
        allocations_aligned.push_back(aligned_alloc);
        allocation_details.push_back(detail);
    }
    void erase_index(size_t index) {
        allocations.erase(std::next(allocations.begin(), index));
        allocations_aligned.erase(std::next(allocations_aligned.begin(), index));
        allocation_details.erase(std::next(allocation_details.begin(), index));
    }
    size_t find_element(void* ptr) {
        auto it = std::find(allocations_aligned.begin(), allocations_aligned.end(), ptr);
        if (it == allocations_aligned.end()) return UNKNOWN_ALLOCATION;
        return it - allocations_aligned.begin();
    }

    void* allocate(size_t size, size_t alignment, VkSystemAllocationScope alloc_scope) {
        if (settings.should_fail_on_allocation && allocation_count == settings.fail_after_allocations) return nullptr;
        if (settings.should_fail_after_set_number_of_calls && call_count == settings.fail_after_calls) return nullptr;
        call_count++;
        AllocationDetails detail{size, size + (alignment - 1), alloc_scope};
        auto alloc = std::unique_ptr<char[]>(new char[detail.actual_size_bytes]);
        if (!alloc) return nullptr;
        uint64_t addr = (uint64_t)alloc.get();
        addr += (alignment - 1);
        addr &= ~(alignment - 1);
        void* aligned_alloc = (void*)addr;
        add_element(std::move(alloc), aligned_alloc, detail);
        allocation_count++;
        return allocations_aligned.back();
    }
    void* reallocate(void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope alloc_scope) {
        if (pOriginal == nullptr) {
            return allocate(size, alignment, alloc_scope);
        }
        size_t index = find_element(pOriginal);
        if (index == UNKNOWN_ALLOCATION) return nullptr;
        size_t original_size = allocation_details[index].requested_size_bytes;

        // We only care about the case where realloc is used to increase the size
        if (size >= original_size && settings.should_fail_after_set_number_of_calls && call_count == settings.fail_after_calls)
            return nullptr;
        call_count++;
        if (size == 0) {
            erase_index(index);
            allocation_count--;
            return nullptr;
        } else if (size < original_size) {
            return pOriginal;
        } else {
            void* new_alloc = allocate(size, alignment, alloc_scope);
            if (new_alloc == nullptr) return nullptr;
            memcpy(new_alloc, pOriginal, original_size);
            erase_index(index);
            return new_alloc;
        }
    }
    void free(void* pMemory) {
        if (pMemory == nullptr) return;
        size_t index = find_element(pMemory);
        if (index == UNKNOWN_ALLOCATION) return;
        erase_index(index);
        assert(allocation_count != 0 && "Cant free when there are no valid allocations");
        allocation_count--;
    }

    // Implementation of public functions
    void* impl_allocation(size_t size, size_t alignment, VkSystemAllocationScope allocationScope) noexcept {
        std::lock_guard<std::mutex> lg(main_mutex);
        void* addr = allocate(size, alignment, allocationScope);
        return addr;
    }
    void* impl_reallocation(void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) noexcept {
        std::lock_guard<std::mutex> lg(main_mutex);
        void* addr = reallocate(pOriginal, size, alignment, allocationScope);
        return addr;
    }
    void impl_free(void* pMemory) noexcept {
        std::lock_guard<std::mutex> lg(main_mutex);
        free(pMemory);
    }
    void impl_internal_allocation_notification(size_t size, VkInternalAllocationType allocationType,
                                               VkSystemAllocationScope allocationScope) noexcept {
        std::lock_guard<std::mutex> lg(main_mutex);
        // TODO?
    }
    void impl_internal_free(size_t size, VkInternalAllocationType allocationType,
                            VkSystemAllocationScope allocationScope) noexcept {
        std::lock_guard<std::mutex> lg(main_mutex);
        // TODO?
    }

   public:
    MemoryTracker(MemoryTrackerSettings settings) noexcept : settings(settings) {
        allocations.reserve(512);
        allocations_aligned.reserve(512);
        allocation_details.reserve(512);

        callbacks.pUserData = this;
        callbacks.pfnAllocation = public_allocation;
        callbacks.pfnReallocation = public_reallocation;
        callbacks.pfnFree = public_free;
        callbacks.pfnInternalAllocation = public_internal_allocation_notification;
        callbacks.pfnInternalFree = public_internal_free;
    }
    MemoryTracker() noexcept : MemoryTracker(MemoryTrackerSettings{}) {}

    VkAllocationCallbacks* get() noexcept { return &callbacks; }

    bool empty() noexcept { return allocation_count == 0; }

    void update_settings(MemoryTrackerSettings new_settings) noexcept { settings = new_settings; }
    size_t current_allocation_count() const noexcept { return allocation_count; }
    size_t current_call_count() const noexcept { return call_count; }
    // Static callbacks
    static VKAPI_ATTR void* VKAPI_CALL public_allocation(void* pUserData, size_t size, size_t alignment,
                                                         VkSystemAllocationScope allocationScope) noexcept {
        return reinterpret_cast<MemoryTracker*>(pUserData)->impl_allocation(size, alignment, allocationScope);
    }
    static VKAPI_ATTR void* VKAPI_CALL public_reallocation(void* pUserData, void* pOriginal, size_t size, size_t alignment,
                                                           VkSystemAllocationScope allocationScope) noexcept {
        return reinterpret_cast<MemoryTracker*>(pUserData)->impl_reallocation(pOriginal, size, alignment, allocationScope);
    }
    static VKAPI_ATTR void VKAPI_CALL public_free(void* pUserData, void* pMemory) noexcept {
        reinterpret_cast<MemoryTracker*>(pUserData)->impl_free(pMemory);
    }
    static VKAPI_ATTR void VKAPI_CALL public_internal_allocation_notification(void* pUserData, size_t size,
                                                                              VkInternalAllocationType allocationType,
                                                                              VkSystemAllocationScope allocationScope) noexcept {
        reinterpret_cast<MemoryTracker*>(pUserData)->impl_internal_allocation_notification(size, allocationType, allocationScope);
    }
    static VKAPI_ATTR void VKAPI_CALL public_internal_free(void* pUserData, size_t size, VkInternalAllocationType allocationType,
                                                           VkSystemAllocationScope allocationScope) noexcept {
        reinterpret_cast<MemoryTracker*>(pUserData)->impl_internal_free(size, allocationType, allocationScope);
    }
};

// Test making sure the allocation functions are called to allocate and cleanup everything during
// a CreateInstance/DestroyInstance call pair.
TEST(Allocation, Instance) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    MemoryTracker tracker;
    {
        InstWrapper inst{env.vulkan_functions, tracker.get()};
        inst.CheckCreate();
    }
    ASSERT_TRUE(tracker.empty());
}

// Test making sure the allocation functions are called to allocate and cleanup everything during
// a CreateInstance/DestroyInstance call pair with a call to GetInstanceProcAddr.
TEST(Allocation, GetInstanceProcAddr) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    MemoryTracker tracker;
    {
        InstWrapper inst{env.vulkan_functions, tracker.get()};
        inst.CheckCreate();

        auto* pfnCreateDevice = inst->vkGetInstanceProcAddr(inst, "vkCreateDevice");
        auto* pfnDestroyDevice = inst->vkGetInstanceProcAddr(inst, "vkDestroyDevice");
        ASSERT_TRUE(pfnCreateDevice != nullptr && pfnDestroyDevice != nullptr);
    }
    ASSERT_TRUE(tracker.empty());
}

// Test making sure the allocation functions are called to allocate and cleanup everything during
// a vkEnumeratePhysicalDevices call pair.
TEST(Allocation, EnumeratePhysicalDevices) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    MemoryTracker tracker;
    auto& driver = env.get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    {
        InstWrapper inst{env.vulkan_functions, tracker.get()};
        inst.CheckCreate();
        uint32_t physical_count = 1;
        uint32_t returned_physical_count = 0;
        ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, nullptr));
        ASSERT_EQ(physical_count, returned_physical_count);

        VkPhysicalDevice physical_device;
        ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, &physical_device));
        ASSERT_EQ(physical_count, returned_physical_count);
    }
    ASSERT_TRUE(tracker.empty());
}

// Test making sure the allocation functions are called to allocate and cleanup everything from
// vkCreateInstance, to vkCreateDevicce, and then through their destructors.  With special
// allocators used on both the instance and device.
TEST(Allocation, InstanceAndDevice) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    MemoryTracker tracker;
    auto& driver = env.get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices[0].add_queue_family_properties({{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, false});
    {
        InstWrapper inst{env.vulkan_functions, tracker.get()};
        inst.CheckCreate();

        uint32_t physical_count = 1;
        uint32_t returned_physical_count = 0;
        ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, nullptr));
        ASSERT_EQ(physical_count, returned_physical_count);

        VkPhysicalDevice physical_device;
        ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, &physical_device));
        ASSERT_EQ(physical_count, returned_physical_count);

        uint32_t family_count = 1;
        uint32_t returned_family_count = 0;
        env.vulkan_functions.vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &returned_family_count, nullptr);
        ASSERT_EQ(returned_family_count, family_count);

        VkQueueFamilyProperties family;
        env.vulkan_functions.vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &returned_family_count, &family);
        ASSERT_EQ(returned_family_count, family_count);
        ASSERT_EQ(family.queueFlags, static_cast<VkQueueFlags>(VK_QUEUE_GRAPHICS_BIT));
        ASSERT_EQ(family.queueCount, family_count);
        ASSERT_EQ(family.timestampValidBits, 0U);

        DeviceCreateInfo dev_create_info;
        DeviceQueueCreateInfo queue_info;
        queue_info.add_priority(0.0f);
        dev_create_info.add_device_queue(queue_info);

        VkDevice device;
        ASSERT_EQ(inst->vkCreateDevice(physical_device, dev_create_info.get(), tracker.get(), &device), VK_SUCCESS);
        inst->vkDestroyDevice(device, tracker.get());
    }
    ASSERT_TRUE(tracker.empty());
}
// Test making sure the allocation functions are called to allocate and cleanup everything from
// vkCreateInstance, to vkCreateDevicce, and then through their destructors.  With special
// allocators used on only the instance and not the device.
TEST(Allocation, InstanceButNotDevice) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    MemoryTracker tracker;
    {
        auto& driver = env.get_test_icd();
        driver.physical_devices.emplace_back("physical_device_0");
        driver.physical_devices[0].add_queue_family_properties({{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, false});

        InstWrapper inst{env.vulkan_functions, tracker.get()};
        inst.CheckCreate();

        uint32_t physical_count = 1;
        uint32_t returned_physical_count = 0;
        ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, nullptr));
        ASSERT_EQ(physical_count, returned_physical_count);

        VkPhysicalDevice physical_device;
        ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, &physical_device));
        ASSERT_EQ(physical_count, returned_physical_count);

        uint32_t family_count = 1;
        uint32_t returned_family_count = 0;
        env.vulkan_functions.vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &returned_family_count, nullptr);
        ASSERT_EQ(returned_family_count, family_count);

        VkQueueFamilyProperties family;
        env.vulkan_functions.vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &returned_family_count, &family);
        ASSERT_EQ(returned_family_count, family_count);
        ASSERT_EQ(family.queueFlags, static_cast<VkQueueFlags>(VK_QUEUE_GRAPHICS_BIT));
        ASSERT_EQ(family.queueCount, family_count);
        ASSERT_EQ(family.timestampValidBits, 0U);

        DeviceCreateInfo dev_create_info;
        DeviceQueueCreateInfo queue_info;
        queue_info.add_priority(0.0f);
        dev_create_info.add_device_queue(queue_info);

        VkDevice device;
        ASSERT_EQ(inst->vkCreateDevice(physical_device, dev_create_info.get(), nullptr, &device), VK_SUCCESS);
        inst->vkDestroyDevice(device, nullptr);
    }
    ASSERT_TRUE(tracker.empty());
}

// Test making sure the allocation functions are called to allocate and cleanup everything from
// vkCreateInstance, to vkCreateDevicce, and then through their destructors.  With special
// allocators used on only the device and not the instance.
TEST(Allocation, DeviceButNotInstance) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    MemoryTracker tracker;
    {
        auto& driver = env.get_test_icd();
        driver.physical_devices.emplace_back("physical_device_0");
        driver.physical_devices[0].add_queue_family_properties({{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, false});

        InstWrapper inst{env.vulkan_functions};
        inst.CheckCreate();

        uint32_t physical_count = 1;
        uint32_t returned_physical_count = 0;
        ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, nullptr));
        ASSERT_EQ(physical_count, returned_physical_count);

        VkPhysicalDevice physical_device;
        ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, &physical_device));
        ASSERT_EQ(physical_count, returned_physical_count);

        uint32_t family_count = 1;
        uint32_t returned_family_count = 0;
        env.vulkan_functions.vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &returned_family_count, nullptr);
        ASSERT_EQ(returned_family_count, family_count);

        VkQueueFamilyProperties family;
        env.vulkan_functions.vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &returned_family_count, &family);
        ASSERT_EQ(returned_family_count, family_count);
        ASSERT_EQ(family.queueFlags, static_cast<VkQueueFlags>(VK_QUEUE_GRAPHICS_BIT));
        ASSERT_EQ(family.queueCount, family_count);
        ASSERT_EQ(family.timestampValidBits, 0U);

        DeviceCreateInfo dev_create_info;
        DeviceQueueCreateInfo queue_info;
        queue_info.add_priority(0.0f);
        dev_create_info.add_device_queue(queue_info);

        VkDevice device;
        ASSERT_EQ(inst->vkCreateDevice(physical_device, dev_create_info.get(), tracker.get(), &device), VK_SUCCESS);
        inst->vkDestroyDevice(device, tracker.get());
    }
    ASSERT_TRUE(tracker.empty());
}

// Test failure during vkCreateInstance to make sure we don't leak memory if
// one of the out-of-memory conditions trigger.
TEST(Allocation, CreateInstanceIntentionalAllocFail) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    size_t fail_index = 0;
    VkResult result = VK_ERROR_OUT_OF_HOST_MEMORY;
    while (result == VK_ERROR_OUT_OF_HOST_MEMORY && fail_index <= 10000) {
        MemoryTracker tracker(MemoryTrackerSettings{false, 0, true, fail_index});

        VkInstance instance;
        InstanceCreateInfo inst_create_info{};
        result = env.vulkan_functions.vkCreateInstance(inst_create_info.get(), tracker.get(), &instance);
        if (result == VK_SUCCESS) {
            env.vulkan_functions.vkDestroyInstance(instance, tracker.get());
        }
        ASSERT_TRUE(tracker.empty());
        fail_index++;
    }
}

// Test failure during vkCreateDevice to make sure we don't leak memory if
// one of the out-of-memory conditions trigger.
// Use 2 physical devices so that anything which copies a list of devices item by item
// may fail.
TEST(Allocation, CreateDeviceIntentionalAllocFail) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    auto& driver = env.get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices[0].add_queue_family_properties({{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, false});
    driver.physical_devices.emplace_back("physical_device_1");
    driver.physical_devices[1].add_queue_family_properties({{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, false});

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    uint32_t physical_count = 2;
    uint32_t returned_physical_count = 0;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, nullptr));
    ASSERT_EQ(physical_count, returned_physical_count);

    VkPhysicalDevice physical_devices[2];
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, physical_devices));
    ASSERT_EQ(physical_count, returned_physical_count);

    uint32_t family_count = 1;
    uint32_t returned_family_count = 0;
    env.vulkan_functions.vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[0], &returned_family_count, nullptr);
    ASSERT_EQ(returned_family_count, family_count);

    VkQueueFamilyProperties family;
    env.vulkan_functions.vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[0], &returned_family_count, &family);
    ASSERT_EQ(returned_family_count, family_count);
    ASSERT_EQ(family.queueFlags, static_cast<VkQueueFlags>(VK_QUEUE_GRAPHICS_BIT));
    ASSERT_EQ(family.queueCount, family_count);
    ASSERT_EQ(family.timestampValidBits, 0U);

    size_t fail_index = 0;
    VkResult result = VK_ERROR_OUT_OF_HOST_MEMORY;
    while (result == VK_ERROR_OUT_OF_HOST_MEMORY) {
        MemoryTracker tracker(MemoryTrackerSettings{false, 0, true, fail_index});

        DeviceCreateInfo dev_create_info;
        DeviceQueueCreateInfo queue_info;
        queue_info.add_priority(0.0f);
        dev_create_info.add_device_queue(queue_info);

        VkDevice device;
        result = inst->vkCreateDevice(physical_devices[0], dev_create_info.get(), tracker.get(), &device);
        if (result == VK_SUCCESS || fail_index > 10000) {
            inst->vkDestroyDevice(device, tracker.get());
            break;
        }
        ASSERT_TRUE(tracker.empty());
        fail_index++;
    }
}

// Test failure during vkCreateInstance and vkCreateDevice to make sure we don't
// leak memory if one of the out-of-memory conditions trigger.
TEST(Allocation, CreateInstanceDeviceIntentionalAllocFail) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    auto& driver = env.get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices[0].add_queue_family_properties({{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, false});

    size_t fail_index = 0;
    VkResult result = VK_ERROR_OUT_OF_HOST_MEMORY;
    while (result == VK_ERROR_OUT_OF_HOST_MEMORY && fail_index <= 10000) {
        MemoryTracker tracker(MemoryTrackerSettings{false, 0, true, fail_index});
        fail_index++;  // applies to the next loop

        VkInstance instance;
        InstanceCreateInfo inst_create_info{};
        result = env.vulkan_functions.vkCreateInstance(inst_create_info.get(), tracker.get(), &instance);
        if (result == VK_ERROR_OUT_OF_HOST_MEMORY) {
            ASSERT_TRUE(tracker.empty());
            continue;
        }

        uint32_t physical_count = 1;
        uint32_t returned_physical_count = 0;
        result = env.vulkan_functions.vkEnumeratePhysicalDevices(instance, &returned_physical_count, nullptr);
        if (result == VK_ERROR_OUT_OF_HOST_MEMORY) {
            env.vulkan_functions.vkDestroyInstance(instance, tracker.get());
            ASSERT_TRUE(tracker.empty());
            continue;
        }
        ASSERT_EQ(physical_count, returned_physical_count);

        VkPhysicalDevice physical_device;
        result = env.vulkan_functions.vkEnumeratePhysicalDevices(instance, &returned_physical_count, &physical_device);
        if (result == VK_ERROR_OUT_OF_HOST_MEMORY) {
            env.vulkan_functions.vkDestroyInstance(instance, tracker.get());
            ASSERT_TRUE(tracker.empty());
            continue;
        }
        ASSERT_EQ(physical_count, returned_physical_count);

        uint32_t family_count = 1;
        uint32_t returned_family_count = 0;
        env.vulkan_functions.vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &returned_family_count, nullptr);
        ASSERT_EQ(returned_family_count, family_count);

        VkQueueFamilyProperties family;
        env.vulkan_functions.vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &returned_family_count, &family);
        ASSERT_EQ(returned_family_count, family_count);
        ASSERT_EQ(family.queueFlags, static_cast<VkQueueFlags>(VK_QUEUE_GRAPHICS_BIT));
        ASSERT_EQ(family.queueCount, family_count);
        ASSERT_EQ(family.timestampValidBits, 0U);

        DeviceCreateInfo dev_create_info;
        DeviceQueueCreateInfo queue_info;
        queue_info.add_priority(0.0f);
        dev_create_info.add_device_queue(queue_info);

        VkDevice device;
        result = env.vulkan_functions.vkCreateDevice(physical_device, dev_create_info.get(), tracker.get(), &device);
        if (result == VK_SUCCESS) {
            env.vulkan_functions.vkDestroyDevice(device, tracker.get());
        }
        env.vulkan_functions.vkDestroyInstance(instance, tracker.get());

        ASSERT_TRUE(tracker.empty());
    }
}

// Test failure during vkCreateInstance when a driver of the wrong architecture is present
// to make sure the loader uses the valid ICD and doesn't report incompatible driver just because
// an incompatible driver exists
TEST(TryLoadWrongBinaries, CreateInstanceIntentionalAllocFail) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    env.add_icd(TestICDDetails(CURRENT_PLATFORM_DUMMY_BINARY_WRONG_TYPE).set_is_fake(true));
    size_t fail_index = 0;
    VkResult result = VK_ERROR_OUT_OF_HOST_MEMORY;
    while (result == VK_ERROR_OUT_OF_HOST_MEMORY && fail_index <= 10000) {
        MemoryTracker tracker(MemoryTrackerSettings{false, 0, true, fail_index});

        VkInstance instance;
        InstanceCreateInfo inst_create_info{};
        result = env.vulkan_functions.vkCreateInstance(inst_create_info.get(), tracker.get(), &instance);
        if (result == VK_SUCCESS) {
            env.vulkan_functions.vkDestroyInstance(instance, tracker.get());
        }
        ASSERT_NE(result, VK_ERROR_INCOMPATIBLE_DRIVER);
        ASSERT_TRUE(tracker.empty());
        fail_index++;
    }
}

// Test failure during vkCreateInstance and vkCreateDevice to make sure we don't
// leak memory if one of the out-of-memory conditions trigger.
TEST(Allocation, EnumeratePhysicalDevicesIntentionalAllocFail) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    size_t fail_index = 0;
    bool reached_the_end = false;
    uint32_t starting_physical_dev_count = 3;
    while (!reached_the_end && fail_index <= 100) {
        fail_index++;  // applies to the next loop
        uint32_t physical_dev_count = starting_physical_dev_count;
        VkResult result = VK_ERROR_OUT_OF_HOST_MEMORY;
        auto& driver = env.reset_icd();

        for (uint32_t i = 0; i < physical_dev_count; i++) {
            driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i));
            driver.physical_devices[i].add_queue_family_properties({{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, false});
        }
        MemoryTracker tracker{MemoryTrackerSettings{false, 0, true, fail_index}};
        InstanceCreateInfo inst_create_info;
        VkInstance instance;
        result = env.vulkan_functions.vkCreateInstance(inst_create_info.get(), tracker.get(), &instance);
        if (result == VK_ERROR_OUT_OF_HOST_MEMORY) {
            ASSERT_TRUE(tracker.empty());
            continue;
        }

        uint32_t returned_physical_count = 0;
        result = env.vulkan_functions.vkEnumeratePhysicalDevices(instance, &returned_physical_count, nullptr);
        if (result == VK_ERROR_OUT_OF_HOST_MEMORY) {
            env.vulkan_functions.vkDestroyInstance(instance, tracker.get());
            ASSERT_TRUE(tracker.empty());
            continue;
        }
        ASSERT_EQ(physical_dev_count, returned_physical_count);

        for (uint32_t i = 0; i < 2; i++) {
            driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(physical_dev_count));
            physical_dev_count += 1;
        }

        std::vector<VkPhysicalDevice> physical_devices{physical_dev_count, VK_NULL_HANDLE};
        result = env.vulkan_functions.vkEnumeratePhysicalDevices(instance, &returned_physical_count, physical_devices.data());
        if (result == VK_ERROR_OUT_OF_HOST_MEMORY) {
            env.vulkan_functions.vkDestroyInstance(instance, tracker.get());
            ASSERT_TRUE(tracker.empty());
            continue;
        }
        if (result == VK_INCOMPLETE) {
            result = env.vulkan_functions.vkEnumeratePhysicalDevices(instance, &returned_physical_count, nullptr);
            if (result == VK_ERROR_OUT_OF_HOST_MEMORY) {
                env.vulkan_functions.vkDestroyInstance(instance, tracker.get());
                ASSERT_TRUE(tracker.empty());
                continue;
            }
            physical_devices.resize(returned_physical_count);
            result = env.vulkan_functions.vkEnumeratePhysicalDevices(instance, &returned_physical_count, physical_devices.data());
            if (result == VK_ERROR_OUT_OF_HOST_MEMORY) {
                env.vulkan_functions.vkDestroyInstance(instance, tracker.get());
                ASSERT_TRUE(tracker.empty());
                continue;
            }
        }
        ASSERT_EQ(physical_dev_count, returned_physical_count);

        std::cout << "fail count " << fail_index << "\n";
        env.vulkan_functions.vkDestroyInstance(instance, tracker.get());
        ASSERT_TRUE(tracker.empty());
        reached_the_end = true;
    }
}
#if defined(WIN32)
// Test failure during vkCreateInstance and vkCreateDevice to make sure we don't
// leak memory if one of the out-of-memory conditions trigger.
TEST(Allocation, CreateInstanceDeviceWithDXGIDriverIntentionalAllocFail) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6).set_discovery_type(ManifestDiscoveryType::none));
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    for (uint32_t i = 0; i < 2; i++) {
        auto& driver = env.get_test_icd(i);
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i));
        driver.physical_devices[0].add_queue_family_properties({{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, false});
    }

    auto& known_driver = known_driver_list.at(2);  // which drive this test pretends to be
    DXGI_ADAPTER_DESC1 desc1{};
    desc1.VendorId = known_driver.vendor_id;
    desc1.AdapterLuid = _LUID{10, 1000};
    env.platform_shim->add_dxgi_adapter(GpuType::discrete, desc1);
    env.get_test_icd().set_adapterLUID(desc1.AdapterLuid);

    env.platform_shim->add_d3dkmt_adapter(D3DKMT_Adapter{0, _LUID{10, 1000}}.add_driver_manifest_path(env.get_icd_manifest_path()));

    size_t fail_index = 0;
    VkResult result = VK_ERROR_OUT_OF_HOST_MEMORY;
    while (result == VK_ERROR_OUT_OF_HOST_MEMORY && fail_index <= 10000) {
        MemoryTracker tracker(MemoryTrackerSettings{false, 0, true, fail_index});
        fail_index++;  // applies to the next loop

        VkInstance instance;
        InstanceCreateInfo inst_create_info{};
        result = env.vulkan_functions.vkCreateInstance(inst_create_info.get(), tracker.get(), &instance);
        if (result == VK_ERROR_OUT_OF_HOST_MEMORY) {
            ASSERT_TRUE(tracker.empty());
            continue;
        }

        uint32_t physical_count = 2;
        uint32_t returned_physical_count = 0;
        result = env.vulkan_functions.vkEnumeratePhysicalDevices(instance, &returned_physical_count, nullptr);
        if (result == VK_ERROR_OUT_OF_HOST_MEMORY || result == VK_INCOMPLETE) {
            env.vulkan_functions.vkDestroyInstance(instance, tracker.get());
            ASSERT_TRUE(tracker.empty());
            continue;
        }
        ASSERT_EQ(physical_count, returned_physical_count);

        std::array<VkPhysicalDevice, 2> physical_devices;
        result = env.vulkan_functions.vkEnumeratePhysicalDevices(instance, &returned_physical_count, physical_devices.data());
        if (result == VK_ERROR_OUT_OF_HOST_MEMORY || result == VK_INCOMPLETE) {
            env.vulkan_functions.vkDestroyInstance(instance, tracker.get());
            ASSERT_TRUE(tracker.empty());
            continue;
        }
        ASSERT_EQ(physical_count, returned_physical_count);

        std::array<VkDevice, 2> devices;
        for (uint32_t i = 0; i < returned_physical_count; i++) {
            uint32_t family_count = 1;
            uint32_t returned_family_count = 0;
            env.vulkan_functions.vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[i], &returned_family_count, nullptr);
            ASSERT_EQ(returned_family_count, family_count);

            VkQueueFamilyProperties family;
            env.vulkan_functions.vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[i], &returned_family_count, &family);
            ASSERT_EQ(returned_family_count, family_count);
            ASSERT_EQ(family.queueFlags, static_cast<VkQueueFlags>(VK_QUEUE_GRAPHICS_BIT));
            ASSERT_EQ(family.queueCount, family_count);
            ASSERT_EQ(family.timestampValidBits, 0U);

            DeviceCreateInfo dev_create_info;
            DeviceQueueCreateInfo queue_info;
            queue_info.add_priority(0.0f);
            dev_create_info.add_device_queue(queue_info);

            result = env.vulkan_functions.vkCreateDevice(physical_devices[i], dev_create_info.get(), tracker.get(), &devices[i]);
        }
        for (uint32_t i = 0; i < returned_physical_count; i++) {
            if (result == VK_SUCCESS) {
                env.vulkan_functions.vkDestroyDevice(devices[i], tracker.get());
            }
        }
        env.vulkan_functions.vkDestroyInstance(instance, tracker.get());

        ASSERT_TRUE(tracker.empty());
    }
}
#endif
