#include <stdio.h>
#include <windows.h>
#include <map>

HHOOK hHook;
std::map<int, int> keymap;
std::map<std::pair<int, int>, int> keymapcombo;
std::map<int, bool> keydown;
bool exitFlag = false;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
      KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
      int vkCode = p->vkCode;

      if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
        if (vkCode == VK_ESCAPE) {
          exitFlag = true;
          PostQuitMessage(0);
          return 1;
        }

        keydown[vkCode] = true;

        for (auto it = keymapcombo.begin(); it != keymapcombo.end(); ++it) {
          if (keydown[it->first.first] && keydown[it->first.second]) {
            keydown[it->first.first] = false;
            keydown[it->first.second] = false;

            INPUT ip;
            ip.type = INPUT_KEYBOARD;
            ip.ki.wVk = it->second;
            ip.ki.dwFlags = 0;
            ip.ki.time = 0;
            ip.ki.dwExtraInfo = 0;

            SendInput(1, &ip, sizeof(INPUT));

            return 1;
          } else if (keydown[it->first.first] || keydown[it->first.second]) {
            return 1;
          }
        }

        if (keymap.find(vkCode) != keymap.end()) {
          INPUT ip;
          ip.type = INPUT_KEYBOARD;
          ip.ki.wVk = keymap[vkCode];
          ip.ki.dwFlags = 0;
          ip.ki.time = 0;
          ip.ki.dwExtraInfo = 0;

          SendInput(1, &ip, sizeof(INPUT));

          return 1;
        }
      } else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
        keydown[vkCode] = false;

        if (keymap.find(vkCode) != keymap.end()) {
          INPUT ip;
          ip.type = INPUT_KEYBOARD;
          ip.ki.wVk = keymap[vkCode];
          ip.ki.dwFlags = KEYEVENTF_KEYUP;
          ip.ki.time = 0;
          ip.ki.dwExtraInfo = 0;

          SendInput(1, &ip, sizeof(INPUT));

          return 1;
        }
      }
    }
    
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

void initKeymap() {
  keymap['A'] = 'B';

  keymapcombo[{'Q', 'W'}] = 'D';
}

int main() {
  initKeymap();
  hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);

  for (auto it = keymap.begin(); it != keymap.end(); ++it) {
    printf("Key %c remapped to %c\n", it->first, it->second);
  }

  for (auto it = keymapcombo.begin(); it != keymapcombo.end(); ++it) {
    printf("Key combo %c + %c remapped to %c\n", it->first.first, it->first.second, it->second);
  }

  printf("Keyboard remapped, press ESC to exit\n");
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  UnhookWindowsHookEx(hHook);
  return 0;
}