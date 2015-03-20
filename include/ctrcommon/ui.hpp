#ifndef __CTRCOMMON_UI_HPP__
#define __CTRCOMMON_UI_HPP__

#include "ctrcommon/app.hpp"
#include "ctrcommon/screen.hpp"
#include "ctrcommon/types.hpp"

#include <functional>
#include <string>
#include <vector>

typedef struct {
    std::string id;
    std::string name;
    std::vector<std::string> details;
} SelectableElement;

typedef struct {
    FILE* fd;
    u64 fileSize;
} RemoteFile;

bool uiSelect(SelectableElement* selected, std::vector<SelectableElement> elements, std::function<bool(std::vector<SelectableElement> &currElements, bool &elementsDirty, bool &resetCursorIfDirty)> onLoop, std::function<bool(SelectableElement select)> onSelect, bool useTopScreen = true, bool alphabetize = true);
bool uiSelectFile(std::string* selectedFile, const std::string rootDirectory, std::vector<std::string> extensions, std::function<bool(bool inRoot)> onLoop, bool useTopScreen = true);
bool uiSelectApp(App* selectedApp, MediaType mediaType, std::function<bool()> onLoop, bool useTopScreen = true);
void uiDisplayMessage(Screen screen, const std::string message);
bool uiPrompt(Screen screen, const std::string message, bool question);
void uiDisplayProgress(Screen screen, const std::string operation, const std::string details, bool quickSwap, int progress);
RemoteFile uiAcceptRemoteFile(Screen screen);

#endif
