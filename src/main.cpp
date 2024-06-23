#include <thread>
#include <chrono>

#include "options.hpp"
#include "overlay.hpp"
#include "log.hpp"
#include "input.hpp"
#include "draw.hpp"

int main() {
    Log log{};
    overlay overlay{};
    try {
        overlay.begin();
    }
    catch (std::invalid_argument exception) {
        log.error(exception.what());
        log.wait();
        return EXIT_FAILURE;
    }
    log.ok("press insert to toggle menu");

    bool showMenu{};
    while (overlay.rendering) {
        overlay.beginRender();

        if (input::toggled(VK_INSERT)) {
            showMenu = !showMenu;
            overlay.clickable(showMenu);
        }
        if (showMenu) draw::menu();

        if (options::crosshair) {
            if (!options::t) draw::line(overlay.centerX, overlay.centerY - options::seperation, overlay.centerX, overlay.centerY - options::size, options::color, options::width);
            draw::line(overlay.centerX, overlay.centerY + options::seperation, overlay.centerX, overlay.centerY + options::size, options::color, options::width);

            draw::line(overlay.centerX - options::seperation, overlay.centerY, overlay.centerX - options::size, overlay.centerY, options::color, options::width);
            draw::line(overlay.centerX + options::seperation, overlay.centerY, overlay.centerX + options::size, overlay.centerY, options::color, options::width);
        }
        overlay.streamproof(options::streamproof);

        overlay.endRender();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    overlay.end();

    return EXIT_SUCCESS;
}