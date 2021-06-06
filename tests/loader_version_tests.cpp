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

class ICDEnvVarSetup : public ::testing::Test {
   protected:
    virtual void SetUp() {}

    virtual void TearDown() {
        remove_env_var("VK_ICD_FILENAMES");
        platform_shim->platform_shim->clear_override();

        driver_store->remove(manifest_name);
        vulkan_functions.reset();
        platform_shim.reset();
        driver_store.reset();
    }

   public:
    std::string manifest_name;
    std::unique_ptr<detail::PlatformShimWrapper> platform_shim;
    std::unique_ptr<fs::FolderManager> driver_store;
    std::unique_ptr<fs::FolderManager> null_dir;
    LibraryWrapper driver_wrapper;
    GetNewTestICDFunc get_new_test_icd;
    std::unique_ptr<VulkanFunctions> vulkan_functions;
};

void SetupICDEnvVar(ICDEnvVarSetup& env, const char* icd_path, const char* manifest_name) {
    env.manifest_name = manifest_name;
    env.platform_shim = std::unique_ptr<detail::PlatformShimWrapper>(new detail::PlatformShimWrapper());
    env.platform_shim->platform_shim->setup_override();
    env.driver_store =
        std::unique_ptr<fs::FolderManager>(new fs::FolderManager(FRAMEWORK_BUILD_DIRECTORY, "version_test_manifests"));
    env.null_dir = std::unique_ptr<fs::FolderManager>(new fs::FolderManager(FRAMEWORK_BUILD_DIRECTORY, "null_dir"));
    env.platform_shim->platform_shim->redirect_all_paths(env.null_dir->location());
    env.driver_wrapper = LibraryWrapper(fs::path(icd_path));
    env.get_new_test_icd = env.driver_wrapper.get_symbol<GetNewTestICDFunc>(GET_NEW_TEST_ICD_FUNC_STR);
#if defined(WIN32)
    env.platform_shim->platform_shim->set_elevation_level(SECURITY_MANDATORY_LOW_RID);
#endif
    ManifestICD icd_manifest;
    icd_manifest.lib_path = icd_path;
    icd_manifest.api_version = VK_MAKE_VERSION(1, 0, 0);

    env.driver_store->write(manifest_name, icd_manifest);
    set_env_var("VK_ICD_FILENAMES", (env.driver_store->location() / manifest_name).str());
    env.vulkan_functions = std::unique_ptr<VulkanFunctions>(new VulkanFunctions());
}

// Don't support vk_icdNegotiateLoaderICDInterfaceVersion
// Loader calls vk_icdGetInstanceProcAddr second
// does not support vk_icdGetInstanceProcAddr
// must export vkGetInstanceProcAddr, vkCreateInstance, vkEnumerateInstanceExtensionProperties
TEST_F(ICDEnvVarSetup, version_0_none) {
    SetupICDEnvVar(*this, TEST_ICD_PATH_EXPORT_NONE, "test_icd_export_none.json");
    auto* driver = get_new_test_icd();

    InstWrapper inst{*vulkan_functions};
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);

    ASSERT_EQ(driver->called_vk_icd_gipa, CalledICDGIPA::vk_gipa);
}

// Don't support vk_icdNegotiateLoaderICDInterfaceVersion
// the loader calls vk_icdGetInstanceProcAddr first
TEST_F(ICDEnvVarSetup, version_1_icd_gipa) {
    SetupICDEnvVar(*this, TEST_ICD_PATH_EXPORT_ICD_GIPA, "test_icd_export_icd_gipa.json");
    auto* driver = get_new_test_icd();

    InstWrapper inst{*vulkan_functions};
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);

    ASSERT_EQ(driver->called_vk_icd_gipa, CalledICDGIPA::vk_icd_gipa);
}

