using System;
using Xenko.Core.Mathematics;

namespace PotamOS.Interfaces
{
    public enum UIPages
    {
        Splash = 0,
        Handshake = 1,
        DynamicScene = 2
    }

    public interface IEngine
    {
        String Name { get; }

        Vector3 CoordinateSystem { get; }

        /// <summary>
        /// This should go in the interface to the WebBrowser. It's here for now.
        /// </summary>
        /// <param name="html"></param>
        void NewPage(UIPages page, string html);
    }
}
