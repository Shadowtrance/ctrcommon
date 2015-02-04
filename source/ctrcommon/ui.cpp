#include "ctrcommon/common.hpp"

#include <sys/dirent.h>
#include <sys/errno.h>
#include <string.h>

#include <stack>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <arpa/inet.h>

struct ui_alphabetize {
    inline bool operator()(SelectableElement a, SelectableElement b) {
        return strcasecmp(a.name.c_str(), b.name.c_str()) < 0;
    }
};

bool ui_select(SelectableElement *selected, std::vector<SelectableElement> elements, std::function<bool(std::vector<SelectableElement> &currElements, bool &elementsDirty)> onLoop, std::function<bool(SelectableElement)> onSelect) {
    u32 cursor = 0;
    u32 scroll = 0;

    u32 selectionScroll = 0;
    u64 selectionScrollEndTime = 0;

    bool elementsDirty = false;
    std::sort(elements.begin(), elements.end(), ui_alphabetize());
    while(platform_is_running()) {
        input_poll();
        if(input_is_pressed(BUTTON_A)) {
            SelectableElement select = elements.at(cursor);
            if(onSelect(select)) {
                *selected = select;
                return true;
            }
        }

        if(input_is_pressed(BUTTON_DOWN) && cursor < elements.size() - 1) {
            cursor++;
            if(cursor >= scroll + 20) {
                scroll++;
            }

            selectionScroll = 0;
            selectionScrollEndTime = 0;
        }

        if(input_is_pressed(BUTTON_UP) && cursor > 0) {
            cursor--;
            if(cursor < scroll) {
                scroll--;
            }

            selectionScroll = 0;
            selectionScrollEndTime = 0;
        }

        screen_begin_draw(BOTTOM_SCREEN);
        screen_clear(0, 0, 0);

        u16 screenWidth = screen_get_width();
        for(std::vector<SelectableElement>::iterator it = elements.begin() + scroll; it != elements.begin() + scroll + 20 && it != elements.end(); it++) {
            SelectableElement element = *it;
            u32 index = (u32) (it - elements.begin());
            u8 color = 255;
            int offset = 0;
            if(index == cursor) {
                color = 0;
                screen_fill(0, (int) (index - scroll) * 12, screenWidth, screen_get_str_height(element.name), 255, 255, 255);
                u32 width = (u32) screen_get_str_width(element.name);
                if(width > screenWidth) {
                    if(selectionScroll + screenWidth >= width) {
                        if(selectionScrollEndTime == 0) {
                            selectionScrollEndTime = platform_get_time();
                        } else if(platform_get_time() - selectionScrollEndTime >= 4000) {
                            selectionScroll = 0;
                            selectionScrollEndTime = 0;
                        }
                    } else {
                        selectionScroll++;
                    }
                }

                offset = -selectionScroll;
            }

            screen_draw_string(element.name, offset, (int) (index - scroll) * 12, color, color, color);
        }

        screen_end_draw();

        screen_begin_draw(TOP_SCREEN);
        screen_clear(0, 0, 0);

        SelectableElement currSelected = elements.at(cursor);
        if(currSelected.details.size() != 0) {
            std::stringstream details;
            for(std::vector<std::string>::iterator it = currSelected.details.begin(); it != currSelected.details.end(); it++) {
                details << *it << "\n";
            }

            screen_draw_string(details.str(), 0, 0, 255, 255, 255);
        }

        bool result = onLoop(elements, elementsDirty);
        if(elementsDirty) {
            cursor = 0;
            scroll = 0;
            selectionScroll = 0;
            selectionScrollEndTime = 0;
            std::sort(elements.begin(), elements.end(), ui_alphabetize());
            elementsDirty = false;
        }

        screen_end_draw();
        screen_swap_buffers();
        if(result) {
            break;
        }
    }

    return false;
}

bool ui_is_directory(const std::string path) {
    DIR *dir = opendir(path.c_str());
    if(!dir) {
        return false;
    }

    closedir(dir);
    return true;
}

