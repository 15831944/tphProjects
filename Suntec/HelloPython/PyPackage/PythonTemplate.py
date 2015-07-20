#encoding=utf-8
#
#by panda
#模板方法模式


def printInfo(info):
    print unicode(info, 'utf-8').encode('gbk')

#金庸小说试题考卷
class TestPaper:
    def TestQuestion1(self):
        print "练成倚天剑和屠龙刀的玄铁可能是[] a.球墨铸铁 b.马口铁 c.高速合金钢 d.碳素纤维"
        print "答案：", self.Answer1()
    
    def TestQuestion2(self):
        print "杨过、程英、陆无双铲除了情花，造成[] a.使这种植物不再害人 b.使一种珍稀物种灭绝 c.破坏生态平衡 d.造成该地区沙漠化"
        print "答案：", self.Answer2()

    def TestQuestion3(self):
        print "蓝凤凰致使华山师徒、桃谷六仙呕吐不止，如果你是大夫，会给他们开什么药[] a.阿司匹林 b. 牛黄解毒片 c.氟哌酸 d.喝大量的生牛奶 e.以上全不对"
        print "答案：", self.Answer3()
    
    def Answer1(self):
        return ''
    
    def Answer2(self):
        return ''
    
    def Answer3(self):
        return ''
    
        
#学生甲
class TestPaperA(TestPaper):
    def Answer1(self):
        return 'A'
    
    def Answer2(self):
        return 'B'
    
    def Answer3(self):
        return 'C'

#学生乙
class TestPaperB(TestPaper):
    def Answer1(self):
        return 'B'
    
    def Answer2(self):
        return 'D'
    
    def Answer3(self):
        return 'E'

def clientUI():
    a = TestPaperA();
    a.TestQuestion1()
    a.TestQuestion2()
    a.TestQuestion3()
    
    b = TestPaperB();
    b.TestQuestion1()
    b.TestQuestion2()
    b.TestQuestion3()
    return

if __name__ == '__main__':
    clientUI();
