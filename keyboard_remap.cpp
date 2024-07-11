#include <stdio.h>
#include <windows.h>
#include <map>

HHOOK hHook;
std::map<int, int> keymap;
bool exitFlag = false;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
      if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;

        if (p->vkCode == VK_ESCAPE) {
          exitFlag = true;
          PostQuitMessage(0);
          return 1;
        }

        if (keymap.find(p->vkCode) != keymap.end())
        {
          INPUT input;
          input.type = INPUT_KEYBOARD;
          input.ki.wVk = keymap[p->vkCode];
          input.ki.dwFlags = 0;

          SendInput(1, &input, sizeof(INPUT));
          return 1;
        }
      }
    }
    
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

void initKeymap() {
  keymap['A'] = 'B';
}

int main() {
  initKeymap();
  hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);

  printf("Keyboard remapped, press ESC to exit\n");
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  UnhookWindowsHookEx(hHook);
  return 0;
}