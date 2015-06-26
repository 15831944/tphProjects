using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Configuration;

namespace EFLib
{
    public class EFLib
    {
        string strConn = ConfigurationManager.ConnectionStrings["connStr1"].ConnectionString;
    }
}