void ui_get_dir_contents(std::vector<SelectableElement> &elements, const std::string directory, std::vector<std::string> extensions) {
    elements.clear();
    elements.push_back({".", "."});
    elements.push_back({"..", ".."});

    DIR *dir = opendir(directory.c_str());
    if(dir == NULL) {
        return;
    }

    while(true) {
        struct dirent *ent = readdir(dir);
        if(ent == NULL) {
            break;
        }

        const std::string dirName = std::string(ent->d_name);
        const std::string path = directory + "/" + dirName;
        if(ui_is_directory(path)) {
            elements.push_back({path, dirName});
        } else {
            std::string::size_type dotPos = path.rfind('.');
            if(extensions.empty() || (dotPos != std::string::npos && std::find(extensions.begin(), extensions.end(), path.substr(dotPos + 1)) != extensions.end())) {
                struct stat st;
                stat(path.c_str(), &st);

                std::vector<std::string> info;
                std::stringstream stream;
                stream << "File Size: " << st.st_size << " bytes (" << std::fixed << std::setprecision(2) << st.st_size / 1024.0f / 1024.0f << "MB)";
                info.push_back(stream.str());
                elements.push_back({path, dirName, info});
            }
        }
    }

    closedir(dir);
}

bool ui_select_file(std::string *selectedFile, const std::string rootDirectory, std::vector<std::string> extensions, std::function<bool()> onLoop) {
    std::stack<std::string> directoryStack;
    std::string currDirectory = rootDirectory;

    std::vector<SelectableElement> elements;
    ui_get_dir_contents(elements, currDirectory, extensions);

    bool changeDirectory = false;
    SelectableElement selected;
    bool result = ui_select(&selected, elements, [&](std::vector<SelectableElement> &currElements, bool &elementsDirty) {
        if(onLoop()) {
            return true;
        }

        if(input_is_pressed(BUTTON_B) && !directoryStack.empty()) {
            currDirectory = directoryStack.top();
            directoryStack.pop();
            changeDirectory = true;
        }

        if(changeDirectory) {
            ui_get_dir_contents(currElements, currDirectory, extensions);
            elementsDirty = true;
            changeDirectory = false;
        }

        return false;
    }, [&](SelectableElement select) {
        if(select.name.compare(".") == 0) {
            return false;
        } else if(select.name.compare("..") == 0) {
            if(!directoryStack.empty()) {
                currDirectory = directoryStack.top();
                directoryStack.pop();
                changeDirectory = true;
            }

            return false;
        } else if(ui_is_directory(select.id)) {
            directoryStack.push(currDirectory);
            currDirectory = select.id;
            changeDirectory = true;
            return false;
        }

        return true;
    });

    if(result) {
        *selectedFile = selected.id;
    }

    return result;
}

bool ui_select_app(App *selectedApp, MediaType mediaType, std::function<bool()> onLoop) {
    std::vector<App> apps = app_list(mediaType);
    std::vector<SelectableElement> elements;
    for(std::vector<App>::iterator it = apps.begin(); it != apps.end(); it++) {
        App app = *it;

        std::stringstream titleId;
        titleId << std::setfill('0') << std::setw(16) << std::hex << app.titleId;

        std::stringstream uniqueId;
        uniqueId << std::setfill('0') << std::setw(8) << std::hex << app.uniqueId;

        std::vector<std::string> details;
        details.push_back("Title ID: " + titleId.str());
        details.push_back("Unique ID: " + uniqueId.str());
        details.push_back("Product Code: " + std::string(app.productCode));
        details.push_back("Platform: " + app_get_platform_name(app.platform));
        details.push_back("Category: " + app_get_category_name(app.category));

        elements.push_back({titleId.str(), app.productCode, details});
    }

    if(elements.size() == 0) {
        elements.push_back({"None", "None"});
    }

    SelectableElement selected;
    bool result = ui_select(&selected, elements, [&](std::vector<SelectableElement> &currElements, bool &elementsDirty) {
        return onLoop();
    }, [&](SelectableElement select) {
        return select.name.compare("None") != 0;
    });

    if(result) {
        for(std::vector<App>::iterator it = apps.begin(); it != apps.end(); it++) {
            App app = *it;
            if(app.titleId == (u64) strtoll(selected.id.c_str(), NULL, 16)) {
                *selectedApp = app;
            }
        }
    }

    return result;
}

