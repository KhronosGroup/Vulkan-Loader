/*
 * Copyright (c) 2021-2022 The Khronos Group Inc.
 * Copyright (c) 2021-2022 Valve Corporation
 * Copyright (c) 2021-2022 LunarG, Inc.
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

TEST(ICDInterfaceVersion2Plus, vk_icdNegotiateLoaderICDInterfaceVersion) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    auto& driver = env.get_test_icd();

    for (uint32_t i = 0; i <= 6; i++) {
        for (uint32_t j = i; j <= 6; j++) {
            driver.min_icd_interface_version = i;
            driver.max_icd_interface_version = j;
            InstWrapper inst{env.vulkan_functions};
            inst.CheckCreate();
        }
    }
}

TEST(ICDInterfaceVersion2Plus, version_3) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    auto& driver = env.get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    {
        driver.min_icd_interface_version = 2;
        driver.enable_icd_wsi = true;
        InstWrapper inst{env.vulkan_functions};
        inst.CheckCreate();

        ASSERT_EQ(driver.is_using_icd_wsi, UsingICDProvidedWSI::not_using);
    }
    {
        driver.min_icd_interface_version = 3;
        driver.enable_icd_wsi = false;
        InstWrapper inst{env.vulkan_functions};
        inst.CheckCreate();

        ASSERT_EQ(driver.is_using_icd_wsi, UsingICDProvidedWSI::not_using);
    }
    {
        driver.min_icd_interface_version = 3;
        driver.enable_icd_wsi = true;
        InstWrapper inst{env.vulkan_functions};
        inst.CheckCreate();

        ASSERT_EQ(driver.is_using_icd_wsi, UsingICDProvidedWSI::is_using);
    }
}

TEST(ICDInterfaceVersion2Plus, version_4) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    auto& driver = env.get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();
}

TEST(ICDInterfaceVersion2Plus, l4_icd4) {
    // ICD must fail with VK_ERROR_INCOMPATIBLE_DRIVER for all vkCreateInstance calls with apiVersion set to > Vulkan 1.0
    // because both the loader and ICD support interface version <= 4. Otherwise, the ICD should behave as normal.
}
TEST(ICDInterfaceVersion2Plus, l4_icd5) {
    // ICD must fail with VK_ERROR_INCOMPATIBLE_DRIVER for all vkCreateInstance calls with apiVersion set to > Vulkan 1.0
    // because the loader is still at interface version <= 4. Otherwise, the ICD should behave as normal.
}
TEST(ICDInterfaceVersion2Plus, l5_icd4) {
    // Loader will fail with VK_ERROR_INCOMPATIBLE_DRIVER if it can't handle the apiVersion. ICD may pass for all apiVersions,
    // but since its interface is <= 4, it is best if it assumes it needs to do the work of rejecting anything > Vulkan 1.0 and
    // fail with VK_ERROR_INCOMPATIBLE_DRIVER. Otherwise, the ICD should behave as normal.
}
TEST(ICDInterfaceVersion2Plus, l5_icd5) {
    // Loader will fail with VK_ERROR_INCOMPATIBLE_DRIVER if it can't handle the apiVersion, and ICDs should fail with
    // VK_ERROR_INCOMPATIBLE_DRIVER only if they can not support the specified apiVersion. Otherwise, the ICD should behave as
    // normal.
}

#if defined(WIN32)
// This test makes sure that EnumerateAdapterPhysicalDevices on drivers found in the Khronos/Vulkan/Drivers registry
TEST(ICDInterfaceVersion2PlusEnumerateAdapterPhysicalDevices, version_6_in_drivers_registry) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_ENUMERATE_ADAPTER_PHYSICAL_DEVICES));
    auto& driver = env.get_test_icd();
    driver.physical_devices.emplace_back("physical_device_1");
    driver.physical_devices.emplace_back("physical_device_0");
    uint32_t physical_count = static_cast<uint32_t>(driver.physical_devices.size());
    uint32_t returned_physical_count = static_cast<uint32_t>(driver.physical_devices.size());
    std::vector<VkPhysicalDevice> physical_device_handles = std::vector<VkPhysicalDevice>(physical_count);

    driver.min_icd_interface_version = 5;

    auto& known_driver = known_driver_list.at(2);  // which drive this test pretends to be
    DXGI_ADAPTER_DESC1 desc1{};
    desc1.AdapterLuid = _LUID{10, 1000};
    desc1.VendorId = known_driver.vendor_id;
    env.platform_shim->add_dxgi_adapter(GpuType::discrete, desc1);
    driver.set_adapterLUID(desc1.AdapterLuid);

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    ASSERT_EQ(VK_SUCCESS,
              env.vulkan_functions.vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, physical_device_handles.data()));
    ASSERT_EQ(physical_count, returned_physical_count);
    ASSERT_TRUE(driver.called_enumerate_adapter_physical_devices);
}
// Make the version_6 driver found through the D3DKMT driver discovery mechanism of the loader
TEST(ICDInterfaceVersion2PlusEnumerateAdapterPhysicalDevices, version_6) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails{TEST_ICD_PATH_VERSION_6, VK_API_VERSION_1_3}.set_discovery_type(ManifestDiscoveryType::none));
    // Version 6 provides a mechanism to allow the loader to sort physical devices.
    // The loader will only attempt to sort physical devices on an ICD if version 6 of the interface is supported.
    // This version provides the vk_icdEnumerateAdapterPhysicalDevices function.
    auto& driver = env.get_test_icd(0);
    driver.physical_devices.emplace_back("physical_device_1");
    driver.physical_devices.emplace_back("physical_device_0");
    uint32_t physical_count = 2;
    uint32_t returned_physical_count = physical_count;
    std::vector<VkPhysicalDevice> physical_device_handles{physical_count};

    driver.min_icd_interface_version = 6;

    auto& known_driver = known_driver_list.at(2);  // which drive this test pretends to be
    DXGI_ADAPTER_DESC1 desc1{};
    desc1.AdapterLuid = _LUID{10, 1000};
    desc1.VendorId = known_driver.vendor_id;
    env.platform_shim->add_dxgi_adapter(GpuType::discrete, desc1);
    driver.set_adapterLUID(desc1.AdapterLuid);

    env.platform_shim->add_d3dkmt_adapter(
        D3DKMT_Adapter{0, desc1.AdapterLuid}.add_driver_manifest_path(env.get_icd_manifest_path(0)));

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, nullptr));
    ASSERT_EQ(physical_count, returned_physical_count);
    ASSERT_EQ(VK_SUCCESS,
              env.vulkan_functions.vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, physical_device_handles.data()));
    ASSERT_EQ(physical_count, returned_physical_count);
    ASSERT_TRUE(driver.called_enumerate_adapter_physical_devices);

    // Make sure that the loader doesn't write past the the end of the pointer
    auto temp_ptr = std::unique_ptr<int>(new int());
    for (auto& phys_dev : physical_device_handles) {
        phys_dev = reinterpret_cast<VkPhysicalDevice>(temp_ptr.get());
    }

    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, nullptr));
    returned_physical_count = 0;
    ASSERT_EQ(VK_INCOMPLETE,
              env.vulkan_functions.vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, physical_device_handles.data()));
    ASSERT_EQ(0U, returned_physical_count);
    for (auto& phys_dev : physical_device_handles) {
        ASSERT_EQ(phys_dev, reinterpret_cast<VkPhysicalDevice>(temp_ptr.get()));
    }
}

// Declare drivers using the D3DKMT driver interface and make sure the loader can find them - but don't export
// EnumerateAdapterPhysicalDevices
TEST(ICDInterfaceVersion2, EnumAdapters2) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails{TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA}.set_discovery_type(ManifestDiscoveryType::none));
    InstWrapper inst{env.vulkan_functions};
    auto& driver = env.get_test_icd();
    driver.physical_devices.emplace_back("physical_device_1");
    driver.physical_devices.emplace_back("physical_device_0");
    uint32_t physical_count = static_cast<uint32_t>(driver.physical_devices.size());
    uint32_t returned_physical_count = static_cast<uint32_t>(driver.physical_devices.size());
    std::vector<VkPhysicalDevice> physical_device_handles = std::vector<VkPhysicalDevice>(physical_count);
    driver.adapterLUID = _LUID{10, 1000};
    env.platform_shim->add_d3dkmt_adapter(D3DKMT_Adapter{0, _LUID{10, 1000}}.add_driver_manifest_path(env.get_icd_manifest_path()));

    inst.CheckCreate();

    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, nullptr));
    ASSERT_EQ(physical_count, returned_physical_count);
    ASSERT_EQ(VK_SUCCESS,
              env.vulkan_functions.vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, physical_device_handles.data()));
    ASSERT_EQ(physical_count, returned_physical_count);
    ASSERT_FALSE(driver.called_enumerate_adapter_physical_devices);
}

// Make sure that physical devices are found through EnumerateAdapterPhysicalDevices
// Verify that the handles are correct by calling vkGetPhysicalDeviceProperties with them
TEST(ICDInterfaceVersion2PlusEnumerateAdapterPhysicalDevices, VerifyPhysDevResults) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails{TEST_ICD_PATH_VERSION_2_EXPORT_ICD_ENUMERATE_ADAPTER_PHYSICAL_DEVICES}.set_discovery_type(
        ManifestDiscoveryType::none));
    auto& driver = env.get_test_icd();
    driver.min_icd_interface_version = 6;
    driver.set_icd_api_version(VK_API_VERSION_1_1);
    const std::vector<std::string> physical_device_names = {"physical_device_4", "physical_device_3", "physical_device_2",
                                                            "physical_device_1", "physical_device_0"};
    for (const auto& dev_name : physical_device_names) driver.physical_devices.push_back(dev_name);

    auto& known_driver = known_driver_list.at(2);  // which drive this test pretends to be
    DXGI_ADAPTER_DESC1 desc1{};
    desc1.VendorId = known_driver.vendor_id;
    desc1.AdapterLuid = _LUID{10, 1000};
    env.platform_shim->add_dxgi_adapter(GpuType::discrete, desc1);
    env.get_test_icd().set_adapterLUID(desc1.AdapterLuid);

    env.platform_shim->add_d3dkmt_adapter(D3DKMT_Adapter{0, _LUID{10, 1000}}.add_driver_manifest_path(env.get_icd_manifest_path()));

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    const size_t phys_dev_count = physical_device_names.size();

    // The test ICD should completely swap the order of devices.
    // Since we can't compare VkPhysicalDevice handles because they will be different per VkInstance, we will
    // compare the property names returned, which should still be equal.

    std::vector<VkPhysicalDevice> adapter_pds{phys_dev_count};
    uint32_t count = static_cast<uint32_t>(adapter_pds.size());
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumeratePhysicalDevices(inst, &count, adapter_pds.data()));
    ASSERT_EQ(phys_dev_count, count);

    for (uint32_t dev = 0; dev < phys_dev_count; ++dev) {
        VkPhysicalDeviceProperties props;
        env.vulkan_functions.vkGetPhysicalDeviceProperties(adapter_pds[dev], &props);
        std::string dev_name = props.deviceName;
        // index in reverse
        ASSERT_EQ(dev_name, physical_device_names[physical_device_names.size() - 1 - dev]);
    }
}

// Make sure physical device groups enumerated through EnumerateAdapterPhysicalDevices are properly found
TEST(ICDInterfaceVersion2PlusEnumerateAdapterPhysicalDevices, VerifyGroupResults) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails{TEST_ICD_PATH_VERSION_2_EXPORT_ICD_ENUMERATE_ADAPTER_PHYSICAL_DEVICES}.set_discovery_type(
        ManifestDiscoveryType::none));
    auto& driver = env.get_test_icd();
    driver.min_icd_interface_version = 6;
    driver.set_icd_api_version(VK_API_VERSION_1_1);
    const std::vector<std::string> physical_device_names = {"physical_device_4", "physical_device_3", "physical_device_2",
                                                            "physical_device_1", "physical_device_0"};
    for (const auto& dev_name : physical_device_names) driver.physical_devices.push_back(dev_name);

    driver.physical_device_groups.emplace_back(driver.physical_devices[0]);
    driver.physical_device_groups.back().use_physical_device(driver.physical_devices[1]);
    driver.physical_device_groups.emplace_back(driver.physical_devices[2]);
    driver.physical_device_groups.emplace_back(driver.physical_devices[3]);
    driver.physical_device_groups.back().use_physical_device(driver.physical_devices[4]);

    auto& known_driver = known_driver_list.at(2);  // which driver this test pretends to be
    DXGI_ADAPTER_DESC1 desc1{};
    desc1.VendorId = known_driver.vendor_id;
    desc1.AdapterLuid = _LUID{10, 1000};
    env.platform_shim->add_dxgi_adapter(GpuType::discrete, desc1);
    env.get_test_icd().set_adapterLUID(desc1.AdapterLuid);

    env.platform_shim->add_d3dkmt_adapter(D3DKMT_Adapter{0, _LUID{10, 1000}}.add_driver_manifest_path(env.get_icd_manifest_path()));

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    // The test ICD should completely swap the order of devices.
    // Since we can't compare VkPhysicalDevice handles because they will be different per VkInstance, we will
    // compare the property names returned, which should still be equal.
    // And, since this is device groups, the groups themselves should also be in reverse order with the devices
    // inside each group in revers order.

    const uint32_t actual_group_count = 3;
    uint32_t count = actual_group_count;
    std::array<VkPhysicalDeviceGroupProperties, actual_group_count> groups{};
    for (uint32_t group = 0; group < actual_group_count; ++group) {
        groups[group].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;
    }
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &count, groups.data()));
    ASSERT_EQ(actual_group_count, count);

    size_t cur_device_name_index = physical_device_names.size() - 1;  // start at last index and reverse through it
    for (uint32_t group = 0; group < actual_group_count; ++group) {
        for (uint32_t dev = 0; dev < groups[group].physicalDeviceCount; ++dev) {
            VkPhysicalDeviceProperties props;
            env.vulkan_functions.vkGetPhysicalDeviceProperties(groups[group].physicalDevices[dev], &props);
            std::string dev_name = props.deviceName;
            ASSERT_EQ(dev_name, physical_device_names[cur_device_name_index]);
            cur_device_name_index--;
        }
    }
}

#endif  // defined(WIN32)

TEST(MultipleICDConfig, Basic) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    env.get_test_icd(0).physical_devices.emplace_back("physical_device_0");
    env.get_test_icd(1).physical_devices.emplace_back("physical_device_1");
    env.get_test_icd(2).physical_devices.emplace_back("physical_device_2");

    env.get_test_icd(0).physical_devices.at(0).properties.deviceType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
    env.get_test_icd(1).physical_devices.at(0).properties.deviceType = VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
    env.get_test_icd(2).physical_devices.at(0).properties.deviceType = VK_PHYSICAL_DEVICE_TYPE_CPU;

    copy_string_to_char_array("dev0", env.get_test_icd(0).physical_devices.at(0).properties.deviceName, VK_MAX_EXTENSION_NAME_SIZE);
    copy_string_to_char_array("dev1", env.get_test_icd(1).physical_devices.at(0).properties.deviceName, VK_MAX_EXTENSION_NAME_SIZE);
    copy_string_to_char_array("dev2", env.get_test_icd(2).physical_devices.at(0).properties.deviceName, VK_MAX_EXTENSION_NAME_SIZE);

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    std::array<VkPhysicalDevice, 3> phys_devs_array;
    uint32_t phys_dev_count = 3;
    ASSERT_EQ(env.vulkan_functions.vkEnumeratePhysicalDevices(inst, &phys_dev_count, phys_devs_array.data()), VK_SUCCESS);
    ASSERT_EQ(phys_dev_count, 3U);
    ASSERT_EQ(env.get_test_icd(0).physical_devices.at(0).properties.deviceType, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
    ASSERT_EQ(env.get_test_icd(1).physical_devices.at(0).properties.deviceType, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
    ASSERT_EQ(env.get_test_icd(2).physical_devices.at(0).properties.deviceType, VK_PHYSICAL_DEVICE_TYPE_CPU);
}

TEST(MultipleDriverConfig, DifferentICDInterfaceVersions) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_EXPORT_ICD_GIPA));
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));

    TestICD& icd0 = env.get_test_icd(0);
    icd0.physical_devices.emplace_back("physical_device_0");
    icd0.max_icd_interface_version = 1;

    TestICD& icd1 = env.get_test_icd(1);
    icd1.physical_devices.emplace_back("physical_device_1");
    icd1.min_icd_interface_version = 2;
    icd1.max_icd_interface_version = 5;

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    std::array<VkPhysicalDevice, 2> phys_devs_array;
    uint32_t phys_dev_count = 2;
    ASSERT_EQ(env.vulkan_functions.vkEnumeratePhysicalDevices(inst, &phys_dev_count, phys_devs_array.data()), VK_SUCCESS);
    ASSERT_EQ(phys_dev_count, 2U);
}

TEST(MultipleDriverConfig, DifferentICDsWithDevices) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_EXPORT_ICD_GIPA));
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));

    // Make sure the loader returns all devices from all active ICDs.  Many of the other
    // tests add multiple devices to a single ICD, this just makes sure the loader combines
    // device info across multiple drivers properly.
    TestICD& icd0 = env.get_test_icd(0);
    icd0.physical_devices.emplace_back("physical_device_0");
    icd0.min_icd_interface_version = 5;
    icd0.max_icd_interface_version = 5;

    TestICD& icd1 = env.get_test_icd(1);
    icd1.physical_devices.emplace_back("physical_device_1");
    icd1.physical_devices.emplace_back("physical_device_2");
    icd1.min_icd_interface_version = 5;
    icd1.max_icd_interface_version = 5;

    TestICD& icd2 = env.get_test_icd(2);
    icd2.physical_devices.emplace_back("physical_device_3");
    icd2.min_icd_interface_version = 5;
    icd2.max_icd_interface_version = 5;

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    std::array<VkPhysicalDevice, 4> phys_devs_array;
    uint32_t phys_dev_count = 4;
    ASSERT_EQ(env.vulkan_functions.vkEnumeratePhysicalDevices(inst, &phys_dev_count, phys_devs_array.data()), VK_SUCCESS);
    ASSERT_EQ(phys_dev_count, 4U);
}

TEST(MultipleDriverConfig, DifferentICDsWithDevicesAndGroups) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_EXPORT_ICD_GIPA));
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));

    // The loader has to be able to handle drivers that support device groups in combination
    // with drivers that don't support device groups.  When this is the case, the loader needs
    // to take every driver that doesn't support device groups and put each of its devices in
    // a separate group.  Then it combines that information with the drivers that support
    // device groups returned info.

    // ICD 0 :  No 1.1 support (so 1 device will become 1 group in loader)
    TestICD& icd0 = env.get_test_icd(0);
    icd0.physical_devices.emplace_back("physical_device_0");
    icd0.min_icd_interface_version = 5;
    icd0.max_icd_interface_version = 5;
    icd0.set_icd_api_version(VK_API_VERSION_1_0);

    // ICD 1 :  1.1 support (with 1 group with 2 devices)
    TestICD& icd1 = env.get_test_icd(1);
    icd1.physical_devices.emplace_back("physical_device_1");
    icd1.physical_devices.emplace_back("physical_device_2");
    icd1.physical_device_groups.emplace_back(icd1.physical_devices[0]);
    icd1.physical_device_groups.back().use_physical_device(icd1.physical_devices[1]);
    icd1.min_icd_interface_version = 5;
    icd1.max_icd_interface_version = 5;
    icd1.set_icd_api_version(VK_API_VERSION_1_1);

    // ICD 2 :  No 1.1 support (so 3 devices will become 3 groups in loader)
    TestICD& icd2 = env.get_test_icd(2);
    icd2.physical_devices.emplace_back("physical_device_3");
    icd2.physical_devices.emplace_back("physical_device_4");
    icd2.physical_devices.emplace_back("physical_device_5");
    icd2.min_icd_interface_version = 5;
    icd2.max_icd_interface_version = 5;
    icd2.set_icd_api_version(VK_API_VERSION_1_0);

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(1, 1, 0);
    inst.CheckCreate();

    uint32_t group_count = static_cast<uint32_t>(5);
    uint32_t returned_group_count = 0;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, nullptr));
    ASSERT_EQ(group_count, returned_group_count);

    std::vector<VkPhysicalDeviceGroupProperties> group_props{};
    group_props.resize(group_count, VkPhysicalDeviceGroupProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES});
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, group_props.data()));
    ASSERT_EQ(group_count, returned_group_count);
}

#if defined(WIN32)
// This is testing when there are drivers that support the Windows device adapter sorting mechanism by exporting
// EnumerateAdapterPhysicalDevices and drivers that do not expose that functionality
TEST(MultipleICDConfig, version_5_and_version_6) {
    FrameworkEnvironment env;

    const char* regular_layer_name = "VK_LAYER_TestLayer1";
    env.add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name(regular_layer_name)
                                                         .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                         .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0))
                                                         .set_disable_environment("DisableMeIfYouCan")),
                           "regular_test_layer.json");

    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    uint32_t physical_count = 0;
    for (uint32_t i = 0; i < 3; i++) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_ENUMERATE_ADAPTER_PHYSICAL_DEVICES));
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
        auto& driver_5 = env.get_test_icd(i * 2 + 1);
        driver_5.set_max_icd_interface_version(5);
        driver_5.set_min_icd_interface_version(5);
        setup_WSI_in_ICD(driver_5);
        driver_5.physical_devices.push_back({});
        driver_5.physical_devices.back().queue_family_properties.push_back(family_props);
        driver_5.physical_devices.push_back({});
        driver_5.physical_devices.back().queue_family_properties.push_back(family_props);
        driver_5.physical_devices.push_back({});
        driver_5.physical_devices.back().queue_family_properties.push_back(family_props);
        physical_count += static_cast<uint32_t>(driver_5.physical_devices.size());

        auto& driver_6 = env.get_test_icd(i * 2);
        setup_WSI_in_ICD(driver_6);
        driver_6.physical_devices.emplace_back("physical_device_0");
        driver_6.physical_devices.back().queue_family_properties.push_back(family_props);
        driver_6.physical_devices.emplace_back("physical_device_1");
        driver_6.physical_devices.back().queue_family_properties.push_back(family_props);
        physical_count += static_cast<uint32_t>(driver_6.physical_devices.size());

        driver_6.set_max_icd_interface_version(6);
        driver_6.set_min_icd_interface_version(5);

        uint32_t driver_index = i % 4;  // which drive this test pretends to be, must stay below 4
        auto& known_driver = known_driver_list.at(driver_index);
        DXGI_ADAPTER_DESC1 desc1{};
        desc1.VendorId = known_driver.vendor_id;
        desc1.AdapterLuid = LUID{100 + i, static_cast<LONG>(100 + i)};
        driver_6.set_adapterLUID(desc1.AdapterLuid);
        env.platform_shim->add_dxgi_adapter(GpuType::discrete, desc1);
        env.get_test_icd().set_adapterLUID(desc1.AdapterLuid);
    }
    uint32_t returned_physical_count = 0;
    InstWrapper inst{env.vulkan_functions};
    setup_WSI_in_create_instance(inst);
    inst.CheckCreate();

    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, nullptr));
    ASSERT_EQ(physical_count, returned_physical_count);
    std::vector<VkPhysicalDevice> physical_device_handles{returned_physical_count};
    ASSERT_EQ(VK_SUCCESS,
              env.vulkan_functions.vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, physical_device_handles.data()));
    ASSERT_EQ(physical_count, returned_physical_count);

    VkSurfaceKHR surface = create_surface(inst);
    for (const auto& handle : physical_device_handles) {
        handle_assert_has_value(handle);

        VkBool32 supported = false;
        EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkGetPhysicalDeviceSurfaceSupportKHR(handle, 0, surface, &supported));
    }
    for (uint32_t i = 0; i < 3; i++) {
        auto& driver_6 = env.get_test_icd(i * 2);
        EXPECT_EQ(driver_6.called_enumerate_adapter_physical_devices, true);
    }
}
#endif  // defined(WIN32)

// shim function pointers for 1.3
// Should use autogen for this - it generates 'shim' functions for validation layers, maybe that could be used here.
void test_vkCmdBeginRendering(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin,
                              VkSubpassContents contents) {}
void test_vkCmdBindVertexBuffers2(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount,
                                  const VkBuffer* pBuffers, const VkDeviceSize* pOffsets, const VkDeviceSize* pSizes,
                                  const VkDeviceSize* pStrides) {}
void test_vkCmdBlitImage2(VkCommandBuffer commandBuffer, const VkBlitImageInfo2* pBlitImageInfo) {}
void test_vkCmdCopyBuffer2(VkCommandBuffer commandBuffer, const VkCopyBufferInfo2* pCopyBufferInfo) {}
void test_vkCmdCopyBufferToImage2(VkCommandBuffer commandBuffer, const VkCopyBufferToImageInfo2* pCopyBufferToImageInfo) {}
void test_vkCmdCopyImage2(VkCommandBuffer commandBuffer, const VkCopyImageInfo2* pCopyImageInfo) {}
void test_vkCmdCopyImageToBuffer2(VkCommandBuffer commandBuffer, const VkCopyImageToBufferInfo2* pCopyImageToBufferInfo) {}
void test_vkCmdEndRendering(VkCommandBuffer commandBuffer) {}
void test_vkCmdPipelineBarrier2(VkCommandBuffer commandBuffer, const VkDependencyInfo* pDependencyInfo) {}
void test_vkCmdResetEvent2(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags2 stageMask) {}
void test_vkCmdResolveImage2(VkCommandBuffer commandBuffer, const VkResolveImageInfo2* pResolveImageInfo) {}
void test_vkCmdSetCullMode(VkCommandBuffer commandBuffer, VkCullModeFlags cullMode) {}
void test_vkCmdSetDepthBiasEnable(VkCommandBuffer commandBuffer, VkBool32 depthBiasEnable) {}
void test_vkCmdSetDepthBoundsTestEnable(VkCommandBuffer commandBuffer, VkBool32 depthBoundsTestEnable) {}
void test_vkCmdSetDepthCompareOp(VkCommandBuffer commandBuffer, VkCompareOp depthCompareOp) {}
void test_vkCmdSetDepthTestEnable(VkCommandBuffer commandBuffer, VkBool32 depthTestEnable) {}
void test_vkCmdSetDepthWriteEnable(VkCommandBuffer commandBuffer, VkBool32 depthWriteEnable) {}
void test_vkCmdSetEvent2(VkCommandBuffer commandBuffer, VkEvent event, const VkDependencyInfo* pDependencyInfo) {}
void test_vkCmdSetFrontFace(VkCommandBuffer commandBuffer, VkFrontFace frontFace) {}
void test_vkCmdSetPrimitiveRestartEnable(VkCommandBuffer commandBuffer, VkBool32 primitiveRestartEnable) {}
void test_vkCmdSetPrimitiveTopology(VkCommandBuffer commandBuffer, VkPrimitiveTopology primitiveTopology) {}
void test_vkCmdSetRasterizerDiscardEnable(VkCommandBuffer commandBuffer, VkBool32 rasterizerDiscardEnable) {}
void test_vkCmdSetScissorWithCount(VkCommandBuffer commandBuffer, uint32_t scissorCount, const VkRect2D* pScissors) {}
void test_vkCmdSetStencilOp(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, VkStencilOp failOp, VkStencilOp passOp,
                            VkStencilOp depthFailOp, VkCompareOp compareOp) {}
void test_vkCmdSetStencilTestEnable(VkCommandBuffer commandBuffer, VkBool32 stencilTestEnable) {}
void test_vkCmdSetViewportWithCount(VkCommandBuffer commandBuffer, uint32_t viewportCount, const VkViewport* pViewports) {}
void test_vkCmdWaitEvents2(VkCommandBuffer commandBuffer, uint32_t eventCount, const VkEvent* pEvents,
                           const VkDependencyInfo* pDependencyInfos) {}
void test_vkCmdWriteTimestamp2(VkCommandBuffer commandBuffer, VkPipelineStageFlags2 stage, VkQueryPool queryPool, uint32_t query) {}
VkResult test_vkCreatePrivateDataSlot(VkDevice device, const VkPrivateDataSlotCreateInfo* pCreateInfo,
                                      const VkAllocationCallbacks* pAllocator, VkPrivateDataSlot* pPrivateDataSlot) {
    return VK_SUCCESS;
}
void test_vkDestroyPrivateDataSlot(VkDevice device, VkPrivateDataSlot privateDataSlot, const VkAllocationCallbacks* pAllocator) {}
void test_vkGetDeviceBufferMemoryRequirements(VkDevice device, const VkDeviceBufferMemoryRequirements* pInfo,
                                              VkMemoryRequirements2* pMemoryRequirements) {}
void test_vkGetDeviceImageMemoryRequirements(VkDevice device, const VkDeviceImageMemoryRequirements* pInfo,
                                             VkMemoryRequirements2* pMemoryRequirements) {}
void test_vkGetDeviceImageSparseMemoryRequirements(VkDevice device, const VkDeviceImageMemoryRequirements* pInfo,
                                                   uint32_t* pSparseMemoryRequirementCount,
                                                   VkSparseImageMemoryRequirements2* pSparseMemoryRequirements) {}
void test_vkGetPrivateData(VkDevice device, VkObjectType objectType, uint64_t objectHandle, VkPrivateDataSlot privateDataSlot,
                           uint64_t* pData) {}
VkResult test_vkQueueSubmit2(VkQueue queue, uint32_t submitCount, const VkSubmitInfo2* pSubmits, VkFence fence) {
    return VK_SUCCESS;
}
VkResult test_vkSetPrivateData(VkDevice device, VkObjectType objectType, uint64_t objectHandle, VkPrivateDataSlot privateDataSlot,
                               uint64_t data) {
    return VK_SUCCESS;
}

TEST(MinorVersionUpdate, Version1_3) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().physical_devices.push_back({});
    auto& icd_phys_dev = env.get_test_icd().physical_devices.back();
    icd_phys_dev.known_device_functions.insert(
        icd_phys_dev.known_device_functions.end(),
        {
            VulkanFunction{"vkCmdBeginRendering", to_vkVoidFunction(test_vkCmdBeginRendering)},
            VulkanFunction{"vkCmdBindVertexBuffers2", to_vkVoidFunction(test_vkCmdBindVertexBuffers2)},
            VulkanFunction{"vkCmdBlitImage2", to_vkVoidFunction(test_vkCmdBlitImage2)},
            VulkanFunction{"vkCmdCopyBuffer2", to_vkVoidFunction(test_vkCmdCopyBuffer2)},
            VulkanFunction{"vkCmdCopyBufferToImage2", to_vkVoidFunction(test_vkCmdCopyBufferToImage2)},
            VulkanFunction{"vkCmdCopyImage2", to_vkVoidFunction(test_vkCmdCopyImage2)},
            VulkanFunction{"vkCmdCopyImageToBuffer2", to_vkVoidFunction(test_vkCmdCopyImageToBuffer2)},
            VulkanFunction{"vkCmdEndRendering", to_vkVoidFunction(test_vkCmdEndRendering)},
            VulkanFunction{"vkCmdPipelineBarrier2", to_vkVoidFunction(test_vkCmdPipelineBarrier2)},
            VulkanFunction{"vkCmdResetEvent2", to_vkVoidFunction(test_vkCmdResetEvent2)},
            VulkanFunction{"vkCmdResolveImage2", to_vkVoidFunction(test_vkCmdResolveImage2)},
            VulkanFunction{"vkCmdSetCullMode", to_vkVoidFunction(test_vkCmdSetCullMode)},
            VulkanFunction{"vkCmdSetDepthBiasEnable", to_vkVoidFunction(test_vkCmdSetDepthBiasEnable)},
            VulkanFunction{"vkCmdSetDepthBoundsTestEnable", to_vkVoidFunction(test_vkCmdSetDepthBoundsTestEnable)},
            VulkanFunction{"vkCmdSetDepthCompareOp", to_vkVoidFunction(test_vkCmdSetDepthCompareOp)},
            VulkanFunction{"vkCmdSetDepthTestEnable", to_vkVoidFunction(test_vkCmdSetDepthTestEnable)},
            VulkanFunction{"vkCmdSetDepthWriteEnable", to_vkVoidFunction(test_vkCmdSetDepthWriteEnable)},
            VulkanFunction{"vkCmdSetEvent2", to_vkVoidFunction(test_vkCmdSetEvent2)},
            VulkanFunction{"vkCmdSetFrontFace", to_vkVoidFunction(test_vkCmdSetFrontFace)},
            VulkanFunction{"vkCmdSetPrimitiveRestartEnable", to_vkVoidFunction(test_vkCmdSetPrimitiveRestartEnable)},
            VulkanFunction{"vkCmdSetPrimitiveTopology", to_vkVoidFunction(test_vkCmdSetPrimitiveTopology)},
            VulkanFunction{"vkCmdSetRasterizerDiscardEnable", to_vkVoidFunction(test_vkCmdSetRasterizerDiscardEnable)},
            VulkanFunction{"vkCmdSetScissorWithCount", to_vkVoidFunction(test_vkCmdSetScissorWithCount)},
            VulkanFunction{"vkCmdSetStencilOp", to_vkVoidFunction(test_vkCmdSetStencilOp)},
            VulkanFunction{"vkCmdSetStencilTestEnable", to_vkVoidFunction(test_vkCmdSetStencilTestEnable)},
            VulkanFunction{"vkCmdSetViewportWithCount", to_vkVoidFunction(test_vkCmdSetViewportWithCount)},
            VulkanFunction{"vkCmdWaitEvents2", to_vkVoidFunction(test_vkCmdWaitEvents2)},
            VulkanFunction{"vkCmdWriteTimestamp2", to_vkVoidFunction(test_vkCmdWriteTimestamp2)},
            VulkanFunction{"vkCreatePrivateDataSlot", to_vkVoidFunction(test_vkCreatePrivateDataSlot)},
            VulkanFunction{"vkDestroyPrivateDataSlot", to_vkVoidFunction(test_vkDestroyPrivateDataSlot)},
            VulkanFunction{"vkGetDeviceBufferMemoryRequirements", to_vkVoidFunction(test_vkGetDeviceBufferMemoryRequirements)},
            VulkanFunction{"vkGetDeviceImageMemoryRequirements", to_vkVoidFunction(test_vkGetDeviceImageMemoryRequirements)},
            VulkanFunction{"vkGetDeviceImageSparseMemoryRequirements",
                           to_vkVoidFunction(test_vkGetDeviceImageSparseMemoryRequirements)},
            VulkanFunction{"vkGetPrivateData", to_vkVoidFunction(test_vkGetPrivateData)},
            VulkanFunction{"vkQueueSubmit2", to_vkVoidFunction(test_vkQueueSubmit2)},
            VulkanFunction{"vkSetPrivateData", to_vkVoidFunction(test_vkSetPrivateData)},
        });
    icd_phys_dev.extensions.push_back({"VK_SOME_EXT_haha"});
    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(1, 3, 0);
    inst.CheckCreate();

    auto phys_dev = inst.GetPhysDev();

    auto GetPhysicalDeviceToolProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceToolProperties>(
        inst.functions->vkGetInstanceProcAddr(inst, "vkGetPhysicalDeviceToolProperties"));
    uint32_t tool_count = 0;
    ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceToolProperties(phys_dev, &tool_count, nullptr));
    ASSERT_EQ(tool_count, 0U);
    VkPhysicalDeviceToolProperties props;
    ASSERT_EQ(VK_SUCCESS, GetPhysicalDeviceToolProperties(phys_dev, &tool_count, &props));

    DeviceWrapper device{inst};
    device.CheckCreate(phys_dev);

    auto CreateCommandPool =
        reinterpret_cast<PFN_vkCreateCommandPool>(inst.functions->vkGetDeviceProcAddr(device, "vkCreateCommandPool"));
    auto AllocateCommandBuffers =
        reinterpret_cast<PFN_vkAllocateCommandBuffers>(inst.functions->vkGetDeviceProcAddr(device, "vkAllocateCommandBuffers"));
    auto DestroyCommandPool =
        reinterpret_cast<PFN_vkDestroyCommandPool>(inst.functions->vkGetDeviceProcAddr(device, "vkDestroyCommandPool"));
    VkCommandPool command_pool{};
    VkCommandPoolCreateInfo pool_create_info{};
    pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    ASSERT_EQ(VK_SUCCESS, CreateCommandPool(device, &pool_create_info, nullptr, &command_pool));
    VkCommandBufferAllocateInfo buffer_allocate_info{};
    buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    buffer_allocate_info.commandPool = command_pool;
    buffer_allocate_info.commandBufferCount = 1;
    VkCommandBuffer command_buffer{};
    ASSERT_EQ(VK_SUCCESS, AllocateCommandBuffers(device, &buffer_allocate_info, &command_buffer));
    DestroyCommandPool(device, command_pool, nullptr);

    auto CmdBeginRendering =
        reinterpret_cast<PFN_vkCmdBeginRendering>(inst.functions->vkGetDeviceProcAddr(device, "vkCmdBeginRendering"));
    VkRenderingInfoKHR rendering_info{};
    CmdBeginRendering(command_buffer, &rendering_info);

    auto CmdBindVertexBuffers2 =
        reinterpret_cast<PFN_vkCmdBindVertexBuffers2>(inst.functions->vkGetDeviceProcAddr(device, "vkCmdBindVertexBuffers2"));
    CmdBindVertexBuffers2(command_buffer, 0, 0, nullptr, nullptr, nullptr, nullptr);

    auto CmdBlitImage2 = reinterpret_cast<PFN_vkCmdBlitImage2>(inst.functions->vkGetDeviceProcAddr(device, "vkCmdBlitImage2"));
    VkBlitImageInfo2 image_info{};
    CmdBlitImage2(command_buffer, &image_info);

    auto CmdCopyBuffer2 = reinterpret_cast<PFN_vkCmdCopyBuffer2>(inst.functions->vkGetDeviceProcAddr(device, "vkCmdCopyBuffer2"));
    VkCopyBufferInfo2 copy_info{};
    CmdCopyBuffer2(command_buffer, &copy_info);

    auto CmdCopyBufferToImage2 =
        reinterpret_cast<PFN_vkCmdCopyBufferToImage2>(inst.functions->vkGetDeviceProcAddr(device, "vkCmdCopyBufferToImage2"));
    VkCopyBufferToImageInfo2 copy_buf_image{};
    CmdCopyBufferToImage2(command_buffer, &copy_buf_image);

    auto CmdCopyImage2 = reinterpret_cast<PFN_vkCmdCopyImage2>(inst.functions->vkGetDeviceProcAddr(device, "vkCmdCopyImage2"));
    VkCopyImageInfo2 copy_image_info{};
    CmdCopyImage2(command_buffer, &copy_image_info);

    auto CmdCopyImageToBuffer2 =
        reinterpret_cast<PFN_vkCmdCopyImageToBuffer2>(inst.functions->vkGetDeviceProcAddr(device, "vkCmdCopyImageToBuffer2"));
    VkCopyImageToBufferInfo2 copy_image_buf;
    CmdCopyImageToBuffer2(command_buffer, &copy_image_buf);

    auto CmdEndRendering =
        reinterpret_cast<PFN_vkCmdEndRendering>(inst.functions->vkGetDeviceProcAddr(device, "vkCmdEndRendering"));
    CmdEndRendering(command_buffer);

    auto CmdPipelineBarrier2 =
        reinterpret_cast<PFN_vkCmdPipelineBarrier2>(inst.functions->vkGetDeviceProcAddr(device, "vkCmdPipelineBarrier2"));
    VkDependencyInfo deps_info;
    CmdPipelineBarrier2(command_buffer, &deps_info);

    auto CmdResetEvent2 = reinterpret_cast<PFN_vkCmdResetEvent2>(inst.functions->vkGetDeviceProcAddr(device, "vkCmdResetEvent2"));
    CmdResetEvent2(command_buffer, {}, VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT);

    auto CmdResolveImage2 =
        reinterpret_cast<PFN_vkCmdResolveImage2>(inst.functions->vkGetDeviceProcAddr(device, "vkCmdResolveImage2"));
    VkResolveImageInfo2 resolve_image{};
    CmdResolveImage2(command_buffer, &resolve_image);

    auto CmdSetCullMode = reinterpret_cast<PFN_vkCmdSetCullMode>(inst.functions->vkGetDeviceProcAddr(device, "vkCmdSetCullMode"));
    CmdSetCullMode(command_buffer, VK_CULL_MODE_BACK_BIT);

    auto CmdSetDepthBiasEnable =
        reinterpret_cast<PFN_vkCmdSetDepthBiasEnable>(inst.functions->vkGetDeviceProcAddr(device, "vkCmdSetDepthBiasEnable"));
    CmdSetDepthBiasEnable(command_buffer, true);

    auto CmdSetDepthBoundsTestEnable = reinterpret_cast<PFN_vkCmdSetDepthBoundsTestEnable>(
        inst.functions->vkGetDeviceProcAddr(device, "vkCmdSetDepthBoundsTestEnable"));
    CmdSetDepthBoundsTestEnable(command_buffer, true);

    auto CmdSetDepthCompareOp =
        reinterpret_cast<PFN_vkCmdSetDepthCompareOp>(inst.functions->vkGetDeviceProcAddr(device, "vkCmdSetDepthCompareOp"));
    CmdSetDepthCompareOp(command_buffer, VK_COMPARE_OP_ALWAYS);

    auto CmdSetDepthTestEnable =
        reinterpret_cast<PFN_vkCmdSetDepthTestEnable>(inst.functions->vkGetDeviceProcAddr(device, "vkCmdSetDepthTestEnable"));
    CmdSetDepthTestEnable(command_buffer, true);

    auto CmdSetDepthWriteEnable =
        reinterpret_cast<PFN_vkCmdSetDepthWriteEnable>(inst.functions->vkGetDeviceProcAddr(device, "vkCmdSetDepthWriteEnable"));
    CmdSetDepthWriteEnable(command_buffer, true);

    auto CmdSetEvent2 = reinterpret_cast<PFN_vkCmdSetEvent2>(inst.functions->vkGetDeviceProcAddr(device, "vkCmdSetEvent2"));
    CmdSetEvent2(command_buffer, {}, &deps_info);

    auto CmdSetFrontFace =
        reinterpret_cast<PFN_vkCmdSetFrontFace>(inst.functions->vkGetDeviceProcAddr(device, "vkCmdSetFrontFace"));
    CmdSetFrontFace(command_buffer, VK_FRONT_FACE_CLOCKWISE);

    auto CmdSetPrimitiveRestartEnable = reinterpret_cast<PFN_vkCmdSetPrimitiveRestartEnable>(
        inst.functions->vkGetDeviceProcAddr(device, "vkCmdSetPrimitiveRestartEnable"));
    CmdSetPrimitiveRestartEnable(command_buffer, true);

    auto CmdSetPrimitiveTopology =
        reinterpret_cast<PFN_vkCmdSetPrimitiveTopology>(inst.functions->vkGetDeviceProcAddr(device, "vkCmdSetPrimitiveTopology"));
    CmdSetPrimitiveTopology(command_buffer, VK_PRIMITIVE_TOPOLOGY_LINE_LIST);

    auto CmdSetRasterizerDiscardEnable = reinterpret_cast<PFN_vkCmdSetRasterizerDiscardEnable>(
        inst.functions->vkGetDeviceProcAddr(device, "vkCmdSetRasterizerDiscardEnable"));
    CmdSetRasterizerDiscardEnable(command_buffer, true);

    auto CmdSetScissorWithCount =
        reinterpret_cast<PFN_vkCmdSetScissorWithCount>(inst.functions->vkGetDeviceProcAddr(device, "vkCmdSetScissorWithCount"));
    CmdSetScissorWithCount(command_buffer, 0, nullptr);

    auto CmdSetStencilOp =
        reinterpret_cast<PFN_vkCmdSetStencilOp>(inst.functions->vkGetDeviceProcAddr(device, "vkCmdSetStencilOp"));
    CmdSetStencilOp(command_buffer, VK_STENCIL_FACE_BACK_BIT, VK_STENCIL_OP_DECREMENT_AND_WRAP, VK_STENCIL_OP_DECREMENT_AND_CLAMP,
                    VK_STENCIL_OP_DECREMENT_AND_WRAP, VK_COMPARE_OP_ALWAYS);

    auto CmdSetStencilTestEnable =
        reinterpret_cast<PFN_vkCmdSetStencilTestEnable>(inst.functions->vkGetDeviceProcAddr(device, "vkCmdSetStencilTestEnable"));
    CmdSetStencilTestEnable(command_buffer, true);

    auto CmdSetViewportWithCount =
        reinterpret_cast<PFN_vkCmdSetViewportWithCount>(inst.functions->vkGetDeviceProcAddr(device, "vkCmdSetViewportWithCount"));
    CmdSetViewportWithCount(command_buffer, 0, nullptr);

    auto CmdWaitEvents2 = reinterpret_cast<PFN_vkCmdWaitEvents2>(inst.functions->vkGetDeviceProcAddr(device, "vkCmdWaitEvents2"));
    CmdWaitEvents2(command_buffer, 0, nullptr, &deps_info);

    auto CmdWriteTimestamp2 =
        reinterpret_cast<PFN_vkCmdWriteTimestamp2>(inst.functions->vkGetDeviceProcAddr(device, "vkCmdWriteTimestamp2"));
    CmdWriteTimestamp2(command_buffer, VK_PIPELINE_STAGE_2_BLIT_BIT, {}, 0);

    auto CreatePrivateDataSlot =
        reinterpret_cast<PFN_vkCreatePrivateDataSlot>(inst.functions->vkGetDeviceProcAddr(device, "vkCreatePrivateDataSlot"));
    CreatePrivateDataSlot(device, nullptr, nullptr, nullptr);
    auto DestroyPrivateDataSlot =
        reinterpret_cast<PFN_vkDestroyPrivateDataSlot>(inst.functions->vkGetDeviceProcAddr(device, "vkDestroyPrivateDataSlot"));
    DestroyPrivateDataSlot(device, VK_NULL_HANDLE, nullptr);
    auto GetDeviceBufferMemoryRequirements = reinterpret_cast<PFN_vkGetDeviceBufferMemoryRequirements>(
        inst.functions->vkGetDeviceProcAddr(device, "vkGetDeviceBufferMemoryRequirements"));
    GetDeviceBufferMemoryRequirements(device, nullptr, nullptr);
    auto GetDeviceImageMemoryRequirements = reinterpret_cast<PFN_vkGetDeviceImageMemoryRequirements>(
        inst.functions->vkGetDeviceProcAddr(device, "vkGetDeviceImageMemoryRequirements"));
    GetDeviceImageMemoryRequirements(device, nullptr, nullptr);
    auto GetDeviceImageSparseMemoryRequirements = reinterpret_cast<PFN_vkGetDeviceImageSparseMemoryRequirements>(
        inst.functions->vkGetDeviceProcAddr(device, "vkGetDeviceImageSparseMemoryRequirements"));
    GetDeviceImageSparseMemoryRequirements(device, nullptr, nullptr, nullptr);
    auto GetPrivateData = reinterpret_cast<PFN_vkGetPrivateData>(inst.functions->vkGetDeviceProcAddr(device, "vkGetPrivateData"));
    GetPrivateData(device, VK_OBJECT_TYPE_UNKNOWN, 0, {}, nullptr);
    auto QueueSubmit2 = reinterpret_cast<PFN_vkQueueSubmit2>(inst.functions->vkGetDeviceProcAddr(device, "vkQueueSubmit2"));
    QueueSubmit2(nullptr, 0, nullptr, VK_NULL_HANDLE);
    auto SetPrivateData = reinterpret_cast<PFN_vkSetPrivateData>(inst.functions->vkGetDeviceProcAddr(device, "vkSetPrivateData"));
    SetPrivateData(device, VK_OBJECT_TYPE_UNKNOWN, 0, {}, 0);
}

TEST(ApplicationInfoVersion, NonVulkanVariant) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().physical_devices.push_back({});

    DebugUtilsLogger log;
    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(VK_MAKE_API_VERSION(1, 0, 0, 0));
    FillDebugUtilsCreateDetails(inst.create_info, log);
    inst.CheckCreate();
    ASSERT_TRUE(log.find(
        std::string("vkCreateInstance: The API Variant specified in pCreateInfo->pApplicationInfo.apiVersion is 1 instead of "
                    "the expected value of 0.")));
}

TEST(DriverManifest, NonVulkanVariant) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA, VK_MAKE_API_VERSION(1, 1, 0, 0)));
    env.get_test_icd().physical_devices.push_back({});

    DebugUtilsLogger log;
    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(VK_MAKE_API_VERSION(0, 1, 0, 0));
    FillDebugUtilsCreateDetails(inst.create_info, log);
    inst.CheckCreate(VK_ERROR_INCOMPATIBLE_DRIVER);
    ASSERT_TRUE(log.find("loader_icd_scan: Driver's ICD JSON "));
    // log prints the path to the file, don't look for it since it is hard to determine inside the test what the path should be.
    ASSERT_TRUE(log.find("\'api_version\' field contains a non-zero variant value of 1.  Skipping ICD JSON."));
}

TEST(LayerManifest, ImplicitNonVulkanVariant) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA, VK_MAKE_API_VERSION(0, 1, 0, 0)));
    env.get_test_icd().physical_devices.push_back({});

    const char* implicit_layer_name = "ImplicitTestLayer";
    env.add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name(implicit_layer_name)
                                                         .set_api_version(VK_MAKE_API_VERSION(1, 1, 0, 0))
                                                         .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                         .set_disable_environment("DISABLE_ME")),
                           "implicit_test_layer.json");

    DebugUtilsLogger log;
    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(VK_MAKE_API_VERSION(0, 1, 0, 0));
    FillDebugUtilsCreateDetails(inst.create_info, log);
    inst.CheckCreate();
    ASSERT_TRUE(log.find(std::string("Layer ") + implicit_layer_name +
                         " has an \'api_version\' field which contains a non-zero variant value of 1.  Skipping Layer."));
}

TEST(LayerManifest, ExplicitNonVulkanVariant) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA, VK_MAKE_API_VERSION(0, 1, 0, 0)));
    env.get_test_icd().physical_devices.push_back({});

    const char* explicit_layer_name = "ExplicitTestLayer";
    env.add_explicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name(explicit_layer_name)
                                                         .set_api_version(VK_MAKE_API_VERSION(1, 1, 0, 0))
                                                         .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
                           "explicit_test_layer.json");

    DebugUtilsLogger log;
    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(VK_MAKE_API_VERSION(0, 1, 0, 0)).add_layer(explicit_layer_name);
    FillDebugUtilsCreateDetails(inst.create_info, log);
    inst.CheckCreate(VK_ERROR_LAYER_NOT_PRESENT);
    ASSERT_TRUE(log.find(std::string("Layer ") + explicit_layer_name +
                         " has an \'api_version\' field which contains a non-zero variant value of 1.  Skipping Layer."));
}

TEST(DriverManifest, UnknownManifestVersion) {
    FrameworkEnvironment env{};
    env.add_icd(
        TestICDDetails(ManifestICD{}.set_lib_path(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA).set_file_format_version({3, 2, 1})));
    env.get_test_icd().physical_devices.push_back({});

    DebugUtilsLogger log;
    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(VK_MAKE_API_VERSION(0, 1, 0, 0));
    FillDebugUtilsCreateDetails(inst.create_info, log);
    inst.CheckCreate();
    ASSERT_TRUE(log.find("loader_icd_scan: "));
    // log prints the path to the file, don't look for it since it is hard to determine inside the test what the path should be.
    ASSERT_TRUE(log.find("has unknown icd manifest file version 3.2.1. May cause errors."));
}

TEST(LayerManifest, UnknownManifestVersion) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().physical_devices.push_back({});

    const char* implicit_layer_name = "ImplicitTestLayer";
    env.add_implicit_layer(ManifestLayer{}
                               .add_layer(ManifestLayer::LayerDescription{}
                                              .set_name(implicit_layer_name)
                                              .set_api_version(VK_MAKE_API_VERSION(1, 1, 0, 0))
                                              .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                              .set_disable_environment("DISABLE_ME"))
                               .set_file_format_version({3, 2, 1}),
                           "implicit_test_layer.json");

    DebugUtilsLogger log;
    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(VK_MAKE_API_VERSION(0, 1, 0, 0));
    FillDebugUtilsCreateDetails(inst.create_info, log);
    inst.CheckCreate();
    ASSERT_TRUE(log.find("loader_add_layer_properties: "));
    // log prints the path to the file, don't look for it since it is hard to determine inside the test what the path should be.
    ASSERT_TRUE(log.find("has unknown layer manifest file version 3.2.1.  May cause errors."));
}
