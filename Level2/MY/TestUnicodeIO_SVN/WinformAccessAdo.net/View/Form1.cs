using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;

namespace WinformAccessAdo.net
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void btnQuery_Click(object sender, EventArgs e)
        {
            
        }

        private void btnAdd_Click(object sender, EventArgs e)
        {
            if(tbName.Text.Length == 0)
            {
                MessageBox.Show("请输入名字");
                tbName.Focus();
                return;
            }
            Person newP = new Person();
            newP._name = tbName.Text;
            if(tbAge.Text.Length > 0)
                newP._age = int.Parse(tbAge.Text);
            PersonManager.AddPerson(newP);
        }

        private void tbAge_KeyPress(object sender, KeyPressEventArgs e)
        {
            Debug.Assert(tbAge.Text.IndexOf('.') == tbAge.Text.LastIndexOf('.'));
            Debug.Assert(tbAge.Text.IndexOf('.') == tbAge.Text.LastIndexOf('.'));
            if (e.KeyChar == (char)8)
            {
                return;
            }
            else if (e.KeyChar == '-')
            {
                if (tbAge.SelectionStart != 0)
                {
                    e.Handled = true;
                    return;
                }
                if (tbAge.SelectionStart != 0 && tbAge.Text[0] == '-')
                {
                    e.Handled = true;
                    return;
                }
            }
            else if (e.KeyChar == '.')
            {
                if (tbAge.Text.IndexOf('.') == -1)
                {
                    if (tbAge.Text[0] == '-' && tbAge.SelectionStart == 0)
                    {
                        e.Handled = true;
                        return;
                    }
                    else
                    {
                        return;
                    }
                }
                else
                {
                    if (tbAge.SelectedText.IndexOf('.') == -1)
                    {
                        e.Handled = true;
                        return;
                    }
                    else
                    {
                        return;
                    }
                }
            }
            else if (Char.IsNumber(e.KeyChar))
            {
                string strTbAge = tbAge.Text;
                if (strTbAge.IndexOf('-') == 0 && tbAge.SelectionStart == 0 && tbAge.SelectionLength == 0)
                {
                    e.Handled = true;
                    return;
                }
                else
                {
                    return;
                }
            }
            else
            {
                e.Handled = true;
                return;
            }
        }
    }
}
