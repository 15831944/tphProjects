using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WinformAccessAdo.net
{
    class Person
    {
        public Person(string name, int? age)
        {
            _name = name;
            _age = age;
        }
        public string _name { get; set; }
        public int? _age { get; set; }
    }
}
