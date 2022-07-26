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
 * Author: Mark Young <markylunarg.com>
 */

#include "test_environment.h"

class EnvVarICDOverrideSetup : public ::testing::Test {
   protected:
    virtual void SetUp() {
        remove_env_var("VK_ICD_FILENAMES");
        remove_env_var("VK_DRIVER_FILES");
        remove_env_var("VK_ADD_DRIVER_FILES");
    }

    virtual void TearDown() {
        remove_env_var("VK_ICD_FILENAMES");
        remove_env_var("VK_DRIVER_FILES");
        remove_env_var("VK_ADD_DRIVER_FILES");
    }
};

// Don't support vk_icdNegotiateLoaderICDInterfaceVersion
// Loader calls vk_icdGetInstanceProcAddr second
// does not support vk_icdGetInstanceProcAddr
// must export vkGetInstanceProcAddr, vkCreateInstance, vkEnumerateInstanceExtensionProperties
TEST(EnvVarICDOverrideSetup, version_0_none) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_EXPORT_NONE).set_discovery_type(ManifestDiscoveryType::env_var));

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    ASSERT_EQ(env.get_test_icd(0).called_vk_icd_gipa, CalledICDGIPA::vk_gipa);
}

// Don't support vk_icdNegotiateLoaderICDInterfaceVersion
// the loader calls vk_icdGetInstanceProcAddr first
TEST(EnvVarICDOverrideSetup, version_1_icd_gipa) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_EXPORT_ICD_GIPA).set_discovery_type(ManifestDiscoveryType::env_var));

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    ASSERT_EQ(env.get_test_icd(0).called_vk_icd_gipa, CalledICDGIPA::vk_icd_gipa);
}

// support vk_icdNegotiateLoaderICDInterfaceVersion but not vk_icdGetInstanceProcAddr
TEST(EnvVarICDOverrideSetup, version_negotiate_interface_version_death_test) {
    FrameworkEnvironment env{};
    env.add_icd(
        TestICDDetails(TEST_ICD_PATH_EXPORT_NEGOTIATE_INTERFACE_VERSION).set_discovery_type(ManifestDiscoveryType::env_var));

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate(VK_ERROR_INCOMPATIBLE_DRIVER);
}

// export vk_icdNegotiateLoaderICDInterfaceVersion and vk_icdGetInstanceProcAddr
TEST(EnvVarICDOverrideSetup, version_2_negotiate_interface_version_and_icd_gipa) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2).set_discovery_type(ManifestDiscoveryType::env_var));

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    ASSERT_EQ(env.get_test_icd(0).called_vk_icd_gipa, CalledICDGIPA::vk_icd_gipa);
}

// export vk_icdNegotiateLoaderICDInterfaceVersion and vk_icdGetInstanceProcAddr
TEST(EnvVarICDOverrideSetup, version_2_negotiate_interface_version_and_icd_gipa_unicode) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_UNICODE)
                    .set_discovery_type(ManifestDiscoveryType::env_var)
                    .set_json_name(TEST_JSON_NAME_VERSION_2_UNICODE));

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    ASSERT_EQ(env.get_test_icd(0).called_vk_icd_gipa, CalledICDGIPA::vk_icd_gipa);
}

