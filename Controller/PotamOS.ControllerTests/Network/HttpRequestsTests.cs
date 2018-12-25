using Microsoft.VisualStudio.TestTools.UnitTesting;
using PotamOS.Controller.Network;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PotamOS.Controller.Network.Tests
{
    [TestClass()]
    public class HttpRequestsTests
    {
        static readonly string goodurl = "http://google.com";
        static readonly string badurl = "http://badhost.bad:9000";

        [TestMethod()]
        public void GetTest()
        {
            string response = HttpRequests.Get(goodurl);
            Assert.IsNotNull(response, "Call to Google should return something.");

            response = null;
            response = HttpRequests.Get(badurl);
            Assert.IsNull(response, "Call to Badhost should return null.");
        }
    }
}