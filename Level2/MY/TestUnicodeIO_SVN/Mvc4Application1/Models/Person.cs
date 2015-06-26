using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.ComponentModel.DataAnnotations;

namespace Mvc4Application1.Models
{
    public class Person
    {
        [Required]
        public int ID { get; set; }
        [Required]
        public string Firstname { get; set; }
        [Required]
        [StringLength(16)]
        public string Lastname { get; set; }
    }
}