// Test VK_DRIVER_FILES environment variable
TEST(EnvVarICDOverrideSetup, TestOnlyDriverEnvVar) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_EXPORT_NONE).set_discovery_type(ManifestDiscoveryType::env_var));
    env.get_test_icd(0).physical_devices.emplace_back("pd0");

    InstWrapper inst1{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst1.create_info, env.debug_log);
    inst1.CheckCreate();
    EXPECT_FALSE(
        env.debug_log.find("Ignoring override VK_ICD_FILENAMES, VK_DRIVER_FILES, and VK_ADD_DRIVER_FILES due to high-integrity"));

    std::array<VkPhysicalDevice, 5> phys_devs_array;
    uint32_t phys_dev_count = 1;
    ASSERT_EQ(inst1->vkEnumeratePhysicalDevices(inst1.inst, &phys_dev_count, phys_devs_array.data()), VK_SUCCESS);
    ASSERT_EQ(phys_dev_count, 1U);

    for (uint32_t add = 0; add < 2; ++add) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_EXPORT_NONE).set_discovery_type(ManifestDiscoveryType::env_var));
        env.get_test_icd(add + 1).physical_devices.emplace_back("pd" + std::to_string(add) + "0");
        env.get_test_icd(add + 1).physical_devices.emplace_back("pd" + std::to_string(add) + "1");
    }

    env.debug_log.clear();

    InstWrapper inst2{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst2.create_info, env.debug_log);
    inst2.CheckCreate();

    phys_dev_count = 5;
    ASSERT_EQ(inst2->vkEnumeratePhysicalDevices(inst2.inst, &phys_dev_count, phys_devs_array.data()), VK_SUCCESS);
    ASSERT_EQ(phys_dev_count, 5U);

    env.debug_log.clear();

    env.platform_shim->set_elevated_privilege(true);

    InstWrapper inst3{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst3.create_info, env.debug_log);
    inst3.CheckCreate(VK_ERROR_INCOMPATIBLE_DRIVER);

    EXPECT_TRUE(env.debug_log.find("vkCreateInstance: Found no drivers!"));

    env.platform_shim->set_elevated_privilege(false);

    remove_env_var("VK_DRIVER_FILES");
}

#if defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__)
// Make sure the loader reports the correct message based on if USE_UNSAFE_FILE_SEARCH is set or not
TEST(EnvVarICDOverrideSetup, NonSecureEnvVarLookup) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().physical_devices.emplace_back("physical_device_0");

    DebugUtilsLogger log{VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    InstWrapper inst{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst.create_info, log);
    inst.CheckCreate();

#if !defined(USE_UNSAFE_FILE_SEARCH)
    ASSERT_FALSE(log.find("Loader is using non-secure environment variable lookup for"));
#else
    ASSERT_TRUE(log.find("Loader is using non-secure environment variable lookup for"));
#endif
}

// Check for proper handling of paths specified via environment variables.
TEST(EnvVarICDOverrideSetup, XDG) {
    // Set up a layer path that includes default and user-specified locations,
    // so that the test app can find them.  Include some badly specified elements as well.
    // Need to redirect the 'home' directory
    fs::path HOME = "/home/fake_home";
    set_env_var("HOME", HOME.str());
    set_env_var("XDG_CONFIG_DIRS", ":/tmp/goober:::::/tmp/goober/::::");
    set_env_var("XDG_CONFIG_HOME", ":/tmp/goober:::::/tmp/goober2/::::");
    set_env_var("XDG_DATA_DIRS", "::::/tmp/goober3:/tmp/goober4/with spaces:::");
    set_env_var("XDG_DATA_HOME", "::::/tmp/goober3:/tmp/goober4/with spaces:::");

    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().physical_devices.push_back({});

    InstWrapper inst{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
    inst.CheckCreate();

    auto check_paths = [](DebugUtilsLogger const& debug_log, ManifestCategory category, fs::path const& HOME) {
        EXPECT_TRUE(debug_log.find((fs::path("/tmp/goober/vulkan") / category_path_name(category)).str()));
        EXPECT_TRUE(debug_log.find((fs::path("/tmp/goober2/vulkan") / category_path_name(category)).str()));
        EXPECT_TRUE(debug_log.find((fs::path("/tmp/goober3/vulkan") / category_path_name(category)).str()));
        EXPECT_TRUE(debug_log.find((fs::path("/tmp/goober4/with spaces/vulkan") / category_path_name(category)).str()));
    };
    check_paths(env.debug_log, ManifestCategory::icd, HOME);
    check_paths(env.debug_log, ManifestCategory::implicit_layer, HOME);
    check_paths(env.debug_log, ManifestCategory::explicit_layer, HOME);
}
#endif

// Test VK_ADD_DRIVER_FILES environment variable
TEST(EnvVarICDOverrideSetup, TestOnlyAddDriverEnvVar) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_EXPORT_NONE).set_discovery_type(ManifestDiscoveryType::add_env_var));
    env.get_test_icd(0).physical_devices.emplace_back("pd0");

    InstWrapper inst1{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst1.create_info, env.debug_log);
    inst1.CheckCreate();

    std::array<VkPhysicalDevice, 1> phys_devs_array;
    uint32_t phys_dev_count = 1;
    ASSERT_EQ(inst1->vkEnumeratePhysicalDevices(inst1.inst, &phys_dev_count, phys_devs_array.data()), VK_SUCCESS);
    ASSERT_EQ(phys_dev_count, 1U);

    env.platform_shim->set_elevated_privilege(true);

    InstWrapper inst2{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst2.create_info, env.debug_log);
    inst2.CheckCreate(VK_ERROR_INCOMPATIBLE_DRIVER);

    EXPECT_TRUE(env.debug_log.find("vkCreateInstance: Found no drivers!"));

    env.platform_shim->set_elevated_privilege(false);

    remove_env_var("VK_ADD_DRIVER_FILES");
}

