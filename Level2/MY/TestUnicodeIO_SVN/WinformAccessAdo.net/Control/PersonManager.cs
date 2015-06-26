using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.OleDb;
using System.Data;

namespace WinformAccessAdo.net
{
    class PersonManager
    {

        public static bool AddPerson(Person p)
        {
            AccessDBHelper.ExcuteNonQuery(@"insert into Persons (姓名,年龄) values (@Name,@Age)",
                new OleDbParameter("@Name", p._name),
                new OleDbParameter("@Age", p._age));
            return true;
        }

        public static List<Person> GetAllPerson1()
        {
            List<Person> personArray = new List<Person>();
            DataSet dataset = AccessDBHelper.ExcuteGetDataset(@"select * from Persons");
            DataTable table = dataset.Tables[0];
            DataRowCollection rows = table.Rows;
            foreach (DataRow row in rows)
            {
                string strName = (string)row[@"姓名"];
                int age = (int)row[@"年龄"];
                personArray.Add(new Person(strName, age));
            }
            return personArray;
        }
    }
}
