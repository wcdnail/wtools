#define WINVER 0x600
#define _WIN32_WINDOWS 0x600
#define _WIN32_WINNT 0x600
#define _WIN32_IE 0x400

#include "quickdialog.h"
using namespace rottedfrog::quickdialog;

void ScratchDialog();
void apply_changes(abstractcontrol *wnd)
{ MessageBoxA(wnd->getparentdialog().gethwnd(), "Changes Applied.", "Apply", 0); }

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{    
  bool checked = true;
  bool option[] = { false, false, true, false, false, false, true, false, false };
  std::string s[] = { "Item 1", "Item 2", "Item 3", "Item 4", "Item 5" };
  std::string combotext("Freeform List");
  std::vector<int> selectedindices;
  check_state trichecked = csUnchecked;
  dialog d("Hello");
  std::wstring text(L"Some Text");
  wdialog dlg(L"Test Dialog");
  int comboindex = 0, listindex = 0, radioindex = 0;
  dlg << image(LoadIcon(NULL, IDI_INFORMATION))
      << columnbreak()
      << -wparagraph(L"Hello, World! This is a really long paragraph in order to show button alignment is centred.")
      << L"Unicode paragraph!"
      << wparagraph(L"Testing the font", L"MS Shell Dlg 2", -24, fsBold, 0x00FF00)
      << L"Link to <A HREF=\"http://thelink.thomsonreuters.com/\">The Link</A>"
      << sectionbreak()
      << check_box("Never show me anything ever again", checked)
      << tri_state_check_box("Maybe never show me anything again", trichecked)
      << -(groupbox("Options") << radio_button("Option 1", option[0], true)
                                    << radio_button("Option 2", option[1]) 
                                    << radio_button("Option 3", option[2]) 
                                    << columnbreak() 
                                    << radio_button("Option 4", option[3]) 
                                    << radio_button("Option 5", option[4]) 
                                    << wradio_button(L"Option 6", option[5]))
      << combo_box(comboindex, s, s + 5)
      << wedit(text)
      << combo_box(combotext, s, s + 5)
      << listbox(listindex, s, s + 5)
      << (paragraph("Multi-select:") | multiselectlistbox(selectedindices, true, s, s + 5))
      << radio_button("Option B1", option[6], true) 
      << radio_button("Option B2", option[7]) 
      << radio_button("Option B3", option[8])
      << (radio_button_group(radioindex) << "Option C1" << "Option C2" << "Option C3" << "Option C4" << "Option C5")
      << (wtab_control(1) + (wtab(L"Hello") << L"xThis is on tab \"Hello\"") + (wtab(L"World") << "This is on tab \"World\""))
//No lambdas in VS2008...
#if !defined(_MSC_VER) || _MSC_VER >= 1600
      << ~*(group(75, 23) | wbutton(L"OK", [] (abstractcontrol *sender) { MessageBoxA(sender->getparentdialog().gethwnd(), "This is fucking awesome...", "Woohoo!", 0); }, qdOK)
#else
      << ~*(group(75, 23) | wbutton(L"&OK", qdOK, true)
#endif
       | button("&Cancel", qdCancel));
  
  //MessageBoxA(NULL, (dlg.show() == qdCancel) ? "Cancelled!" : "Okayed!", "Result", 0);
  dlg.show();

  // Samples from Codeproject article:
  {
    dialog d("My first dialog");
    d << "Hello, World!"
      << spacer()
      << *button("OK");
    d.show();
    MessageBoxA(NULL, "Hello, World", "My first dialog", MB_YESNO);
  }

  {
    bool check = false;
    HICON ico = (HICON)LoadImageA(NULL, "ChickenEgg.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

    dialog d("My second dialog");
    d << image(ico)
      << columnbreak()
      << spacer(20)
      << "Which came first, the chicken or the egg?"
      << sectionbreak()
      << spacer()
      << ~*(/*group(75,23) |*/ button("&Chicken", qdYes) | button("&Egg", qdNo) | button("&Don't Care", qdCancel))
      << spacer()
      << check_box("Don't ask me pointless questions again", check);
    if (d.show() == qdYes)
    { /*...*/ }
  }

  {
    int fontsize = 2; 
    std::string autosaveinterval("10");
    const char* fontsizecaptions[] = { "Very Small", "Small", "Normal", "Large", "Very Large" };
    bool bold = false, italic = false, underline = false, strikethough = false;
    bool superscript = false, subscript = false, smallcaps = false, normal = true;

    bool reloadonstartup = false, multiinstance = false, splashscreen = true, lockfiles = false;
    bool linenums = false, autosave = true;
    dialog d("Options");
    d << (tab_control(0) + (tab("Font") << (paragraph("Font Size:") | combo_box(fontsize, fontsizecaptions, fontsizecaptions + 5))
                                        << (groupbox("Styles") << check_box("Bold", bold)
                                                                << check_box("Italic", italic)
                                                                << check_box("Underline", underline)
                                                                << check_box("Strikethrough", strikethough)
                                                                << columnbreak()
                                                                << radio_button("Normal", normal)
                                                                << radio_button("Subscript", subscript)
                                                                << radio_button("Superscript", superscript)))
                          + (tab("Other options") << check_box("Reload last document at startup", reloadonstartup)
                                                  << check_box("Allow multiple instances to run", multiinstance)
                                                  << check_box("Display Splash screen", splashscreen)
                                                  << check_box("Keep files locked while editing", lockfiles)
                                                  << check_box("Show line numbers", linenums)
                                                  << (check_box("Autosave every", autosave) | edit(autosaveinterval, esNumber, 25) | paragraph("minutes"))))
                                                  << ~+(button("&Apply", apply_changes, qdNone) 
                                                      | button("&OK", qdOK, true) 
                                                      | button("&Cancel", qdCancel));
    d.show();
  }
  return 0;
}