// Test Both VK_DRIVER_FILES and VK_ADD_DRIVER_FILES environment variable
TEST(EnvVarICDOverrideSetup, TestBothDriverEnvVars) {
    FrameworkEnvironment env{};

    // Add a driver that isn't enabled with the environment variable
    env.add_icd(TestICDDetails(TEST_ICD_PATH_EXPORT_NONE).set_discovery_type(ManifestDiscoveryType::env_var));
    env.get_test_icd(0).physical_devices.emplace_back("pd0");
    env.get_test_icd(0).physical_devices.emplace_back("pd1");

    env.add_icd(TestICDDetails(TEST_ICD_PATH_EXPORT_NONE).set_discovery_type(ManifestDiscoveryType::add_env_var));
    env.get_test_icd(0).physical_devices.emplace_back("pd2");

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    // The enumerate should now detect both drivers because we used the add
    std::array<VkPhysicalDevice, 3> phys_devs_array;
    uint32_t phys_dev_count = 3;
    ASSERT_EQ(inst->vkEnumeratePhysicalDevices(inst.inst, &phys_dev_count, phys_devs_array.data()), VK_SUCCESS);
    ASSERT_EQ(phys_dev_count, 3U);

    remove_env_var("VK_DRIVER_FILES");
    remove_env_var("VK_ADD_DRIVER_FILES");
}

#if defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__)
// Test VK_LAYER_PATH environment variable
TEST(EnvVarICDOverrideSetup, TestOnlyLayerEnvVar) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().physical_devices.push_back({});
    env.platform_shim->redirect_path("/tmp/carol", env.get_folder(ManifestLocation::explicit_layer_env_var).location());

    const char* layer_name = "TestLayer";
    env.add_explicit_layer(
        TestLayerDetails(ManifestLayer{}.add_layer(
                             ManifestLayer::LayerDescription{}.set_name(layer_name).set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
                         "test_layer.json")
            .set_discovery_type(ManifestDiscoveryType::env_var));

    // Now set up a layer path that includes default and user-specified locations,
    // so that the test app can find them.  Include some badly specified elements as well.
    // Need to redirect the 'home' directory
    fs::path HOME = "/home/fake_home";
    set_env_var("HOME", HOME.str());
    std::string vk_layer_path = ":/tmp/carol::::/:";
    vk_layer_path += (HOME / "/ with spaces/:::::/tandy:").str();
    set_env_var("VK_LAYER_PATH", vk_layer_path);

    InstWrapper inst1{env.vulkan_functions};
    inst1.create_info.add_layer(layer_name);
    FillDebugUtilsCreateDetails(inst1.create_info, env.debug_log);
    inst1.CheckCreate();

    // look for VK_LAYER_PATHS
    EXPECT_TRUE(env.debug_log.find("/tmp/carol"));
    EXPECT_TRUE(env.debug_log.find("/tandy"));
    EXPECT_TRUE(env.debug_log.find((HOME / "/ with spaces/").str()));

    env.debug_log.clear();

    env.platform_shim->set_elevated_privilege(true);

    InstWrapper inst2{env.vulkan_functions};
    inst2.create_info.add_layer(layer_name);
    FillDebugUtilsCreateDetails(inst2.create_info, env.debug_log);
    inst2.CheckCreate(VK_ERROR_LAYER_NOT_PRESENT);

    EXPECT_FALSE(env.debug_log.find("/tmp/carol"));

    env.platform_shim->set_elevated_privilege(false);

    remove_env_var("VK_LAYER_PATH");
}