// support vk_icdNegotiateLoaderICDInterfaceVersion but not vk_icdGetInstanceProcAddr
// should assert that `interface_vers == 0` due to version mismatch, only checkable in Debug Mode
TEST_F(ICDEnvVarSetup, version_negotiate_interface_version_death_test) {
    // may be needed to surpress debug assert popups on windows
    //::testing::FLAGS_gtest_death_test_style = "threadsafe";
    SetupICDEnvVar(*this, TEST_ICD_PATH_EXPORT_NEGOTIATE_INTERFACE_VERSION, "test_icd_export_negotiate_interface_version.json");

    InstWrapper inst{*vulkan_functions};
    InstanceCreateInfo inst_create_info;
#if !defined(NDEBUG)
#if defined(WIN32)
    ASSERT_DEATH(CreateInst(inst, inst_create_info), "");
#else
    ASSERT_DEATH(CreateInst(inst, inst_create_info), "interface_vers == 0");
#endif
#else
    ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);
#endif
}

// export vk_icdNegotiateLoaderICDInterfaceVersion and vk_icdGetInstanceProcAddr
TEST_F(ICDEnvVarSetup, version_2_negotiate_interface_version_and_icd_gipa) {
    SetupICDEnvVar(*this, TEST_ICD_PATH_VERSION_2, "test_icd_version_2.json");
    auto* driver = get_new_test_icd();

    InstWrapper inst{*vulkan_functions};
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);

    ASSERT_EQ(driver->called_vk_icd_gipa, CalledICDGIPA::vk_icd_gipa);
}

class ICDInterfaceVersion2Plus : public ::testing::Test {
   protected:
    virtual void SetUp() { env = std::unique_ptr<SingleICDShim>(new SingleICDShim(TEST_ICD_PATH_VERSION_2)); }

    virtual void TearDown() { env.reset(); }
    std::unique_ptr<SingleICDShim> env;
};

TEST_F(ICDInterfaceVersion2Plus, vk_icdNegotiateLoaderICDInterfaceVersion) {
    auto& driver = env->get_test_icd();

    for (uint32_t i = 0; i <= 6; i++) {
        for (uint32_t j = i; j <= 6; j++) {
            driver.min_icd_interface_version = i;
            driver.max_icd_interface_version = j;
            InstWrapper inst{env->vulkan_functions};
            InstanceCreateInfo inst_create_info;
            EXPECT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);
        }
    }
}

TEST_F(ICDInterfaceVersion2Plus, version_3) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    {
        driver.min_icd_interface_version = 2;
        driver.enable_icd_wsi = true;
        InstWrapper inst{env->vulkan_functions};
        InstanceCreateInfo inst_create_info;
        ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);
        ASSERT_EQ(driver.is_using_icd_wsi, UsingICDProvidedWSI::not_using);
    }
    {
        driver.min_icd_interface_version = 3;
        driver.enable_icd_wsi = false;
        InstWrapper inst{env->vulkan_functions};
        InstanceCreateInfo inst_create_info;
        ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);
        ASSERT_EQ(driver.is_using_icd_wsi, UsingICDProvidedWSI::not_using);
    }
    {
        driver.min_icd_interface_version = 3;
        driver.enable_icd_wsi = true;
        InstWrapper inst{env->vulkan_functions};
        InstanceCreateInfo inst_create_info;
        ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);
        ASSERT_EQ(driver.is_using_icd_wsi, UsingICDProvidedWSI::is_using);
    }
}

TEST_F(ICDInterfaceVersion2Plus, version_4) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    InstWrapper inst{env->vulkan_functions};
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);
}

TEST_F(ICDInterfaceVersion2Plus, l4_icd4) {
    // ICD must fail with VK_ERROR_INCOMPATIBLE_DRIVER for all vkCreateInstance calls with apiVersion set to > Vulkan 1.0
    // because both the loader and ICD support interface version <= 4. Otherwise, the ICD should behave as normal.
}
TEST_F(ICDInterfaceVersion2Plus, l4_icd5) {
    // ICD must fail with VK_ERROR_INCOMPATIBLE_DRIVER for all vkCreateInstance calls with apiVersion set to > Vulkan 1.0
    // because the loader is still at interface version <= 4. Otherwise, the ICD should behave as normal.
}
TEST_F(ICDInterfaceVersion2Plus, l5_icd4) {
    // Loader will fail with VK_ERROR_INCOMPATIBLE_DRIVER if it can't handle the apiVersion. ICD may pass for all apiVersions,
    // but since its interface is <= 4, it is best if it assumes it needs to do the work of rejecting anything > Vulkan 1.0 and
    // fail with VK_ERROR_INCOMPATIBLE_DRIVER. Otherwise, the ICD should behave as normal.
}
TEST_F(ICDInterfaceVersion2Plus, l5_icd5) {
    // Loader will fail with VK_ERROR_INCOMPATIBLE_DRIVER if it can't handle the apiVersion, and ICDs should fail with
    // VK_ERROR_INCOMPATIBLE_DRIVER only if they can not support the specified apiVersion. Otherwise, the ICD should behave as
    // normal.
}

