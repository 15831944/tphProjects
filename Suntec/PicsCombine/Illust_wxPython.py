import wx

class MyFrame(wx.Frame):
    def __init__(self, parent, theId, title):
        wx.Frame.__init__(self,parent,theId,title)
        
        vbox = wx.BoxSizer(wx.VERTICAL)  
        hbox1 = wx.BoxSizer(wx.HORIZONTAL)  
        hbox2 = wx.BoxSizer(wx.HORIZONTAL)  
        
        pnl1 = wx.Panel(self, -1, style=wx.SIMPLE_BORDER)  
        pnl2 = wx.Panel(self, -1, style=wx.RAISED_BORDER)  
        pnl3 = wx.Panel(self, -1, style=wx.SUNKEN_BORDER)  
        pnl4 = wx.Panel(self, -1, style=wx.DOUBLE_BORDER)  
        pnl5 = wx.Panel(self, -1, style=wx.STATIC_BORDER)  
        pnl6 = wx.Panel(self, -1, style=wx.NO_BORDER)  
        
        hbox1.Add(pnl1, 1, wx.EXPAND | wx.ALL, 3)  
        hbox1.Add(pnl2, 1, wx.EXPAND | wx.ALL, 3)  
        hbox1.Add(pnl3, 1, wx.EXPAND | wx.ALL, 3)  
        
        hbox2.Add(pnl4, 1, wx.EXPAND | wx.ALL, 3)  
        hbox2.Add(pnl5, 1, wx.EXPAND | wx.ALL, 3)  
        hbox2.Add(pnl6, 1, wx.EXPAND | wx.ALL, 3)  
        
        vbox.Add(hbox1, 1, wx.EXPAND)  
        vbox.Add(hbox2, 1, wx.EXPAND)  
        
        self.SetSizer(vbox)  
        self.Centre()  

class MyApp(wx.App):  
    def OnInit(self):  
        frame = MyFrame(None, -1, 'borders.py')  
        frame.Show(True)  
        return True

def main():
    app = MyApp(0)  
    app.MainLoop()  

if __name__ == '__main__':
    main()
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    