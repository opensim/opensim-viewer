using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using PotamOS.Interfaces;
using PotamOS.Controller;

namespace PotamOS
{
    public class ControllerAsync : IController
    {
        private PotamOSController m_Controller;

        public ControllerAsync(IEngine eng)
        {
            m_Controller = new PotamOSController(eng);
        }

        public void Goto(string hppo)
        {
            Task.Run(() => m_Controller.Goto(hppo));
        }

        public void SubmitForm(Dictionary<string, string> data)
        {
            Task.Run(() => m_Controller.SubmitForm(data));
        }

    }
}
