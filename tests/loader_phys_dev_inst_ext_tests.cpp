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
 * Author: Mark Young <marky@lunarg.com>
 */

#include "test_environment.h"

// These tests are all instance extension tests that touch physical devices.  This was
// before the idea that physical device extensions were more appropriately found in the
// list of device extensions.  Because of that, all these tests need to support devices
// that don't support the extension and have a fallback path in the loader that needs
// validation.

class LoaderInstPhysDevExts : public ::testing::Test {
   protected:
    virtual void SetUp() {
        env = std::unique_ptr<FrameworkEnvironment>(new FrameworkEnvironment());
        env->add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    }

    virtual void TearDown() { env.reset(); }
    std::unique_ptr<FrameworkEnvironment> env;
};

// Fill in random but valid data into the device properties struct for the current physical device
static void FillInRandomICDInfo(uint32_t& vendor_id, uint32_t& driver_vers) {
    vendor_id = VK_MAKE_API_VERSION(0, rand() % 64, rand() % 255, rand() % 255);
    driver_vers = VK_MAKE_API_VERSION(0, rand() % 64, rand() % 255, rand() % 255);
}

// Fill in random but valid data into the device properties struct for the current physical device
static void FillInRandomDeviceProps(VkPhysicalDeviceProperties& props, uint32_t api_vers, uint32_t vendor, uint32_t driver_vers) {
    props.apiVersion = api_vers;
    props.driverVersion = driver_vers;
    props.vendorID = vendor;
    props.deviceID = (static_cast<uint32_t>(rand()) >> 4) + (static_cast<uint32_t>(rand()) << 2);
    props.deviceType = static_cast<VkPhysicalDeviceType>(rand() % 5);
    for (uint8_t idx = 0; idx < VK_UUID_SIZE; ++idx) {
        props.pipelineCacheUUID[idx] = static_cast<uint8_t>(rand() % 255);
    }
}

//
// VK_KHR_get_physical_device_properties2
//

// Test vkGetPhysicalDeviceProperties2KHR where nothing supports it.
TEST_F(LoaderInstPhysDevExts, PhysDevProps2KHRNoSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.CheckCreate();

    auto GetPhysDevProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties2KHR"));
    ASSERT_EQ(GetPhysDevProps2, nullptr);
}

// Test vkGetPhysicalDeviceProperties2KHR where instance supports it, but nothing else.
TEST_F(LoaderInstPhysDevExts, PhysDevProps2KHRNoICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    instance.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    auto GetPhysDevProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties2KHR"));
    ASSERT_EQ(GetPhysDevProps2, nullptr);
}

// Test vkGetPhysicalDeviceProperties2KHR where instance and ICD supports it, but device does not support it.
TEST_F(LoaderInstPhysDevExts, PhysDevProps2KHRInstanceAndICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    FillInRandomDeviceProps(env.get_test_icd(0).physical_devices.back().properties, VK_API_VERSION_1_0, 5, 123);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    instance.CheckCreate();

    auto GetPhysDevProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties2KHR"));
    ASSERT_NE(GetPhysDevProps2, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    VkPhysicalDeviceProperties props{};
    instance->vkGetPhysicalDeviceProperties(physical_device, &props);
    VkPhysicalDeviceProperties2KHR props2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR};
    GetPhysDevProps2(physical_device, &props2);

    // Both properties should match
    ASSERT_EQ(props.apiVersion, props2.properties.apiVersion);
    ASSERT_EQ(props.driverVersion, props2.properties.driverVersion);
    ASSERT_EQ(props.vendorID, props2.properties.vendorID);
    ASSERT_EQ(props.deviceID, props2.properties.deviceID);
    ASSERT_EQ(props.deviceType, props2.properties.deviceType);
    ASSERT_EQ(0, memcmp(props.pipelineCacheUUID, props2.properties.pipelineCacheUUID, VK_UUID_SIZE));
}

// Test vkGetPhysicalDeviceProperties2 where instance supports, an ICD, and a device under that ICD
// also support, so everything should work and return properly.
TEST_F(LoaderInstPhysDevExts, PhysDevProps2Simple) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).icd_api_version = VK_API_VERSION_1_1;
    env.get_test_icd(0).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    env.get_test_icd(0).physical_devices.back().extensions.push_back({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, 0});
    FillInRandomDeviceProps(env.get_test_icd(0).physical_devices.back().properties, VK_API_VERSION_1_1, 5, 123);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(VK_API_VERSION_1_1);
    instance.CheckCreate();

    auto GetPhysDevProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties2"));
    ASSERT_NE(GetPhysDevProps2, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    VkPhysicalDeviceProperties props{};
    instance->vkGetPhysicalDeviceProperties(physical_device, &props);
    VkPhysicalDeviceProperties2KHR props2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR};
    GetPhysDevProps2(physical_device, &props2);

    // Both properties should match
    ASSERT_EQ(props.apiVersion, props2.properties.apiVersion);
    ASSERT_EQ(props.driverVersion, props2.properties.driverVersion);
    ASSERT_EQ(props.vendorID, props2.properties.vendorID);
    ASSERT_EQ(props.deviceID, props2.properties.deviceID);
    ASSERT_EQ(props.deviceType, props2.properties.deviceType);
    ASSERT_EQ(0, memcmp(props.pipelineCacheUUID, props2.properties.pipelineCacheUUID, VK_UUID_SIZE));
}

// Test vkGetPhysicalDeviceProperties2 where instance supports it with some ICDs that both support
// and don't support it:
//    ICD 0 supports
//        Physical device 0 does not
//        Physical device 1 does
//        Physical device 2 does not
//    ICD 1 doesn't support
//        Physical device 3 does not
//    ICD 2 supports
//        Physical device 4 does not
//        Physical device 5 does not
//    ICD 3 supports
//        Physical device 6 does
TEST_F(LoaderInstPhysDevExts, PhysDevProps2Mixed) {
    FrameworkEnvironment env{};
    const uint32_t max_icd_count = 4;
    const uint32_t dev_counts[max_icd_count] = {3, 1, 2, 1};
    const uint32_t max_phys_devs = 7;

    for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
        auto& cur_icd = env.get_test_icd(icd);

        // ICD 1 should not have 1.1
        if (icd != 1) {
            cur_icd.icd_api_version = VK_API_VERSION_1_1;
            cur_icd.add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
        } else {
            cur_icd.icd_api_version = VK_API_VERSION_1_0;
        }

        uint32_t rand_vendor_id;
        uint32_t rand_driver_vers;
        FillInRandomICDInfo(rand_vendor_id, rand_driver_vers);

        for (uint32_t dev = 0; dev < dev_counts[icd]; ++dev) {
            uint32_t device_version = VK_API_VERSION_1_0;
            cur_icd.physical_devices.push_back({});
            auto& cur_dev = cur_icd.physical_devices.back();

            // 2nd device in ICD 0 and the one device in ICD 3 support the extension and 1.1
            if ((icd == 0 && dev == 1) || icd == 3) {
                cur_dev.extensions.push_back({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, 0});
                device_version = VK_API_VERSION_1_1;
            }
            FillInRandomDeviceProps(cur_dev.properties, device_version, rand_vendor_id, rand_driver_vers);
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(VK_API_VERSION_1_1);
    instance.CheckCreate();

    auto GetPhysDevProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties2"));
    ASSERT_NE(GetPhysDevProps2, nullptr);

    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        VkPhysicalDeviceProperties props{};
        instance->vkGetPhysicalDeviceProperties(physical_devices[dev], &props);
        VkPhysicalDeviceProperties2KHR props2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
        GetPhysDevProps2(physical_devices[dev], &props2);

        // Both properties should match
        ASSERT_EQ(props.apiVersion, props2.properties.apiVersion);
        ASSERT_EQ(props.driverVersion, props2.properties.driverVersion);
        ASSERT_EQ(props.vendorID, props2.properties.vendorID);
        ASSERT_EQ(props.deviceID, props2.properties.deviceID);
        ASSERT_EQ(props.deviceType, props2.properties.deviceType);
        ASSERT_EQ(0, memcmp(props.pipelineCacheUUID, props2.properties.pipelineCacheUUID, VK_UUID_SIZE));
    }
}

// Fill in random but valid data into the features struct for the current physical device
static void FillInRandomFeatures(VkPhysicalDeviceFeatures& feats) {
    uint32_t num_bool_32 = sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32);
    VkBool32* cur = reinterpret_cast<VkBool32*>(&feats);
    for (uint32_t val = 0; val < num_bool_32; ++val) {
        *cur = (rand() % 2 == 0) ? VK_FALSE : VK_TRUE;
        cur++;
    }
}

// Compare the contents of the feature structs
static void CompareFeatures(const VkPhysicalDeviceFeatures& feats1, const VkPhysicalDeviceFeatures2& feats2) {
    uint32_t num_bool_32 = sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32);
    const VkBool32* v1 = reinterpret_cast<const VkBool32*>(&feats1);
    const VkBool32* v2 = reinterpret_cast<const VkBool32*>(&feats2.features);
    for (uint32_t val = 0; val < num_bool_32; ++val) {
        ASSERT_EQ(*v1, *v2);
        ++v1;
        ++v2;
    }
}

// Test vkGetPhysicalDeviceFeatures2KHR where nothing supports it.
TEST_F(LoaderInstPhysDevExts, PhysDevFeats2KHRNoSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.CheckCreate();

    auto GetPhysDevFeats2 = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFeatures2KHR"));
    ASSERT_EQ(GetPhysDevFeats2, nullptr);
}

// Test vkGetPhysicalDeviceFeatures2KHR where instance supports it, but nothing else.
TEST_F(LoaderInstPhysDevExts, PhysDevFeatsKHRNoICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    instance.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    auto GetPhysDevFeats2 = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFeatures2KHR"));
    ASSERT_EQ(GetPhysDevFeats2, nullptr);
}

// Test vkGetPhysicalDeviceFeatures2KHR where instance and ICD supports it, but device does not support it.
TEST_F(LoaderInstPhysDevExts, PhysDevFeats2KHRInstanceAndICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    FillInRandomFeatures(env.get_test_icd(0).physical_devices.back().features);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    instance.CheckCreate();

    auto GetPhysDevFeats2 = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFeatures2KHR"));
    ASSERT_NE(GetPhysDevFeats2, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    VkPhysicalDeviceFeatures feats{};
    instance->vkGetPhysicalDeviceFeatures(physical_device, &feats);
    VkPhysicalDeviceFeatures2 feats2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR};
    GetPhysDevFeats2(physical_device, &feats2);
    CompareFeatures(feats, feats2);
}

// Test vkGetPhysicalDeviceFeatures2 where instance supports, an ICD, and a device under that ICD
// also support, so everything should work and return properly.
TEST_F(LoaderInstPhysDevExts, PhysDevFeats2Simple) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).icd_api_version = VK_API_VERSION_1_1;
    env.get_test_icd(0).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    env.get_test_icd(0).physical_devices.back().extensions.push_back({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, 0});
    FillInRandomFeatures(env.get_test_icd(0).physical_devices.back().features);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(VK_API_VERSION_1_1);
    instance.CheckCreate();

    auto GetPhysDevFeats2 = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures2>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFeatures2"));
    ASSERT_NE(GetPhysDevFeats2, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    VkPhysicalDeviceFeatures feats{};
    instance->vkGetPhysicalDeviceFeatures(physical_device, &feats);
    VkPhysicalDeviceFeatures2 feats2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
    GetPhysDevFeats2(physical_device, &feats2);
    CompareFeatures(feats, feats2);
}

// Test vkGetPhysicalDeviceFeatures2 where instance supports it with some ICDs that both support
// and don't support it:
//    ICD 0 supports
//        Physical device 0 does not
//        Physical device 1 does
//        Physical device 2 does not
//    ICD 1 doesn't support
//        Physical device 3 does not
//    ICD 2 supports
//        Physical device 4 does not
//        Physical device 5 does not
//    ICD 3 supports
//        Physical device 6 does
TEST_F(LoaderInstPhysDevExts, PhysDevFeatsMixed) {
    FrameworkEnvironment env{};
    const uint32_t max_icd_count = 4;
    const uint32_t dev_counts[max_icd_count] = {3, 1, 2, 1};
    const uint32_t max_phys_devs = 7;

    for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
        auto& cur_icd = env.get_test_icd(icd);

        // ICD 1 should not have 1.1
        if (icd != 1) {
            cur_icd.icd_api_version = VK_API_VERSION_1_1;
            cur_icd.add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
        } else {
            cur_icd.icd_api_version = VK_API_VERSION_1_0;
        }

        uint32_t rand_vendor_id;
        uint32_t rand_driver_vers;
        FillInRandomICDInfo(rand_vendor_id, rand_driver_vers);

        for (uint32_t dev = 0; dev < dev_counts[icd]; ++dev) {
            uint32_t device_version = VK_API_VERSION_1_0;
            cur_icd.physical_devices.push_back({});
            auto& cur_dev = cur_icd.physical_devices.back();

            // 2nd device in ICD 0 and the one device in ICD 3 support the extension and 1.1
            if ((icd == 0 && dev == 1) || icd == 3) {
                cur_dev.extensions.push_back({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, 0});
                device_version = VK_API_VERSION_1_1;
            }

            // Still set physical device properties (so we can determine if device is correct API version)
            FillInRandomDeviceProps(cur_dev.properties, device_version, rand_vendor_id, rand_driver_vers);
            FillInRandomFeatures(cur_dev.features);
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(VK_API_VERSION_1_1);
    instance.CheckCreate();

    auto GetPhysDevFeats2 = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures2>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFeatures2"));
    ASSERT_NE(GetPhysDevFeats2, nullptr);

    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        VkPhysicalDeviceFeatures feats{};
        instance->vkGetPhysicalDeviceFeatures(physical_devices[dev], &feats);
        VkPhysicalDeviceFeatures2 feats2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
        GetPhysDevFeats2(physical_devices[dev], &feats2);
        CompareFeatures(feats, feats2);
    }
}

// Fill in random but valid data into the format properties struct for the current physical device
static void FillInRandomFormatProperties(std::vector<VkFormatProperties>& props) {
    props.resize(5);
    for (uint8_t form = 0; form < 5; ++form) {
        props[form].bufferFeatures = static_cast<VkFormatFeatureFlags>(rand());
        props[form].linearTilingFeatures = static_cast<VkFormatFeatureFlags>(rand());
        props[form].optimalTilingFeatures = static_cast<VkFormatFeatureFlags>(rand());
    }
}

// Test vkGetPhysicalDeviceFormatProperties2KHR where nothing supports it.
TEST_F(LoaderInstPhysDevExts, PhysDevFormatProps2KHRNoSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.CheckCreate();

    auto GetPhysDevFormatProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFormatProperties2KHR"));
    ASSERT_EQ(GetPhysDevFormatProps2, nullptr);
}

// Test vkGetPhysicalDeviceFormatProperties2KHR where instance supports it, but nothing else.
TEST_F(LoaderInstPhysDevExts, PhysDevFormatPropsKHRNoICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    instance.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    auto GetPhysDevFormatProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFormatProperties2KHR"));
    ASSERT_EQ(GetPhysDevFormatProps2, nullptr);
}

// Test vkGetPhysicalDeviceFormatProperties2KHR where instance and ICD supports it, but device does not support it.
TEST_F(LoaderInstPhysDevExts, PhysDevFormatProps2KHRInstanceAndICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    FillInRandomFormatProperties(env.get_test_icd(0).physical_devices.back().format_properties);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    instance.CheckCreate();

    auto GetPhysDevFormatProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFormatProperties2KHR"));
    ASSERT_NE(GetPhysDevFormatProps2, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    VkFormatProperties props{};
    instance->vkGetPhysicalDeviceFormatProperties(physical_device, VK_FORMAT_R4G4_UNORM_PACK8, &props);
    VkFormatProperties2 props2{VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2};
    GetPhysDevFormatProps2(physical_device, VK_FORMAT_R4G4_UNORM_PACK8, &props2);

    ASSERT_EQ(props.bufferFeatures, props2.formatProperties.bufferFeatures);
    ASSERT_EQ(props.linearTilingFeatures, props2.formatProperties.linearTilingFeatures);
    ASSERT_EQ(props.optimalTilingFeatures, props2.formatProperties.optimalTilingFeatures);
}

// Test vkGetPhysicalDeviceFormatProperties2 where instance supports, an ICD, and a device under that ICD
// also support, so everything should work and return properly.
TEST_F(LoaderInstPhysDevExts, PhysDevFormatProps2Simple) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).icd_api_version = VK_API_VERSION_1_1;
    env.get_test_icd(0).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    env.get_test_icd(0).physical_devices.back().extensions.push_back({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, 0});
    FillInRandomFormatProperties(env.get_test_icd(0).physical_devices.back().format_properties);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(VK_API_VERSION_1_1);
    instance.CheckCreate();

    auto GetPhysDevFormatProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties2>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFormatProperties2"));
    ASSERT_NE(GetPhysDevFormatProps2, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    VkFormatProperties props{};
    instance->vkGetPhysicalDeviceFormatProperties(physical_device, VK_FORMAT_R4G4_UNORM_PACK8, &props);
    VkFormatProperties2 props2{VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2};
    GetPhysDevFormatProps2(physical_device, VK_FORMAT_R4G4_UNORM_PACK8, &props2);

    ASSERT_EQ(props.bufferFeatures, props2.formatProperties.bufferFeatures);
    ASSERT_EQ(props.linearTilingFeatures, props2.formatProperties.linearTilingFeatures);
    ASSERT_EQ(props.optimalTilingFeatures, props2.formatProperties.optimalTilingFeatures);
}

// Test vkGetPhysicalDeviceFormatProperties2 where instance supports it with some ICDs that both support
// and don't support it:
//    ICD 0 supports
//        Physical device 0 does not
//        Physical device 1 does
//        Physical device 2 does not
//    ICD 1 doesn't support
//        Physical device 3 does not
//    ICD 2 supports
//        Physical device 4 does not
//        Physical device 5 does not
//    ICD 3 supports
//        Physical device 6 does
TEST_F(LoaderInstPhysDevExts, PhysDevFormatPropsMixed) {
    FrameworkEnvironment env{};
    const uint32_t max_icd_count = 4;
    const uint32_t dev_counts[max_icd_count] = {3, 1, 2, 1};
    const uint32_t max_phys_devs = 7;

    for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
        auto& cur_icd = env.get_test_icd(icd);

        // ICD 1 should not have 1.1
        if (icd != 1) {
            cur_icd.icd_api_version = VK_API_VERSION_1_1;
            cur_icd.add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
        } else {
            cur_icd.icd_api_version = VK_API_VERSION_1_0;
        }

        uint32_t rand_vendor_id;
        uint32_t rand_driver_vers;
        FillInRandomICDInfo(rand_vendor_id, rand_driver_vers);

        for (uint32_t dev = 0; dev < dev_counts[icd]; ++dev) {
            uint32_t device_version = VK_API_VERSION_1_0;
            cur_icd.physical_devices.push_back({});
            auto& cur_dev = cur_icd.physical_devices.back();

            // 2nd device in ICD 0 and the one device in ICD 3 support the extension and 1.1
            if ((icd == 0 && dev == 1) || icd == 3) {
                cur_dev.extensions.push_back({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, 0});
                device_version = VK_API_VERSION_1_1;
            }

            // Still set physical device properties (so we can determine if device is correct API version)
            FillInRandomDeviceProps(cur_dev.properties, device_version, rand_vendor_id, rand_driver_vers);
            FillInRandomFormatProperties(cur_dev.format_properties);
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(VK_API_VERSION_1_1);
    instance.CheckCreate();

    auto GetPhysDevFormatProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties2>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFormatProperties2"));
    ASSERT_NE(GetPhysDevFormatProps2, nullptr);

    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        VkFormat format = static_cast<VkFormat>((dev + 1) % 5);
        VkFormatProperties props{};
        instance->vkGetPhysicalDeviceFormatProperties(physical_devices[dev], format, &props);
        VkFormatProperties2 props2{VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2};
        GetPhysDevFormatProps2(physical_devices[dev], format, &props2);

        ASSERT_EQ(props.bufferFeatures, props2.formatProperties.bufferFeatures);
        ASSERT_EQ(props.linearTilingFeatures, props2.formatProperties.linearTilingFeatures);
        ASSERT_EQ(props.optimalTilingFeatures, props2.formatProperties.optimalTilingFeatures);
    }
}

// Fill in random but valid data into the image format data struct for the current physical device
static void FillInRandomImageFormatData(VkImageFormatProperties& props) {
    props.maxExtent = {static_cast<uint32_t>(rand() % 512), static_cast<uint32_t>(rand() % 512),
                       static_cast<uint32_t>(rand() % 512)};
    props.maxMipLevels = static_cast<uint32_t>(1 << (rand() % 16));
    props.maxArrayLayers = static_cast<uint32_t>(1 << (rand() % 16));
    props.sampleCounts = static_cast<VkSampleCountFlags>(1 << (rand() % 7));
    props.maxResourceSize = static_cast<uint64_t>(rand());
}

// Test vkGetPhysicalDeviceImageFormatProperties2KHR where nothing supports it.
TEST_F(LoaderInstPhysDevExts, PhysDevImageFormatProps2KHRNoSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.CheckCreate();

    auto GetPhysDevImageFormatProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceImageFormatProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceImageFormatProperties2KHR"));
    ASSERT_EQ(GetPhysDevImageFormatProps2, nullptr);
}

// Test vkGetPhysicalDeviceImageFormatProperties2KHR where instance supports it, but nothing else.
TEST_F(LoaderInstPhysDevExts, PhysDevImageFormatPropsKHRNoICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    instance.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    auto GetPhysDevImageFormatProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceImageFormatProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceImageFormatProperties2KHR"));
    ASSERT_EQ(GetPhysDevImageFormatProps2, nullptr);
}

// Test vkGetPhysicalDeviceImageFormatProperties2KHR where instance and ICD supports it, but device does not support it.
TEST_F(LoaderInstPhysDevExts, PhysDevImageFormatProps2KHRInstanceAndICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    FillInRandomImageFormatData(env.get_test_icd(0).physical_devices.back().image_format_properties);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    instance.CheckCreate();

    auto GetPhysDevImageFormatProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceImageFormatProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceImageFormatProperties2KHR"));
    ASSERT_NE(GetPhysDevImageFormatProps2, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    VkImageFormatProperties props{};
    ASSERT_EQ(VK_SUCCESS,
              instance->vkGetPhysicalDeviceImageFormatProperties(physical_device, VK_FORMAT_R4G4_UNORM_PACK8, VK_IMAGE_TYPE_2D,
                                                                 VK_IMAGE_TILING_OPTIMAL, 0, 0, &props));

    VkPhysicalDeviceImageFormatInfo2 info2{
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2,  // sType
        nullptr,                                                // pNext
        VK_FORMAT_R4G4_UNORM_PACK8,                             // format
        VK_IMAGE_TYPE_2D,                                       // type
        VK_IMAGE_TILING_OPTIMAL,                                // tiling
        0,                                                      // usage
        0,                                                      // flags
    };
    VkImageFormatProperties2 props2{VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2};
    ASSERT_EQ(VK_SUCCESS, GetPhysDevImageFormatProps2(physical_device, &info2, &props2));

    ASSERT_EQ(props.maxExtent.width, props2.imageFormatProperties.maxExtent.width);
    ASSERT_EQ(props.maxExtent.height, props2.imageFormatProperties.maxExtent.height);
    ASSERT_EQ(props.maxExtent.depth, props2.imageFormatProperties.maxExtent.depth);
    ASSERT_EQ(props.maxMipLevels, props2.imageFormatProperties.maxMipLevels);
    ASSERT_EQ(props.maxArrayLayers, props2.imageFormatProperties.maxArrayLayers);
    ASSERT_EQ(props.sampleCounts, props2.imageFormatProperties.sampleCounts);
    ASSERT_EQ(props.maxResourceSize, props2.imageFormatProperties.maxResourceSize);
}

// Test vkGetPhysicalDeviceImageFormatProperties2 where instance supports, an ICD, and a device under that ICD
// also support, so everything should work and return properly.
TEST_F(LoaderInstPhysDevExts, PhysDevImageFormatProps2Simple) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).icd_api_version = VK_API_VERSION_1_1;
    env.get_test_icd(0).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    env.get_test_icd(0).physical_devices.back().extensions.push_back({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, 0});
    FillInRandomImageFormatData(env.get_test_icd(0).physical_devices.back().image_format_properties);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(VK_API_VERSION_1_1);
    instance.CheckCreate();

    auto GetPhysDevImageFormatProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceImageFormatProperties2>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceImageFormatProperties2"));
    ASSERT_NE(GetPhysDevImageFormatProps2, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    VkImageFormatProperties props{};
    ASSERT_EQ(VK_SUCCESS,
              instance->vkGetPhysicalDeviceImageFormatProperties(physical_device, VK_FORMAT_R4G4_UNORM_PACK8, VK_IMAGE_TYPE_2D,
                                                                 VK_IMAGE_TILING_OPTIMAL, 0, 0, &props));

    VkPhysicalDeviceImageFormatInfo2 info2{
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2,  // sType
        nullptr,                                                // pNext
        VK_FORMAT_R4G4_UNORM_PACK8,                             // format
        VK_IMAGE_TYPE_2D,                                       // type
        VK_IMAGE_TILING_OPTIMAL,                                // tiling
        0,                                                      // usage
        0,                                                      // flags
    };

    VkImageFormatProperties2 props2{VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2};
    ASSERT_EQ(VK_SUCCESS, GetPhysDevImageFormatProps2(physical_device, &info2, &props2));

    ASSERT_EQ(props.maxExtent.width, props2.imageFormatProperties.maxExtent.width);
    ASSERT_EQ(props.maxExtent.height, props2.imageFormatProperties.maxExtent.height);
    ASSERT_EQ(props.maxExtent.depth, props2.imageFormatProperties.maxExtent.depth);
    ASSERT_EQ(props.maxMipLevels, props2.imageFormatProperties.maxMipLevels);
    ASSERT_EQ(props.maxArrayLayers, props2.imageFormatProperties.maxArrayLayers);
    ASSERT_EQ(props.sampleCounts, props2.imageFormatProperties.sampleCounts);
    ASSERT_EQ(props.maxResourceSize, props2.imageFormatProperties.maxResourceSize);
}

