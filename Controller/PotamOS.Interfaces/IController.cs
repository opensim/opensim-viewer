using System;
using System.Collections.Generic;

namespace PotamOS.Interfaces
{
    public interface IController
    {
        void Goto(string hppo);
        void SubmitForm(Dictionary<string, string> data);
    }
}
