#include <aut/AUL_Utils.h>
#include <CL/cl.hpp>
#include <lua.hpp>

int OpticsCompensation(lua_State *L) {
    aut::PixelRGBA *image_data;
    aut::Size2D image_size;
    aut::getpixeldata(L, &image_data, &image_size);

    for (int y = 0; y < image_size.h; y++) {
        for (int x = 0; x < image_size.w; x++) {
            int index = x + (y * image_size.w);
            image_data[index].r *= 0.5;
        }
    }

    aut::putpixeldata(L, image_data);

    return 0;
}

// Lua側に登録する関数
static luaL_Reg optics_compensation[] = {
{"OpticsCompensation", OpticsCompensation},
{nullptr, nullptr}
};

// Lua側にモジュールを登録
extern "C" {
__declspec(dllexport) int luaopen_OpticsCompensation_s(lua_State *L) {
    luaL_register(L, "OpticsCompensation_s", optics_compensation);
    return 1;
}
}