// Test vkGetPhysicalDeviceImageFormatProperties2 where instance supports it with some ICDs that both support
// and don't support it:
//    ICD 0 supports
//        Physical device 0 does not
//        Physical device 1 does
//        Physical device 2 does not
//    ICD 1 doesn't support
//        Physical device 3 does not
//    ICD 2 supports
//        Physical device 4 does not
//        Physical device 5 does not
//    ICD 3 supports
//        Physical device 6 does
TEST_F(LoaderInstPhysDevExts, PhysDevImageFormatPropsMixed) {
    FrameworkEnvironment env{};
    const uint32_t max_icd_count = 4;
    const uint32_t dev_counts[max_icd_count] = {3, 1, 2, 1};
    const uint32_t max_phys_devs = 7;

    for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
        auto& cur_icd = env.get_test_icd(icd);

        // ICD 1 should not have 1.1
        if (icd != 1) {
            cur_icd.icd_api_version = VK_API_VERSION_1_1;
            cur_icd.add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
        } else {
            cur_icd.icd_api_version = VK_API_VERSION_1_0;
        }

        uint32_t rand_vendor_id;
        uint32_t rand_driver_vers;
        FillInRandomICDInfo(rand_vendor_id, rand_driver_vers);

        for (uint32_t dev = 0; dev < dev_counts[icd]; ++dev) {
            uint32_t device_version = VK_API_VERSION_1_0;
            cur_icd.physical_devices.push_back({});
            auto& cur_dev = cur_icd.physical_devices.back();

            // 2nd device in ICD 0 and the one device in ICD 3 support the extension and 1.1
            if ((icd == 0 && dev == 1) || icd == 3) {
                cur_dev.extensions.push_back({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, 0});
                device_version = VK_API_VERSION_1_1;
            }

            // Still set physical device properties (so we can determine if device is correct API version)
            FillInRandomDeviceProps(cur_dev.properties, device_version, rand_vendor_id, rand_driver_vers);
            FillInRandomImageFormatData(cur_dev.image_format_properties);
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(VK_API_VERSION_1_1);
    instance.CheckCreate();

    auto GetPhysDevImageFormatProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceImageFormatProperties2>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceImageFormatProperties2"));
    ASSERT_NE(GetPhysDevImageFormatProps2, nullptr);

    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        VkImageFormatProperties props{};
        ASSERT_EQ(VK_SUCCESS,
                  instance->vkGetPhysicalDeviceImageFormatProperties(physical_devices[dev], VK_FORMAT_R4G4_UNORM_PACK8,
                                                                     VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, 0, 0, &props));

        VkPhysicalDeviceImageFormatInfo2 info2{
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2,  // sType
            nullptr,                                                // pNext
            VK_FORMAT_R4G4_UNORM_PACK8,                             // format
            VK_IMAGE_TYPE_2D,                                       // type
            VK_IMAGE_TILING_OPTIMAL,                                // tiling
            0,                                                      // usage
            0,                                                      // flags
        };
        VkImageFormatProperties2 props2{VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2};
        ASSERT_EQ(VK_SUCCESS, GetPhysDevImageFormatProps2(physical_devices[dev], &info2, &props2));

        ASSERT_EQ(props.maxExtent.width, props2.imageFormatProperties.maxExtent.width);
        ASSERT_EQ(props.maxExtent.height, props2.imageFormatProperties.maxExtent.height);
        ASSERT_EQ(props.maxExtent.depth, props2.imageFormatProperties.maxExtent.depth);
        ASSERT_EQ(props.maxMipLevels, props2.imageFormatProperties.maxMipLevels);
        ASSERT_EQ(props.maxArrayLayers, props2.imageFormatProperties.maxArrayLayers);
        ASSERT_EQ(props.sampleCounts, props2.imageFormatProperties.sampleCounts);
        ASSERT_EQ(props.maxResourceSize, props2.imageFormatProperties.maxResourceSize);
    }
}

// Test vkGetPhysicalDeviceMemoryProperties2KHR where nothing supports it.
TEST_F(LoaderInstPhysDevExts, PhysDevMemoryProps2KHRNoSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.CheckCreate();

    auto GetPhysDevMemoryProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMemoryProperties2KHR"));
    ASSERT_EQ(GetPhysDevMemoryProps2, nullptr);
}

// Test vkGetPhysicalDeviceMemoryProperties2KHR where instance supports it, but nothing else.
TEST_F(LoaderInstPhysDevExts, PhysDevMemoryPropsKHRNoICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    instance.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    auto GetPhysDevMemoryProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMemoryProperties2KHR"));
    ASSERT_EQ(GetPhysDevMemoryProps2, nullptr);
}

// Fill in random but valid data into the memory data struct for the current physical device
static void FillInRandomMemoryData(VkPhysicalDeviceMemoryProperties& props) {
    props.memoryTypeCount = (rand() % 7) + 1;
    props.memoryHeapCount = (rand() % 7) + 1;
    for (uint32_t i = 0; i < props.memoryHeapCount; ++i) {
        props.memoryHeaps[i].size = (rand() % 728) + (rand() % 728) + 1;
        props.memoryHeaps[i].flags = (rand() % 2) + 1;
    }
    for (uint32_t i = 0; i < props.memoryTypeCount; ++i) {
        props.memoryTypes[i].propertyFlags = static_cast<VkMemoryPropertyFlags>((rand() % 2) + 1);
        props.memoryTypes[i].heapIndex = rand() % props.memoryHeapCount;
    }
}

// Compare the memory structs
static void CompareMemoryData(const VkPhysicalDeviceMemoryProperties& props1, const VkPhysicalDeviceMemoryProperties2& props2) {
    ASSERT_EQ(props1.memoryTypeCount, props2.memoryProperties.memoryTypeCount);
    ASSERT_EQ(props1.memoryHeapCount, props2.memoryProperties.memoryHeapCount);
    for (uint32_t i = 0; i < props1.memoryHeapCount; ++i) {
        ASSERT_EQ(props1.memoryHeaps[i].size, props2.memoryProperties.memoryHeaps[i].size);
        ASSERT_EQ(props1.memoryHeaps[i].flags, props2.memoryProperties.memoryHeaps[i].flags);
    }
    for (uint32_t i = 0; i < props1.memoryTypeCount; ++i) {
        ASSERT_EQ(props1.memoryTypes[i].propertyFlags, props2.memoryProperties.memoryTypes[i].propertyFlags);
        ASSERT_EQ(props1.memoryTypes[i].heapIndex, props2.memoryProperties.memoryTypes[i].heapIndex);
    }
}

// Test vkGetPhysicalDeviceMemoryProperties2KHR where instance and ICD supports it, but device does not support it.
TEST_F(LoaderInstPhysDevExts, PhysDevMemoryProps2KHRInstanceAndICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    FillInRandomMemoryData(env.get_test_icd(0).physical_devices.back().memory_properties);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    instance.CheckCreate();

    auto GetPhysDevMemoryProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMemoryProperties2KHR"));
    ASSERT_NE(GetPhysDevMemoryProps2, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    VkPhysicalDeviceMemoryProperties props{};
    instance->vkGetPhysicalDeviceMemoryProperties(physical_device, &props);

    VkPhysicalDeviceMemoryProperties2 props2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2};
    GetPhysDevMemoryProps2(physical_device, &props2);
    CompareMemoryData(props, props2);
}

// Test vkGetPhysicalDeviceMemoryProperties2 where instance supports, an ICD, and a device under that ICD
// also support, so everything should work and return properly.
TEST_F(LoaderInstPhysDevExts, PhysDevMemoryProps2Simple) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).icd_api_version = VK_API_VERSION_1_1;
    env.get_test_icd(0).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    env.get_test_icd(0).physical_devices.back().extensions.push_back({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, 0});
    FillInRandomMemoryData(env.get_test_icd(0).physical_devices.back().memory_properties);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(VK_API_VERSION_1_1);
    instance.CheckCreate();

    auto GetPhysDevMemoryProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties2>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMemoryProperties2"));
    ASSERT_NE(GetPhysDevMemoryProps2, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    VkPhysicalDeviceMemoryProperties props{};
    instance->vkGetPhysicalDeviceMemoryProperties(physical_device, &props);

    VkPhysicalDeviceMemoryProperties2 props2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2};
    GetPhysDevMemoryProps2(physical_device, &props2);
    CompareMemoryData(props, props2);
}

// Test vkGetPhysicalDeviceMemoryProperties2 where instance supports it with some ICDs that both support
// and don't support it:
//    ICD 0 supports
//        Physical device 0 does not
//        Physical device 1 does
//        Physical device 2 does not
//    ICD 1 doesn't support
//        Physical device 3 does not
//    ICD 2 supports
//        Physical device 4 does not
//        Physical device 5 does not
//    ICD 3 supports
//        Physical device 6 does
TEST_F(LoaderInstPhysDevExts, PhysDevMemoryPropsMixed) {
    FrameworkEnvironment env{};
    const uint32_t max_icd_count = 4;
    const uint32_t dev_counts[max_icd_count] = {3, 1, 2, 1};
    const uint32_t max_phys_devs = 7;

    for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
        auto& cur_icd = env.get_test_icd(icd);

        // ICD 1 should not have 1.1
        if (icd != 1) {
            cur_icd.icd_api_version = VK_API_VERSION_1_1;
            cur_icd.add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
        } else {
            cur_icd.icd_api_version = VK_API_VERSION_1_0;
        }

        uint32_t rand_vendor_id;
        uint32_t rand_driver_vers;
        FillInRandomICDInfo(rand_vendor_id, rand_driver_vers);

        for (uint32_t dev = 0; dev < dev_counts[icd]; ++dev) {
            uint32_t device_version = VK_API_VERSION_1_0;
            cur_icd.physical_devices.push_back({});
            auto& cur_dev = cur_icd.physical_devices.back();

            // 2nd device in ICD 0 and the one device in ICD 3 support the extension and 1.1
            if ((icd == 0 && dev == 1) || icd == 3) {
                cur_dev.extensions.push_back({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, 0});
                device_version = VK_API_VERSION_1_1;
            }

            // Still set physical device properties (so we can determine if device is correct API version)
            FillInRandomDeviceProps(cur_dev.properties, device_version, rand_vendor_id, rand_driver_vers);
            FillInRandomMemoryData(cur_dev.memory_properties);
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(VK_API_VERSION_1_1);
    instance.CheckCreate();

    auto GetPhysDevMemoryProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties2>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMemoryProperties2"));
    ASSERT_NE(GetPhysDevMemoryProps2, nullptr);

    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        VkPhysicalDeviceMemoryProperties props{};
        instance->vkGetPhysicalDeviceMemoryProperties(physical_devices[dev], &props);

        VkPhysicalDeviceMemoryProperties2 props2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2};
        GetPhysDevMemoryProps2(physical_devices[dev], &props2);
        CompareMemoryData(props, props2);
    }
}

// Test vkGetPhysicalDeviceQueueFamilyProperties2KHR where nothing supports it.
TEST_F(LoaderInstPhysDevExts, PhysDevQueueFamilyProps2KHRNoSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.CheckCreate();

    auto GetPhysDevQueueFamilyProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties2KHR"));
    ASSERT_EQ(GetPhysDevQueueFamilyProps2, nullptr);
}

// Test vkGetPhysicalDeviceQueueFamilyProperties2KHR where instance supports it, but nothing else.
TEST_F(LoaderInstPhysDevExts, PhysDevQueueFamilyPropsKHRNoICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    instance.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    auto GetPhysDevQueueFamilyProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties2KHR"));
    ASSERT_EQ(GetPhysDevQueueFamilyProps2, nullptr);
}

// Fill in random but valid data into the queue family data struct for the current physical device
static uint32_t FillInRandomQueueFamilyData(std::vector<MockQueueFamilyProperties>& props) {
    props.resize((rand() % 4) + 1);
    for (uint32_t i = 0; i < props.size(); ++i) {
        props[i].properties.queueFlags = (rand() % 30) + 1;
        props[i].properties.queueCount = (rand() % 7) + 1;
        props[i].properties.timestampValidBits = (rand() % 30) + 7;
        props[i].properties.minImageTransferGranularity.width = (rand() % 30) + 1;
        props[i].properties.minImageTransferGranularity.height = (rand() % 30) + 1;
        props[i].properties.minImageTransferGranularity.depth = (rand() % 30) + 1;
        props[i].support_present = (rand() % 2);
    }
    return props.size();
}

// Compare the queue family structs
static void CompareQueueFamilyData(const std::vector<VkQueueFamilyProperties>& props1,
                                   const std::vector<VkQueueFamilyProperties2>& props2) {
    ASSERT_EQ(props1.size(), props2.size());
    for (uint32_t i = 0; i < props1.size(); ++i) {
        ASSERT_EQ(props1[i].queueFlags, props2[i].queueFamilyProperties.queueFlags);
        ASSERT_EQ(props1[i].queueCount, props2[i].queueFamilyProperties.queueCount);
        ASSERT_EQ(props1[i].timestampValidBits, props2[i].queueFamilyProperties.timestampValidBits);
        ASSERT_EQ(props1[i].minImageTransferGranularity.width, props2[i].queueFamilyProperties.minImageTransferGranularity.width);
        ASSERT_EQ(props1[i].minImageTransferGranularity.height, props2[i].queueFamilyProperties.minImageTransferGranularity.height);
        ASSERT_EQ(props1[i].minImageTransferGranularity.depth, props2[i].queueFamilyProperties.minImageTransferGranularity.depth);
    }
}

// Test vkGetPhysicalDeviceQueueFamilyProperties2KHR where instance and ICD supports it, but device does not support it.
TEST_F(LoaderInstPhysDevExts, PhysDevQueueFamilyProps2KHRInstanceAndICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    uint32_t num_fam = FillInRandomQueueFamilyData(env.get_test_icd(0).physical_devices.back().queue_family_properties);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    instance.CheckCreate();

    auto GetPhysDevQueueFamilyProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties2KHR"));
    ASSERT_NE(GetPhysDevQueueFamilyProps2, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    uint32_t ret_fam_1 = 0;
    std::vector<VkQueueFamilyProperties> props{};
    instance->vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &ret_fam_1, nullptr);
    ASSERT_EQ(num_fam, ret_fam_1);
    props.resize(ret_fam_1);

    instance->vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &ret_fam_1, props.data());

    std::vector<VkQueueFamilyProperties2> props2{};
    uint32_t ret_fam_2 = 0;
    GetPhysDevQueueFamilyProps2(physical_device, &ret_fam_2, nullptr);
    ASSERT_EQ(ret_fam_1, ret_fam_2);
    props2.resize(ret_fam_2, VkQueueFamilyProperties2{VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2});
    GetPhysDevQueueFamilyProps2(physical_device, &ret_fam_2, props2.data());
    CompareQueueFamilyData(props, props2);
}

// Test vkGetPhysicalDeviceQueueFamilyProperties2 where instance supports, an ICD, and a device under that ICD
// also support, so everything should work and return properly.
TEST_F(LoaderInstPhysDevExts, PhysDevQueueFamilyProps2Simple) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).icd_api_version = VK_API_VERSION_1_1;
    env.get_test_icd(0).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    env.get_test_icd(0).physical_devices.back().extensions.push_back({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, 0});
    uint32_t num_fam = FillInRandomQueueFamilyData(env.get_test_icd(0).physical_devices.back().queue_family_properties);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(VK_API_VERSION_1_1);
    instance.CheckCreate();

    auto GetPhysDevQueueFamilyProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties2>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties2"));
    ASSERT_NE(GetPhysDevQueueFamilyProps2, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    uint32_t ret_fam_1 = 0;
    std::vector<VkQueueFamilyProperties> props{};
    instance->vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &ret_fam_1, nullptr);
    ASSERT_EQ(num_fam, ret_fam_1);
    props.resize(ret_fam_1);

    instance->vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &ret_fam_1, props.data());

    std::vector<VkQueueFamilyProperties2> props2{};
    uint32_t ret_fam_2 = 0;
    GetPhysDevQueueFamilyProps2(physical_device, &ret_fam_2, nullptr);
    ASSERT_EQ(ret_fam_1, ret_fam_2);
    props2.resize(ret_fam_2, VkQueueFamilyProperties2{VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2});
    GetPhysDevQueueFamilyProps2(physical_device, &ret_fam_2, props2.data());
    CompareQueueFamilyData(props, props2);
}

// Test vkGetPhysicalDeviceQueueFamilyProperties2 where instance supports it with some ICDs that both support
// and don't support it:
//    ICD 0 supports
//        Physical device 0 does not
//        Physical device 1 does
//        Physical device 2 does not
//    ICD 1 doesn't support
//        Physical device 3 does not
//    ICD 2 supports
//        Physical device 4 does not
//        Physical device 5 does not
//    ICD 3 supports
//        Physical device 6 does
TEST_F(LoaderInstPhysDevExts, PhysDevQueueFamilyPropsMixed) {
    FrameworkEnvironment env{};
    const uint32_t max_icd_count = 4;
    const uint32_t dev_counts[max_icd_count] = {3, 1, 2, 1};
    const uint32_t max_phys_devs = 7;

    for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
        auto& cur_icd = env.get_test_icd(icd);

        // ICD 1 should not have 1.1
        if (icd != 1) {
            cur_icd.icd_api_version = VK_API_VERSION_1_1;
            cur_icd.add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
        } else {
            cur_icd.icd_api_version = VK_API_VERSION_1_0;
        }

        uint32_t rand_vendor_id;
        uint32_t rand_driver_vers;
        FillInRandomICDInfo(rand_vendor_id, rand_driver_vers);

        for (uint32_t dev = 0; dev < dev_counts[icd]; ++dev) {
            uint32_t device_version = VK_API_VERSION_1_0;
            cur_icd.physical_devices.push_back({});
            auto& cur_dev = cur_icd.physical_devices.back();

            // 2nd device in ICD 0 and the one device in ICD 3 support the extension and 1.1
            if ((icd == 0 && dev == 1) || icd == 3) {
                cur_dev.extensions.push_back({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, 0});
                device_version = VK_API_VERSION_1_1;
            }

            // Still set physical device properties (so we can determine if device is correct API version)
            FillInRandomDeviceProps(cur_dev.properties, device_version, rand_vendor_id, rand_driver_vers);
            FillInRandomQueueFamilyData(cur_dev.queue_family_properties);
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(VK_API_VERSION_1_1);
    instance.CheckCreate();

    auto GetPhysDevQueueFamilyProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties2>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties2"));
    ASSERT_NE(GetPhysDevQueueFamilyProps2, nullptr);

    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        uint32_t ret_fam_1 = 0;
        std::vector<VkQueueFamilyProperties> props{};
        instance->vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[dev], &ret_fam_1, nullptr);
        props.resize(ret_fam_1);
        instance->vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[dev], &ret_fam_1, props.data());

        std::vector<VkQueueFamilyProperties2> props2{};
        uint32_t ret_fam_2 = 0;
        GetPhysDevQueueFamilyProps2(physical_devices[dev], &ret_fam_2, nullptr);
        ASSERT_EQ(ret_fam_1, ret_fam_2);
        props2.resize(ret_fam_2, VkQueueFamilyProperties2{VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2});
        GetPhysDevQueueFamilyProps2(physical_devices[dev], &ret_fam_2, props2.data());
        CompareQueueFamilyData(props, props2);
    }
}

// Test vkGetPhysicalDeviceSparseImageFormatProperties2KHR where nothing supports it.
TEST_F(LoaderInstPhysDevExts, PhysDevSparseImageFormatProps2KHRNoSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.CheckCreate();

    auto GetPhysDevSparseImageFormatProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceSparseImageFormatProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSparseImageFormatProperties2KHR"));
    ASSERT_EQ(GetPhysDevSparseImageFormatProps2, nullptr);
}

// Test vkGetPhysicalDeviceSparseImageFormatProperties2KHR where instance supports it, but nothing else.
TEST_F(LoaderInstPhysDevExts, PhysDevSparseImageFormatPropsKHRNoICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    instance.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    auto GetPhysDevSparseImageFormatProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceSparseImageFormatProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSparseImageFormatProperties2KHR"));
    ASSERT_EQ(GetPhysDevSparseImageFormatProps2, nullptr);
}

// Fill in random but valid data into the sparse image format data struct for the current physical device
static void FillInRandomSparseImageFormatData(std::vector<VkSparseImageFormatProperties>& props) {
    props.resize((rand() % 4) + 1);
    for (uint32_t i = 0; i < props.size(); ++i) {
        props[i].aspectMask = static_cast<VkImageAspectFlags>((rand() % 0x7FE) + 1);
        props[i].imageGranularity = {static_cast<uint32_t>(rand() % 512), static_cast<uint32_t>(rand() % 512),
                                     static_cast<uint32_t>(rand() % 512)};
        props[i].flags = static_cast<VkSparseImageFormatFlags>((rand() % 6) + 1);
    }
}

// Compare the sparse image format structs
static void CompareSparseImageFormatData(const std::vector<VkSparseImageFormatProperties>& props1,
                                         const std::vector<VkSparseImageFormatProperties2>& props2) {
    ASSERT_EQ(props1.size(), props2.size());
    for (uint32_t i = 0; i < props1.size(); ++i) {
        ASSERT_EQ(props1[i].aspectMask, props2[i].properties.aspectMask);
        ASSERT_EQ(props1[i].imageGranularity.width, props2[i].properties.imageGranularity.width);
        ASSERT_EQ(props1[i].imageGranularity.height, props2[i].properties.imageGranularity.height);
        ASSERT_EQ(props1[i].imageGranularity.depth, props2[i].properties.imageGranularity.depth);
        ASSERT_EQ(props1[i].flags, props2[i].properties.flags);
    }
}