// Test VK_ADD_LAYER_PATH environment variable
TEST(EnvVarICDOverrideSetup, TestOnlyAddLayerEnvVar) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().physical_devices.push_back({});
    env.platform_shim->redirect_path("/tmp/carol", env.get_folder(ManifestLocation::explicit_layer_add_env_var).location());

    const char* layer_name = "TestLayer";
    env.add_explicit_layer(
        TestLayerDetails(ManifestLayer{}.add_layer(
                             ManifestLayer::LayerDescription{}.set_name(layer_name).set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
                         "test_layer.json")
            .set_discovery_type(ManifestDiscoveryType::add_env_var));

    // Set up a layer path that includes default and user-specified locations,
    // so that the test app can find them.  Include some badly specified elements as well.
    // Need to redirect the 'home' directory
    fs::path HOME = "/home/fake_home";
    set_env_var("HOME", HOME.str());
    std::string vk_layer_path = ":/tmp/carol::::/:";
    vk_layer_path += (HOME / "/ with spaces/:::::/tandy:").str();
    set_env_var("VK_ADD_LAYER_PATH", vk_layer_path);

    InstWrapper inst1{env.vulkan_functions};
    inst1.create_info.add_layer(layer_name);
    FillDebugUtilsCreateDetails(inst1.create_info, env.debug_log);
    inst1.CheckCreate();

    // look for VK_ADD_LAYER_PATHS
    EXPECT_TRUE(env.debug_log.find("/tmp/carol"));
    EXPECT_TRUE(env.debug_log.find("/tandy"));
    EXPECT_TRUE(env.debug_log.find((HOME / "/ with spaces/").str()));

    env.debug_log.clear();

    env.platform_shim->set_elevated_privilege(true);

    InstWrapper inst2{env.vulkan_functions};
    inst2.create_info.add_layer(layer_name);
    FillDebugUtilsCreateDetails(inst2.create_info, env.debug_log);
    inst2.CheckCreate(VK_ERROR_LAYER_NOT_PRESENT);

    EXPECT_FALSE(env.debug_log.find("/tmp/carol"));

    env.platform_shim->set_elevated_privilege(false);

    remove_env_var("VK_ADD_LAYER_PATH");
}

#endif

