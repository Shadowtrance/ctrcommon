#include "ctrcommon/common.hpp"

#include <sys/dirent.h>
#include <string.h>

#include <stack>
#include <algorithm>
#include <sstream>
#include <iomanip>

typedef struct {
    std::string id;
    std::string name;
    std::vector<std::string> details;
} SelectableElement;

SelectionResult ui_select(std::vector<SelectableElement> elements, SelectableElement* selected, bool enableBack, std::function<bool()> onLoop) {
    u32 cursor = 0;
    u32 scroll = 0;

    u32 selectionScroll = 0;
    u64 selectionScrollEndTime = 0;

    while(platform_is_running()) {
        input_poll();
        if(input_is_pressed(BUTTON_A)) {
            *selected = elements.at(cursor);
            return SELECTED;
        }

        if(enableBack && input_is_pressed(BUTTON_B)) {
            return BACK;
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

        u32 screenWidth = (u32) screen_get_width();
        for(std::vector<SelectableElement>::iterator it = elements.begin() + scroll; it != elements.begin() + scroll + 20 && it != elements.end(); it++) {
            SelectableElement element = *it;
            u32 index = (u32) (it - elements.begin());
            u8 color = 255;
            int offset = 0;
            if(index == cursor) {
                color = 0;
                screen_fill(0, (int) (index - scroll) * 12, (int) screenWidth, screen_get_str_height(element.name), 255, 255, 255);
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
            for(std::vector<std::string>::iterator it = currSelected.details.begin(); it != currSelected.details.end(); it++) {
                std::string detail = *it;
                u32 index = (u32) (it - currSelected.details.begin());
                screen_draw_string(detail, 0, (int) index * 12, 255, 255, 255);
            }
        }

        bool result = onLoop();

        screen_end_draw();
        screen_swap_buffers();
        if(result) {
            return MANUAL_BREAK;
        }
    }

    return APP_CLOSING;
}

bool ui_is_directory(const std::string path) {
    DIR *dir = opendir(path.c_str());
    if(!dir) {
        return false;
    }

    closedir(dir);
    return true;
}

struct ui_alphabetize {
    inline bool operator() (SelectableElement a, SelectableElement b) {
        return strcasecmp(a.name.c_str(), b.name.c_str()) < 0;
    }
};

std::vector<SelectableElement> ui_get_dir_elements(const std::string directory, const std::string extension) {
    std::vector<SelectableElement> elements;
    DIR *dir = opendir(directory.c_str());
    if(dir != NULL) {
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
                if(dotPos != std::string::npos && path.substr(dotPos + 1).compare(extension) == 0) {
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

    std::sort(elements.begin(), elements.end(), ui_alphabetize());
    elements.insert(elements.begin(), {"..", ".."});
    elements.insert(elements.begin(), {".", "."});
    return elements;
}

bool ui_select_file(const std::string rootDirectory, const std::string extension, std::string* selectedFile, std::function<bool()> onLoop) {
    std::stack<std::string> directoryStack;
    std::string currDirectory = rootDirectory;
    while(platform_is_running()) {
        SelectableElement selected;
        std::vector<SelectableElement> contents = ui_get_dir_elements(currDirectory, extension);
        SelectionResult result = ui_select(contents, &selected, !directoryStack.empty(), onLoop);
        if(result == APP_CLOSING || result == MANUAL_BREAK) {
            break;
        } else if(result == BACK) {
            currDirectory = directoryStack.top();
            directoryStack.pop();
        } else if(result == SELECTED) {
            if(selected.name.compare(".") == 0) {
                continue;
            } else if(selected.name.compare("..") == 0) {
                if(directoryStack.empty()) {
                    continue;
                }

                currDirectory = directoryStack.top();
                directoryStack.pop();
            } else {
                if(ui_is_directory(selected.id)) {
                    directoryStack.push(currDirectory);
                    currDirectory = selected.id;
                } else {
                    *selectedFile = selected.id;
                    return true;
                }
            }
        }
    }

    return false;
}

bool ui_select_app(MediaType mediaType, App* selectedApp, std::function<bool()> onLoop) {
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

    std::sort(elements.begin(), elements.end(), ui_alphabetize());

    SelectableElement selected;
    SelectionResult result = ui_select(elements, &selected, false, onLoop);
    if(result != APP_CLOSING && result != MANUAL_BREAK && selected.id.compare("None") != 0) {
        for(std::vector<App>::iterator it = apps.begin(); it != apps.end(); it++) {
            App app = *it;
            if(app.titleId == (u64) strtoll(selected.id.c_str(), NULL, 16)) {
                *selectedApp = app;
            }
        }

        return true;
    }

    return false;
}