// Test vkGetPhysicalDeviceSparseImageFormatProperties2KHR where instance and ICD supports it, but device does not support it.
TEST_F(LoaderInstPhysDevExts, PhysDevSparseImageFormatProps2KHRInstanceAndICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    FillInRandomSparseImageFormatData(env.get_test_icd(0).physical_devices.back().sparse_image_format_properties);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    instance.CheckCreate();

    auto GetPhysDevSparseImageFormatProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceSparseImageFormatProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSparseImageFormatProperties2KHR"));
    ASSERT_NE(GetPhysDevSparseImageFormatProps2, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    std::vector<VkSparseImageFormatProperties> props{};
    uint32_t sparse_count_1 = 0;
    instance->vkGetPhysicalDeviceSparseImageFormatProperties(physical_device, VK_FORMAT_R4G4_UNORM_PACK8, VK_IMAGE_TYPE_2D,
                                                             VK_SAMPLE_COUNT_4_BIT, VK_IMAGE_USAGE_STORAGE_BIT,
                                                             VK_IMAGE_TILING_OPTIMAL, &sparse_count_1, nullptr);
    ASSERT_NE(sparse_count_1, 0);
    props.resize(sparse_count_1);
    instance->vkGetPhysicalDeviceSparseImageFormatProperties(physical_device, VK_FORMAT_R4G4_UNORM_PACK8, VK_IMAGE_TYPE_2D,
                                                             VK_SAMPLE_COUNT_4_BIT, VK_IMAGE_USAGE_STORAGE_BIT,
                                                             VK_IMAGE_TILING_OPTIMAL, &sparse_count_1, props.data());
    ASSERT_NE(sparse_count_1, 0);

    VkPhysicalDeviceSparseImageFormatInfo2 info2{
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SPARSE_IMAGE_FORMAT_INFO_2,  // sType
        nullptr,                                                       // pNext
        VK_FORMAT_R4G4_UNORM_PACK8,                                    // format
        VK_IMAGE_TYPE_2D,                                              // type
        VK_SAMPLE_COUNT_4_BIT,                                         // samples
        VK_IMAGE_USAGE_STORAGE_BIT,                                    // usage
        VK_IMAGE_TILING_OPTIMAL,                                       // tiling
    };
    std::vector<VkSparseImageFormatProperties2> props2{};
    uint32_t sparse_count_2 = 0;
    GetPhysDevSparseImageFormatProps2(physical_device, &info2, &sparse_count_2, nullptr);
    ASSERT_EQ(sparse_count_1, sparse_count_2);
    props2.resize(sparse_count_2, VkSparseImageFormatProperties2{VK_STRUCTURE_TYPE_SPARSE_IMAGE_FORMAT_PROPERTIES_2});
    GetPhysDevSparseImageFormatProps2(physical_device, &info2, &sparse_count_2, props2.data());
    ASSERT_EQ(sparse_count_1, sparse_count_2);
    CompareSparseImageFormatData(props, props2);
}

// Test vkGetPhysicalDeviceSparseImageFormatProperties2 where instance supports, an ICD, and a device under that ICD
// also support, so everything should work and return properly.
TEST_F(LoaderInstPhysDevExts, PhysDevSparseImageFormatProps2Simple) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).icd_api_version = VK_API_VERSION_1_1;
    env.get_test_icd(0).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    env.get_test_icd(0).physical_devices.back().extensions.push_back({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, 0});
    FillInRandomSparseImageFormatData(env.get_test_icd(0).physical_devices.back().sparse_image_format_properties);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(VK_API_VERSION_1_1);
    instance.CheckCreate();

    auto GetPhysDevSparseImageFormatProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceSparseImageFormatProperties2>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSparseImageFormatProperties2"));
    ASSERT_NE(GetPhysDevSparseImageFormatProps2, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    std::vector<VkSparseImageFormatProperties> props{};
    uint32_t sparse_count_1 = 0;
    instance->vkGetPhysicalDeviceSparseImageFormatProperties(physical_device, VK_FORMAT_R4G4_UNORM_PACK8, VK_IMAGE_TYPE_2D,
                                                             VK_SAMPLE_COUNT_4_BIT, VK_IMAGE_USAGE_STORAGE_BIT,
                                                             VK_IMAGE_TILING_OPTIMAL, &sparse_count_1, nullptr);
    ASSERT_NE(sparse_count_1, 0);
    props.resize(sparse_count_1);
    instance->vkGetPhysicalDeviceSparseImageFormatProperties(physical_device, VK_FORMAT_R4G4_UNORM_PACK8, VK_IMAGE_TYPE_2D,
                                                             VK_SAMPLE_COUNT_4_BIT, VK_IMAGE_USAGE_STORAGE_BIT,
                                                             VK_IMAGE_TILING_OPTIMAL, &sparse_count_1, props.data());
    ASSERT_NE(sparse_count_1, 0);

    VkPhysicalDeviceSparseImageFormatInfo2 info2{
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SPARSE_IMAGE_FORMAT_INFO_2,  // sType
        nullptr,                                                       // pNext
        VK_FORMAT_R4G4_UNORM_PACK8,                                    // format
        VK_IMAGE_TYPE_2D,                                              // type
        VK_SAMPLE_COUNT_4_BIT,                                         // samples
        VK_IMAGE_USAGE_STORAGE_BIT,                                    // usage
        VK_IMAGE_TILING_OPTIMAL,                                       // tiling
    };
    std::vector<VkSparseImageFormatProperties2> props2{};
    uint32_t sparse_count_2 = 0;
    GetPhysDevSparseImageFormatProps2(physical_device, &info2, &sparse_count_2, nullptr);
    ASSERT_EQ(sparse_count_1, sparse_count_2);
    props2.resize(sparse_count_2, VkSparseImageFormatProperties2{VK_STRUCTURE_TYPE_SPARSE_IMAGE_FORMAT_PROPERTIES_2});
    GetPhysDevSparseImageFormatProps2(physical_device, &info2, &sparse_count_2, props2.data());
    ASSERT_EQ(sparse_count_1, sparse_count_2);
    CompareSparseImageFormatData(props, props2);
}

// Test vkGetPhysicalDeviceSparseImageFormatProperties2 where instance supports it with some ICDs that both support
// and don't support it:
//    ICD 0 supports
//        Physical device 0 does not
//        Physical device 1 does
//        Physical device 2 does not
//    ICD 1 doesn't support
//        Physical device 3 does not
//    ICD 2 supports
//        Physical device 4 does not
//        Physical device 5 does not
//    ICD 3 supports
//        Physical device 6 does
TEST_F(LoaderInstPhysDevExts, PhysDevSparseImageFormatPropsMixed) {
    FrameworkEnvironment env{};
    const uint32_t max_icd_count = 4;
    const uint32_t dev_counts[max_icd_count] = {3, 1, 2, 1};
    const uint32_t max_phys_devs = 7;

    for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
        auto& cur_icd = env.get_test_icd(icd);

        // ICD 1 should not have 1.1
        if (icd != 1) {
            cur_icd.icd_api_version = VK_API_VERSION_1_1;
            cur_icd.add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
        } else {
            cur_icd.icd_api_version = VK_API_VERSION_1_0;
        }

        uint32_t rand_vendor_id;
        uint32_t rand_driver_vers;
        FillInRandomICDInfo(rand_vendor_id, rand_driver_vers);

        for (uint32_t dev = 0; dev < dev_counts[icd]; ++dev) {
            uint32_t device_version = VK_API_VERSION_1_0;
            cur_icd.physical_devices.push_back({});
            auto& cur_dev = cur_icd.physical_devices.back();

            // 2nd device in ICD 0 and the one device in ICD 3 support the extension and 1.1
            if ((icd == 0 && dev == 1) || icd == 3) {
                cur_dev.extensions.push_back({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, 0});
                device_version = VK_API_VERSION_1_1;
            }

            // Still set physical device properties (so we can determine if device is correct API version)
            FillInRandomDeviceProps(cur_dev.properties, device_version, rand_vendor_id, rand_driver_vers);
            FillInRandomSparseImageFormatData(cur_dev.sparse_image_format_properties);
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(VK_API_VERSION_1_1);
    instance.CheckCreate();

    auto GetPhysDevSparseImageFormatProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceSparseImageFormatProperties2>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSparseImageFormatProperties2"));
    ASSERT_NE(GetPhysDevSparseImageFormatProps2, nullptr);

    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        std::vector<VkSparseImageFormatProperties> props{};
        uint32_t sparse_count_1 = 0;
        instance->vkGetPhysicalDeviceSparseImageFormatProperties(
            physical_devices[dev], VK_FORMAT_R4G4_UNORM_PACK8, VK_IMAGE_TYPE_2D, VK_SAMPLE_COUNT_4_BIT, VK_IMAGE_USAGE_STORAGE_BIT,
            VK_IMAGE_TILING_OPTIMAL, &sparse_count_1, nullptr);
        ASSERT_NE(sparse_count_1, 0);
        props.resize(sparse_count_1);
        instance->vkGetPhysicalDeviceSparseImageFormatProperties(
            physical_devices[dev], VK_FORMAT_R4G4_UNORM_PACK8, VK_IMAGE_TYPE_2D, VK_SAMPLE_COUNT_4_BIT, VK_IMAGE_USAGE_STORAGE_BIT,
            VK_IMAGE_TILING_OPTIMAL, &sparse_count_1, props.data());
        ASSERT_NE(sparse_count_1, 0);

        VkPhysicalDeviceSparseImageFormatInfo2 info2{
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SPARSE_IMAGE_FORMAT_INFO_2,  // sType
            nullptr,                                                       // pNext
            VK_FORMAT_R4G4_UNORM_PACK8,                                    // format
            VK_IMAGE_TYPE_2D,                                              // type
            VK_SAMPLE_COUNT_4_BIT,                                         // samples
            VK_IMAGE_USAGE_STORAGE_BIT,                                    // usage
            VK_IMAGE_TILING_OPTIMAL,                                       // tiling
        };
        std::vector<VkSparseImageFormatProperties2> props2{};
        uint32_t sparse_count_2 = 0;
        GetPhysDevSparseImageFormatProps2(physical_devices[dev], &info2, &sparse_count_2, nullptr);
        ASSERT_EQ(sparse_count_1, sparse_count_2);
        props2.resize(sparse_count_2, VkSparseImageFormatProperties2{VK_STRUCTURE_TYPE_SPARSE_IMAGE_FORMAT_PROPERTIES_2});
        GetPhysDevSparseImageFormatProps2(physical_devices[dev], &info2, &sparse_count_2, props2.data());
        ASSERT_EQ(sparse_count_1, sparse_count_2);
        CompareSparseImageFormatData(props, props2);
    }
}

//
// VK_KHR_external_memory_capabilities
//

// Test vkGetPhysicalDeviceExternalBufferPropertiesKHR where nothing supports it.
TEST_F(LoaderInstPhysDevExts, PhysDevExtBufPropsKHRNoSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.CheckCreate();

    auto GetPhysicalDeviceExternalBufferProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalBufferPropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalBufferPropertiesKHR"));
    ASSERT_EQ(GetPhysicalDeviceExternalBufferProperties, nullptr);
}

// Test vkGetPhysicalDeviceExternalBufferPropertiesKHR where instance supports it, but nothing else.
TEST_F(LoaderInstPhysDevExts, PhysDevExtBufPropsKHRNoICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME);
    instance.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    auto GetPhysicalDeviceExternalBufferProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalBufferPropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalBufferPropertiesKHR"));
    ASSERT_EQ(GetPhysicalDeviceExternalBufferProperties, nullptr);
}

// Fill in random but valid data into the external memorydata struct for the current physical device
static void FillInRandomExtMemoryData(VkExternalMemoryProperties& props) {
    props.externalMemoryFeatures = static_cast<VkExternalMemoryFeatureFlags>((rand() % 6) + 1);
    props.exportFromImportedHandleTypes = static_cast<VkExternalMemoryHandleTypeFlags>((rand() % 0x1FFE) + 1);
    props.compatibleHandleTypes = static_cast<VkExternalMemoryHandleTypeFlags>((rand() % 0x1FFE) + 1);
}

// Compare the external memory data structs
static void CompareExtMemoryData(const VkExternalMemoryProperties& props1, const VkExternalMemoryProperties& props2,
                                 bool supported = true) {
    if (supported) {
        ASSERT_EQ(props1.externalMemoryFeatures, props2.externalMemoryFeatures);
        ASSERT_EQ(props1.exportFromImportedHandleTypes, props2.exportFromImportedHandleTypes);
        ASSERT_EQ(props1.compatibleHandleTypes, props2.compatibleHandleTypes);
    } else {
        ASSERT_EQ(0, props2.externalMemoryFeatures);
        ASSERT_EQ(0, props2.exportFromImportedHandleTypes);
        ASSERT_EQ(0, props2.compatibleHandleTypes);
    }
}

// Test vkGetPhysicalDeviceExternalBufferPropertiesKHR where instance and ICD supports it, but device does not support it.
TEST_F(LoaderInstPhysDevExts, PhysDevExtBufProps2KHRInstanceAndICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).add_instance_extension({VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    FillInRandomExtMemoryData(env.get_test_icd(0).physical_devices.back().external_memory_properties);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME);
    instance.CheckCreate();

    auto GetPhysicalDeviceExternalBufferProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalBufferPropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalBufferPropertiesKHR"));
    ASSERT_NE(GetPhysicalDeviceExternalBufferProperties, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    VkPhysicalDeviceExternalBufferInfoKHR info{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_BUFFER_INFO_KHR};
    VkExternalBufferPropertiesKHR props{VK_STRUCTURE_TYPE_EXTERNAL_BUFFER_PROPERTIES_KHR};
    GetPhysicalDeviceExternalBufferProperties(physical_device, &info, &props);
    CompareExtMemoryData(env.get_test_icd(0).physical_devices.back().external_memory_properties, props.externalMemoryProperties);
}

// Test vkGetPhysicalDeviceExternalBufferProperties where instance supports, an ICD, and a device under that ICD
// also support, so everything should work and return properly.
TEST_F(LoaderInstPhysDevExts, PhysDevExtBufProps2Simple) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).icd_api_version = VK_API_VERSION_1_1;
    env.get_test_icd(0).add_instance_extension({VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    env.get_test_icd(0).physical_devices.back().extensions.push_back({VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME, 0});
    FillInRandomExtMemoryData(env.get_test_icd(0).physical_devices.back().external_memory_properties);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(VK_API_VERSION_1_1);
    instance.CheckCreate();

    auto GetPhysicalDeviceExternalBufferProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalBufferProperties>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalBufferProperties"));
    ASSERT_NE(GetPhysicalDeviceExternalBufferProperties, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    VkPhysicalDeviceExternalBufferInfo info{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_BUFFER_INFO};
    VkExternalBufferProperties props{VK_STRUCTURE_TYPE_EXTERNAL_BUFFER_PROPERTIES};
    GetPhysicalDeviceExternalBufferProperties(physical_device, &info, &props);
    CompareExtMemoryData(env.get_test_icd(0).physical_devices.back().external_memory_properties, props.externalMemoryProperties);
}

// Test vkGetPhysicalDeviceExternalBufferProperties where instance supports it with some ICDs that both support
// and don't support it:
//    ICD 0 supports
//        Physical device 0 does not
//        Physical device 1 does
//        Physical device 2 does not
//    ICD 1 doesn't support
//        Physical device 3 does not
//    ICD 2 supports
//        Physical device 4 does not
//        Physical device 5 does not
//    ICD 3 supports
//        Physical device 6 does
TEST_F(LoaderInstPhysDevExts, PhysDevExtBufPropsMixed) {
    FrameworkEnvironment env{};
    const uint32_t max_icd_count = 4;
    const uint32_t dev_counts[max_icd_count] = {3, 1, 2, 1};
    const uint32_t max_phys_devs = 7;

    for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
        auto& cur_icd = env.get_test_icd(icd);

        // ICD 1 should not have 1.1
        if (icd != 1) {
            cur_icd.icd_api_version = VK_API_VERSION_1_1;
            cur_icd.add_instance_extension({VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME});
        } else {
            cur_icd.icd_api_version = VK_API_VERSION_1_0;
        }

        uint32_t rand_vendor_id;
        uint32_t rand_driver_vers;
        FillInRandomICDInfo(rand_vendor_id, rand_driver_vers);

        for (uint32_t dev = 0; dev < dev_counts[icd]; ++dev) {
            uint32_t device_version = VK_API_VERSION_1_0;
            cur_icd.physical_devices.push_back({});
            auto& cur_dev = cur_icd.physical_devices.back();

            // 2nd device in ICD 0 and the one device in ICD 3 support the extension and 1.1
            if ((icd == 0 && dev == 1) || icd == 3) {
                cur_dev.extensions.push_back({VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME, 0});
                device_version = VK_API_VERSION_1_1;
            }

            // Still set physical device properties (so we can determine if device is correct API version)
            FillInRandomDeviceProps(cur_dev.properties, device_version, rand_vendor_id, rand_driver_vers);
            FillInRandomExtMemoryData(cur_dev.external_memory_properties);
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(VK_API_VERSION_1_1);
    instance.CheckCreate();

    auto GetPhysicalDeviceExternalBufferProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalBufferProperties>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalBufferProperties"));
    ASSERT_NE(GetPhysicalDeviceExternalBufferProperties, nullptr);

    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        VkPhysicalDeviceProperties pd_props{};
        instance->vkGetPhysicalDeviceProperties(physical_devices[dev], &pd_props);

        for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
            auto& cur_icd = env.get_test_icd(icd);
            bool found = false;
            for (uint32_t pd = 0; pd < dev_counts[icd]; ++pd) {
                auto& cur_dev = cur_icd.physical_devices[pd];
                // Find the ICD device matching the physical device we're looking at info for so we can compare the
                // physical devices info with the returned info.
                if (cur_dev.properties.apiVersion == pd_props.apiVersion && cur_dev.properties.deviceID == pd_props.deviceID &&
                    cur_dev.properties.deviceType == pd_props.deviceType &&
                    cur_dev.properties.driverVersion == pd_props.driverVersion &&
                    cur_dev.properties.vendorID == pd_props.vendorID) {
                    VkPhysicalDeviceExternalBufferInfo info{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_BUFFER_INFO};
                    VkExternalBufferProperties props{VK_STRUCTURE_TYPE_EXTERNAL_BUFFER_PROPERTIES};
                    GetPhysicalDeviceExternalBufferProperties(physical_devices[dev], &info, &props);
                    // No driver support for extension or 1.1 for ICD 1, all others support
                    CompareExtMemoryData(cur_dev.external_memory_properties, props.externalMemoryProperties, icd != 1);
                    found = true;
                    break;
                }
            }
            if (found) {
                break;
            }
        }
    }
}

//
// VK_KHR_external_semaphore_capabilities
//

// Test vkGetPhysicalDeviceExternalSemaphorePropertiesKHR where nothing supports it.
TEST_F(LoaderInstPhysDevExts, PhysDevExtSemPropsKHRNoSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.CheckCreate();

    auto GetPhysicalDeviceExternalSemaphoreProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalSemaphorePropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalSemaphorePropertiesKHR"));
    ASSERT_EQ(GetPhysicalDeviceExternalSemaphoreProperties, nullptr);
}

// Test vkGetPhysicalDeviceExternalSemaphorePropertiesKHR where instance supports it, but nothing else.
TEST_F(LoaderInstPhysDevExts, PhysDevExtSemPropsKHRNoICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME);
    instance.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    auto GetPhysicalDeviceExternalSemaphoreProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalSemaphorePropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalSemaphorePropertiesKHR"));
    ASSERT_EQ(GetPhysicalDeviceExternalSemaphoreProperties, nullptr);
}

// Fill in random but valid data into the external semaphore data struct for the current physical device
static void FillInRandomExtSemData(VkExternalSemaphoreProperties& props) {
    props.sType = VK_STRUCTURE_TYPE_EXTERNAL_SEMAPHORE_PROPERTIES;
    props.pNext = nullptr;
    props.exportFromImportedHandleTypes = static_cast<VkExternalSemaphoreHandleTypeFlags>((rand() % 0xFFF) + 1);
    props.compatibleHandleTypes = static_cast<VkExternalSemaphoreHandleTypeFlags>((rand() % 0xFFF) + 1);
    props.externalSemaphoreFeatures = static_cast<VkExternalSemaphoreFeatureFlags>((rand() % 0xFFF) + 1);
}

// Compare the external semaphore data structs
static void CompareExtSemaphoreData(const VkExternalSemaphoreProperties& props1, const VkExternalSemaphoreProperties& props2,
                                    bool supported = true) {
    if (supported) {
        ASSERT_EQ(props1.externalSemaphoreFeatures, props2.externalSemaphoreFeatures);
        ASSERT_EQ(props1.exportFromImportedHandleTypes, props2.exportFromImportedHandleTypes);
        ASSERT_EQ(props1.compatibleHandleTypes, props2.compatibleHandleTypes);
    } else {
        ASSERT_EQ(0, props2.externalSemaphoreFeatures);
        ASSERT_EQ(0, props2.exportFromImportedHandleTypes);
        ASSERT_EQ(0, props2.compatibleHandleTypes);
    }
}

// Test vkGetPhysicalDeviceExternalSemaphorePropertiesKHR where instance and ICD supports it, but device does not support it.
TEST_F(LoaderInstPhysDevExts, PhysDevExtSemProps2KHRInstanceAndICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).add_instance_extension({VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    FillInRandomExtSemData(env.get_test_icd(0).physical_devices.back().external_semaphore_properties);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME);
    instance.CheckCreate();

    auto GetPhysicalDeviceExternalSemaphoreProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalSemaphorePropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalSemaphorePropertiesKHR"));
    ASSERT_NE(GetPhysicalDeviceExternalSemaphoreProperties, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    VkPhysicalDeviceExternalSemaphoreInfoKHR info{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_SEMAPHORE_INFO_KHR};
    VkExternalSemaphorePropertiesKHR props{VK_STRUCTURE_TYPE_EXTERNAL_SEMAPHORE_PROPERTIES_KHR};
    GetPhysicalDeviceExternalSemaphoreProperties(physical_device, &info, &props);
    CompareExtSemaphoreData(env.get_test_icd(0).physical_devices.back().external_semaphore_properties, props);
}

// Test vkGetPhysicalDeviceExternalSemaphoreProperties where instance supports, an ICD, and a device under that ICD
// also support, so everything should work and return properly.
TEST_F(LoaderInstPhysDevExts, PhysDevExtSemProps2Simple) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).icd_api_version = VK_API_VERSION_1_1;
    env.get_test_icd(0).add_instance_extension({VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    env.get_test_icd(0).physical_devices.back().extensions.push_back({VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME, 0});
    FillInRandomExtSemData(env.get_test_icd(0).physical_devices.back().external_semaphore_properties);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(VK_API_VERSION_1_1);
    instance.CheckCreate();

    auto GetPhysicalDeviceExternalSemaphoreProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalSemaphoreProperties>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalSemaphoreProperties"));
    ASSERT_NE(GetPhysicalDeviceExternalSemaphoreProperties, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    VkPhysicalDeviceExternalSemaphoreInfo info{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_SEMAPHORE_INFO};
    VkExternalSemaphoreProperties props{VK_STRUCTURE_TYPE_EXTERNAL_SEMAPHORE_PROPERTIES};
    GetPhysicalDeviceExternalSemaphoreProperties(physical_device, &info, &props);
    CompareExtSemaphoreData(env.get_test_icd(0).physical_devices.back().external_semaphore_properties, props);
}

// Test vkGetPhysicalDeviceExternalSemaphoreProperties where instance supports it with some ICDs that both support
// and don't support it:
//    ICD 0 supports
//        Physical device 0 does not
//        Physical device 1 does
//        Physical device 2 does not
//    ICD 1 doesn't support
//        Physical device 3 does not
//    ICD 2 supports
//        Physical device 4 does not
//        Physical device 5 does not
//    ICD 3 supports
//        Physical device 6 does
TEST_F(LoaderInstPhysDevExts, PhysDevExtSemPropsMixed) {
    FrameworkEnvironment env{};
    const uint32_t max_icd_count = 4;
    const uint32_t dev_counts[max_icd_count] = {3, 1, 2, 1};
    const uint32_t max_phys_devs = 7;

    for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
        auto& cur_icd = env.get_test_icd(icd);

        // ICD 1 should not have 1.1
        if (icd != 1) {
            cur_icd.icd_api_version = VK_API_VERSION_1_1;
            cur_icd.add_instance_extension({VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME});
        } else {
            cur_icd.icd_api_version = VK_API_VERSION_1_0;
        }

        uint32_t rand_vendor_id;
        uint32_t rand_driver_vers;
        FillInRandomICDInfo(rand_vendor_id, rand_driver_vers);

        for (uint32_t dev = 0; dev < dev_counts[icd]; ++dev) {
            uint32_t device_version = VK_API_VERSION_1_0;
            cur_icd.physical_devices.push_back({});
            auto& cur_dev = cur_icd.physical_devices.back();

            // 2nd device in ICD 0 and the one device in ICD 3 support the extension and 1.1
            if ((icd == 0 && dev == 1) || icd == 3) {
                cur_dev.extensions.push_back({VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME, 0});
                device_version = VK_API_VERSION_1_1;
            }

            // Still set physical device properties (so we can determine if device is correct API version)
            FillInRandomDeviceProps(cur_dev.properties, device_version, rand_vendor_id, rand_driver_vers);
            FillInRandomExtSemData(cur_dev.external_semaphore_properties);
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(VK_API_VERSION_1_1);
    instance.CheckCreate();

    auto GetPhysicalDeviceExternalSemaphoreProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalSemaphoreProperties>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalSemaphoreProperties"));
    ASSERT_NE(GetPhysicalDeviceExternalSemaphoreProperties, nullptr);

    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        VkPhysicalDeviceProperties pd_props{};
        instance->vkGetPhysicalDeviceProperties(physical_devices[dev], &pd_props);

        for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
            auto& cur_icd = env.get_test_icd(icd);
            bool found = false;
            for (uint32_t pd = 0; pd < dev_counts[icd]; ++pd) {
                auto& cur_dev = cur_icd.physical_devices[pd];
                // Find the ICD device matching the physical device we're looking at info for so we can compare the
                // physical devices info with the returned info.
                if (cur_dev.properties.apiVersion == pd_props.apiVersion && cur_dev.properties.deviceID == pd_props.deviceID &&
                    cur_dev.properties.deviceType == pd_props.deviceType &&
                    cur_dev.properties.driverVersion == pd_props.driverVersion &&
                    cur_dev.properties.vendorID == pd_props.vendorID) {
                    VkPhysicalDeviceExternalSemaphoreInfo info{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_SEMAPHORE_INFO};
                    VkExternalSemaphoreProperties props{VK_STRUCTURE_TYPE_EXTERNAL_SEMAPHORE_PROPERTIES};
                    GetPhysicalDeviceExternalSemaphoreProperties(physical_devices[dev], &info, &props);
                    // No driver support for extension or 1.1 for ICD 1, all others support
                    CompareExtSemaphoreData(cur_dev.external_semaphore_properties, props, icd != 1);
                    found = true;
                    break;
                }
            }
            if (found) {
                break;
            }
        }
    }
}

//
// VK_KHR_external_fence_capabilities
//

// Test vkGetPhysicalDeviceExternalFencePropertiesKHR where nothing supports it.
TEST_F(LoaderInstPhysDevExts, PhysDevExtFencePropsKHRNoSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.CheckCreate();

    auto GetPhysicalDeviceExternalFenceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalFencePropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalFencePropertiesKHR"));
    ASSERT_EQ(GetPhysicalDeviceExternalFenceProperties, nullptr);
}

// Test vkGetPhysicalDeviceExternalFencePropertiesKHR where instance supports it, but nothing else.
TEST_F(LoaderInstPhysDevExts, PhysDevExtFencePropsKHRNoICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME);
    instance.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    auto GetPhysicalDeviceExternalFenceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalFencePropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalFencePropertiesKHR"));
    ASSERT_EQ(GetPhysicalDeviceExternalFenceProperties, nullptr);
}