class ICDInterfaceVersion2PlusEnumerateAdapterPhysicalDevices : public ::testing::Test {
   protected:
    virtual void SetUp() {
        env = std::unique_ptr<SingleICDShim>(
            new SingleICDShim(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_ENUMERATE_ADAPTER_PHYSICAL_DEVICES));
    }

    virtual void TearDown() { env.reset(); }
    std::unique_ptr<SingleICDShim> env;
};

// Need more work to shim dxgi for this test to work
#if defined(WIN32)
// Version 6 provides a mechanism to allow the loader to sort physical devices.
// The loader will only attempt to sort physical devices on an ICD if version 6 of the interface is supported.
// This version provides the vk_icdEnumerateAdapterPhysicalDevices function.
TEST_F(ICDInterfaceVersion2Plus, version_5) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_1");
    driver.physical_devices.emplace_back("physical_device_0");
    uint32_t physical_count = driver.physical_devices.size();
    uint32_t returned_physical_count = driver.physical_devices.size();
    std::vector<VkPhysicalDevice> physical_device_handles = std::vector<VkPhysicalDevice>(physical_count);

    driver.min_icd_interface_version = 5;

    InstWrapper inst{env->vulkan_functions};
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);

    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.fp_vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count,
                                                                              physical_device_handles.data()));
    ASSERT_EQ(physical_count, returned_physical_count);
    ASSERT_EQ(driver.called_enumerate_adapter_physical_devices, CalledEnumerateAdapterPhysicalDevices::not_called);
}
TEST_F(ICDInterfaceVersion2PlusEnumerateAdapterPhysicalDevices, version_6) {
    // Version 6 provides a mechanism to allow the loader to sort physical devices.
    // The loader will only attempt to sort physical devices on an ICD if version 6 of the interface is supported.
    // This version provides the vk_icdEnumerateAdapterPhysicalDevices function.
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_1");
    driver.physical_devices.emplace_back("physical_device_0");
    uint32_t physical_count = driver.physical_devices.size();
    uint32_t returned_physical_count = driver.physical_devices.size();
    std::vector<VkPhysicalDevice> physical_device_handles = std::vector<VkPhysicalDevice>(physical_count);

    driver.min_icd_interface_version = 6;

    uint32_t driver_index = 2; //which drive this test pretends to be
    auto& known_driver = known_driver_list.at(2);
    DXGI_ADAPTER_DESC1 desc1{};
    wcsncpy(&desc1.Description[0], L"TestDriver1", 128);
    desc1.VendorId = known_driver.vendor_id;
    desc1.AdapterLuid;
    desc1.Flags = DXGI_ADAPTER_FLAG_NONE;
    env->platform_shim->add_dxgi_adapter(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_ENUMERATE_ADAPTER_PHYSICAL_DEVICES, GpuType::discrete, driver_index, desc1);


    InstWrapper inst{env->vulkan_functions};
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);

    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.fp_vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, nullptr));
    ASSERT_EQ(physical_count, returned_physical_count);
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.fp_vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count,
                                                                              physical_device_handles.data()));
    ASSERT_EQ(physical_count, returned_physical_count);
    ASSERT_EQ(driver.called_enumerate_adapter_physical_devices, CalledEnumerateAdapterPhysicalDevices::called);
}

