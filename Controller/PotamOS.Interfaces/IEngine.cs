using System;
using Xenko.Core.Mathematics;

namespace PotamOS.Interfaces
{
    public interface IEngine
    {
        String Name { get; }

        Vector3 CoordinateSystem { get; }
    }
}