// Fill in random but valid data into the external fence data struct for the current physical device
static void FillInRandomExtFenceData(VkExternalFenceProperties& props) {
    props.sType = VK_STRUCTURE_TYPE_EXTERNAL_FENCE_PROPERTIES;
    props.pNext = nullptr;
    props.exportFromImportedHandleTypes = static_cast<VkExternalFenceHandleTypeFlags>((rand() % 0xFFF) + 1);
    props.compatibleHandleTypes = static_cast<VkExternalFenceHandleTypeFlags>((rand() % 0xFFF) + 1);
    props.externalFenceFeatures = static_cast<VkExternalFenceFeatureFlags>((rand() % 0xFFF) + 1);
}

// Compare the external fence data structs
static void CompareExtFenceData(const VkExternalFenceProperties& props1, const VkExternalFenceProperties& props2,
                                bool supported = true) {
    if (supported) {
        ASSERT_EQ(props1.externalFenceFeatures, props2.externalFenceFeatures);
        ASSERT_EQ(props1.exportFromImportedHandleTypes, props2.exportFromImportedHandleTypes);
        ASSERT_EQ(props1.compatibleHandleTypes, props2.compatibleHandleTypes);
    } else {
        ASSERT_EQ(0, props2.externalFenceFeatures);
        ASSERT_EQ(0, props2.exportFromImportedHandleTypes);
        ASSERT_EQ(0, props2.compatibleHandleTypes);
    }
}

// Test vkGetPhysicalDeviceExternalFencePropertiesKHR where instance and ICD supports it, but device does not support it.
TEST_F(LoaderInstPhysDevExts, PhysDevExtFenceProps2KHRInstanceAndICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).add_instance_extension({VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    FillInRandomExtFenceData(env.get_test_icd(0).physical_devices.back().external_fence_properties);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME);
    instance.CheckCreate();

    auto GetPhysicalDeviceExternalFenceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalFencePropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalFencePropertiesKHR"));
    ASSERT_NE(GetPhysicalDeviceExternalFenceProperties, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    VkPhysicalDeviceExternalFenceInfoKHR info{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_FENCE_INFO_KHR};
    VkExternalFencePropertiesKHR props{VK_STRUCTURE_TYPE_EXTERNAL_FENCE_PROPERTIES_KHR};
    GetPhysicalDeviceExternalFenceProperties(physical_device, &info, &props);
    CompareExtFenceData(env.get_test_icd(0).physical_devices.back().external_fence_properties, props);
}

// Test vkGetPhysicalDeviceExternalFenceProperties where instance supports, an ICD, and a device under that ICD
// also support, so everything should work and return properly.
TEST_F(LoaderInstPhysDevExts, PhysDevExtFenceProps2Simple) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).icd_api_version = VK_API_VERSION_1_1;
    env.get_test_icd(0).add_instance_extension({VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    env.get_test_icd(0).physical_devices.back().extensions.push_back({VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME, 0});
    FillInRandomExtFenceData(env.get_test_icd(0).physical_devices.back().external_fence_properties);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(VK_API_VERSION_1_1);
    instance.CheckCreate();

    auto GetPhysicalDeviceExternalFenceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalFenceProperties>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalFenceProperties"));
    ASSERT_NE(GetPhysicalDeviceExternalFenceProperties, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    VkPhysicalDeviceExternalFenceInfo info{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_FENCE_INFO};
    VkExternalFenceProperties props{VK_STRUCTURE_TYPE_EXTERNAL_FENCE_PROPERTIES};
    GetPhysicalDeviceExternalFenceProperties(physical_device, &info, &props);
    CompareExtFenceData(env.get_test_icd(0).physical_devices.back().external_fence_properties, props);
}

// Test vkGetPhysicalDeviceExternalFenceProperties where instance supports it with some ICDs that both support
// and don't support it:
//    ICD 0 supports
//        Physical device 0 does not
//        Physical device 1 does
//        Physical device 2 does not
//    ICD 1 doesn't support
//        Physical device 3 does not
//    ICD 2 supports
//        Physical device 4 does not
//        Physical device 5 does not
//    ICD 3 supports
//        Physical device 6 does
TEST_F(LoaderInstPhysDevExts, PhysDevExtFencePropsMixed) {
    FrameworkEnvironment env{};
    const uint32_t max_icd_count = 4;
    const uint32_t dev_counts[max_icd_count] = {3, 1, 2, 1};
    const uint32_t max_phys_devs = 7;

    for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
        auto& cur_icd = env.get_test_icd(icd);

        // ICD 1 should not have 1.1
        if (icd != 1) {
            cur_icd.icd_api_version = VK_API_VERSION_1_1;
            cur_icd.add_instance_extension({VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME});
        } else {
            cur_icd.icd_api_version = VK_API_VERSION_1_0;
        }

        uint32_t rand_vendor_id;
        uint32_t rand_driver_vers;
        FillInRandomICDInfo(rand_vendor_id, rand_driver_vers);

        for (uint32_t dev = 0; dev < dev_counts[icd]; ++dev) {
            uint32_t device_version = VK_API_VERSION_1_0;
            cur_icd.physical_devices.push_back({});
            auto& cur_dev = cur_icd.physical_devices.back();

            // 2nd device in ICD 0 and the one device in ICD 3 support the extension and 1.1
            if ((icd == 0 && dev == 1) || icd == 3) {
                cur_dev.extensions.push_back({VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME, 0});
                device_version = VK_API_VERSION_1_1;
            }

            // Still set physical device properties (so we can determine if device is correct API version)
            FillInRandomDeviceProps(cur_dev.properties, device_version, rand_vendor_id, rand_driver_vers);
            FillInRandomExtFenceData(cur_dev.external_fence_properties);
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(VK_API_VERSION_1_1);
    instance.CheckCreate();

    auto GetPhysicalDeviceExternalFenceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalFenceProperties>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalFenceProperties"));
    ASSERT_NE(GetPhysicalDeviceExternalFenceProperties, nullptr);

    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        VkPhysicalDeviceProperties pd_props{};
        instance->vkGetPhysicalDeviceProperties(physical_devices[dev], &pd_props);

        for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
            auto& cur_icd = env.get_test_icd(icd);
            bool found = false;
            for (uint32_t pd = 0; pd < dev_counts[icd]; ++pd) {
                auto& cur_dev = cur_icd.physical_devices[pd];
                // Find the ICD device matching the physical device we're looking at info for so we can compare the
                // physical devices info with the returned info.
                if (cur_dev.properties.apiVersion == pd_props.apiVersion && cur_dev.properties.deviceID == pd_props.deviceID &&
                    cur_dev.properties.deviceType == pd_props.deviceType &&
                    cur_dev.properties.driverVersion == pd_props.driverVersion &&
                    cur_dev.properties.vendorID == pd_props.vendorID) {
                    VkPhysicalDeviceExternalFenceInfo info{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_FENCE_INFO};
                    VkExternalFenceProperties props{VK_STRUCTURE_TYPE_EXTERNAL_FENCE_PROPERTIES};
                    GetPhysicalDeviceExternalFenceProperties(physical_devices[dev], &info, &props);
                    // No driver support for extension or 1.1 for ICD 1, all others support
                    CompareExtFenceData(cur_dev.external_fence_properties, props, icd != 1);
                    found = true;
                    break;
                }
            }
            if (found) {
                break;
            }
        }
    }
}

//
// VK_KHR_get_surface_capabilities2
//

// Test vkGetPhysicalDeviceSurfaceCapabilities2KHR where nothing supports it.
TEST_F(LoaderInstPhysDevExts, PhysDevSurfaceCaps2KHRNoSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.CheckCreate();

    auto GetPhysicalDeviceSurfaceCapabilities2 = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilities2KHR"));
    ASSERT_EQ(GetPhysicalDeviceSurfaceCapabilities2, nullptr);
}

// Test vkGetPhysicalDeviceSurfaceCapabilities2KHR where instance supports it, but nothing else.
TEST_F(LoaderInstPhysDevExts, PhysDevSurfaceCaps2KHRNoICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME);
    instance.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    auto GetPhysicalDeviceSurfaceCapabilities2 = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilities2KHR"));
    ASSERT_EQ(GetPhysicalDeviceSurfaceCapabilities2, nullptr);
}

// Fill in random but valid data into the surface capability data struct for the current physical device
static void FillInRandomSurfaceCapsData(VkSurfaceCapabilitiesKHR& props) {
    props.minImageCount = (rand() % 0xFFF) + 1;
    props.maxImageCount = (rand() % 0xFFF) + 1;
    props.currentExtent.width = (rand() % 0xFFF) + 1;
    props.currentExtent.height = (rand() % 0xFFF) + 1;
    props.minImageExtent.width = (rand() % 0xFFF) + 1;
    props.minImageExtent.height = (rand() % 0xFFF) + 1;
    props.maxImageExtent.width = (rand() % 0xFFF) + 1;
    props.maxImageExtent.height = (rand() % 0xFFF) + 1;
    props.maxImageArrayLayers = (rand() % 0xFFF) + 1;
    props.supportedTransforms = static_cast<VkSurfaceTransformFlagsKHR>((rand() % 0xFFF) + 1);
    props.currentTransform = static_cast<VkSurfaceTransformFlagBitsKHR>((rand() % 0xFFF) + 1);
    props.supportedCompositeAlpha = static_cast<VkCompositeAlphaFlagsKHR>((rand() % 0xFFF) + 1);
    props.supportedUsageFlags = static_cast<VkImageUsageFlags>((rand() % 0xFFF) + 1);
}

// Compare the surface capability data structs
static void CompareSurfaceCapsData(const VkSurfaceCapabilitiesKHR& props1, const VkSurfaceCapabilitiesKHR& props2,
                                   bool supported = true) {
    if (supported) {
        ASSERT_EQ(props1.minImageCount, props2.minImageCount);
        ASSERT_EQ(props1.maxImageCount, props2.maxImageCount);
        ASSERT_EQ(props1.currentExtent.width, props2.currentExtent.width);
        ASSERT_EQ(props1.currentExtent.height, props2.currentExtent.height);
        ASSERT_EQ(props1.minImageExtent.width, props2.minImageExtent.width);
        ASSERT_EQ(props1.minImageExtent.height, props2.minImageExtent.height);
        ASSERT_EQ(props1.maxImageExtent.width, props2.maxImageExtent.width);
        ASSERT_EQ(props1.maxImageExtent.height, props2.maxImageExtent.height);
        ASSERT_EQ(props1.maxImageArrayLayers, props2.maxImageArrayLayers);
        ASSERT_EQ(props1.supportedTransforms, props2.supportedTransforms);
        ASSERT_EQ(props1.currentTransform, props2.currentTransform);
        ASSERT_EQ(props1.supportedCompositeAlpha, props2.supportedCompositeAlpha);
        ASSERT_EQ(props1.supportedUsageFlags, props2.supportedUsageFlags);
    } else {
        ASSERT_EQ(0, props2.minImageCount);
        ASSERT_EQ(0, props2.maxImageCount);
        ASSERT_EQ(0, props2.currentExtent.width);
        ASSERT_EQ(0, props2.currentExtent.height);
        ASSERT_EQ(0, props2.minImageExtent.width);
        ASSERT_EQ(0, props2.minImageExtent.height);
        ASSERT_EQ(0, props2.maxImageExtent.width);
        ASSERT_EQ(0, props2.maxImageExtent.height);
        ASSERT_EQ(0, props2.maxImageArrayLayers);
        ASSERT_EQ(0, props2.supportedTransforms);
        ASSERT_EQ(0, props2.currentTransform);
        ASSERT_EQ(0, props2.supportedCompositeAlpha);
        ASSERT_EQ(0, props2.supportedUsageFlags);
    }
}

// Test vkGetPhysicalDeviceSurfaceCapabilities2KHR where instance and ICD supports it, but device does not support it.
TEST_F(LoaderInstPhysDevExts, PhysDevSurfaceCaps2KHRInstanceAndICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    Extension first_ext{VK_KHR_SURFACE_EXTENSION_NAME};
    Extension second_ext{VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME};
    Extension third_ext{VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME};
    env.get_test_icd(0).add_instance_extensions({first_ext, second_ext, third_ext});
    env.get_test_icd(0).physical_devices.push_back({});
    FillInRandomSurfaceCapsData(env.get_test_icd(0).physical_devices.back().surface_capabilities);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extensions(
        {VK_KHR_SURFACE_EXTENSION_NAME, VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME, VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME});
    instance.CheckCreate();

    auto GetPhysicalDeviceSurfaceCapabilities = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
    ASSERT_NE(GetPhysicalDeviceSurfaceCapabilities, nullptr);
    auto CreateHeadlessSurfaceEXT = reinterpret_cast<PFN_vkCreateHeadlessSurfaceEXT>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkCreateHeadlessSurfaceEXT"));
    ASSERT_NE(CreateHeadlessSurfaceEXT, nullptr);
    auto DestroySurfaceKHR =
        reinterpret_cast<PFN_vkDestroySurfaceKHR>(instance.functions->vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR"));
    ASSERT_NE(DestroySurfaceKHR, nullptr);
    auto GetPhysicalDeviceSurfaceCapabilities2 = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilities2KHR"));
    ASSERT_NE(GetPhysicalDeviceSurfaceCapabilities2, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    VkSurfaceKHR surface;
    VkHeadlessSurfaceCreateInfoEXT create_info{VK_STRUCTURE_TYPE_HEADLESS_SURFACE_CREATE_INFO_EXT};
    ASSERT_EQ(VK_SUCCESS, CreateHeadlessSurfaceEXT(instance.inst, &create_info, nullptr, &surface));

    VkSurfaceCapabilitiesKHR props{};
    ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceSurfaceCapabilities(physical_device, surface, &props));

    VkPhysicalDeviceSurfaceInfo2KHR info{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR, nullptr, surface};
    VkSurfaceCapabilities2KHR props2{VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR};
    ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceSurfaceCapabilities2(physical_device, &info, &props2));
    CompareSurfaceCapsData(props, props2.surfaceCapabilities);

    DestroySurfaceKHR(instance.inst, surface, nullptr);
}

// Test vkGetPhysicalDeviceSurfaceCapabilities2 where instance supports it with some ICDs that both support
// and don't support it:
//    ICD 0 supports
//        Physical device 0 does not
//        Physical device 1 does
//        Physical device 2 does not
//    ICD 1 doesn't support
//        Physical device 3 does not
//    ICD 2 supports
//        Physical device 4 does not
//        Physical device 5 does not
//    ICD 3 supports
//        Physical device 6 does
TEST_F(LoaderInstPhysDevExts, PhysDevSurfaceCaps2KHRMixed) {
    FrameworkEnvironment env{};
    const uint32_t max_icd_count = 4;
    const uint32_t dev_counts[max_icd_count] = {3, 1, 2, 1};
    const uint32_t max_phys_devs = 7;
    Extension first_ext{VK_KHR_SURFACE_EXTENSION_NAME};
    Extension second_ext{VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME};
    Extension third_ext{VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME};

    for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_0;
        cur_icd.add_instance_extensions({first_ext, third_ext});

        // ICD 1 should not have 1.1
        if (icd != 1) {
            cur_icd.icd_api_version = VK_API_VERSION_1_1;
            cur_icd.add_instance_extension(second_ext);
        }

        uint32_t rand_vendor_id;
        uint32_t rand_driver_vers;
        FillInRandomICDInfo(rand_vendor_id, rand_driver_vers);

        for (uint32_t dev = 0; dev < dev_counts[icd]; ++dev) {
            uint32_t device_version = VK_API_VERSION_1_0;
            cur_icd.physical_devices.push_back({});
            auto& cur_dev = cur_icd.physical_devices.back();
            cur_dev.extensions.push_back({VK_KHR_SURFACE_EXTENSION_NAME, 0});
            cur_dev.extensions.push_back({VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME, 0});

            // 2nd device in ICD 0 and the one device in ICD 3 support the extension and 1.1
            if ((icd == 0 && dev == 1) || icd == 3) {
                cur_dev.extensions.push_back({VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME, 0});
                device_version = VK_API_VERSION_1_1;
            }

            // Still set physical device properties (so we can determine if device is correct API version)
            FillInRandomDeviceProps(cur_dev.properties, device_version, rand_vendor_id, rand_driver_vers);
            FillInRandomSurfaceCapsData(cur_dev.surface_capabilities);
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extensions(
        {VK_KHR_SURFACE_EXTENSION_NAME, VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME, VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME});
    instance.CheckCreate();

    auto GetPhysicalDeviceSurfaceCapabilities = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
    ASSERT_NE(GetPhysicalDeviceSurfaceCapabilities, nullptr);
    auto CreateHeadlessSurfaceEXT = reinterpret_cast<PFN_vkCreateHeadlessSurfaceEXT>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkCreateHeadlessSurfaceEXT"));
    ASSERT_NE(CreateHeadlessSurfaceEXT, nullptr);
    auto DestroySurfaceKHR =
        reinterpret_cast<PFN_vkDestroySurfaceKHR>(instance.functions->vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR"));
    ASSERT_NE(DestroySurfaceKHR, nullptr);
    auto GetPhysicalDeviceSurfaceCapabilities2 = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilities2KHR"));
    ASSERT_NE(GetPhysicalDeviceSurfaceCapabilities2, nullptr);

    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    VkSurfaceKHR surface;
    VkHeadlessSurfaceCreateInfoEXT create_info{VK_STRUCTURE_TYPE_HEADLESS_SURFACE_CREATE_INFO_EXT};
    ASSERT_EQ(VK_SUCCESS, CreateHeadlessSurfaceEXT(instance.inst, &create_info, nullptr, &surface));

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        VkSurfaceCapabilitiesKHR props{};
        ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceSurfaceCapabilities(physical_devices[dev], surface, &props));

        VkPhysicalDeviceSurfaceInfo2KHR info{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR, nullptr, surface};
        VkSurfaceCapabilities2KHR props2{VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR};
        ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceSurfaceCapabilities2(physical_devices[dev], &info, &props2));
        CompareSurfaceCapsData(props, props2.surfaceCapabilities);
    }

    DestroySurfaceKHR(instance.inst, surface, nullptr);
}

// Test vkGetPhysicalDeviceSurfaceFormats2KHR where nothing supports it.
TEST_F(LoaderInstPhysDevExts, PhysDevSurfaceFormats2KHRNoSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.CheckCreate();

    auto GetPhysicalDeviceSurfaceFormats2 = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormats2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormats2KHR"));
    ASSERT_EQ(GetPhysicalDeviceSurfaceFormats2, nullptr);
}

// Test vkGetPhysicalDeviceSurfaceFormats2KHR where instance supports it, but nothing else.
TEST_F(LoaderInstPhysDevExts, PhysDevSurfaceFormats2KHRNoICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME);
    instance.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    auto GetPhysicalDeviceSurfaceFormats2 = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormats2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormats2KHR"));
    ASSERT_EQ(GetPhysicalDeviceSurfaceFormats2, nullptr);
}

// Fill in random but valid data into the surface formats data struct for the current physical device
static void FillInRandomSurfaceFormatsData(std::vector<VkSurfaceFormatKHR>& props) {
    props.resize((rand() % 5) + 1);
    for (uint32_t i = 0; i < props.size(); ++i) {
        props[i].format = static_cast<VkFormat>((rand() % 0xFFF) + 1);
        props[i].colorSpace = static_cast<VkColorSpaceKHR>((rand() % 0xFFF) + 1);
    }
}

// Compare the surface formats data structs
static void CompareSurfaceFormatsData(const std::vector<VkSurfaceFormatKHR>& props1, const std::vector<VkSurfaceFormat2KHR>& props2,
                                      bool supported = true) {
    ASSERT_EQ(props1.size(), props2.size());
    for (uint32_t i = 0; i < props1.size(); ++i) {
        if (supported) {
            ASSERT_EQ(props1[i].format, props2[i].surfaceFormat.format);
            ASSERT_EQ(props1[i].colorSpace, props2[i].surfaceFormat.colorSpace);
        } else {
            ASSERT_EQ(0, props2[i].surfaceFormat.format);
            ASSERT_EQ(0, props2[i].surfaceFormat.colorSpace);
        }
    }
}

