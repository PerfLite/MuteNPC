set_xmakever("2.8.2")

set_project("MuteNPC")
set_version("1.0.0")

set_languages("c++23")
set_warnings("allextra")

add_rules("mode.release", "mode.debug")
add_rules("plugin.vsxmake.autoupdate")

add_requires("commonlibsse-ng")

target("MuteNPC")
    set_kind("shared")
    add_packages("commonlibsse-ng")

    add_files("src/*.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/Pch.h")

    set_runtimes("MD")
    add_cxflags("/utf-8")
    add_syslinks("user32", "gdi32", "shell32")
