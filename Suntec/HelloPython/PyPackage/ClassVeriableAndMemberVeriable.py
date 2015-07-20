class A:
    int_a = 10
    def __init__(self):
        self.m_b = 100
   
def main():
    a1 = A()
    a2 = A()
    A.int_a = 101
    a1.m_b = 102
    print A.int_a
    print a1.int_a
    print a1.m_b
    print a2.int_a
    print a2.m_b
    
    a1.int_a = 103
    print A.int_a
    print a1.int_a
    print a1.m_b
    print a2.int_a
    print a2.m_b
    
    a2.int_a = 104
    print A.int_a
    print a1.int_a
    print a1.m_b
    print a2.int_a
    print a2.m_b
    
    A.int_a = 105
    print A.int_a
    print a1.int_a
    print a1.m_b
    print a2.int_a
    print a2.m_b
    
    A.m_b = 106
    print A.int_a
    print a1.int_a
    print a1.m_b
    print a2.int_a
    print a2.m_b
    print a2.m_b
    
main()
    