// Test vkGetPhysicalDeviceSurfaceFormats2KHR where instance and ICD supports it, but device does not support it.
TEST_F(LoaderInstPhysDevExts, PhysDevSurfaceFormats2KHRInstanceAndICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    Extension first_ext{VK_KHR_SURFACE_EXTENSION_NAME};
    Extension second_ext{VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME};
    Extension third_ext{VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME};
    env.get_test_icd(0).add_instance_extensions({first_ext, second_ext, third_ext});
    env.get_test_icd(0).physical_devices.push_back({});
    FillInRandomSurfaceFormatsData(env.get_test_icd(0).physical_devices.back().surface_formats);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extensions(
        {VK_KHR_SURFACE_EXTENSION_NAME, VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME, VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME});
    instance.CheckCreate();

    auto GetPhysicalDeviceSurfaceFormats = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR"));
    ASSERT_NE(GetPhysicalDeviceSurfaceFormats, nullptr);
    auto CreateHeadlessSurfaceEXT = reinterpret_cast<PFN_vkCreateHeadlessSurfaceEXT>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkCreateHeadlessSurfaceEXT"));
    ASSERT_NE(CreateHeadlessSurfaceEXT, nullptr);
    auto DestroySurfaceKHR =
        reinterpret_cast<PFN_vkDestroySurfaceKHR>(instance.functions->vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR"));
    ASSERT_NE(DestroySurfaceKHR, nullptr);
    auto GetPhysicalDeviceSurfaceFormats2 = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormats2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormats2KHR"));
    ASSERT_NE(GetPhysicalDeviceSurfaceFormats2, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    VkSurfaceKHR surface;
    VkHeadlessSurfaceCreateInfoEXT create_info{VK_STRUCTURE_TYPE_HEADLESS_SURFACE_CREATE_INFO_EXT};
    ASSERT_EQ(VK_SUCCESS, CreateHeadlessSurfaceEXT(instance.inst, &create_info, nullptr, &surface));

    std::vector<VkSurfaceFormatKHR> props{};
    uint32_t count_1 = 0;
    ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceSurfaceFormats(physical_device, surface, &count_1, nullptr));
    ASSERT_EQ(env.get_test_icd(0).physical_devices.back().surface_formats.size(), count_1);
    props.resize(count_1);
    ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceSurfaceFormats(physical_device, surface, &count_1, props.data()));
    ASSERT_EQ(env.get_test_icd(0).physical_devices.back().surface_formats.size(), count_1);

    VkPhysicalDeviceSurfaceInfo2KHR info{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR, nullptr, surface};
    std::vector<VkSurfaceFormat2KHR> props2{};
    uint32_t count_2 = 0;
    ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceSurfaceFormats2(physical_device, &info, &count_2, nullptr));
    ASSERT_EQ(count_1, count_2);
    props2.resize(count_2, VkSurfaceFormat2KHR{VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR});
    ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceSurfaceFormats2(physical_device, &info, &count_2, props2.data()));
    CompareSurfaceFormatsData(props, props2);

    DestroySurfaceKHR(instance.inst, surface, nullptr);
}

// Test vkGetPhysicalDeviceSurfaceFormats2 where instance supports it with some ICDs that both support
// and don't support it:
//    ICD 0 supports
//        Physical device 0 does not
//        Physical device 1 does
//        Physical device 2 does not
//    ICD 1 doesn't support
//        Physical device 3 does not
//    ICD 2 supports
//        Physical device 4 does not
//        Physical device 5 does not
//    ICD 3 supports
//        Physical device 6 does
TEST_F(LoaderInstPhysDevExts, PhysDevSurfaceFormats2KHRMixed) {
    FrameworkEnvironment env{};
    const uint32_t max_icd_count = 4;
    const uint32_t dev_counts[max_icd_count] = {3, 1, 2, 1};
    const uint32_t max_phys_devs = 7;
    Extension first_ext{VK_KHR_SURFACE_EXTENSION_NAME};
    Extension second_ext{VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME};
    Extension third_ext{VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME};

    for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_0;
        cur_icd.add_instance_extensions({first_ext, third_ext});

        // ICD 1 should not have 1.1
        if (icd != 1) {
            cur_icd.icd_api_version = VK_API_VERSION_1_1;
            cur_icd.add_instance_extension(second_ext);
        }

        uint32_t rand_vendor_id;
        uint32_t rand_driver_vers;
        FillInRandomICDInfo(rand_vendor_id, rand_driver_vers);

        for (uint32_t dev = 0; dev < dev_counts[icd]; ++dev) {
            uint32_t device_version = VK_API_VERSION_1_0;
            cur_icd.physical_devices.push_back({});
            auto& cur_dev = cur_icd.physical_devices.back();
            cur_dev.extensions.push_back({VK_KHR_SURFACE_EXTENSION_NAME, 0});
            cur_dev.extensions.push_back({VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME, 0});

            // 2nd device in ICD 0 and the one device in ICD 3 support the extension and 1.1
            if ((icd == 0 && dev == 1) || icd == 3) {
                cur_dev.extensions.push_back({VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME, 0});
                device_version = VK_API_VERSION_1_1;
            }

            // Still set physical device properties (so we can determine if device is correct API version)
            FillInRandomDeviceProps(cur_dev.properties, device_version, rand_vendor_id, rand_driver_vers);
            FillInRandomSurfaceFormatsData(cur_dev.surface_formats);
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extensions(
        {VK_KHR_SURFACE_EXTENSION_NAME, VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME, VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME});
    instance.CheckCreate();

    auto GetPhysicalDeviceSurfaceFormats = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR"));
    ASSERT_NE(GetPhysicalDeviceSurfaceFormats, nullptr);
    auto CreateHeadlessSurfaceEXT = reinterpret_cast<PFN_vkCreateHeadlessSurfaceEXT>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkCreateHeadlessSurfaceEXT"));
    ASSERT_NE(CreateHeadlessSurfaceEXT, nullptr);
    auto DestroySurfaceKHR =
        reinterpret_cast<PFN_vkDestroySurfaceKHR>(instance.functions->vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR"));
    ASSERT_NE(DestroySurfaceKHR, nullptr);
    auto GetPhysicalDeviceSurfaceFormats2 = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormats2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormats2KHR"));
    ASSERT_NE(GetPhysicalDeviceSurfaceFormats2, nullptr);

    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    VkSurfaceKHR surface;
    VkHeadlessSurfaceCreateInfoEXT create_info{VK_STRUCTURE_TYPE_HEADLESS_SURFACE_CREATE_INFO_EXT};
    ASSERT_EQ(VK_SUCCESS, CreateHeadlessSurfaceEXT(instance.inst, &create_info, nullptr, &surface));

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        std::vector<VkSurfaceFormatKHR> props{};
        uint32_t count_1 = 0;
        ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceSurfaceFormats(physical_devices[dev], surface, &count_1, nullptr));
        ASSERT_NE(0, count_1);
        props.resize(count_1);
        ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceSurfaceFormats(physical_devices[dev], surface, &count_1, props.data()));
        ASSERT_NE(0, count_1);

        VkPhysicalDeviceSurfaceInfo2KHR info{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR, nullptr, surface};
        std::vector<VkSurfaceFormat2KHR> props2{};
        uint32_t count_2 = 0;
        ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceSurfaceFormats2(physical_devices[dev], &info, &count_2, nullptr));
        ASSERT_EQ(count_1, count_2);
        props2.resize(count_2, VkSurfaceFormat2KHR{VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR});
        ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceSurfaceFormats2(physical_devices[dev], &info, &count_2, props2.data()));
        ASSERT_EQ(count_1, count_2);
        CompareSurfaceFormatsData(props, props2);
    }

    DestroySurfaceKHR(instance.inst, surface, nullptr);
}

//
// VK_KHR_display
//

// Test vkGetPhysicalDeviceDisplayPropertiesKHR where nothing supports it.
TEST_F(LoaderInstPhysDevExts, PhysDevDispPropsKHRNoSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.CheckCreate();

    auto GetPhysicalDeviceDisplayProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayPropertiesKHR"));
    ASSERT_EQ(GetPhysicalDeviceDisplayProperties, nullptr);
}

// Test vkGetPhysicalDeviceDisplayPropertiesKHR where instance supports it, but nothing else.
TEST_F(LoaderInstPhysDevExts, PhysDevDispPropsKHRNoICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_DISPLAY_EXTENSION_NAME);
    instance.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    auto GetPhysicalDeviceDisplayProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayPropertiesKHR"));
    ASSERT_EQ(GetPhysicalDeviceDisplayProperties, nullptr);
}

// Fill in random but valid data into the display property data struct for the current physical device
static void FillInRandomDisplayPropData(std::vector<VkDisplayPropertiesKHR>& props) {
    props.resize((rand() % 5) + 1);
    for (uint32_t i = 0; i < props.size(); ++i) {
        props[i].display = (VkDisplayKHR)((rand() % 0xFFFFFFF) + 1);
        props[i].physicalDimensions.width = static_cast<uint32_t>((rand() % 0xFFF) + 1);
        props[i].physicalDimensions.height = static_cast<uint32_t>((rand() % 0xFFF) + 1);
        props[i].physicalResolution.width = static_cast<uint32_t>((rand() % 0xFFF) + 1);
        props[i].physicalResolution.height = static_cast<uint32_t>((rand() % 0xFFF) + 1);
        props[i].supportedTransforms = static_cast<VkSurfaceTransformFlagsKHR>((rand() % 0xFFE) + 1);
        props[i].planeReorderPossible = rand() % 2 > 0 ? VK_TRUE : VK_FALSE;
        props[i].persistentContent = rand() % 2 > 0 ? VK_TRUE : VK_FALSE;
    }
}

// Compare the display property data structs
static void CompareDisplayPropData(const std::vector<VkDisplayPropertiesKHR>& props1,
                                   const std::vector<VkDisplayPropertiesKHR>& props2) {
    ASSERT_EQ(props1.size(), props2.size());
    for (uint32_t i = 0; i < props1.size(); ++i) {
        ASSERT_EQ(props1[i].display, props2[i].display);
        ASSERT_EQ(props1[i].physicalDimensions.width, props2[i].physicalDimensions.width);
        ASSERT_EQ(props1[i].physicalDimensions.height, props2[i].physicalDimensions.height);
        ASSERT_EQ(props1[i].physicalResolution.width, props2[i].physicalResolution.width);
        ASSERT_EQ(props1[i].physicalResolution.height, props2[i].physicalResolution.height);
        ASSERT_EQ(props1[i].supportedTransforms, props2[i].supportedTransforms);
        ASSERT_EQ(props1[i].planeReorderPossible, props2[i].planeReorderPossible);
        ASSERT_EQ(props1[i].persistentContent, props2[i].persistentContent);
    }
}

// Test vGetPhysicalDeviceDisplayPropertiesKHR where instance and ICD supports it, but device does not support it.
TEST_F(LoaderInstPhysDevExts, PhysDevDispPropsKHRInstanceAndICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).add_instance_extension({VK_KHR_DISPLAY_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    FillInRandomDisplayPropData(env.get_test_icd(0).physical_devices.back().display_properties);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension({VK_KHR_DISPLAY_EXTENSION_NAME});
    instance.CheckCreate();

    auto GetPhysicalDeviceDisplayProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayPropertiesKHR"));
    ASSERT_NE(GetPhysicalDeviceDisplayProperties, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    std::vector<VkDisplayPropertiesKHR> props{};
    uint32_t prop_count = 0;
    ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceDisplayProperties(physical_device, &prop_count, nullptr));
    ASSERT_EQ(env.get_test_icd(0).physical_devices.back().display_properties.size(), prop_count);
    props.resize(prop_count);
    ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceDisplayProperties(physical_device, &prop_count, props.data()));
    ASSERT_EQ(env.get_test_icd(0).physical_devices.back().display_properties.size(), prop_count);

    CompareDisplayPropData(props, env.get_test_icd(0).physical_devices.back().display_properties);
}

// Test vkGetPhysicalDeviceDisplayPropertiesKHR where instance supports it with some ICDs that both support
// and don't support it:
//    ICD 0 supports
//        Physical device 0 does not
//        Physical device 1 does
//        Physical device 2 does not
//    ICD 1 doesn't support
//        Physical device 3 does not
//    ICD 2 supports
//        Physical device 4 does not
//        Physical device 5 does not
//    ICD 3 supports
//        Physical device 6 does
TEST_F(LoaderInstPhysDevExts, PhysDevDispPropsKHRMixed) {
    FrameworkEnvironment env{};
    const uint32_t max_icd_count = 4;
    const uint32_t dev_counts[max_icd_count] = {3, 1, 2, 1};
    const uint32_t max_phys_devs = 7;

    for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_0;

        // ICD 1 should not have 1.1
        if (icd != 1) {
            cur_icd.icd_api_version = VK_API_VERSION_1_1;
            cur_icd.add_instance_extension({VK_KHR_DISPLAY_EXTENSION_NAME});
        }

        uint32_t rand_vendor_id;
        uint32_t rand_driver_vers;
        FillInRandomICDInfo(rand_vendor_id, rand_driver_vers);

        for (uint32_t dev = 0; dev < dev_counts[icd]; ++dev) {
            uint32_t device_version = VK_API_VERSION_1_0;
            cur_icd.physical_devices.push_back({});
            auto& cur_dev = cur_icd.physical_devices.back();

            // 2nd device in ICD 0 and the one device in ICD 3 support the extension and 1.1
            if ((icd == 0 && dev == 1) || icd == 3) {
                cur_dev.extensions.push_back({VK_KHR_DISPLAY_EXTENSION_NAME, 0});
                device_version = VK_API_VERSION_1_1;
            }

            // Still set physical device properties (so we can determine if device is correct API version)
            FillInRandomDeviceProps(cur_dev.properties, device_version, rand_vendor_id, rand_driver_vers);
            FillInRandomDisplayPropData(cur_dev.display_properties);
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_DISPLAY_EXTENSION_NAME);
    instance.CheckCreate();

    auto GetPhysicalDeviceDisplayProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayPropertiesKHR"));
    ASSERT_NE(GetPhysicalDeviceDisplayProperties, nullptr);

    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        VkPhysicalDeviceProperties pd_props{};
        instance->vkGetPhysicalDeviceProperties(physical_devices[dev], &pd_props);

        for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
            auto& cur_icd = env.get_test_icd(icd);
            bool found = false;
            for (uint32_t pd = 0; pd < dev_counts[icd]; ++pd) {
                auto& cur_dev = cur_icd.physical_devices[pd];
                // Find the ICD device matching the physical device we're looking at info for so we can compare the
                // physical devices info with the returned info.
                if (cur_dev.properties.apiVersion == pd_props.apiVersion && cur_dev.properties.deviceID == pd_props.deviceID &&
                    cur_dev.properties.deviceType == pd_props.deviceType &&
                    cur_dev.properties.driverVersion == pd_props.driverVersion &&
                    cur_dev.properties.vendorID == pd_props.vendorID) {
                    std::vector<VkDisplayPropertiesKHR> props{};
                    uint32_t prop_count = 0;
                    ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceDisplayProperties(physical_devices[dev], &prop_count, nullptr));
                    if (icd == 1) {
                        // For this extension, if no support exists (like for ICD 1), the value of 0 should be returned by the
                        // loader.
                        ASSERT_EQ(0, prop_count);
                    } else {
                        ASSERT_EQ(cur_dev.display_properties.size(), prop_count);
                        props.resize(prop_count);
                        ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceDisplayProperties(physical_devices[dev], &prop_count, props.data()));
                        ASSERT_EQ(cur_dev.display_properties.size(), prop_count);

                        CompareDisplayPropData(props, cur_dev.display_properties);
                    }
                    found = true;
                    break;
                }
            }
            if (found) {
                break;
            }
        }
    }
}

// Test vkGetPhysicalDeviceDisplayPlanePropertiesKHR where nothing supports it.
TEST_F(LoaderInstPhysDevExts, PhysDevDispPlanePropsKHRNoSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.CheckCreate();

    auto GetPhysicalDeviceDisplayPlaneProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR"));
    ASSERT_EQ(GetPhysicalDeviceDisplayPlaneProperties, nullptr);
}

// Test vkGetPhysicalDeviceDisplayPlanePropertiesKHR where instance supports it, but nothing else.
TEST_F(LoaderInstPhysDevExts, PhysDevDispPlanePropsKHRNoICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_DISPLAY_EXTENSION_NAME);
    instance.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    auto GetPhysicalDeviceDisplayPlaneProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR"));
    ASSERT_EQ(GetPhysicalDeviceDisplayPlaneProperties, nullptr);
}

// Fill in random but valid data into the display plane property data struct for the current physical device
static void FillInRandomDisplayPlanePropData(std::vector<VkDisplayPlanePropertiesKHR>& props) {
    props.resize((rand() % 5) + 1);
    for (uint32_t i = 0; i < props.size(); ++i) {
        props[i].currentDisplay = (VkDisplayKHR)((rand() % 0xFFFFFFF) + 1);
        props[i].currentStackIndex = static_cast<uint32_t>((rand() % 0xFFF) + (rand() % 0xFFF) + 1);
    }
}

// Compare the display plane property data structs
static void CompareDisplayPlanePropData(const std::vector<VkDisplayPlanePropertiesKHR>& props1,
                                        const std::vector<VkDisplayPlanePropertiesKHR>& props2) {
    ASSERT_EQ(props1.size(), props2.size());
    for (uint32_t i = 0; i < props1.size(); ++i) {
        ASSERT_EQ(props1[i].currentDisplay, props2[i].currentDisplay);
        ASSERT_EQ(props1[i].currentStackIndex, props2[i].currentStackIndex);
    }
}

// Test vGetPhysicalDeviceDisplayPlanePropertiesKHR where instance and ICD supports it, but device does not support it.
TEST_F(LoaderInstPhysDevExts, PhysDevDispPlanePropsKHRInstanceAndICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).add_instance_extension({VK_KHR_DISPLAY_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    FillInRandomDisplayPlanePropData(env.get_test_icd(0).physical_devices.back().display_plane_properties);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension({VK_KHR_DISPLAY_EXTENSION_NAME});
    instance.CheckCreate();

    auto GetPhysicalDeviceDisplayPlaneProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR"));
    ASSERT_NE(GetPhysicalDeviceDisplayPlaneProperties, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    std::vector<VkDisplayPlanePropertiesKHR> props{};
    uint32_t prop_count = 0;
    ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceDisplayPlaneProperties(physical_device, &prop_count, nullptr));
    ASSERT_EQ(env.get_test_icd(0).physical_devices.back().display_plane_properties.size(), prop_count);
    props.resize(prop_count);
    ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceDisplayPlaneProperties(physical_device, &prop_count, props.data()));
    ASSERT_EQ(env.get_test_icd(0).physical_devices.back().display_plane_properties.size(), prop_count);

    CompareDisplayPlanePropData(props, env.get_test_icd(0).physical_devices.back().display_plane_properties);
}

// Test vkGetPhysicalDeviceDisplayPlanePropertiesKHR where instance supports it with some ICDs that both support
// and don't support it:
//    ICD 0 supports
//        Physical device 0 does not
//        Physical device 1 does
//        Physical device 2 does not
//    ICD 1 doesn't support
//        Physical device 3 does not
//    ICD 2 supports
//        Physical device 4 does not
//        Physical device 5 does not
//    ICD 3 supports
//        Physical device 6 does
TEST_F(LoaderInstPhysDevExts, PhysDevDispPlanePropsKHRMixed) {
    FrameworkEnvironment env{};
    const uint32_t max_icd_count = 4;
    const uint32_t dev_counts[max_icd_count] = {3, 1, 2, 1};
    const uint32_t max_phys_devs = 7;

    for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_0;

        // ICD 1 should not have 1.1
        if (icd != 1) {
            cur_icd.icd_api_version = VK_API_VERSION_1_1;
            cur_icd.add_instance_extension({VK_KHR_DISPLAY_EXTENSION_NAME});
        }

        uint32_t rand_vendor_id;
        uint32_t rand_driver_vers;
        FillInRandomICDInfo(rand_vendor_id, rand_driver_vers);

        for (uint32_t dev = 0; dev < dev_counts[icd]; ++dev) {
            uint32_t device_version = VK_API_VERSION_1_0;
            cur_icd.physical_devices.push_back({});
            auto& cur_dev = cur_icd.physical_devices.back();

            // 2nd device in ICD 0 and the one device in ICD 3 support the extension and 1.1
            if ((icd == 0 && dev == 1) || icd == 3) {
                cur_dev.extensions.push_back({VK_KHR_DISPLAY_EXTENSION_NAME, 0});
                device_version = VK_API_VERSION_1_1;
            }

            // Still set physical device properties (so we can determine if device is correct API version)
            FillInRandomDeviceProps(cur_dev.properties, device_version, rand_vendor_id, rand_driver_vers);
            FillInRandomDisplayPlanePropData(cur_dev.display_plane_properties);
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_DISPLAY_EXTENSION_NAME);
    instance.CheckCreate();

    auto GetPhysicalDeviceDisplayPlaneProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR"));
    ASSERT_NE(GetPhysicalDeviceDisplayPlaneProperties, nullptr);

    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        VkPhysicalDeviceProperties pd_props{};
        instance->vkGetPhysicalDeviceProperties(physical_devices[dev], &pd_props);

        for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
            auto& cur_icd = env.get_test_icd(icd);
            bool found = false;
            for (uint32_t pd = 0; pd < dev_counts[icd]; ++pd) {
                auto& cur_dev = cur_icd.physical_devices[pd];
                // Find the ICD device matching the physical device we're looking at info for so we can compare the
                // physical devices info with the returned info.
                if (cur_dev.properties.apiVersion == pd_props.apiVersion && cur_dev.properties.deviceID == pd_props.deviceID &&
                    cur_dev.properties.deviceType == pd_props.deviceType &&
                    cur_dev.properties.driverVersion == pd_props.driverVersion &&
                    cur_dev.properties.vendorID == pd_props.vendorID) {
                    std::vector<VkDisplayPlanePropertiesKHR> props{};
                    uint32_t prop_count = 0;
                    ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceDisplayPlaneProperties(physical_devices[dev], &prop_count, nullptr));
                    if (icd == 1) {
                        // For this extension, if no support exists (like for ICD 1), the value of 0 should be returned by the
                        // loader.
                        ASSERT_EQ(0, prop_count);
                    } else {
                        ASSERT_EQ(cur_dev.display_plane_properties.size(), prop_count);
                        props.resize(prop_count);
                        ASSERT_EQ(VK_SUCCESS,
                                  GetPhysicalDeviceDisplayPlaneProperties(physical_devices[dev], &prop_count, props.data()));
                        ASSERT_EQ(cur_dev.display_plane_properties.size(), prop_count);

                        CompareDisplayPlanePropData(props, cur_dev.display_plane_properties);
                    }
                    found = true;
                    break;
                }
            }
            if (found) {
                break;
            }
        }
    }
}

// Test vkGetDisplayPlaneSupportedDisplaysKHR where nothing supports it.
TEST_F(LoaderInstPhysDevExts, GetDispPlaneSupDispsKHRNoSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.CheckCreate();

    auto GetDisplayPlaneSupportedDisplays = reinterpret_cast<PFN_vkGetDisplayPlaneSupportedDisplaysKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetDisplayPlaneSupportedDisplaysKHR"));
    ASSERT_EQ(GetDisplayPlaneSupportedDisplays, nullptr);
}

// Test vkGetDisplayPlaneSupportedDisplaysKHR where instance supports it, but nothing else.
TEST_F(LoaderInstPhysDevExts, GetDispPlaneSupDispsKHRNoICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_DISPLAY_EXTENSION_NAME);
    instance.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    auto GetDisplayPlaneSupportedDisplays = reinterpret_cast<PFN_vkGetDisplayPlaneSupportedDisplaysKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetDisplayPlaneSupportedDisplaysKHR"));
    ASSERT_EQ(GetDisplayPlaneSupportedDisplays, nullptr);
}

// Fill in random but valid data into the display plane property data struct for the current physical device
static void GenerateRandomDisplays(std::vector<VkDisplayKHR>& disps) {
    disps.resize((rand() % 5) + 1);
    for (uint32_t i = 0; i < disps.size(); ++i) {
        disps[i] = (VkDisplayKHR)((rand() % 0xFFFFFFF) + 1);
    }
}

// Compare the display plane property data structs
static void CompareDisplays(const std::vector<VkDisplayKHR>& disps1, const std::vector<VkDisplayKHR>& disps2) {
    ASSERT_EQ(disps1.size(), disps2.size());
    for (uint32_t i = 0; i < disps1.size(); ++i) {
        ASSERT_EQ(disps1[i], disps2[i]);
    }
}

