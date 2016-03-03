import win32ui
import win32con
def demo():
    dlg = win32ui.CreateFontDialog(None,win32con.CF_EFFECTS|win32con.CF_SCREENFONTS,None,None)
    if dlg.DoModal() != win32con.IDOK: return None
    print dlg.GetCharFormat()
    return dlg.GetCharFormat()

if __name__ == "__main__":
    demo() 