// Test that the driver filter select will only enable driver manifest files that match the filter
TEST(EnvVarICDOverrideSetup, FilterSelectDriver) {
    FrameworkEnvironment env{};
    const char* filter_select_env_var = "VK_LOADER_DRIVERS_SELECT";

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6).set_disable_icd_inc(true).set_json_name("ABC_ICD"));
    env.add_icd(TestICDDetails{TEST_ICD_PATH_VERSION_6, VK_API_VERSION_1_2}.set_disable_icd_inc(true).set_json_name("BCD_ICD"));
    env.add_icd(TestICDDetails{TEST_ICD_PATH_VERSION_6, VK_API_VERSION_1_3}.set_disable_icd_inc(true).set_json_name("CDE_ICD"));

    InstWrapper inst1{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst1.create_info, env.debug_log);
    inst1.CheckCreate();

    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "ABC_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "BCD_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "CDE_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because it was disabled by env var"));

    // Match full-name
    env.debug_log.clear();
    set_env_var(filter_select_env_var, "ABC_ICD.json");

    InstWrapper inst2{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst2.create_info, env.debug_log);
    inst2.CheckCreate();

    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "ABC_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "BCD_ICD.json"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "CDE_ICD.json"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because it was disabled by env var"));

    // Match prefix
    env.debug_log.clear();
    set_env_var(filter_select_env_var, "ABC*");

    InstWrapper inst3{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst3.create_info, env.debug_log);
    inst3.CheckCreate();

    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "ABC_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "BCD_ICD.json"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "CDE_ICD.json"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because it was disabled by env var"));

    // Match suffix
    env.debug_log.clear();
    set_env_var(filter_select_env_var, "*C_ICD.json");

    InstWrapper inst4{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst4.create_info, env.debug_log);
    inst4.CheckCreate();

    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "ABC_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "BCD_ICD.json"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "CDE_ICD.json"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because it was disabled by env var"));

    // Match sub-string
    env.debug_log.clear();
    set_env_var(filter_select_env_var, "*BC*");

    InstWrapper inst5{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst5.create_info, env.debug_log);
    inst5.CheckCreate();

    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "ABC_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "BCD_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "CDE_ICD.json"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because it was disabled by env var"));

    // Match all with star '*'
    env.debug_log.clear();
    set_env_var(filter_select_env_var, "*");

    InstWrapper inst6{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst6.create_info, env.debug_log);
    inst6.CheckCreate();

    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "ABC_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "BCD_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "CDE_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because it was disabled by env var"));

    // Match all with special name
    env.debug_log.clear();
    set_env_var(filter_select_env_var, "~all~");

    InstWrapper inst7{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst7.create_info, env.debug_log);
    inst7.CheckCreate();

    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "ABC_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "BCD_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "CDE_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because it was disabled by env var"));

    remove_env_var(filter_select_env_var);
}

// Test that the driver filter disable disables driver manifest files that match the filter
TEST(EnvVarICDOverrideSetup, FilterDisableDriver) {
    FrameworkEnvironment env{};
    const char* filter_disable_env_var = "VK_LOADER_DRIVERS_DISABLE";

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6).set_disable_icd_inc(true).set_json_name("ABC_ICD"));
    env.add_icd(TestICDDetails{TEST_ICD_PATH_VERSION_6, VK_API_VERSION_1_2}.set_disable_icd_inc(true).set_json_name("BCD_ICD"));
    env.add_icd(TestICDDetails{TEST_ICD_PATH_VERSION_6, VK_API_VERSION_1_3}.set_disable_icd_inc(true).set_json_name("CDE_ICD"));

    InstWrapper inst1{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst1.create_info, env.debug_log);
    inst1.CheckCreate();

    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "ABC_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "BCD_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "CDE_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because it was disabled by env var"));

    // Match full-name
    env.debug_log.clear();
    set_env_var(filter_disable_env_var, "ABC_ICD.json");

    InstWrapper inst2{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst2.create_info, env.debug_log);
    inst2.CheckCreate();

    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "ABC_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because not selected by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "BCD_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "CDE_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because it was disabled by env var"));

    // Match prefix
    env.debug_log.clear();
    set_env_var(filter_disable_env_var, "ABC_*");

    InstWrapper inst3{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst3.create_info, env.debug_log);
    inst3.CheckCreate();

    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "ABC_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because not selected by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "BCD_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "CDE_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because it was disabled by env var"));

    // Match suffix
    env.debug_log.clear();
    set_env_var(filter_disable_env_var, "*C_ICD.json");

    InstWrapper inst4{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst4.create_info, env.debug_log);
    inst4.CheckCreate();

    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "ABC_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because not selected by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "BCD_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "CDE_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because it was disabled by env var"));

    // Match substring
    env.debug_log.clear();
    set_env_var(filter_disable_env_var, "*BC*");

    InstWrapper inst5{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst5.create_info, env.debug_log);
    inst5.CheckCreate();

    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "ABC_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because not selected by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "BCD_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because not selected by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "CDE_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because it was disabled by env var"));

    // Match all with star '*'
    env.debug_log.clear();
    set_env_var(filter_disable_env_var, "*");

    InstWrapper inst6{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst6.create_info, env.debug_log);
    inst6.CheckCreate(VK_ERROR_INCOMPATIBLE_DRIVER);

    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "ABC_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because not selected by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "BCD_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because not selected by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "CDE_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because not selected by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because it was disabled by env var"));

    // Match all with special name
    env.debug_log.clear();
    set_env_var(filter_disable_env_var, "~all~");

    InstWrapper inst7{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst7.create_info, env.debug_log);
    inst7.CheckCreate(VK_ERROR_INCOMPATIBLE_DRIVER);

    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "ABC_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because not selected by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "BCD_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because not selected by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "CDE_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because not selected by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because it was disabled by env var"));

    remove_env_var(filter_disable_env_var);
}