// Test vGetDisplayPlaneSupportedDisplaysKHR where instance and ICD supports it, but device does not support it.
TEST_F(LoaderInstPhysDevExts, GetDispPlaneSupDispsKHRInstanceAndICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).add_instance_extension({VK_KHR_DISPLAY_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    GenerateRandomDisplays(env.get_test_icd(0).physical_devices.back().displays);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension({VK_KHR_DISPLAY_EXTENSION_NAME});
    instance.CheckCreate();

    auto GetDisplayPlaneSupportedDisplays = reinterpret_cast<PFN_vkGetDisplayPlaneSupportedDisplaysKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetDisplayPlaneSupportedDisplaysKHR"));
    ASSERT_NE(GetDisplayPlaneSupportedDisplays, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    std::vector<VkDisplayKHR> disps{};
    uint32_t disp_count = 0;
    ASSERT_EQ(VK_SUCCESS, GetDisplayPlaneSupportedDisplays(physical_device, 0, &disp_count, nullptr));
    ASSERT_EQ(env.get_test_icd(0).physical_devices.back().displays.size(), disp_count);
    disps.resize(disp_count);
    ASSERT_EQ(VK_SUCCESS, GetDisplayPlaneSupportedDisplays(physical_device, 0, &disp_count, disps.data()));
    ASSERT_EQ(env.get_test_icd(0).physical_devices.back().displays.size(), disp_count);

    CompareDisplays(disps, env.get_test_icd(0).physical_devices.back().displays);
}

// Test vkGetDisplayPlaneSupportedDisplaysKHR where instance supports it with some ICDs that both support
// and don't support it:
//    ICD 0 supports
//        Physical device 0 does not
//        Physical device 1 does
//        Physical device 2 does not
//    ICD 1 doesn't support
//        Physical device 3 does not
//    ICD 2 supports
//        Physical device 4 does not
//        Physical device 5 does not
//    ICD 3 supports
//        Physical device 6 does
TEST_F(LoaderInstPhysDevExts, GetDispPlaneSupDispsKHRMixed) {
    FrameworkEnvironment env{};
    const uint32_t max_icd_count = 4;
    const uint32_t dev_counts[max_icd_count] = {3, 1, 2, 1};
    const uint32_t max_phys_devs = 7;

    for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_0;

        // ICD 1 should not have 1.1
        if (icd != 1) {
            cur_icd.icd_api_version = VK_API_VERSION_1_1;
            cur_icd.add_instance_extension({VK_KHR_DISPLAY_EXTENSION_NAME});
        }

        uint32_t rand_vendor_id;
        uint32_t rand_driver_vers;
        FillInRandomICDInfo(rand_vendor_id, rand_driver_vers);

        for (uint32_t dev = 0; dev < dev_counts[icd]; ++dev) {
            uint32_t device_version = VK_API_VERSION_1_0;
            cur_icd.physical_devices.push_back({});
            auto& cur_dev = cur_icd.physical_devices.back();

            // 2nd device in ICD 0 and the one device in ICD 3 support the extension and 1.1
            if ((icd == 0 && dev == 1) || icd == 3) {
                cur_dev.extensions.push_back({VK_KHR_DISPLAY_EXTENSION_NAME, 0});
                device_version = VK_API_VERSION_1_1;
            }

            // Still set physical device properties (so we can determine if device is correct API version)
            FillInRandomDeviceProps(cur_dev.properties, device_version, rand_vendor_id, rand_driver_vers);
            GenerateRandomDisplays(cur_dev.displays);
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_DISPLAY_EXTENSION_NAME);
    instance.CheckCreate();

    auto GetDisplayPlaneSupportedDisplays = reinterpret_cast<PFN_vkGetDisplayPlaneSupportedDisplaysKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetDisplayPlaneSupportedDisplaysKHR"));
    ASSERT_NE(GetDisplayPlaneSupportedDisplays, nullptr);

    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        VkPhysicalDeviceProperties pd_props{};
        instance->vkGetPhysicalDeviceProperties(physical_devices[dev], &pd_props);

        for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
            auto& cur_icd = env.get_test_icd(icd);
            bool found = false;
            for (uint32_t pd = 0; pd < dev_counts[icd]; ++pd) {
                auto& cur_dev = cur_icd.physical_devices[pd];
                // Find the ICD device matching the physical device we're looking at info for so we can compare the
                // physical devices info with the returned info.
                if (cur_dev.properties.apiVersion == pd_props.apiVersion && cur_dev.properties.deviceID == pd_props.deviceID &&
                    cur_dev.properties.deviceType == pd_props.deviceType &&
                    cur_dev.properties.driverVersion == pd_props.driverVersion &&
                    cur_dev.properties.vendorID == pd_props.vendorID) {
                    std::vector<VkDisplayKHR> disps{};
                    uint32_t disp_count = 0;
                    ASSERT_EQ(VK_SUCCESS, GetDisplayPlaneSupportedDisplays(physical_devices[dev], 0, &disp_count, nullptr));
                    if (icd == 1) {
                        // For this extension, if no support exists (like for ICD 1), the value of 0 should be returned by the
                        // loader.
                        ASSERT_EQ(0, disp_count);
                    } else {
                        ASSERT_EQ(cur_dev.displays.size(), disp_count);
                        disps.resize(disp_count);
                        ASSERT_EQ(VK_SUCCESS,
                                  GetDisplayPlaneSupportedDisplays(physical_devices[dev], 0, &disp_count, disps.data()));
                        ASSERT_EQ(cur_dev.displays.size(), disp_count);

                        CompareDisplays(disps, cur_dev.displays);
                    }
                    found = true;
                    break;
                }
            }
            if (found) {
                break;
            }
        }
    }
}

// Test vkGetDisplayModePropertiesKHR where nothing supports it.
TEST_F(LoaderInstPhysDevExts, GetDispModePropsKHRNoSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.CheckCreate();

    auto GetDisplayModeProperties = reinterpret_cast<PFN_vkGetDisplayModePropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetDisplayModePropertiesKHR"));
    ASSERT_EQ(GetDisplayModeProperties, nullptr);
}

// Test vkGetDisplayModePropertiesKHR where instance supports it, but nothing else.
TEST_F(LoaderInstPhysDevExts, GetDispModePropsKHRNoICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_DISPLAY_EXTENSION_NAME);
    instance.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    auto GetDisplayModeProperties = reinterpret_cast<PFN_vkGetDisplayModePropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetDisplayModePropertiesKHR"));
    ASSERT_EQ(GetDisplayModeProperties, nullptr);
}

// Fill in random but valid data into the display mode properties data struct for the current physical device
static void GenerateRandomDisplayModeProps(std::vector<VkDisplayModePropertiesKHR>& disps) {
    disps.resize((rand() % 5) + 1);
    for (uint32_t i = 0; i < disps.size(); ++i) {
        disps[i].displayMode = (VkDisplayModeKHR)((rand() % 0xFFFFFFF) + 1);
        disps[i].parameters.visibleRegion.width = static_cast<uint32_t>((rand() % 0xFFFFFFF) + 1);
        disps[i].parameters.visibleRegion.height = static_cast<uint32_t>((rand() % 0xFFFFFFF) + 1);
        disps[i].parameters.refreshRate = 1 << (rand() % 8);
    }
}

// Compare the display mode properties data structs
static void CompareDisplayModeProps(const std::vector<VkDisplayModePropertiesKHR>& disps1,
                                    const std::vector<VkDisplayModePropertiesKHR>& disps2) {
    ASSERT_EQ(disps1.size(), disps2.size());
    for (uint32_t i = 0; i < disps1.size(); ++i) {
        ASSERT_EQ(disps1[i].displayMode, disps2[i].displayMode);
        ASSERT_EQ(disps1[i].parameters.visibleRegion.width, disps2[i].parameters.visibleRegion.width);
        ASSERT_EQ(disps1[i].parameters.visibleRegion.height, disps2[i].parameters.visibleRegion.height);
        ASSERT_EQ(disps1[i].parameters.refreshRate, disps2[i].parameters.refreshRate);
    }
}

// Test vGetDisplayModePropertiesKHR where instance and ICD supports it, but device does not support it.
TEST_F(LoaderInstPhysDevExts, GetDispModePropsKHRInstanceAndICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).add_instance_extension({VK_KHR_DISPLAY_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    GenerateRandomDisplayModeProps(env.get_test_icd(0).physical_devices.back().display_mode_properties);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension({VK_KHR_DISPLAY_EXTENSION_NAME});
    instance.CheckCreate();

    auto GetDisplayModeProperties = reinterpret_cast<PFN_vkGetDisplayModePropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetDisplayModePropertiesKHR"));
    ASSERT_NE(GetDisplayModeProperties, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    std::vector<VkDisplayModePropertiesKHR> props{};
    uint32_t props_count = 0;
    ASSERT_EQ(VK_SUCCESS, GetDisplayModeProperties(physical_device, VK_NULL_HANDLE, &props_count, nullptr));
    ASSERT_EQ(env.get_test_icd(0).physical_devices.back().display_mode_properties.size(), props_count);
    props.resize(props_count);
    ASSERT_EQ(VK_SUCCESS, GetDisplayModeProperties(physical_device, VK_NULL_HANDLE, &props_count, props.data()));
    ASSERT_EQ(env.get_test_icd(0).physical_devices.back().display_mode_properties.size(), props_count);

    CompareDisplayModeProps(props, env.get_test_icd(0).physical_devices.back().display_mode_properties);
}

// Test vkGetDisplayModePropertiesKHR where instance supports it with some ICDs that both support
// and don't support it:
//    ICD 0 supports
//        Physical device 0 does not
//        Physical device 1 does
//        Physical device 2 does not
//    ICD 1 doesn't support
//        Physical device 3 does not
//    ICD 2 supports
//        Physical device 4 does not
//        Physical device 5 does not
//    ICD 3 supports
//        Physical device 6 does
TEST_F(LoaderInstPhysDevExts, GetDispModePropsKHRMixed) {
    FrameworkEnvironment env{};
    const uint32_t max_icd_count = 4;
    const uint32_t dev_counts[max_icd_count] = {3, 1, 2, 1};
    const uint32_t max_phys_devs = 7;

    for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_0;

        // ICD 1 should not have 1.1
        if (icd != 1) {
            cur_icd.icd_api_version = VK_API_VERSION_1_1;
            cur_icd.add_instance_extension({VK_KHR_DISPLAY_EXTENSION_NAME});
        }

        uint32_t rand_vendor_id;
        uint32_t rand_driver_vers;
        FillInRandomICDInfo(rand_vendor_id, rand_driver_vers);

        for (uint32_t dev = 0; dev < dev_counts[icd]; ++dev) {
            uint32_t device_version = VK_API_VERSION_1_0;
            cur_icd.physical_devices.push_back({});
            auto& cur_dev = cur_icd.physical_devices.back();

            // 2nd device in ICD 0 and the one device in ICD 3 support the extension and 1.1
            if ((icd == 0 && dev == 1) || icd == 3) {
                cur_dev.extensions.push_back({VK_KHR_DISPLAY_EXTENSION_NAME, 0});
                device_version = VK_API_VERSION_1_1;
            }

            // Still set physical device properties (so we can determine if device is correct API version)
            FillInRandomDeviceProps(cur_dev.properties, device_version, rand_vendor_id, rand_driver_vers);
            GenerateRandomDisplayModeProps(cur_dev.display_mode_properties);
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_DISPLAY_EXTENSION_NAME);
    instance.CheckCreate();

    auto GetDisplayModeProperties = reinterpret_cast<PFN_vkGetDisplayModePropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetDisplayModePropertiesKHR"));
    ASSERT_NE(GetDisplayModeProperties, nullptr);

    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        VkPhysicalDeviceProperties pd_props{};
        instance->vkGetPhysicalDeviceProperties(physical_devices[dev], &pd_props);

        for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
            auto& cur_icd = env.get_test_icd(icd);
            bool found = false;
            for (uint32_t pd = 0; pd < dev_counts[icd]; ++pd) {
                auto& cur_dev = cur_icd.physical_devices[pd];
                // Find the ICD device matching the physical device we're looking at info for so we can compare the
                // physical devices info with the returned info.
                if (cur_dev.properties.apiVersion == pd_props.apiVersion && cur_dev.properties.deviceID == pd_props.deviceID &&
                    cur_dev.properties.deviceType == pd_props.deviceType &&
                    cur_dev.properties.driverVersion == pd_props.driverVersion &&
                    cur_dev.properties.vendorID == pd_props.vendorID) {
                    uint32_t props_count = 0;
                    ASSERT_EQ(VK_SUCCESS, GetDisplayModeProperties(physical_devices[dev], VK_NULL_HANDLE, &props_count, nullptr));
                    if (icd == 1) {
                        // For this extension, if no support exists (like for ICD 1), the value of 0 should be returned by the
                        // loader.
                        ASSERT_EQ(0, props_count);
                    } else {
                        std::vector<VkDisplayModePropertiesKHR> props{};
                        ASSERT_EQ(cur_dev.display_mode_properties.size(), props_count);
                        props.resize(props_count);
                        ASSERT_EQ(VK_SUCCESS,
                                  GetDisplayModeProperties(physical_devices[dev], VK_NULL_HANDLE, &props_count, props.data()));
                        ASSERT_EQ(cur_dev.display_mode_properties.size(), props_count);

                        CompareDisplayModeProps(props, cur_dev.display_mode_properties);
                    }
                    found = true;
                    break;
                }
            }
            if (found) {
                break;
            }
        }
    }
}

// Test vkCreateDisplayModeKHR where nothing supports it.
TEST_F(LoaderInstPhysDevExts, GetDispModesKHRNoSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.CheckCreate();

    auto CreateDisplayMode =
        reinterpret_cast<PFN_vkCreateDisplayModeKHR>(instance.functions->vkGetInstanceProcAddr(instance, "vkCreateDisplayModeKHR"));
    ASSERT_EQ(CreateDisplayMode, nullptr);
}

// Test vkCreateDisplayModeKHR where instance supports it, but nothing else.
TEST_F(LoaderInstPhysDevExts, GetDispModesKHRNoICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_DISPLAY_EXTENSION_NAME);
    instance.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    auto CreateDisplayMode =
        reinterpret_cast<PFN_vkCreateDisplayModeKHR>(instance.functions->vkGetInstanceProcAddr(instance, "vkCreateDisplayModeKHR"));
    ASSERT_EQ(CreateDisplayMode, nullptr);
}

// Fill in random but valid data into the display modes for the current physical device
static void GenerateRandomDisplayMode(VkDisplayModeKHR& mode) { mode = (VkDisplayModeKHR)((rand() % 0xFFFFFFF) + 1); }

// Compare the display modes
static void CompareDisplayModes(const VkDisplayModeKHR& disps1, VkDisplayModeKHR& disps2) { ASSERT_EQ(disps1, disps2); }

// Test vkCreateDisplayModeKHR where instance and ICD supports it, but device does not support it.
TEST_F(LoaderInstPhysDevExts, GetDispModesKHRInstanceAndICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).add_instance_extension({VK_KHR_DISPLAY_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    GenerateRandomDisplayMode(env.get_test_icd(0).physical_devices.back().display_mode);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension({VK_KHR_DISPLAY_EXTENSION_NAME});
    instance.CheckCreate();

    auto CreateDisplayMode =
        reinterpret_cast<PFN_vkCreateDisplayModeKHR>(instance.functions->vkGetInstanceProcAddr(instance, "vkCreateDisplayModeKHR"));
    ASSERT_NE(CreateDisplayMode, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    VkDisplayModeKHR mode{};
    VkDisplayModeCreateInfoKHR create_info{VK_STRUCTURE_TYPE_DISPLAY_MODE_CREATE_INFO_KHR};
    ASSERT_EQ(VK_SUCCESS, CreateDisplayMode(physical_device, VK_NULL_HANDLE, &create_info, nullptr, &mode));
    CompareDisplayModes(mode, env.get_test_icd(0).physical_devices.back().display_mode);
}

// Test vkCreateDisplayModeKHR where instance supports it with some ICDs that both support
// and don't support it:
//    ICD 0 supports
//        Physical device 0 does not
//        Physical device 1 does
//        Physical device 2 does not
//    ICD 1 doesn't support
//        Physical device 3 does not
//    ICD 2 supports
//        Physical device 4 does not
//        Physical device 5 does not
//    ICD 3 supports
//        Physical device 6 does
TEST_F(LoaderInstPhysDevExts, GetDispModesKHRMixed) {
    FrameworkEnvironment env{};
    const uint32_t max_icd_count = 4;
    const uint32_t dev_counts[max_icd_count] = {3, 1, 2, 1};
    const uint32_t max_phys_devs = 7;

    for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_0;

        // ICD 1 should not have 1.1
        if (icd != 1) {
            cur_icd.icd_api_version = VK_API_VERSION_1_1;
            cur_icd.add_instance_extension({VK_KHR_DISPLAY_EXTENSION_NAME});
        }

        uint32_t rand_vendor_id;
        uint32_t rand_driver_vers;
        FillInRandomICDInfo(rand_vendor_id, rand_driver_vers);

        for (uint32_t dev = 0; dev < dev_counts[icd]; ++dev) {
            uint32_t device_version = VK_API_VERSION_1_0;
            cur_icd.physical_devices.push_back({});
            auto& cur_dev = cur_icd.physical_devices.back();

            // 2nd device in ICD 0 and the one device in ICD 3 support the extension and 1.1
            if ((icd == 0 && dev == 1) || icd == 3) {
                cur_dev.extensions.push_back({VK_KHR_DISPLAY_EXTENSION_NAME, 0});
                device_version = VK_API_VERSION_1_1;
            }

            // Still set physical device properties (so we can determine if device is correct API version)
            FillInRandomDeviceProps(cur_dev.properties, device_version, rand_vendor_id, rand_driver_vers);
            GenerateRandomDisplayMode(cur_dev.display_mode);
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_DISPLAY_EXTENSION_NAME);
    instance.CheckCreate();

    auto CreateDisplayMode =
        reinterpret_cast<PFN_vkCreateDisplayModeKHR>(instance.functions->vkGetInstanceProcAddr(instance, "vkCreateDisplayModeKHR"));
    ASSERT_NE(CreateDisplayMode, nullptr);

    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        VkPhysicalDeviceProperties pd_props{};
        instance->vkGetPhysicalDeviceProperties(physical_devices[dev], &pd_props);

        for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
            auto& cur_icd = env.get_test_icd(icd);
            bool found = false;
            for (uint32_t pd = 0; pd < dev_counts[icd]; ++pd) {
                auto& cur_dev = cur_icd.physical_devices[pd];
                // Find the ICD device matching the physical device we're looking at info for so we can compare the
                // physical devices info with the returned info.
                if (cur_dev.properties.apiVersion == pd_props.apiVersion && cur_dev.properties.deviceID == pd_props.deviceID &&
                    cur_dev.properties.deviceType == pd_props.deviceType &&
                    cur_dev.properties.driverVersion == pd_props.driverVersion &&
                    cur_dev.properties.vendorID == pd_props.vendorID) {
                    VkDisplayModeKHR mode{};
                    VkDisplayModeCreateInfoKHR create_info{VK_STRUCTURE_TYPE_DISPLAY_MODE_CREATE_INFO_KHR};
                    if (icd == 1) {
                        // Unsupported ICD should return initialization failed (instead of crash)
                        ASSERT_EQ(VK_ERROR_INITIALIZATION_FAILED,
                                  CreateDisplayMode(physical_devices[dev], VK_NULL_HANDLE, &create_info, nullptr, &mode));
                    } else {
                        ASSERT_EQ(VK_SUCCESS,
                                  CreateDisplayMode(physical_devices[dev], VK_NULL_HANDLE, &create_info, nullptr, &mode));
                        CompareDisplayModes(mode, cur_dev.display_mode);
                    }
                    found = true;
                    break;
                }
            }
            if (found) {
                break;
            }
        }
    }
}

// Test vkGetDisplayPlaneCapabilitiesKHR where nothing supports it.
TEST_F(LoaderInstPhysDevExts, GetDispPlaneCapsKHRNoSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.CheckCreate();

    auto GetDisplayPlaneCapabilities = reinterpret_cast<PFN_vkGetDisplayPlaneCapabilitiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetDisplayPlaneCapabilitiesKHR"));
    ASSERT_EQ(GetDisplayPlaneCapabilities, nullptr);
}

// Test vkGetDisplayPlaneCapabilitiesKHR where instance supports it, but nothing else.
TEST_F(LoaderInstPhysDevExts, GetDispPlaneCapsKHRNoICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_DISPLAY_EXTENSION_NAME);
    instance.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    auto GetDisplayPlaneCapabilities = reinterpret_cast<PFN_vkGetDisplayPlaneCapabilitiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetDisplayPlaneCapabilitiesKHR"));
    ASSERT_EQ(GetDisplayPlaneCapabilities, nullptr);
}

// Fill in random but valid data into the display plane caps for the current physical device
static void GenerateRandomDisplayPlaneCaps(VkDisplayPlaneCapabilitiesKHR& caps) {
    caps.supportedAlpha = static_cast<VkDisplayPlaneAlphaFlagsKHR>((rand() % 0xFFFFFFF) + 1);
    caps.minSrcPosition.x = static_cast<uint32_t>((rand() % 0xFFFFFFF) + 1);
    caps.minSrcPosition.y = static_cast<uint32_t>((rand() % 0xFFFFFFF) + 1);
    caps.maxSrcPosition.x = static_cast<uint32_t>((rand() % 0xFFFFFFF) + 1);
    caps.maxSrcPosition.y = static_cast<uint32_t>((rand() % 0xFFFFFFF) + 1);
    caps.minSrcExtent.width = static_cast<uint32_t>((rand() % 0xFFFFFFF) + 1);
    caps.minSrcExtent.height = static_cast<uint32_t>((rand() % 0xFFFFFFF) + 1);
    caps.maxSrcExtent.width = static_cast<uint32_t>((rand() % 0xFFFFFFF) + 1);
    caps.maxSrcExtent.height = static_cast<uint32_t>((rand() % 0xFFFFFFF) + 1);
    caps.minDstPosition.x = static_cast<uint32_t>((rand() % 0xFFFFFFF) + 1);
    caps.minDstPosition.y = static_cast<uint32_t>((rand() % 0xFFFFFFF) + 1);
    caps.maxDstPosition.x = static_cast<uint32_t>((rand() % 0xFFFFFFF) + 1);
    caps.maxDstPosition.y = static_cast<uint32_t>((rand() % 0xFFFFFFF) + 1);
    caps.minDstExtent.width = static_cast<uint32_t>((rand() % 0xFFFFFFF) + 1);
    caps.minDstExtent.height = static_cast<uint32_t>((rand() % 0xFFFFFFF) + 1);
    caps.maxDstExtent.width = static_cast<uint32_t>((rand() % 0xFFFFFFF) + 1);
    caps.maxDstExtent.height = static_cast<uint32_t>((rand() % 0xFFFFFFF) + 1);
}

// Compare the display plane caps
static void CompareDisplayPlaneCaps(const VkDisplayPlaneCapabilitiesKHR& caps1, VkDisplayPlaneCapabilitiesKHR& caps2,
                                    bool supported = true) {
    if (supported) {
        ASSERT_EQ(caps1.supportedAlpha, caps2.supportedAlpha);
        ASSERT_EQ(caps1.minSrcPosition.x, caps2.minSrcPosition.x);
        ASSERT_EQ(caps1.minSrcPosition.y, caps2.minSrcPosition.y);
        ASSERT_EQ(caps1.maxSrcPosition.x, caps2.maxSrcPosition.x);
        ASSERT_EQ(caps1.maxSrcPosition.y, caps2.maxSrcPosition.y);
        ASSERT_EQ(caps1.minSrcExtent.width, caps2.minSrcExtent.width);
        ASSERT_EQ(caps1.minSrcExtent.height, caps2.minSrcExtent.height);
        ASSERT_EQ(caps1.maxSrcExtent.width, caps2.maxSrcExtent.width);
        ASSERT_EQ(caps1.maxSrcExtent.height, caps2.maxSrcExtent.height);
        ASSERT_EQ(caps1.minDstPosition.x, caps2.minDstPosition.x);
        ASSERT_EQ(caps1.minDstPosition.y, caps2.minDstPosition.y);
        ASSERT_EQ(caps1.maxDstPosition.x, caps2.maxDstPosition.x);
        ASSERT_EQ(caps1.maxDstPosition.y, caps2.maxDstPosition.y);
        ASSERT_EQ(caps1.minDstExtent.width, caps2.minDstExtent.width);
        ASSERT_EQ(caps1.minDstExtent.height, caps2.minDstExtent.height);
        ASSERT_EQ(caps1.maxDstExtent.width, caps2.maxDstExtent.width);
        ASSERT_EQ(caps1.maxDstExtent.height, caps2.maxDstExtent.height);
    } else {
        ASSERT_EQ(caps1.supportedAlpha, 0);
        ASSERT_EQ(caps1.minSrcPosition.x, 0);
        ASSERT_EQ(caps1.minSrcPosition.y, 0);
        ASSERT_EQ(caps1.maxSrcPosition.x, 0);
        ASSERT_EQ(caps1.maxSrcPosition.y, 0);
        ASSERT_EQ(caps1.minSrcExtent.width, 0);
        ASSERT_EQ(caps1.minSrcExtent.height, 0);
        ASSERT_EQ(caps1.maxSrcExtent.width, 0);
        ASSERT_EQ(caps1.maxSrcExtent.height, 0);
        ASSERT_EQ(caps1.minDstPosition.x, 0);
        ASSERT_EQ(caps1.minDstPosition.y, 0);
        ASSERT_EQ(caps1.maxDstPosition.x, 0);
        ASSERT_EQ(caps1.maxDstPosition.y, 0);
        ASSERT_EQ(caps1.minDstExtent.width, 0);
        ASSERT_EQ(caps1.minDstExtent.height, 0);
        ASSERT_EQ(caps1.maxDstExtent.width, 0);
        ASSERT_EQ(caps1.maxDstExtent.height, 0);
    }
}

