using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using PotamOS.Controller.Network;

namespace PotamOS.Controller
{
    public class Handshaker
    {
        public Handshaker(HppoInfo hinfo)
        {
            string url = hinfo.Gatekeeper.ToString() + "/hppo";
            string response = HttpRequests.Get(url);
        }
    }
}
