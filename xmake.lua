set_project("CoopAndreas")

local force_msvc = true;

set_languages("cxx17")
set_arch("x86")
set_plat("windows")
set_toolchains("clang-cl") -- required to generate compile_commands.json properly

add_rules("plugin.compile_commands.autoupdate")
add_rules("mode.debug", "mode.release")

if is_os("windows") then
    if is_mode("debug") then
        set_runtimes("MTd")
    else 
        set_runtimes("MT")
    end
end

target("client")
    set_kind("shared")
    if force_msvc == true then
        set_toolchains("msvc")
    end
    set_basename("CoopAndreasSA")

    local gta_sa_dir = os.getenv("GTA_SA_DIR")
    if gta_sa_dir then
        set_targetdir(gta_sa_dir)
    end

    set_arch("x86")
    set_plat("windows")
    
    add_files("client/src/*.cpp")
    add_files("client/src/Commands/*.cpp")
    add_files("client/src/Commands/Commands/*.cpp")
    add_files("client/src/game_sa/*.cpp")
    add_files("client/src/Hooks/*.cpp")

    add_headerfiles("client/src/*.h")
    add_headerfiles("client/src/Commands/*.h")
    add_headerfiles("client/src/Commands/Commands/*.h")
    add_headerfiles("client/src/game_sa/*.h")
    add_headerfiles("client/src/Hooks/*.h")

    add_includedirs("client/src")

    add_files("third_party/enet/*.c")
    add_files("third_party/DiscordRPC/SDK/src/*.cpp")

    set_pcxxheader("client/src/stdafx.h")

    add_includedirs("shared", "third_party")
    add_includedirs("third_party/DiscordRPC/SDK/include")

    add_syslinks("kernel32", "user32", "ws2_32", "winmm", "Advapi32")
    
    add_files("client/version.rc")

    add_defines("_CRT_SECURE_NO_WARNINGS", "_CRT_NON_CONFORMING_SWPRINTFS", "_USE_MATH_DEFINES", "_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING", "GTASA", "PLUGIN_SGV_10US", "RW")

    if is_mode("debug") then
        add_defines("DEBUG")
    else
        add_defines("NDEBUG")
        set_strip("all")
        set_optimize("fastest")
    end

    on_load(function (target)
        local plugin_sdk = os.getenv("PLUGIN_SDK_DIR")
        if not plugin_sdk then
            raise("PLUGIN_SDK_DIR environment variable is not set")
        end

        target:add("includedirs", 
            path.join(plugin_sdk, "shared"), 
            path.join(plugin_sdk, "shared/game"), 
            path.join(plugin_sdk, "shared/dxsdk"), 
            path.join(plugin_sdk, "plugin_sa"),
            path.join(plugin_sdk, "plugin_sa/game_sa")
        )

        target:add("linkdirs", 
            path.join(plugin_sdk, "output/lib"),
            path.join(plugin_sdk, "shared/bass"),
            path.join(plugin_sdk, "shared/dxsdk")
        )

        if is_mode("debug") then 
            target:add("links", "plugin_d")
        else 
            target:add("links", "plugin")
        end

        target:add("links", "d3dx9")
    end)

target("server")
    set_kind("binary")

    add_files("server/src/*.cpp")
    add_headerfiles("server/src/*.h")
    
    add_files("third_party/enet/*.c")
    add_headerfiles("third_party/enet/*.h")

    add_files("third_party/INIReader/cpp/INIReader.cpp")
    add_files("third_party/INIReader/ini.c")

    add_includedirs("shared", "third_party")

    if is_mode("debug") then
        add_defines("_DEBUG")
        set_optimize("none")
        set_symbols("debug")
    else
        add_defines("NDEBUG")
        set_optimize("fastest")
        set_strip("all")
    end

    if is_os("windows") then
        if force_msvc == true then
            set_toolchains("msvc")
        end
        add_files("server/version.rc")
        add_defines("_CRT_SECURE_NO_WARNINGS", "WIN32", "_CONSOLE")

    elseif is_os("linux") then
        -- TODO
    end

target("proxy")
    set_kind("shared")
    if force_msvc == true then
        set_toolchains("msvc")
    end
    set_arch("x86")
    set_plat("windows")

    add_files("proxy/src/*.cpp")
    add_headerfiles("proxy/src/*.h")
    
    add_syslinks("kernel32", "user32")

    add_defines("PROXY_EXPORTS", "_WINDOWS", "_USRDLL")

    if is_mode("debug") then
        add_defines("_DEBUG")
        set_symbols("debug")
    else
        add_defines("NDEBUG")
        set_optimize("fastest")
        set_strip("all")
    end