// Test vkGetDisplayPlaneCapabilitiesKHR where instance and ICD supports it, but device does not support it.
TEST_F(LoaderInstPhysDevExts, GetDispPlaneCapsKHRInstanceAndICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).add_instance_extension({VK_KHR_DISPLAY_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({});
    GenerateRandomDisplayPlaneCaps(env.get_test_icd(0).physical_devices.back().display_plane_capabilities);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension({VK_KHR_DISPLAY_EXTENSION_NAME});
    instance.CheckCreate();

    auto GetDisplayPlaneCapabilities = reinterpret_cast<PFN_vkGetDisplayPlaneCapabilitiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetDisplayPlaneCapabilitiesKHR"));
    ASSERT_NE(GetDisplayPlaneCapabilities, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    VkDisplayPlaneCapabilitiesKHR caps{};
    ASSERT_EQ(VK_SUCCESS, GetDisplayPlaneCapabilities(physical_device, 0, 0, &caps));
    CompareDisplayPlaneCaps(caps, env.get_test_icd(0).physical_devices.back().display_plane_capabilities);
}

// Test vkGetDisplayPlaneCapabilitiesKHR where instance supports it with some ICDs that both support
// and don't support it:
//    ICD 0 supports
//        Physical device 0 does not
//        Physical device 1 does
//        Physical device 2 does not
//    ICD 1 doesn't support
//        Physical device 3 does not
//    ICD 2 supports
//        Physical device 4 does not
//        Physical device 5 does not
//    ICD 3 supports
//        Physical device 6 does
TEST_F(LoaderInstPhysDevExts, GetDispPlaneCapsKHRMixed) {
    FrameworkEnvironment env{};
    const uint32_t max_icd_count = 4;
    const uint32_t dev_counts[max_icd_count] = {3, 1, 2, 1};
    const uint32_t max_phys_devs = 7;

    for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_0;

        // ICD 1 should not have 1.1
        if (icd != 1) {
            cur_icd.icd_api_version = VK_API_VERSION_1_1;
            cur_icd.add_instance_extension({VK_KHR_DISPLAY_EXTENSION_NAME});
        }

        uint32_t rand_vendor_id;
        uint32_t rand_driver_vers;
        FillInRandomICDInfo(rand_vendor_id, rand_driver_vers);

        for (uint32_t dev = 0; dev < dev_counts[icd]; ++dev) {
            uint32_t device_version = VK_API_VERSION_1_0;
            cur_icd.physical_devices.push_back({});
            auto& cur_dev = cur_icd.physical_devices.back();

            // 2nd device in ICD 0 and the one device in ICD 3 support the extension and 1.1
            if ((icd == 0 && dev == 1) || icd == 3) {
                cur_dev.extensions.push_back({VK_KHR_DISPLAY_EXTENSION_NAME, 0});
                device_version = VK_API_VERSION_1_1;
            }

            // Still set physical device properties (so we can determine if device is correct API version)
            FillInRandomDeviceProps(cur_dev.properties, device_version, rand_vendor_id, rand_driver_vers);
            GenerateRandomDisplayPlaneCaps(cur_dev.display_plane_capabilities);
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_DISPLAY_EXTENSION_NAME);
    instance.CheckCreate();

    auto GetDisplayPlaneCapabilities = reinterpret_cast<PFN_vkGetDisplayPlaneCapabilitiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetDisplayPlaneCapabilitiesKHR"));
    ASSERT_NE(GetDisplayPlaneCapabilities, nullptr);

    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        VkPhysicalDeviceProperties pd_props{};
        instance->vkGetPhysicalDeviceProperties(physical_devices[dev], &pd_props);

        for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
            auto& cur_icd = env.get_test_icd(icd);
            bool found = false;
            for (uint32_t pd = 0; pd < dev_counts[icd]; ++pd) {
                auto& cur_dev = cur_icd.physical_devices[pd];
                // Find the ICD device matching the physical device we're looking at info for so we can compare the
                // physical devices info with the returned info.
                if (cur_dev.properties.apiVersion == pd_props.apiVersion && cur_dev.properties.deviceID == pd_props.deviceID &&
                    cur_dev.properties.deviceType == pd_props.deviceType &&
                    cur_dev.properties.driverVersion == pd_props.driverVersion &&
                    cur_dev.properties.vendorID == pd_props.vendorID) {
                    VkDisplayPlaneCapabilitiesKHR caps{};
                    ASSERT_EQ(VK_SUCCESS, GetDisplayPlaneCapabilities(physical_devices[dev], 0, 0, &caps));
                    CompareDisplayPlaneCaps(caps, cur_dev.display_plane_capabilities, icd != 1);
                    found = true;
                    break;
                }
            }
            if (found) {
                break;
            }
        }
    }
}

//
// VK_KHR_get_display_properties2
//

// Test vkGetPhysicalDeviceDisplayProperties2KHR where nothing supports it.
TEST_F(LoaderInstPhysDevExts, PhysDevDispProps2KHRNoSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.CheckCreate();

    auto GetPhysicalDeviceDisplayProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayProperties2KHR"));
    ASSERT_EQ(GetPhysicalDeviceDisplayProperties2, nullptr);
}

// Test vkGetPhysicalDeviceDisplayProperties2KHR where instance supports it, but nothing else.
TEST_F(LoaderInstPhysDevExts, PhysDevDispProps2KHRNoICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME);
    instance.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    auto GetPhysicalDeviceDisplayProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayProperties2KHR"));
    ASSERT_EQ(GetPhysicalDeviceDisplayProperties2, nullptr);
}

// Compare the display property data structs
static void CompareDisplayPropData(const std::vector<VkDisplayPropertiesKHR>& props1,
                                   const std::vector<VkDisplayProperties2KHR>& props2) {
    ASSERT_EQ(props1.size(), props2.size());
    for (uint32_t i = 0; i < props1.size(); ++i) {
        ASSERT_EQ(props1[i].display, props2[i].displayProperties.display);
        ASSERT_EQ(props1[i].physicalDimensions.width, props2[i].displayProperties.physicalDimensions.width);
        ASSERT_EQ(props1[i].physicalDimensions.height, props2[i].displayProperties.physicalDimensions.height);
        ASSERT_EQ(props1[i].physicalResolution.width, props2[i].displayProperties.physicalResolution.width);
        ASSERT_EQ(props1[i].physicalResolution.height, props2[i].displayProperties.physicalResolution.height);
        ASSERT_EQ(props1[i].supportedTransforms, props2[i].displayProperties.supportedTransforms);
        ASSERT_EQ(props1[i].planeReorderPossible, props2[i].displayProperties.planeReorderPossible);
        ASSERT_EQ(props1[i].persistentContent, props2[i].displayProperties.persistentContent);
    }
}

// Test vGetPhysicalDeviceDisplayProperties2KHR where instance and ICD supports it, but device does not support it.
TEST_F(LoaderInstPhysDevExts, PhysDevDispProps2KHRInstanceAndICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    Extension first_ext{VK_KHR_DISPLAY_EXTENSION_NAME};
    Extension second_ext{VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME};
    env.get_test_icd(0).add_instance_extensions({first_ext, second_ext});
    env.get_test_icd(0).physical_devices.push_back({});
    FillInRandomDisplayPropData(env.get_test_icd(0).physical_devices.back().display_properties);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extensions({VK_KHR_DISPLAY_EXTENSION_NAME, VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME});
    instance.CheckCreate();

    auto GetPhysicalDeviceDisplayProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayPropertiesKHR"));
    ASSERT_NE(GetPhysicalDeviceDisplayProperties, nullptr);
    auto GetPhysicalDeviceDisplayProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayProperties2KHR"));
    ASSERT_NE(GetPhysicalDeviceDisplayProperties2, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    std::vector<VkDisplayPropertiesKHR> props{};
    uint32_t prop_count = 0;
    ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceDisplayProperties(physical_device, &prop_count, nullptr));
    ASSERT_NE(0, prop_count);
    props.resize(prop_count);
    ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceDisplayProperties(physical_device, &prop_count, props.data()));

    std::vector<VkDisplayProperties2KHR> props2{};
    uint32_t prop_count_2 = 0;
    ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceDisplayProperties2(physical_device, &prop_count_2, nullptr));
    ASSERT_EQ(prop_count, prop_count_2);
    props2.resize(prop_count_2, {VK_STRUCTURE_TYPE_DISPLAY_PROPERTIES_2_KHR});
    ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceDisplayProperties2(physical_device, &prop_count_2, props2.data()));
    ASSERT_EQ(prop_count, prop_count_2);

    CompareDisplayPropData(props, props2);
}

// Test vkGetPhysicalDeviceDisplayProperties2KHR where instance supports it with some ICDs that both support
// and don't support it:
//    ICD 0 supports
//        Physical device 0 does not
//        Physical device 1 does
//        Physical device 2 does not
//    ICD 1 doesn't support
//        Physical device 3 does not
//    ICD 2 supports
//        Physical device 4 does not
//        Physical device 5 does not
//    ICD 3 supports
//        Physical device 6 does
TEST_F(LoaderInstPhysDevExts, PhysDevDispProps2KHRMixed) {
    FrameworkEnvironment env{};
    const uint32_t max_icd_count = 4;
    const uint32_t dev_counts[max_icd_count] = {3, 1, 2, 1};
    const uint32_t max_phys_devs = 7;

    for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_0;
        cur_icd.add_instance_extension({VK_KHR_DISPLAY_EXTENSION_NAME});

        // ICD 1 should not have 1.1
        if (icd != 1) {
            cur_icd.icd_api_version = VK_API_VERSION_1_1;
            cur_icd.add_instance_extension({VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME});
        }

        uint32_t rand_vendor_id;
        uint32_t rand_driver_vers;
        FillInRandomICDInfo(rand_vendor_id, rand_driver_vers);

        for (uint32_t dev = 0; dev < dev_counts[icd]; ++dev) {
            uint32_t device_version = VK_API_VERSION_1_0;
            cur_icd.physical_devices.push_back({});
            auto& cur_dev = cur_icd.physical_devices.back();
            cur_dev.extensions.push_back({VK_KHR_DISPLAY_EXTENSION_NAME, 0});

            // 2nd device in ICD 0 and the one device in ICD 3 support the extension and 1.1
            if ((icd == 0 && dev == 1) || icd == 3) {
                cur_dev.extensions.push_back({VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME, 0});
                device_version = VK_API_VERSION_1_1;
            }

            // Still set physical device properties (so we can determine if device is correct API version)
            FillInRandomDeviceProps(cur_dev.properties, device_version, rand_vendor_id, rand_driver_vers);
            FillInRandomDisplayPropData(cur_dev.display_properties);
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extensions({VK_KHR_DISPLAY_EXTENSION_NAME, VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME});
    instance.CheckCreate();

    auto GetPhysicalDeviceDisplayProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayPropertiesKHR"));
    ASSERT_NE(GetPhysicalDeviceDisplayProperties, nullptr);
    auto GetPhysicalDeviceDisplayProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayProperties2KHR"));
    ASSERT_NE(GetPhysicalDeviceDisplayProperties2, nullptr);

    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        std::vector<VkDisplayPropertiesKHR> props{};
        uint32_t prop_count = 0;
        ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceDisplayProperties(physical_devices[dev], &prop_count, nullptr));
        ASSERT_NE(0, prop_count);
        props.resize(prop_count);
        ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceDisplayProperties(physical_devices[dev], &prop_count, props.data()));

        std::vector<VkDisplayProperties2KHR> props2{};
        uint32_t prop_count_2 = 0;
        ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceDisplayProperties2(physical_devices[dev], &prop_count_2, nullptr));
        ASSERT_EQ(prop_count, prop_count_2);
        props2.resize(prop_count_2, {VK_STRUCTURE_TYPE_DISPLAY_PROPERTIES_2_KHR});
        ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceDisplayProperties2(physical_devices[dev], &prop_count_2, props2.data()));
        ASSERT_EQ(prop_count, prop_count_2);

        CompareDisplayPropData(props, props2);
    }
}

// Test vkGetPhysicalDeviceDisplayPlaneProperties2KHR where nothing supports it.
TEST_F(LoaderInstPhysDevExts, PhysDevDispPlaneProps2KHRNoSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.CheckCreate();

    auto GetPhysicalDeviceDisplayPlaneProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayPlaneProperties2KHR"));
    ASSERT_EQ(GetPhysicalDeviceDisplayPlaneProperties2, nullptr);
}

// Test vkGetPhysicalDeviceDisplayPlaneProperties2KHR where instance supports it, but nothing else.
TEST_F(LoaderInstPhysDevExts, PhysDevDispPlaneProps2KHRNoICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME);
    instance.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    auto GetPhysicalDeviceDisplayPlaneProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayPlaneProperties2KHR"));
    ASSERT_EQ(GetPhysicalDeviceDisplayPlaneProperties2, nullptr);
}

// Compare the display plane property data structs
static void CompareDisplayPlanePropData(const std::vector<VkDisplayPlanePropertiesKHR>& props1,
                                        const std::vector<VkDisplayPlaneProperties2KHR>& props2) {
    ASSERT_EQ(props1.size(), props2.size());
    for (uint32_t i = 0; i < props1.size(); ++i) {
        ASSERT_EQ(props1[i].currentDisplay, props2[i].displayPlaneProperties.currentDisplay);
        ASSERT_EQ(props1[i].currentStackIndex, props2[i].displayPlaneProperties.currentStackIndex);
    }
}

// Test vGetPhysicalDeviceDisplayPlaneProperties2KHR where instance and ICD supports it, but device does not support it.
TEST_F(LoaderInstPhysDevExts, PhysDevDispPlaneProps2KHRInstanceAndICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    Extension first_ext{VK_KHR_DISPLAY_EXTENSION_NAME};
    Extension second_ext{VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME};
    env.get_test_icd(0).add_instance_extensions({first_ext, second_ext});
    env.get_test_icd(0).physical_devices.push_back({});
    FillInRandomDisplayPlanePropData(env.get_test_icd(0).physical_devices.back().display_plane_properties);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extensions({VK_KHR_DISPLAY_EXTENSION_NAME, VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME});
    instance.CheckCreate();

    auto GetPhysicalDeviceDisplayPlaneProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR"));
    ASSERT_NE(GetPhysicalDeviceDisplayPlaneProperties, nullptr);
    auto GetPhysicalDeviceDisplayPlaneProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayPlaneProperties2KHR"));
    ASSERT_NE(GetPhysicalDeviceDisplayPlaneProperties2, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    std::vector<VkDisplayPlanePropertiesKHR> props{};
    uint32_t prop_count = 0;
    ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceDisplayPlaneProperties(physical_device, &prop_count, nullptr));
    ASSERT_NE(0, prop_count);
    props.resize(prop_count);
    ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceDisplayPlaneProperties(physical_device, &prop_count, props.data()));

    std::vector<VkDisplayPlaneProperties2KHR> props2{};
    uint32_t prop_count2 = 0;
    ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceDisplayPlaneProperties2(physical_device, &prop_count2, nullptr));
    ASSERT_EQ(prop_count, prop_count2);
    props2.resize(prop_count2, {VK_STRUCTURE_TYPE_DISPLAY_PLANE_PROPERTIES_2_KHR});
    ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceDisplayPlaneProperties2(physical_device, &prop_count2, props2.data()));

    CompareDisplayPlanePropData(props, props2);
}

// Test vkGetPhysicalDeviceDisplayPlaneProperties2KHR where instance supports it with some ICDs that both support
// and don't support it:
//    ICD 0 supports
//        Physical device 0 does not
//        Physical device 1 does
//        Physical device 2 does not
//    ICD 1 doesn't support
//        Physical device 3 does not
//    ICD 2 supports
//        Physical device 4 does not
//        Physical device 5 does not
//    ICD 3 supports
//        Physical device 6 does
TEST_F(LoaderInstPhysDevExts, PhysDevDispPlaneProps2KHRMixed) {
    FrameworkEnvironment env{};
    const uint32_t max_icd_count = 4;
    const uint32_t dev_counts[max_icd_count] = {3, 1, 2, 1};
    const uint32_t max_phys_devs = 7;

    for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_0;
        cur_icd.add_instance_extension({VK_KHR_DISPLAY_EXTENSION_NAME});

        // ICD 1 should not have 1.1
        if (icd != 1) {
            cur_icd.icd_api_version = VK_API_VERSION_1_1;
            cur_icd.add_instance_extension({VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME});
        }

        uint32_t rand_vendor_id;
        uint32_t rand_driver_vers;
        FillInRandomICDInfo(rand_vendor_id, rand_driver_vers);

        for (uint32_t dev = 0; dev < dev_counts[icd]; ++dev) {
            uint32_t device_version = VK_API_VERSION_1_0;
            cur_icd.physical_devices.push_back({});
            auto& cur_dev = cur_icd.physical_devices.back();
            cur_dev.extensions.push_back({VK_KHR_DISPLAY_EXTENSION_NAME, 0});

            // 2nd device in ICD 0 and the one device in ICD 3 support the extension and 1.1
            if ((icd == 0 && dev == 1) || icd == 3) {
                cur_dev.extensions.push_back({VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME, 0});
                device_version = VK_API_VERSION_1_1;
            }

            // Still set physical device properties (so we can determine if device is correct API version)
            FillInRandomDeviceProps(cur_dev.properties, device_version, rand_vendor_id, rand_driver_vers);
            FillInRandomDisplayPlanePropData(cur_dev.display_plane_properties);
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extensions({VK_KHR_DISPLAY_EXTENSION_NAME, VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME});
    instance.CheckCreate();

    auto GetPhysicalDeviceDisplayPlaneProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR"));
    ASSERT_NE(GetPhysicalDeviceDisplayPlaneProperties, nullptr);
    auto GetPhysicalDeviceDisplayPlaneProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayPlaneProperties2KHR"));
    ASSERT_NE(GetPhysicalDeviceDisplayPlaneProperties2, nullptr);

    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        std::vector<VkDisplayPlanePropertiesKHR> props{};
        uint32_t prop_count = 0;
        ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceDisplayPlaneProperties(physical_devices[dev], &prop_count, nullptr));
        ASSERT_NE(0, prop_count);
        props.resize(prop_count);
        ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceDisplayPlaneProperties(physical_devices[dev], &prop_count, props.data()));

        std::vector<VkDisplayPlaneProperties2KHR> props2{};
        uint32_t prop_count2 = 0;
        ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceDisplayPlaneProperties2(physical_devices[dev], &prop_count2, nullptr));
        ASSERT_EQ(prop_count, prop_count2);
        props2.resize(prop_count2, {VK_STRUCTURE_TYPE_DISPLAY_PLANE_PROPERTIES_2_KHR});
        ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceDisplayPlaneProperties2(physical_devices[dev], &prop_count2, props2.data()));

        CompareDisplayPlanePropData(props, props2);
    }
}

// Test vkGetDisplayModeProperties2KHR where nothing supports it.
TEST_F(LoaderInstPhysDevExts, GetDispModeProps2KHRNoSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.CheckCreate();

    auto GetDisplayModeProperties2 = reinterpret_cast<PFN_vkGetDisplayModeProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetDisplayModeProperties2KHR"));
    ASSERT_EQ(GetDisplayModeProperties2, nullptr);
}

// Test vkGetDisplayModeProperties2KHR where instance supports it, but nothing else.
TEST_F(LoaderInstPhysDevExts, GetDispModeProps2KHRNoICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME);
    instance.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    auto GetDisplayModeProperties2 = reinterpret_cast<PFN_vkGetDisplayModeProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetDisplayModeProperties2KHR"));
    ASSERT_EQ(GetDisplayModeProperties2, nullptr);
}

// Compare the display mode properties data structs
static void CompareDisplayModeProps(const std::vector<VkDisplayModePropertiesKHR>& disps1,
                                    const std::vector<VkDisplayModeProperties2KHR>& disps2) {
    ASSERT_EQ(disps1.size(), disps2.size());
    for (uint32_t i = 0; i < disps1.size(); ++i) {
        ASSERT_EQ(disps1[i].displayMode, disps2[i].displayModeProperties.displayMode);
        ASSERT_EQ(disps1[i].parameters.visibleRegion.width, disps2[i].displayModeProperties.parameters.visibleRegion.width);
        ASSERT_EQ(disps1[i].parameters.visibleRegion.height, disps2[i].displayModeProperties.parameters.visibleRegion.height);
        ASSERT_EQ(disps1[i].parameters.refreshRate, disps2[i].displayModeProperties.parameters.refreshRate);
    }
}

