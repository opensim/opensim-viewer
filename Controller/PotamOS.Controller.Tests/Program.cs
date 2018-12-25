using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using PotamOS.Controller.Network;

namespace PotamOS.Controller.Tests
{
    public class Program
    {
        static void Main(string[] args)
        {
            IDictionary<string, string> map = HttpRequests.GetJson("http://localhost:9000/json_grid_info");
            foreach (string key in map.Keys)
            {
                object value = map[key];
                Console.WriteLine(key + " : " + value);

                // do something with key and value
            }
        }
    }
}
