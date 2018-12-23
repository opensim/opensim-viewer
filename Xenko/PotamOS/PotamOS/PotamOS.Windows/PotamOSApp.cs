using Xenko.Engine;

namespace PotamOS
{
    class PotamOSApp
    {
        static void Main(string[] args)
        {
            using (var game = new Game())
            {
                game.Run();
            }
        }
    }
}