// Test that the when both driver filter select and disable environment variables are present, that the
// appropriate drivers are enabled and disabled
TEST(EnvVarICDOverrideSetup, FilterSelectAndDisableDriver) {
    FrameworkEnvironment env{};
    const char* filter_select_env_var = "VK_LOADER_DRIVERS_SELECT";
    const char* filter_disable_env_var = "VK_LOADER_DRIVERS_DISABLE";

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6).set_disable_icd_inc(true).set_json_name("ABC_ICD"));
    env.add_icd(TestICDDetails{TEST_ICD_PATH_VERSION_6, VK_API_VERSION_1_2}.set_disable_icd_inc(true).set_json_name("BCD_ICD"));
    env.add_icd(TestICDDetails{TEST_ICD_PATH_VERSION_6, VK_API_VERSION_1_3}.set_disable_icd_inc(true).set_json_name("CDE_ICD"));

    InstWrapper inst1{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst1.create_info, env.debug_log);
    inst1.CheckCreate();

    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "ABC_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "BCD_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "CDE_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because it was disabled by env var"));

    // Disable two, but enable one
    env.debug_log.clear();
    set_env_var(filter_disable_env_var, "*BC*");
    set_env_var(filter_select_env_var, "BCD*");

    InstWrapper inst2{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst2.create_info, env.debug_log);
    inst2.CheckCreate();

    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "ABC_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because not selected by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "BCD_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "CDE_ICD.json"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because it was disabled by env var"));

    // Disable all, but enable two
    env.debug_log.clear();
    set_env_var(filter_disable_env_var, "*");
    set_env_var(filter_select_env_var, "*BC*");

    InstWrapper inst3{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst3.create_info, env.debug_log);
    inst3.CheckCreate();

    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "ABC_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "BCD_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "CDE_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because not selected by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because it was disabled by env var"));

    // Disable all, but enable all
    env.debug_log.clear();
    set_env_var(filter_disable_env_var, "*");
    set_env_var(filter_select_env_var, "*");

    InstWrapper inst4{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst4.create_info, env.debug_log);
    inst4.CheckCreate();

    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "ABC_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "ABC_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "BCD_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "BCD_ICD.json", "ignored because it was disabled by env var"));
    ASSERT_TRUE(FindPrefixPostfixStringOnLine(env.debug_log, "Found ICD manifest file", "CDE_ICD.json"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because not selected by env var"));
    ASSERT_FALSE(FindPrefixPostfixStringOnLine(env.debug_log, "CDE_ICD.json", "ignored because it was disabled by env var"));

    remove_env_var(filter_select_env_var);
    remove_env_var(filter_disable_env_var);
}