TEST_F(ICDInterfaceVersion2PlusEnumerateAdapterPhysicalDevices, EnumAdapters2) {

InstWrapper inst{env->vulkan_functions};
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_1");
    driver.physical_devices.emplace_back("physical_device_0");
    uint32_t physical_count = driver.physical_devices.size();
    uint32_t returned_physical_count = driver.physical_devices.size();
    std::vector<VkPhysicalDevice> physical_device_handles = std::vector<VkPhysicalDevice>(physical_count);

    SHIM_D3DKMT_ADAPTERINFO d3dkmt_adapter_info{};
    d3dkmt_adapter_info.hAdapter = 0; //
    d3dkmt_adapter_info.AdapterLuid = _LUID{10, 1000};
    d3dkmt_adapter_info.NumOfSources = 1;
    d3dkmt_adapter_info.bPresentMoveRegionsPreferred = true;

    env->platform_shim->add_d3dkmt_adapter(d3dkmt_adapter_info, env->get_test_icd_path());

    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);

    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.fp_vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, nullptr));
    ASSERT_EQ(physical_count, returned_physical_count);
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.fp_vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count,
                                                                              physical_device_handles.data()));
    ASSERT_EQ(physical_count, returned_physical_count);
}
#endif  // defined(WIN32)

TEST(MultipleICDConfig, Basic) {
    MultipleICDShim env(
        {TestICDDetails(TEST_ICD_PATH_VERSION_2), TestICDDetails(TEST_ICD_PATH_VERSION_2), TestICDDetails(TEST_ICD_PATH_VERSION_2)},
        DebugMode::none);

    env.get_test_icd(0).physical_devices.emplace_back("physical_device_0");
    env.get_test_icd(1).physical_devices.emplace_back("physical_device_1");
    env.get_test_icd(2).physical_devices.emplace_back("physical_device_2");

    env.get_test_icd(0).physical_devices.at(0).properties.deviceType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
    env.get_test_icd(1).physical_devices.at(0).properties.deviceType = VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
    env.get_test_icd(2).physical_devices.at(0).properties.deviceType = VK_PHYSICAL_DEVICE_TYPE_CPU;

    strcpy(env.get_test_icd(0).physical_devices.at(0).properties.deviceName, "dev0");
    strcpy(env.get_test_icd(1).physical_devices.at(0).properties.deviceName, "dev1");
    strcpy(env.get_test_icd(2).physical_devices.at(0).properties.deviceName, "dev2");

    InstWrapper inst{env.vulkan_functions};
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);

    std::array<VkPhysicalDevice, 3> phys_devs_array;
    uint32_t phys_dev_count = 3;
    ASSERT_EQ(env.vulkan_functions.fp_vkEnumeratePhysicalDevices(inst, &phys_dev_count, phys_devs_array.data()), VK_SUCCESS);
    ASSERT_EQ(phys_dev_count, 3);
    ASSERT_EQ(env.get_test_icd(0).physical_devices.at(0).properties.deviceType, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
    ASSERT_EQ(env.get_test_icd(1).physical_devices.at(0).properties.deviceType, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
    ASSERT_EQ(env.get_test_icd(2).physical_devices.at(0).properties.deviceType, VK_PHYSICAL_DEVICE_TYPE_CPU);
}

TEST(MultipleDriverConfig, DifferentICDInterfaceVersions) {
    MultipleICDShim env({TestICDDetails(TEST_ICD_PATH_EXPORT_ICD_GIPA), TestICDDetails(TEST_ICD_PATH_VERSION_2),
                            TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA)},
                           DebugMode::none);

    TestICD& icd0 = env.get_test_icd(0);
    icd0.physical_devices.emplace_back("physical_device_0");
    icd0.max_icd_interface_version = 1;

    TestICD& icd1 = env.get_test_icd(1);
    icd1.physical_devices.emplace_back("physical_device_1");
    icd1.min_icd_interface_version = 2;
    icd1.max_icd_interface_version = 5;

    InstWrapper inst{env.vulkan_functions};
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);

    std::array<VkPhysicalDevice, 2> phys_devs_array;
    uint32_t phys_dev_count = 2;
    ASSERT_EQ(env.vulkan_functions.fp_vkEnumeratePhysicalDevices(inst, &phys_dev_count, phys_devs_array.data()), VK_SUCCESS);
    ASSERT_EQ(phys_dev_count, 2);
}