void ui_display_message(const std::string message) {
    screen_begin_draw(TOP_SCREEN);
    screen_clear(0, 0, 0);
    screen_draw_string(message, (screen_get_width() - screen_get_str_width(message)) / 2, (screen_get_height() - screen_get_str_height(message)) / 2, 255, 255, 255);
    screen_end_draw();
    screen_swap_buffers();
}

bool ui_prompt(const std::string message, bool question) {
    std::stringstream stream;
    stream << message << "\n";
    if(question) {
        stream << "Press A to confirm, B to cancel." << "\n";
    } else {
        stream << "Press Start to continue." << "\n";
    }

    std::string str = stream.str();
    while(platform_is_running()) {
        input_poll();
        if(question) {
            if(input_is_pressed(BUTTON_A)) {
                return true;
            }

            if(input_is_pressed(BUTTON_B)) {
                return false;
            }
        } else {
            if(input_is_pressed(BUTTON_START)) {
                return true;
            }
        }

        ui_display_message(str);
    }

    return false;
}

void ui_display_progress(const std::string operation, const std::string details, bool quickSwap, int progress) {
    std::stringstream stream;
    stream << operation << ": [";
    int progressBars = progress / 4;
    for(int i = 0; i < 25; i++) {
        if(i < progressBars) {
            stream << '|';
        } else {
            stream << ' ';
        }
    }

    std::ios state(NULL);
    state.copyfmt(stream);
    stream << "] " << std::setfill(' ') << std::setw(3) << progress;
    stream.copyfmt(state);
    stream << "%" << "\n";
    stream << details << "\n";

    std::string str = stream.str();

    screen_begin_draw(TOP_SCREEN);
    screen_clear(0, 0, 0);
    screen_draw_string(str, (screen_get_width() - screen_get_str_width(str)) / 2, (screen_get_height() - screen_get_str_height(str)) / 2, 255, 255, 255);
    screen_end_draw();
    if(quickSwap) {
        screen_swap_buffers_quick();
    } else {
        screen_swap_buffers();
    }
}

RemoteFile ui_accept_remote_file() {
    ui_display_message("Initializing...");

    int listen = socket_listen(5000);
    if(listen < 0) {
        std::stringstream errStream;
        errStream << "Failed to initialize: Error " << errno;
        ui_prompt(errStream.str(), false);
        return {-1, 0};
    }

    std::stringstream waitStream;
    waitStream << "Waiting for peer to connect..." << "\n";
    waitStream << "IP: " << inet_ntoa({socket_get_host_ip()}) << "\n";
    waitStream << "Press B to cancel." << "\n";
    ui_display_message(waitStream.str());

    int socket;
    while((socket = socket_accept(listen)) < 0) {
        if(!platform_is_io_waiting()) {
            socket_close(listen);

            std::stringstream errStream;
            errStream << "Failed to accept peer: Error " << errno;
            ui_prompt(errStream.str(), false);
            return {-1, 0};
        } else if(platform_is_running()) {
            input_poll();
            if(input_is_pressed(BUTTON_B)) {
                socket_close(listen);
                return {-1, 0};
            }
        } else {
            return {-1, 0};
        }
    }

    socket_close(listen);

    ui_display_message("Reading info...");

    u64 fileSize;
    if(socket_read(socket, &fileSize, sizeof(fileSize)) < 0) {
        socket_close(socket);

        std::stringstream errStream;
        errStream << "Failed to read info: Error " << errno;
        ui_prompt(errStream.str(), false);
        return {-1, 0};
    }

    fileSize = ntohll(fileSize);
    return {socket, fileSize};
}