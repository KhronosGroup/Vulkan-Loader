### Android

vkjson_info for Android is built as an executable for devices with root access.

To use, simply push it to the device and run it:

    ./build_all.sh
    adb push obj/local/<abi>/vkjson_info /data/tmp/
    adb shell /data/tmp/vkjson_info

The resulting json file will be found in:

    /sdcard/Android/<device_name>.json