// Test vGetDisplayModeProperties2KHR where instance and ICD supports it, but device does not support it.
TEST_F(LoaderInstPhysDevExts, GetDispModeProps2KHRInstanceAndICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    Extension first_ext{VK_KHR_DISPLAY_EXTENSION_NAME};
    Extension second_ext{VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME};
    env.get_test_icd(0).add_instance_extensions({first_ext, second_ext});
    env.get_test_icd(0).physical_devices.push_back({});
    GenerateRandomDisplayModeProps(env.get_test_icd(0).physical_devices.back().display_mode_properties);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extensions({VK_KHR_DISPLAY_EXTENSION_NAME, VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME});
    instance.CheckCreate();

    auto GetDisplayModeProperties = reinterpret_cast<PFN_vkGetDisplayModePropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetDisplayModePropertiesKHR"));
    ASSERT_NE(GetDisplayModeProperties, nullptr);
    auto GetDisplayModeProperties2 = reinterpret_cast<PFN_vkGetDisplayModeProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetDisplayModeProperties2KHR"));
    ASSERT_NE(GetDisplayModeProperties2, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    std::vector<VkDisplayModePropertiesKHR> props{};
    uint32_t props_count1 = 0;
    ASSERT_EQ(VK_SUCCESS, GetDisplayModeProperties(physical_device, VK_NULL_HANDLE, &props_count1, nullptr));
    ASSERT_NE(0, props_count1);
    props.resize(props_count1);
    ASSERT_EQ(VK_SUCCESS, GetDisplayModeProperties(physical_device, VK_NULL_HANDLE, &props_count1, props.data()));

    std::vector<VkDisplayModeProperties2KHR> props2{};
    uint32_t props_count2 = 0;
    ASSERT_EQ(VK_SUCCESS, GetDisplayModeProperties2(physical_device, VK_NULL_HANDLE, &props_count2, nullptr));
    ASSERT_EQ(props_count1, props_count2);
    props2.resize(props_count2, {VK_STRUCTURE_TYPE_DISPLAY_MODE_PROPERTIES_2_KHR});
    ASSERT_EQ(VK_SUCCESS, GetDisplayModeProperties2(physical_device, VK_NULL_HANDLE, &props_count2, props2.data()));

    CompareDisplayModeProps(props, props2);
}

// Test vkGetDisplayModeProperties2KHR where instance supports it with some ICDs that both support
// and don't support it:
//    ICD 0 supports
//        Physical device 0 does not
//        Physical device 1 does
//        Physical device 2 does not
//    ICD 1 doesn't support
//        Physical device 3 does not
//    ICD 2 supports
//        Physical device 4 does not
//        Physical device 5 does not
//    ICD 3 supports
//        Physical device 6 does
TEST_F(LoaderInstPhysDevExts, GetDispModeProps2KHRMixed) {
    FrameworkEnvironment env{};
    const uint32_t max_icd_count = 4;
    const uint32_t dev_counts[max_icd_count] = {3, 1, 2, 1};
    const uint32_t max_phys_devs = 7;

    for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_0;
        cur_icd.add_instance_extension({VK_KHR_DISPLAY_EXTENSION_NAME});

        // ICD 1 should not have 1.1
        if (icd != 1) {
            cur_icd.icd_api_version = VK_API_VERSION_1_1;
            cur_icd.add_instance_extension({VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME});
        }

        uint32_t rand_vendor_id;
        uint32_t rand_driver_vers;
        FillInRandomICDInfo(rand_vendor_id, rand_driver_vers);

        for (uint32_t dev = 0; dev < dev_counts[icd]; ++dev) {
            uint32_t device_version = VK_API_VERSION_1_0;
            cur_icd.physical_devices.push_back({});
            auto& cur_dev = cur_icd.physical_devices.back();
            cur_dev.extensions.push_back({VK_KHR_DISPLAY_EXTENSION_NAME, 0});

            // 2nd device in ICD 0 and the one device in ICD 3 support the extension and 1.1
            if ((icd == 0 && dev == 1) || icd == 3) {
                cur_dev.extensions.push_back({VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME, 0});
                device_version = VK_API_VERSION_1_1;
            }

            // Still set physical device properties (so we can determine if device is correct API version)
            FillInRandomDeviceProps(cur_dev.properties, device_version, rand_vendor_id, rand_driver_vers);
            GenerateRandomDisplayModeProps(cur_dev.display_mode_properties);
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extensions({VK_KHR_DISPLAY_EXTENSION_NAME, VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME});
    instance.CheckCreate();

    auto GetDisplayModeProperties = reinterpret_cast<PFN_vkGetDisplayModePropertiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetDisplayModePropertiesKHR"));
    ASSERT_NE(GetDisplayModeProperties, nullptr);
    auto GetDisplayModeProperties2 = reinterpret_cast<PFN_vkGetDisplayModeProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetDisplayModeProperties2KHR"));
    ASSERT_NE(GetDisplayModeProperties2, nullptr);

    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        std::vector<VkDisplayModePropertiesKHR> props{};
        uint32_t props_count1 = 0;
        ASSERT_EQ(VK_SUCCESS, GetDisplayModeProperties(physical_devices[dev], VK_NULL_HANDLE, &props_count1, nullptr));
        ASSERT_NE(0, props_count1);
        props.resize(props_count1);
        ASSERT_EQ(VK_SUCCESS, GetDisplayModeProperties(physical_devices[dev], VK_NULL_HANDLE, &props_count1, props.data()));

        std::vector<VkDisplayModeProperties2KHR> props2{};
        uint32_t props_count2 = 0;
        ASSERT_EQ(VK_SUCCESS, GetDisplayModeProperties2(physical_devices[dev], VK_NULL_HANDLE, &props_count2, nullptr));
        ASSERT_EQ(props_count1, props_count2);
        props2.resize(props_count2, {VK_STRUCTURE_TYPE_DISPLAY_MODE_PROPERTIES_2_KHR});
        ASSERT_EQ(VK_SUCCESS, GetDisplayModeProperties2(physical_devices[dev], VK_NULL_HANDLE, &props_count2, props2.data()));

        CompareDisplayModeProps(props, props2);
    }
}

// Test vkGetDisplayPlaneCapabilities2KHR where nothing supports it.
TEST_F(LoaderInstPhysDevExts, GetDispPlaneCaps2KHRNoSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.CheckCreate();

    auto GetDisplayPlaneCapabilities = reinterpret_cast<PFN_vkGetDisplayPlaneCapabilitiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetDisplayPlaneCapabilitiesKHR"));
    ASSERT_EQ(GetDisplayPlaneCapabilities, nullptr);
}

// Test vkGetDisplayPlaneCapabilities2KHR where instance supports it, but nothing else.
TEST_F(LoaderInstPhysDevExts, GetDispPlaneCaps2KHRNoICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME);
    instance.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    auto GetDisplayPlaneCapabilities = reinterpret_cast<PFN_vkGetDisplayPlaneCapabilitiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetDisplayPlaneCapabilitiesKHR"));
    ASSERT_EQ(GetDisplayPlaneCapabilities, nullptr);
}

// Compare the display plane caps
static void CompareDisplayPlaneCaps(const VkDisplayPlaneCapabilitiesKHR& caps1, VkDisplayPlaneCapabilities2KHR& caps2) {
    ASSERT_EQ(caps1.supportedAlpha, caps2.capabilities.supportedAlpha);
    ASSERT_EQ(caps1.minSrcPosition.x, caps2.capabilities.minSrcPosition.x);
    ASSERT_EQ(caps1.minSrcPosition.y, caps2.capabilities.minSrcPosition.y);
    ASSERT_EQ(caps1.maxSrcPosition.x, caps2.capabilities.maxSrcPosition.x);
    ASSERT_EQ(caps1.maxSrcPosition.y, caps2.capabilities.maxSrcPosition.y);
    ASSERT_EQ(caps1.minSrcExtent.width, caps2.capabilities.minSrcExtent.width);
    ASSERT_EQ(caps1.minSrcExtent.height, caps2.capabilities.minSrcExtent.height);
    ASSERT_EQ(caps1.maxSrcExtent.width, caps2.capabilities.maxSrcExtent.width);
    ASSERT_EQ(caps1.maxSrcExtent.height, caps2.capabilities.maxSrcExtent.height);
    ASSERT_EQ(caps1.minDstPosition.x, caps2.capabilities.minDstPosition.x);
    ASSERT_EQ(caps1.minDstPosition.y, caps2.capabilities.minDstPosition.y);
    ASSERT_EQ(caps1.maxDstPosition.x, caps2.capabilities.maxDstPosition.x);
    ASSERT_EQ(caps1.maxDstPosition.y, caps2.capabilities.maxDstPosition.y);
    ASSERT_EQ(caps1.minDstExtent.width, caps2.capabilities.minDstExtent.width);
    ASSERT_EQ(caps1.minDstExtent.height, caps2.capabilities.minDstExtent.height);
    ASSERT_EQ(caps1.maxDstExtent.width, caps2.capabilities.maxDstExtent.width);
    ASSERT_EQ(caps1.maxDstExtent.height, caps2.capabilities.maxDstExtent.height);
}

// Test vkGetDisplayPlaneCapabilities2KHR where instance and ICD supports it, but device does not support it.
TEST_F(LoaderInstPhysDevExts, GetDispPlaneCaps2KHRInstanceAndICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    Extension first_ext{VK_KHR_DISPLAY_EXTENSION_NAME};
    Extension second_ext{VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME};
    env.get_test_icd(0).add_instance_extensions({first_ext, second_ext});
    env.get_test_icd(0).physical_devices.push_back({});
    FillInRandomDisplayPropData(env.get_test_icd(0).physical_devices.back().display_properties);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extensions({VK_KHR_DISPLAY_EXTENSION_NAME, VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME});
    instance.CheckCreate();

    auto GetDisplayPlaneCapabilities = reinterpret_cast<PFN_vkGetDisplayPlaneCapabilitiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetDisplayPlaneCapabilitiesKHR"));
    ASSERT_NE(GetDisplayPlaneCapabilities, nullptr);
    auto GetDisplayPlaneCapabilities2 = reinterpret_cast<PFN_vkGetDisplayPlaneCapabilities2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetDisplayPlaneCapabilities2KHR"));
    ASSERT_NE(GetDisplayPlaneCapabilities2, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    VkDisplayPlaneCapabilitiesKHR caps{};
    ASSERT_EQ(VK_SUCCESS, GetDisplayPlaneCapabilities(physical_device, 0, 0, &caps));
    VkDisplayPlaneCapabilities2KHR caps2{};
    VkDisplayPlaneInfo2KHR info{VK_STRUCTURE_TYPE_DISPLAY_PLANE_INFO_2_KHR};
    ASSERT_EQ(VK_SUCCESS, GetDisplayPlaneCapabilities2(physical_device, &info, &caps2));
    CompareDisplayPlaneCaps(caps, caps2);
}

// Test vkGetDisplayPlaneCapabilities2KHR where instance supports it with some ICDs that both support
// and don't support it:
//    ICD 0 supports
//        Physical device 0 does not
//        Physical device 1 does
//        Physical device 2 does not
//    ICD 1 doesn't support
//        Physical device 3 does not
//    ICD 2 supports
//        Physical device 4 does not
//        Physical device 5 does not
//    ICD 3 supports
//        Physical device 6 does
TEST_F(LoaderInstPhysDevExts, GetDispPlaneCaps2KHRMixed) {
    FrameworkEnvironment env{};
    const uint32_t max_icd_count = 4;
    const uint32_t dev_counts[max_icd_count] = {3, 1, 2, 1};
    const uint32_t max_phys_devs = 7;

    for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_0;
        cur_icd.add_instance_extension({VK_KHR_DISPLAY_EXTENSION_NAME});

        // ICD 1 should not have 1.1
        if (icd != 1) {
            cur_icd.icd_api_version = VK_API_VERSION_1_1;
            cur_icd.add_instance_extension({VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME});
        }

        uint32_t rand_vendor_id;
        uint32_t rand_driver_vers;
        FillInRandomICDInfo(rand_vendor_id, rand_driver_vers);

        for (uint32_t dev = 0; dev < dev_counts[icd]; ++dev) {
            uint32_t device_version = VK_API_VERSION_1_0;
            cur_icd.physical_devices.push_back({});
            auto& cur_dev = cur_icd.physical_devices.back();
            cur_dev.extensions.push_back({VK_KHR_DISPLAY_EXTENSION_NAME, 0});

            // 2nd device in ICD 0 and the one device in ICD 3 support the extension and 1.1
            if ((icd == 0 && dev == 1) || icd == 3) {
                cur_dev.extensions.push_back({VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME, 0});
                device_version = VK_API_VERSION_1_1;
            }

            // Still set physical device properties (so we can determine if device is correct API version)
            FillInRandomDeviceProps(cur_dev.properties, device_version, rand_vendor_id, rand_driver_vers);
            GenerateRandomDisplayPlaneCaps(cur_dev.display_plane_capabilities);
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extensions({VK_KHR_DISPLAY_EXTENSION_NAME, VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME});
    instance.CheckCreate();

    auto GetDisplayPlaneCapabilities = reinterpret_cast<PFN_vkGetDisplayPlaneCapabilitiesKHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetDisplayPlaneCapabilitiesKHR"));
    ASSERT_NE(GetDisplayPlaneCapabilities, nullptr);
    auto GetDisplayPlaneCapabilities2 = reinterpret_cast<PFN_vkGetDisplayPlaneCapabilities2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetDisplayPlaneCapabilities2KHR"));
    ASSERT_NE(GetDisplayPlaneCapabilities2, nullptr);

    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        VkDisplayPlaneCapabilitiesKHR caps{};
        ASSERT_EQ(VK_SUCCESS, GetDisplayPlaneCapabilities(physical_devices[dev], 0, 0, &caps));
        VkDisplayPlaneCapabilities2KHR caps2{};
        VkDisplayPlaneInfo2KHR info{VK_STRUCTURE_TYPE_DISPLAY_PLANE_INFO_2_KHR};
        ASSERT_EQ(VK_SUCCESS, GetDisplayPlaneCapabilities2(physical_devices[dev], &info, &caps2));
        CompareDisplayPlaneCaps(caps, caps2);
    }
}

//
// VK_EXT_acquire_drm_display
//

// Test vkAcquireDrmDisplayEXT where nothing supports it.
TEST_F(LoaderInstPhysDevExts, AcquireDrmDisplayEXTNoSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.CheckCreate();

    auto AcquireDrmDisplay =
        reinterpret_cast<PFN_vkAcquireDrmDisplayEXT>(instance.functions->vkGetInstanceProcAddr(instance, "vkAcquireDrmDisplayEXT"));
    ASSERT_EQ(AcquireDrmDisplay, nullptr);
}

// Test vkAcquireDrmDisplayEXT where instance supports it, but nothing else.
TEST_F(LoaderInstPhysDevExts, AcquireDrmDisplayEXTNoICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_EXT_ACQUIRE_DRM_DISPLAY_EXTENSION_NAME);
    instance.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    auto AcquireDrmDisplay =
        reinterpret_cast<PFN_vkAcquireDrmDisplayEXT>(instance.functions->vkGetInstanceProcAddr(instance, "vkAcquireDrmDisplayEXT"));
    ASSERT_EQ(AcquireDrmDisplay, nullptr);
}

// Test vkAcquireDrmDisplayEXT where instance and ICD supports it, but device does not support it.
TEST_F(LoaderInstPhysDevExts, AcquireDrmDisplayEXTInstanceAndICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    Extension first_ext{VK_KHR_DISPLAY_EXTENSION_NAME};
    Extension second_ext{VK_EXT_ACQUIRE_DRM_DISPLAY_EXTENSION_NAME};
    env.get_test_icd(0).add_instance_extensions({first_ext, second_ext});
    env.get_test_icd(0).physical_devices.push_back({});
    GenerateRandomDisplays(env.get_test_icd(0).physical_devices.back().displays);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extensions({VK_KHR_DISPLAY_EXTENSION_NAME, VK_EXT_ACQUIRE_DRM_DISPLAY_EXTENSION_NAME});
    instance.CheckCreate();

    auto AcquireDrmDisplay =
        reinterpret_cast<PFN_vkAcquireDrmDisplayEXT>(instance.functions->vkGetInstanceProcAddr(instance, "vkAcquireDrmDisplayEXT"));
    ASSERT_NE(AcquireDrmDisplay, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    VkDisplayKHR display = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, AcquireDrmDisplay(physical_device, 0, display));
}

// Test vkAcquireDrmDisplayEXT where instance supports it with some ICDs that both support
// and don't support it:
//    ICD 0 supports
//        Physical device 0 does not
//        Physical device 1 does
//        Physical device 2 does not
//    ICD 1 doesn't support
//        Physical device 3 does not
//    ICD 2 supports
//        Physical device 4 does not
//        Physical device 5 does not
//    ICD 3 supports
//        Physical device 6 does
TEST_F(LoaderInstPhysDevExts, AcquireDrmDisplayEXTMixed) {
    FrameworkEnvironment env{};
    const uint32_t max_icd_count = 4;
    const uint32_t dev_counts[max_icd_count] = {3, 1, 2, 1};
    const uint32_t max_phys_devs = 7;

    for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_0;
        cur_icd.add_instance_extension({VK_KHR_DISPLAY_EXTENSION_NAME});

        // ICD 1 should not have 1.1
        if (icd != 1) {
            cur_icd.icd_api_version = VK_API_VERSION_1_1;
            cur_icd.add_instance_extension({VK_EXT_ACQUIRE_DRM_DISPLAY_EXTENSION_NAME});
        }

        uint32_t rand_vendor_id;
        uint32_t rand_driver_vers;
        FillInRandomICDInfo(rand_vendor_id, rand_driver_vers);

        for (uint32_t dev = 0; dev < dev_counts[icd]; ++dev) {
            uint32_t device_version = VK_API_VERSION_1_0;
            cur_icd.physical_devices.push_back({});
            auto& cur_dev = cur_icd.physical_devices.back();
            cur_dev.extensions.push_back({VK_KHR_DISPLAY_EXTENSION_NAME, 0});

            // 2nd device in ICD 0 and the one device in ICD 3 support the extension and 1.1
            if ((icd == 0 && dev == 1) || icd == 3) {
                cur_dev.extensions.push_back({VK_EXT_ACQUIRE_DRM_DISPLAY_EXTENSION_NAME, 0});
                device_version = VK_API_VERSION_1_1;
            }

            // Still set physical device properties (so we can determine if device is correct API version)
            FillInRandomDeviceProps(cur_dev.properties, device_version, rand_vendor_id, rand_driver_vers);
            GenerateRandomDisplays(cur_dev.displays);
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extensions({VK_KHR_DISPLAY_EXTENSION_NAME, VK_EXT_ACQUIRE_DRM_DISPLAY_EXTENSION_NAME});
    instance.CheckCreate();

    auto AcquireDrmDisplay =
        reinterpret_cast<PFN_vkAcquireDrmDisplayEXT>(instance.functions->vkGetInstanceProcAddr(instance, "vkAcquireDrmDisplayEXT"));
    ASSERT_NE(AcquireDrmDisplay, nullptr);

    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        VkPhysicalDeviceProperties pd_props{};
        instance->vkGetPhysicalDeviceProperties(physical_devices[dev], &pd_props);

        for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
            auto& cur_icd = env.get_test_icd(icd);
            bool found = false;
            for (uint32_t pd = 0; pd < dev_counts[icd]; ++pd) {
                auto& cur_dev = cur_icd.physical_devices[pd];
                // Find the ICD device matching the physical device we're looking at info for so we can compare the
                // physical devices info with the returned info.
                if (cur_dev.properties.apiVersion == pd_props.apiVersion && cur_dev.properties.deviceID == pd_props.deviceID &&
                    cur_dev.properties.deviceType == pd_props.deviceType &&
                    cur_dev.properties.driverVersion == pd_props.driverVersion &&
                    cur_dev.properties.vendorID == pd_props.vendorID) {
                    VkDisplayKHR display = VK_NULL_HANDLE;
                    if (icd == 1) {
                        // For this extension, if no support exists (like for ICD 1), the value of 0 should be returned by the
                        // loader.
                        ASSERT_EQ(VK_ERROR_INITIALIZATION_FAILED, AcquireDrmDisplay(physical_devices[dev], 0, display));
                    } else {
                        ASSERT_EQ(VK_SUCCESS, AcquireDrmDisplay(physical_devices[dev], 0, display));
                    }
                    found = true;
                    break;
                }
            }
            if (found) {
                break;
            }
        }
    }
}

// Test vkGetDrmDisplayEXT where nothing supports it.
TEST_F(LoaderInstPhysDevExts, GetDrmDisplayEXTNoSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.CheckCreate();

    auto GetDrmDisplay =
        reinterpret_cast<PFN_vkGetDrmDisplayEXT>(instance.functions->vkGetInstanceProcAddr(instance, "vkGetDrmDisplayEXT"));
    ASSERT_EQ(GetDrmDisplay, nullptr);
}

// Test vkGetDrmDisplayEXT where instance supports it, but nothing else.
TEST_F(LoaderInstPhysDevExts, GetDrmDisplayEXTNoICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd(0).physical_devices.push_back({});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_EXT_ACQUIRE_DRM_DISPLAY_EXTENSION_NAME);
    instance.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    auto GetDrmDisplay =
        reinterpret_cast<PFN_vkGetDrmDisplayEXT>(instance.functions->vkGetInstanceProcAddr(instance, "vkGetDrmDisplayEXT"));
    ASSERT_EQ(GetDrmDisplay, nullptr);
}

// Test vkGetDrmDisplayEXT where instance and ICD supports it, but device does not support it.
TEST_F(LoaderInstPhysDevExts, GetDrmDisplayEXTInstanceAndICDSupport) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    Extension first_ext{VK_KHR_DISPLAY_EXTENSION_NAME};
    Extension second_ext{VK_EXT_ACQUIRE_DRM_DISPLAY_EXTENSION_NAME};
    env.get_test_icd(0).add_instance_extensions({first_ext, second_ext});
    env.get_test_icd(0).physical_devices.push_back({});
    GenerateRandomDisplays(env.get_test_icd(0).physical_devices.back().displays);

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extensions({VK_KHR_DISPLAY_EXTENSION_NAME, VK_EXT_ACQUIRE_DRM_DISPLAY_EXTENSION_NAME});
    instance.CheckCreate();

    auto GetDrmDisplay =
        reinterpret_cast<PFN_vkGetDrmDisplayEXT>(instance.functions->vkGetInstanceProcAddr(instance, "vkGetDrmDisplayEXT"));
    ASSERT_NE(GetDrmDisplay, nullptr);

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    VkDisplayKHR display = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, GetDrmDisplay(physical_device, 0, 0, &display));
    ASSERT_EQ(display, env.get_test_icd(0).physical_devices.back().displays[0]);
}

// Test vkGetDrmDisplayEXT where instance supports it with some ICDs that both support
// and don't support it:
//    ICD 0 supports
//        Physical device 0 does not
//        Physical device 1 does
//        Physical device 2 does not
//    ICD 1 doesn't support
//        Physical device 3 does not
//    ICD 2 supports
//        Physical device 4 does not
//        Physical device 5 does not
//    ICD 3 supports
//        Physical device 6 does
TEST_F(LoaderInstPhysDevExts, GetDrmDisplayEXTMixed) {
    FrameworkEnvironment env{};
    const uint32_t max_icd_count = 4;
    const uint32_t dev_counts[max_icd_count] = {3, 1, 2, 1};
    const uint32_t max_phys_devs = 7;

    for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_0;
        cur_icd.add_instance_extension({VK_KHR_DISPLAY_EXTENSION_NAME});

        // ICD 1 should not have 1.1
        if (icd != 1) {
            cur_icd.icd_api_version = VK_API_VERSION_1_1;
            cur_icd.add_instance_extension({VK_EXT_ACQUIRE_DRM_DISPLAY_EXTENSION_NAME});
        }

        uint32_t rand_vendor_id;
        uint32_t rand_driver_vers;
        FillInRandomICDInfo(rand_vendor_id, rand_driver_vers);

        for (uint32_t dev = 0; dev < dev_counts[icd]; ++dev) {
            uint32_t device_version = VK_API_VERSION_1_0;
            cur_icd.physical_devices.push_back({});
            auto& cur_dev = cur_icd.physical_devices.back();
            cur_dev.extensions.push_back({VK_KHR_DISPLAY_EXTENSION_NAME, 0});

            // 2nd device in ICD 0 and the one device in ICD 3 support the extension and 1.1
            if ((icd == 0 && dev == 1) || icd == 3) {
                cur_dev.extensions.push_back({VK_EXT_ACQUIRE_DRM_DISPLAY_EXTENSION_NAME, 0});
                device_version = VK_API_VERSION_1_1;
            }

            // Still set physical device properties (so we can determine if device is correct API version)
            FillInRandomDeviceProps(cur_dev.properties, device_version, rand_vendor_id, rand_driver_vers);
            GenerateRandomDisplays(cur_dev.displays);
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extensions({VK_KHR_DISPLAY_EXTENSION_NAME, VK_EXT_ACQUIRE_DRM_DISPLAY_EXTENSION_NAME});
    instance.CheckCreate();

    auto GetDrmDisplay =
        reinterpret_cast<PFN_vkGetDrmDisplayEXT>(instance.functions->vkGetInstanceProcAddr(instance, "vkGetDrmDisplayEXT"));
    ASSERT_NE(GetDrmDisplay, nullptr);

    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        VkPhysicalDeviceProperties pd_props{};
        instance->vkGetPhysicalDeviceProperties(physical_devices[dev], &pd_props);

        for (uint32_t icd = 0; icd < max_icd_count; ++icd) {
            auto& cur_icd = env.get_test_icd(icd);
            bool found = false;
            for (uint32_t pd = 0; pd < dev_counts[icd]; ++pd) {
                auto& cur_dev = cur_icd.physical_devices[pd];
                // Find the ICD device matching the physical device we're looking at info for so we can compare the
                // physical devices info with the returned info.
                if (cur_dev.properties.apiVersion == pd_props.apiVersion && cur_dev.properties.deviceID == pd_props.deviceID &&
                    cur_dev.properties.deviceType == pd_props.deviceType &&
                    cur_dev.properties.driverVersion == pd_props.driverVersion &&
                    cur_dev.properties.vendorID == pd_props.vendorID) {
                    VkDisplayKHR display = VK_NULL_HANDLE;
                    if (icd == 1) {
                        // For this extension, if no support exists (like for ICD 1), the value of 0 should be returned by the
                        // loader.
                        ASSERT_EQ(VK_ERROR_INITIALIZATION_FAILED, GetDrmDisplay(physical_devices[dev], 0, 0, &display));
                    } else {
                        ASSERT_EQ(VK_SUCCESS, GetDrmDisplay(physical_devices[dev], 0, 0, &display));
                        ASSERT_EQ(display, cur_dev.displays[0]);
                    }
                    found = true;
                    break;
                }
            }
            if (found) {
                break;
            }
        }
    }
}