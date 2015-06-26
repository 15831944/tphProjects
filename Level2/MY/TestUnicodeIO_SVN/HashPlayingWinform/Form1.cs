using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.Net;
using System.IO;
using System.Net.Sockets;

namespace HashPlayingWinform
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            tbUri.Text = @"http://www.baidu.com";
        }

        private void button1_Click(object sender, EventArgs e)
        {
            #region WebRequest
            //             Hashtable tb = new Hashtable();
            //             string str = (string)tb["d"];
            //             string strUri = tbUri.Text;
            //             WebRequest wr = WebRequest.Create(strUri);
            //             WebResponse response = wr.GetResponse();
            //             if (response is HttpWebResponse || response is FtpWebResponse)
            //             {
            //                 Encoding ed = Encoding.GetEncoding ("gb2312");
            //                 StreamReader sr = new StreamReader(response.GetResponseStream(), ed);
            //                 string strRes = sr.ReadToEnd();
            //             }
            #endregion


            string hostName = @"www.baidu.com";
            int port = 80;
            IPHostEntry ipInfo = Dns.GetHostEntry(hostName);
            IPAddress[] ipAddr = ipInfo.AddressList;
            IPAddress ip = ipAddr[0];
            IPEndPoint hostEP = new IPEndPoint(ip, port);
            Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            try
            {
                socket.Connect(hostEP);
            }
            catch (Exception se)
            {
                MessageBox.Show("connection error" + se.Message, "information", MessageBoxButtons.RetryCancel, MessageBoxIcon.Information);
            }

            string sendStr = "GET / HTTP/1.1\r\nHost: " + hostName + "\r\nConnection: Close\r\n\r\n";
            byte[] bytesSendStr = new byte[1024];
            bytesSendStr = Encoding.ASCII.GetBytes(sendStr);
            try
            {
                socket.Send(bytesSendStr, bytesSendStr.Length, 0);
            }
            catch (Exception ce)
            {
                MessageBox.Show("send error:" + ce.Message, "information", MessageBoxButtons.RetryCancel, MessageBoxIcon.Information);
            }

            string recvStr = "";
            byte[] recvBytes = new byte[1024];
            int bytes = 0;
            while (true)
            {
                bytes = socket.Receive(recvBytes, recvBytes.Length, 0);
                if (bytes <= 0)
                    break;
                recvStr += Encoding.ASCII.GetString(recvBytes, 0, bytes);
            }
            byte[] content = Encoding.ASCII.GetBytes(recvStr);
            try
            {
//                 FileStream fs = new FileStream(fileName, FileMode.OpenOrCreate, FileAccess.ReadWrite);
//                 fs.Write(content, 0, content.Length);
            }
            catch (Exception fe)
            {
                MessageBox.Show("file error:" + fe.Message, "information", MessageBoxButtons.RetryCancel, MessageBoxIcon.Information);
            }
            socket.Shutdown(SocketShutdown.Both);
            socket.Close();
        }
    }
}
